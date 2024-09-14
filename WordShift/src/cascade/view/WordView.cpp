//
//  WordView.cpp
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "WordView.h"
#include "GameColor.h"
#include "BoardTimeline.h"
#include "SoundStation.h"

using namespace cascade;
using namespace ci;
using namespace std;
using pockets::Sprite;
using pockets::SpriteRef;

void ClockOutline::setCoordinates(const ci::Vec2f &tl, const ci::Vec2f &tr, const ci::Vec2f &br, const ci::Vec2f &bl )
{
  mLines.at( 0 ) = pk::ExpandedLine2d::create( tl, tr );
  mLines.at( 1 ) = pk::ExpandedLine2d::create( tr, br );
  mLines.at( 2 ) = pk::ExpandedLine2d::create( br, bl );
  mLines.at( 3 ) = pk::ExpandedLine2d::create( bl, tl );

  float total_length = 0;
  for( int i = 0; i < mLines.size(); ++i )
  {
    total_length += mLines.at(i)->getLength();
  }
  for( int i = 0; i < mPortions.size(); ++i )
  {
    mPortions.at( i ) = mLines.at( i )->getLength() / total_length;
  }
}

void ClockOutline::setTime(float t)
{
  if( t >= 1.0f )
  {
    for( auto &line : mLines )
    {
      line->scaleLength( 1.0f );
    }
  }
  else if ( t <= 0.0f )
  {
    for( auto &line : mLines )
    {
      line->scaleLength( 0.0f );
    }
  }
  else
  {
    for( size_t i = 0; i < mLines.size(); ++i )
    {
      float portion = mPortions.at( i );
      float local_scale = t > 0.0f ? std::min( t / portion, 1.0f ) : 0.0f;
      mLines.at( i )->scaleLength( local_scale );
      t -= portion;
    }
  }
}

void ClockOutline::setTimeInverse(float t)
{
  if( t >= 1.0f )
  {
    for( auto &line : mLines )
    {
      line->scaleLengthInverse( 1.0f );
    }
  }
  else if ( t <= 0.0f )
  {
    for( auto &line : mLines )
    {
      line->scaleLengthInverse( 0.0f );
    }
  }
  else
  {
    for( int i = mLines.size() - 1; i >= 0; --i )
    {
      float portion = mPortions.at( i );
      float local_scale = t > 0.0f ? std::min( t / portion, 1.0f ) : 0.0f;
      mLines.at( i )->scaleLengthInverse( local_scale );
      t -= portion;
    }
  }
}

void ClockOutline::matchSprite(const pk::SpriteData &sprite)
{
  for( auto &line : mLines )
  {
    line->matchSprite( sprite );
  }
}

void ClockOutline::render()
{
  gl::color( mTint );
  for( auto &line : mLines )
  {
    line->render();
  }
}

WordView::WordView( WordRef word, ReadingDirection left_right, ReadingDirection top_bottom, const ci::Vec2f &tile_size, const pk::SpriteData &corner, const pk::SpriteData &line ):
mModel( word )
{
  word->appendReceiver( this );
  float half_width = tile_size.x / 2;
  float half_height = tile_size.y / 2;

  Vec2f start = Vec2f(word->firstLetter()->getPosition()) * tile_size;
  Vec2f end = Vec2f(word->lastLetter()->getPosition()) * tile_size;

  Vec2f nw( -half_width, -half_height );
  Vec2f ne( half_width, -half_height );
  Vec2f se( half_width, half_height );
  Vec2f sw( -half_width, half_height );

  if( left_right == ReadingDirection::Negative )
  { // flip offsets horizontally
    nw.x = half_width;
    sw.x = half_width;
    ne.x = -half_width;
    se.x = -half_width;
  }
  if( top_bottom == ReadingDirection::Negative )
  { // flip offsets vertically
    nw.y = half_height;
    ne.y = half_height;
    se.y = -half_height;
    sw.y = -half_height;
  }

  if( word->isInRow() )
  {
    nw += start;
    ne += end;
    se += end;
    sw += start;
  }
  else
  {
    nw += start;
    ne += start;
    se += end;
    sw += end;
  }

  mCornerLocs[0] = nw;
  mCornerLocs[1] = ne;
  mCornerLocs[2] = se;
  mCornerLocs[3] = sw;


  const bool device_horizontal = (left_right != top_bottom);
  if( device_horizontal )
  { // reorder coordinates
    mCornerLocs[1] = sw;
    mCornerLocs[3] = ne;
  }
  mOutline.setCoordinates( mCornerLocs[0], mCornerLocs[1], mCornerLocs[2], mCornerLocs[3] );
  mOutline.matchSprite( line );
  mOutline.setLayer( 1 );
  mOutline.setTime( 0.0f );
  mOutline.setTint( getBackingHighlightColor() );

  const float base_angle = readingDirectionAngle( left_right, top_bottom );

  mCorners.at( 0 ) = SpriteRef( new Sprite(corner) );
  mCorners.at( 1 ) = SpriteRef( new Sprite(corner) );
  mCorners.at( 2 ) = SpriteRef( new Sprite(corner) );
  mCorners.at( 3 ) = SpriteRef( new Sprite(corner) );

  mCorners.at( 0 )->setLoc( mCornerLocs[0] );
  mCorners.at( 0 )->getLocus().setRotation( base_angle + M_PI );
  mCorners.at( 1 )->setLoc( mCornerLocs[1] );
  mCorners.at( 1 )->getLocus().setRotation( base_angle -M_PI / 2 );
  mCorners.at( 2 )->setLoc( mCornerLocs[2] );
  mCorners.at( 2 )->getLocus().setRotation( base_angle );
  mCorners.at( 3 )->setLoc( mCornerLocs[3] );
  mCorners.at( 3 )->getLocus().setRotation( base_angle + M_PI / 2 );

  for( int i = 0; i < mCorners.size(); ++i )
  {
    mCorners.at( i )->setTint( getBackingHighlightColor() );
  }

  Vec2f center = Vec2f::zero();
  for( int i = 0; i < mCornerLocs.size(); ++i )
  {
    center += mCornerLocs[i];
  }
  center /= mCornerLocs.size();
  // animate all but first corner (since that's where the line appears)
  for( int i = 1; i < mCornerLocs.size(); ++i )
  {
    Vec2f start = mCornerLocs[i]() + (center - mCornerLocs[i]()).normalized() * 4.0f;
    mCorners.at( i )->setLoc( start );
    boardTimeline().appendTo( &mCornerLocs[i], start, mCornerLocs[i](), 0.2f, EaseOutQuint() )
    .updateFn( [this, i](){
      mCorners.at( i )->setLoc( mCornerLocs[i]() );
    });
  }
}

WordView::~WordView()
{}

void WordView::update()
{
  if( !mSounded )
  { // first update happens after possible ALREADY_PLAYED event
    // since that marks the sound as played, too, we avoid double-sounding
    Sound().cueSound( SoundStation::eWordFound, 0.0f );
    mSounded = true;
  }
  WordRef model = mModel.lock();
  if( model ){ mOutline.setTime( model->getNormalizedTime() ); }
}

void WordView::connectToRenderer(pk::SimpleRenderer *renderer)
{
  for( auto corner : mCorners )
  {
    renderer->add( corner.get() );
  }
  renderer->add( &mOutline );
}

void WordView::vanish()
{
  mIsDead = true;
}

void WordView::warnAlreadyPlayed()
{
  Sound().cueSound( SoundStation::eWarnUsed, 0.0f );
  mSounded = true;
  auto update_fn = [this](){
    for( auto &corner : mCorners )
    {
      corner->setTint( mColorAnim );
    }
  };
  mOutline.setTime( 0 );
  ColorA warning( getWarningColor(), 1.0f );
  ColorA warning_out( getWarningColor(), 0.0f );
  mColorAnim = warning;
  update_fn();
  boardTimeline().appendTo( &mColorAnim, warning_out, 0.04f, EaseInQuint() )
                .updateFn( update_fn )
                .delay( 0.24f );
  boardTimeline().appendTo( &mColorAnim, warning, 0.16f, EaseOutQuint() )
              .updateFn( update_fn );
  boardTimeline().appendTo( &mColorAnim, warning_out, 0.04f, EaseInQuint() )
                .updateFn( update_fn )
                .finishFn( [this](){ vanish(); } );
}

void WordView::receive(const cascade::WordMessage &msg)
{
  switch ( msg.getEvent() ) {
    case WordMessage::VANISH:
      vanish();
      break;
    case WordMessage::ALREADY_PLAYED:
      warnAlreadyPlayed();
      break;
    default:
      break;
  }
}
