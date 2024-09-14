//
//  HardRules.cpp
//  WordShift
//
//  Created by David Wicks on 6/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "HardRules.h"
#include "GameColor.h"
#include "BoardTimeline.h"
#include "Drawing.h"

using namespace cascade;
using namespace cinder;
using namespace pockets;
using namespace std;

HardRules::HardRules( const SpriteSheet &sprite_sheet, float duration, int swaps ):
PlayRules( "Hard" ),
mSecondsRemaining( duration ),
mEasedSecondsRemaining( duration ),
mSwapsRemaining( swaps ),
mSwapOutline( sprite_sheet.getSprite( "swap-border" ) ),
mClockOutline( sprite_sheet.getSprite( "clock-border" ) ),
mPieFilling( sprite_sheet.getSpriteData( "pie-fill" ) ),
mTileWidth( sprite_sheet.getSpriteData( "turn-border" ).getSize().x )
{ // time bonuses
  mWordLengthTimeBonuses[3] = 0.0f;
  mWordLengthTimeBonuses[4] = 1.25f;  // 0.3125
  mWordLengthTimeBonuses[5] = 4.0f;   // 0.8  + 0
  mWordLengthTimeBonuses[6] = 10.0f;  // 1.5  + 1
  mWordLengthTimeBonuses[7] = 16.0f;  // 2    + 2
  mWordLengthTimeBonuses[8] = 24.0f;  // 3    + 0
  mWordLengthTimeBonuses[9] = 30.0f;  // 3    + 3
  // swap bonuses
  mWordLengthSwapBonuses[3] = 0;
  mWordLengthSwapBonuses[4] = 1;  // 0.25
  mWordLengthSwapBonuses[5] = 2;  // 0.5
  mWordLengthSwapBonuses[6] = 6;  // 1
  mWordLengthSwapBonuses[7] = 11; // 1.5
  mWordLengthSwapBonuses[8] = 17; // 2 + 1
  mWordLengthSwapBonuses[9] = 30; // 3 + 3

  setScoreboardId( "wordshift.hard_mode_score" );
  setScoreAchievementPrefix( "wordshift.hard.score." );

  for( int i = 0; i < 10; ++i )
  {
    mNumerals[i] = sprite_sheet.getSprite( toString( i ) );
    mNumerals[i].setRegistrationPoint( Vec2f(mNumerals[i].getSize()) * Vec2f( 0, 0.5f ) );  // draw around its center
    mLargeNumerals[i] = sprite_sheet.getSprite( "g" + toString( i ) );
    mLargeNumerals[i].setRegistrationPoint( mLargeNumerals[i].getSize() / 2 );
  }
}

HardRules::~HardRules()
{}

void HardRules::beginTurn()
{
  SwapStation::instance().appendSwapReceiver( this );
  SwapStation::instance().appendPendingReceiver( this );
}

void HardRules::endTurn()
{
  SwapStation::instance().removeSwapReceiver( this );
  SwapStation::instance().removePendingReceiver( this );
}

string HardRules::getPlayStats() const
{
  if( endReached() )
  {
    if( mSecondsPlayed > 90.99f )
    {
      int seconds = fmodf( mSecondsPlayed, 60 );
      int minutes = mSecondsPlayed / 60;
      string second_string = toString(seconds);
      if( second_string.size() == 1 ){ second_string = "0" + second_string; }
      string time_string = toString( minutes ) + ":" + second_string;
      return toString(mSwapsMade) + " swaps, " + time_string + " played";
    }
    return toString(mSwapsMade) + " swaps, " + toString( static_cast<int>(mSecondsPlayed) ) + " seconds played";
  }
  return toString(mSwapsRemaining) + " swaps, " + toString( static_cast<int>(mSecondsRemaining) ) + " seconds left";
}

void HardRules::receive(const cascade::SwapMessage &msg)
{
  if( msg.userInitiated() )
  {
    mSwapsMade += msg.getNumSwaps();
    mSwapsRemaining -= msg.getNumSwaps();
  }
}

void HardRules::receive(const cascade::PendingSwapMessage &msg)
{
  mSwapsUnderConsideration = msg.getNumSwaps();
}

bool HardRules::swapAllowed() const
{
  return mSwapsUnderConsideration < mSwapsRemaining;
}

void HardRules::assignCredit( int wordLength )
{
  if( !endReached() )
  {
    addTime( mWordLengthTimeBonuses[wordLength] );
    mSwapsRemaining += mWordLengthSwapBonuses[wordLength];
  }
}

void HardRules::evaluateEnd( float deltaTime )
{
  mSecondsRemaining = math<float>::max( mSecondsRemaining - deltaTime, 0 );
  mSecondsPlayed += deltaTime;
  mSecondsCreditGiven = 0;
}

int HardRules::quantityRemaining() const
{
  return math<int>::min( math<float>::ceil( mSecondsRemaining ), (mSwapsRemaining - mSwapsUnderConsideration) );
}

void HardRules::addTime( float seconds )
{
  if( mEasedSecondsRemaining.isComplete() )
  {
    mEasedSecondsRemaining = mSecondsRemaining;
  }
  mSecondsRemaining += seconds;
  mSecondsCreditGiven += seconds;
  boardTimeline().apply( &mEasedSecondsRemaining, mSecondsRemaining, mSecondsCreditGiven * 0.1f, EaseInOutQuad() );
}

float HardRules::visibleTime() const
{
  float t = math<float>::min( mSecondsRemaining, mEasedSecondsRemaining() );
  return math<float>::clamp( t / 60.0f );
}

void HardRules::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  // Draw Time Information
  gl::color( getAltTextColor() );
  mClockOutline.draw();
  if( mSecondsRemaining < 15.0f )
  {
    gl::color( getWarningColor() );
  }
  drawPie( mPieFilling
    , visibleTime()
    , math<float>::ceil( mSecondsRemaining + 1 ) );
  if( mSecondsRemaining < 5.0f )
  { // add numeral at very end, since wedge is hard to see then
    gl::color( getAltTextColor() );
    int seconds = math<float>::ceil(mSecondsRemaining);
    mLargeNumerals[seconds].draw();
  }
  // Draw Swap Information
  gl::translate( Vec2f{ -mTileWidth, 0 } );
  gl::color( getAltTextColor() );
  mSwapOutline.draw();

  int q = mSwapsRemaining - mSwapsUnderConsideration;
  if( q < 10 )
  {
    gl::color( getWarningColor() );
  }
  if( q > 9 )
  {
    float total_width = 0;
    vector<Sprite> sprites;
    while( q > 0 )
    { // start with the ones place, and move left each step
      auto sprite = mNumerals[q % 10];
      q /= 10;
      sprites.push_back( sprite );
      total_width += sprite.getSize().x;
    }
    Vec2f pos( total_width / sprites.size() - total_width / 2, 0 );
    for( auto &sprite : sprites )
    {
      sprite.setRegistrationPoint( sprite.getRegistrationPoint() + pos );
      pos.x += sprite.getSize().x;
    }
    for( auto &sprite : sprites )
    {
      sprite.draw();
    }
  }
  else
  {
    mLargeNumerals[q].draw();
  }

  gl::popModelView();
}
