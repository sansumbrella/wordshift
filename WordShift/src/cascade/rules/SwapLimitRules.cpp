//
//  SwapLimitRules.cpp
//  WordShift
//
//  Created by David Wicks on 3/19/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SwapLimitRules.h"
#include "GameColor.h"

using namespace cascade;
using namespace cinder;
using namespace pockets;
using namespace std;

SwapLimitRules::SwapLimitRules( const SpriteSheet &sprite_sheet, int32_t max_swaps ):
PlayRules( "Strategy Mode" )
, mSwapsRemaining( max_swaps )
, mSwapOutline( sprite_sheet.getSprite( "swap-border" ) )
{
  setScoreboardId( "wordshift.strategy_mode_score" );
  setScoreAchievementPrefix( "wordshift.strategy.score." );
  // single player games get bigger move bonuses for long words
  // hopefully, this takes some of the fear out of moving to make words
  mWordLengthBonuses[3] = 0;
  mWordLengthBonuses[4] = 1;  // 0.25
  mWordLengthBonuses[5] = 2;  // 0.5
  mWordLengthBonuses[6] = 6;  // 1
  mWordLengthBonuses[7] = 11; // 1.5
  mWordLengthBonuses[8] = 17; // 2 + 1
  mWordLengthBonuses[9] = 30; // 3 + 3

  for( int i = 0; i < 10; ++i )
  {
    mNumerals[i] = sprite_sheet.getSprite( toString( i ) );
    mNumerals[i].setRegistrationPoint( Vec2f(mNumerals[i].getSize()) * Vec2f( 0, 0.5f ) );  // draw around its center
    mLargeNumerals[i] = sprite_sheet.getSprite( "g" + toString( i ) );
    mLargeNumerals[i].setRegistrationPoint( mLargeNumerals[i].getSize() / 2 );
  }
}

SwapLimitRules::~SwapLimitRules()
{}

string SwapLimitRules::getPlayStats() const
{
  if( endReached() )
  {
    return toString( mSwapsMade ) + " swaps made";
  }
  return toString( mSwapsRemaining ) + " swaps left";
}

void SwapLimitRules::beginTurn()
{
  SwapStation::instance().appendSwapReceiver( this );
  SwapStation::instance().appendPendingReceiver( this );
}

void SwapLimitRules::endTurn()
{
  SwapStation::instance().removeSwapReceiver( this );
  SwapStation::instance().removePendingReceiver( this );
}

void SwapLimitRules::receive(const cascade::SwapMessage &msg)
{
  if( msg.userInitiated() )
  {
    mSwapsMade += msg.getNumSwaps();
    mSwapsRemaining -= msg.getNumSwaps();
  }
}

void SwapLimitRules::receive(const cascade::PendingSwapMessage &msg)
{
  mSwapsUnderConsideration = msg.getNumSwaps();
}

bool SwapLimitRules::swapAllowed() const
{
  return mSwapsUnderConsideration < mSwapsRemaining;
}

void SwapLimitRules::assignCredit(int wordLength)
{
  if( !endReached() )
  {
    mSwapsRemaining += mWordLengthBonuses[wordLength];
  }
}

void SwapLimitRules::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );
  gl::color( getAltTextColor() );
  mSwapOutline.draw();

  if( quantityRemaining() < 10 )
  {
    gl::color( getWarningColor() );
  }

  int q = quantityRemaining();
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
    mLargeNumerals[quantityRemaining()].draw();
  }
  gl::popModelView();
}


