//
//  PlayerView.cpp
//  WordShift
//
//  Created by David Wicks on 2/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "PlayerView.h"
#include "cinder/Utilities.h"
#include "cinder/ip/Resize.h"
#include "cinder/ip/Fill.h"
#include "GameColor.h"
#include "cinder/Text.h"

using namespace cascade;
using namespace ci;
using namespace std;

PlayerView::PlayerView( const Font &font, const int width, PlayRulesRef end_condition ):
mFont( font )
, mEndCondition( end_condition )
{
  TextLayout layout;
  layout.setFont( mFont );
  layout.setColor( Color::white() );
  layout.addLine( "0 points" );
  Surface render = layout.render( true, true );
  const int height = app::toPoints( render.getHeight() );

  mScoreSurface = Surface( app::toPixels(width), render.getHeight(), true, SurfaceChannelOrder::BGRA );
  ip::fill( &mScoreSurface, ColorA( 0, 0, 0, 0 ) );
  mScoreSurface.copyFrom( render, render.getBounds() );

  mEndCondition->getLocus()->setParent( getLocus() );

  mScreenBounds = Rectf( 0, 0, width, height );
  mTextureRegion = app::toPixels( Area(0, 0, width, height) );
  mScoreTexture = gl::Texture( mScoreSurface );
}

PlayerView::~PlayerView()
{}

void PlayerView::receive(const cascade::PlayerScoreMessage &msg)
{
  ScoreUpdate update;
  update.word = msg.getWord();
  update.delta = "+" + toString( msg.getWordValue() );
  update.new_score = toString( msg.getTotalScore() );
  mUpdates.push_back( update );
}

void PlayerView::receive(const cascade::PlayerTurnMessage &msg)
{
  if( msg.isTurnBeginning() )
  {
    setIntensity( 1.0f );
  }
  else
  {
    setIntensity( 0.4f );
  }
}

void PlayerView::setIntensity( float value )
{
  mIntensity = value;
}

void PlayerView::update( float deltaTime )
{
  float delay = mTextureOffset.isComplete() ? 0.0f : 0.2f;
  for( ScoreUpdate &score : mUpdates )
  { // show each word
    changeDisplaySoft( score.word + " " + score.delta, delay );
    delay = 0.2f;
  }
  if( !mUpdates.empty() )
  { // show total score
    changeDisplaySoft( mUpdates.back().new_score + " points", delay );
  }
  mUpdates.clear();
  mTimeline->step( deltaTime );
}

void PlayerView::changeDisplaySoft( const std::string &new_text, float delay )
{
  // slide down, change when hidden
  mTimeline->appendTo( &mTextureOffset, Vec2f( 0, app::toPixels(-getHeight()) ), 0.33f, EaseInQuint() )
  .delay( delay )
  .finishFn( [=](){ changeDisplay( new_text ); } );
  // slide up
  mTimeline->appendTo( &mTextureOffset, Vec2f::zero(), 0.4f, EaseInOutQuint() );
}

void PlayerView::changeDisplay( const string &new_message )
{
  TextLayout layout;
  layout.setFont( mFont );
  layout.clear( ColorA( 0, 0, 0, 0 ) );
  layout.setColor( Color::white() );
  layout.addLine( new_message );
  Surface render = layout.render( true, true );
  ip::fill( &mScoreSurface, ColorA( 0, 0, 0, 0 ) );
  mScoreSurface.copyFrom( render, render.getBounds() );
  mScoreTexture.update( mScoreSurface );
}

void PlayerView::draw()
{
  gl::pushModelView();
  gl::multModelView( *mLocus );
  gl::color( ColorA( getAltTextColor(), mIntensity ) );
  gl::draw( mScoreTexture, mTextureRegion + mTextureOffset(), mScreenBounds );
  gl::popModelView();
  mEndCondition->draw();
}
