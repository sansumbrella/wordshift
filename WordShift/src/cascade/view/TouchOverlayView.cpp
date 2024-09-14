//
//  TouchOverlayView.cpp
//  WordShift
//
//  Created by David Wicks on 5/9/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TouchOverlayView.h"
#include "pockets/CollectionUtilities.hpp"

using namespace cascade;
using namespace cinder;
using namespace std;

TouchOverlayView::TouchOverlayView()
{}

TouchOverlayView::~TouchOverlayView()
{
  disconnect();
}

void TouchOverlayView::setGraphic(const ci::Surface &surface)
{
  mTextureSize = app::toPoints( surface.getBounds() - surface.getSize()/2 );
  mTouchTexture = gl::Texture( surface );
}

void TouchOverlayView::connect( ci::app::WindowRef window, int group )
{ // draw on top
  disconnect();
  if( group != NO_GROUP )
  {
    storeConnection( window->getSignalDraw().connect( group, [this](){ draw(); } ) );
  }
  else
  {
    storeConnection( window->getSignalDraw().connect( [this](){ draw(); } ) );
  }
  storeConnection( app::App::get()->getSignalUpdate().connect( [this](){ update(); } ) );
  storeConnection( window->getSignalTouchesBegan().connect( [this](app::TouchEvent &event){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this](app::TouchEvent &event){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this](app::TouchEvent &event){ touchesEnded( event ); } ) );
  mTimer.start();
}

void TouchOverlayView::disconnect()
{
  for( auto c : mConnections )
  {
    c.disconnect();
  }
  mConnections.clear();
  mTimer.stop();
}

void TouchOverlayView::touchesBegan(const ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    TouchViewRef v( new TouchView{ touch.getPos(), 0.5f, 1.0f, true } );
    mTimeline->apply( &v->scale, 1.0f, 0.12f, EaseOutQuad() );
    mTouches[touch.getId()] = v;
  }
}

void TouchOverlayView::touchesMoved(const ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    mTouches[touch.getId()]->loc = touch.getPos();
  }
}

void TouchOverlayView::touchesEnded(const ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    //
    auto v = mTouches[touch.getId()];
    mTouches.erase( touch.getId() );
    mTimeline->appendTo( &v->scale, 1.25f, 0.3f, EaseOutQuad() );
    mTimeline->apply( &v->alpha, 0.0f, 0.3f, EaseInQuad() )
    .finishFn( [v](){ v->alive = false; } );
    mDyingTouches.push_back( v );
  }
}

void TouchOverlayView::update()
{
  mTimeline->step( 1.0f / 60.0f ); // ( mTimer.getSeconds() );
  pk::vector_erase_if( &mDyingTouches, [](TouchViewRef v){ return !v->alive; } );
}

void TouchOverlayView::draw()
{
  gl::enableAlphaBlending( true );
  mTouchTexture.enableAndBind();
  for( auto &v : mDyingTouches )
  {
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, v->alpha() ) );
    gl::drawSolidRect( (mTextureSize * v->scale()) + v->loc );
  }

  gl::color( Color( 1.0f, 1.0f, 1.0f ) );
  for( auto &pair : mTouches )
  {
    auto v = pair.second;
    gl::drawSolidRect( (mTextureSize * v->scale()) + v->loc );
  }
  mTouchTexture.unbind();
}

