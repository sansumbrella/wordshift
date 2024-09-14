//
//  LetterView.cpp
//  Cascade
//
//  Created by David Wicks on 12/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "LetterView.h"
#include "cinder/app/App.h"
#include "pockets/AnimationUtils.h"
#include "cinder/Rand.h"
#include "BoardTimeline.h"

using namespace cascade;
using namespace ci;
using namespace std;

LetterView::LetterView( LetterRef model, const pk::Sprite &backing_sprite ):
mModel( model )
, mBacking( backing_sprite )
{
  mGlyphSprite = pk::Sprite{ letterSprites()[mModel->getChar()] };
}

void LetterView::receive( const LetterMessage &message )
{
  switch( message.getEvent() )
  {
    case LetterMessage::CHANGE:
      updateGlyph( mModel->getChar(), message.getChangeDirection(), message.getOffset() );
      break;
    case LetterMessage::SELECT:
      mColor = mModel->isSelected() ? getBackingHighlightColor() : getBackingColor();
      break;
    case LetterMessage::ROTATE:
      break;
    default:
      break;
  }
}

void LetterView::updateGlyph(char glyph, float direction, float delay)
{
  mSwaps.push_back( LetterSwap( glyph, direction, delay ) );
}

void LetterView::update()
{
  float currentTime = timeline().getCurrentTime();
  if( !mSwaps.empty() )
  {
    float out_duration = 0.18f;
    float back_duration = 0.24f;
    auto swap = mSwaps.begin();
    auto next_swap = swap + 1;
    while( next_swap != mSwaps.end() )
    { // as long as there is a swap after this one
      float delta_time = next_swap->time - swap->time;
      if( delta_time < (back_duration + out_duration) )
      { // not enough time for a full animation
        flip( swap->letter, swap->direction, currentTime + swap->time, delta_time / 2, delta_time / 2, EaseNone() );
      }
      else
      { // play full animation
        flip( swap->letter, swap->direction, currentTime + swap->time, out_duration, back_duration, EaseOutQuint() );
      }
      swap++;
      next_swap++;
    }
    // play final animation
    flip( swap->letter, swap->direction, currentTime + swap->time, out_duration, back_duration, EaseOutQuint() );
    mSwaps.clear();
  }
}

void LetterView::updateFromModel()
{
  mGlyphSprite = pk::Sprite{ letterSprites()[mModel->getChar()] };
}

void LetterView::flip(const char letter, float direction, float at_time, float out_duration, float back_duration, EaseFn back_ease )
{
///*
  Vec2f offset( mBounds.getWidth(), 0 );
  offset.rotate( direction - getRotation() );
  timeline().appendTo( &mOffset, offset, out_duration )
  .finishFn( [this,letter](){
    mGlyphSprite = pk::Sprite{ letterSprites()[letter] };
  } )
  .startTime( at_time );
  timeline().appendTo( &mOffset, Vec2f::zero(), back_duration, back_ease );
//*/
}

void LetterView::appear( const Vec2f &from_direction, float duration, float delay )
{
  mOffset = mBounds.getSize() * Vec2f(from_direction);
  timeline().appendTo( &mOffset, Vec2f::zero(), duration, EaseOutQuint() )
            .delay( delay );
//  mTileRotation = Rand::randFloat( 360.0f );
//  timeline().appendTo( &mTileRotation, 0.0f, duration, EaseOutQuint() )
//            .delay( delay );
}

void LetterView::setRotation(float rotation, float delay)
{
  if( rotation - mRotation > M_PI )
  {
    rotation -= 2 * M_PI;
  }
  else if ( mRotation - rotation > M_PI )
  {
    mRotation() -= 2 * M_PI;
  }
  timeline().apply( &mRotation, rotation, 0.4165f, EaseOutQuint() )
                 .delay( delay );
}

void LetterView::pulse( float delay )
{
  boardTimeline().apply( &mTextColor, getWarningColor(), 0.133f, EaseOutQuint() )
  .delay( delay );
  boardTimeline().appendTo( &mTextColor, getTextColor(), 0.2f, EaseInOutQuint() )
  .delay( 0.18f );
}

void LetterView::draw()
{
  gl::pushModelView();
  gl::translate( mLoc );
  gl::rotate( mTileRotation );
  gl::color( mColor );
  mBacking.draw();
//  sprite_sheet->draw( "tile-backing", mBounds.getCenter() );
  gl::rotate( getRotation() * 180 / M_PI );
	gl::color( mTextColor() );
  mGlyphSprite.setRegistrationPoint( mGlyphSprite.getSize() / 2 - mOffset() );
  mGlyphSprite.clipBy( mBounds );
  mGlyphSprite.draw();
//  sprite_sheet->drawInRect( mGlyph, mBounds.getCenter() + mOffset, mBounds );
  gl::popModelView();
}

ci::Timeline& LetterView::timeline()
{
  static std::shared_ptr<Timeline>	timeline( Timeline::create() );
  return *timeline;
}

map<char, pk::SpriteData>& LetterView::letterSprites()
{
  static map<char, pk::SpriteData> sMap;
  return sMap;
}
