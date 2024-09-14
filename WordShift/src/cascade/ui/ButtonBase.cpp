//
//  ButtonBase.cpp
//  WordShift
//
//  Created by David Wicks on 4/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "ButtonBase.h"
#include "SoundStation.h"

using namespace cascade;
using namespace ci;

ButtonBase::ButtonBase( const Rectf &bounds ):
mHitBounds( bounds )
{
  setWidth( bounds.getWidth() );
  setHeight( bounds.getHeight() );
}

ButtonBase::~ButtonBase()
{}

void ButtonBase::expandHitBounds( float horizontal, float vertical )
{
  mHitBounds.x1 -= horizontal;
  mHitBounds.x2 += horizontal;
  mHitBounds.y1 -= vertical;
  mHitBounds.y2 += vertical;
}

void ButtonBase::connect( app::WindowRef window )
{
  mActive = true;
  storeConnection( window->getSignalTouchesBegan().connect( [this]( app::TouchEvent &event ){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this]( app::TouchEvent &event ){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this]( app::TouchEvent &event ){ touchesEnded( event ); } ) );
}

void ButtonBase::cancelInteractions()
{
  mTrackedTouch = 0;
  endHovering( false );
}

void ButtonBase::endHovering( bool selected )
{
  if( mHovering )
  {
    mHovering = false;
    auto sound = selected ? SoundStation::eButtonSelect : SoundStation::eButtonExit;
    Sound().cueSound( sound, 0.0f );
    hoverEnd();
  }
}

void ButtonBase::setHovering()
{
  if( !mHovering )
  {
    mHovering = true;
    Sound().cueSound( SoundStation::eButtonHover, 0.0f );
    hoverStart();
  }
}

void ButtonBase::touchesBegan(ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    if( contains( touch.getPos() ) )
    {
      mTrackedTouch = touch.getId();
      setHovering();
    }
  }
}

void ButtonBase::touchesMoved(ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    if( touch.getId() == mTrackedTouch )
    {
      if( contains( touch.getPos() ) )
      {
        setHovering();
      }
      else
      {
        endHovering( false );
      }
    }
  }
}

void ButtonBase::touchesEnded(ci::app::TouchEvent &event)
{
  bool selected = false;
  for( auto &touch : event.getTouches() )
  {
    if( touch.getId() == mTrackedTouch )
    {
      mTrackedTouch = 0;
      if( contains( touch.getPos() ) )
      {
        endHovering( true );
        selected = true;
        break;
      }
      endHovering( false );
    }
  }
  if( selected )
  {
    // in case of side effects in select function, emit selection last
    // e.g. if button navigates to a new screen, it will destroy itself
    emitSelect();
  }
}
