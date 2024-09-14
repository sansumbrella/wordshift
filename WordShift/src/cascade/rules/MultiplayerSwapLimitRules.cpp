//
//  MultiplayerSwapLimitRules.cpp
//  WordShift
//
//  Created by David Wicks on 5/6/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "MultiplayerSwapLimitRules.h"
#include "GameColor.h"

using namespace cascade;
using namespace cinder;
using namespace pockets;
using namespace std;

MultiplayerSwapLimitRules::MultiplayerSwapLimitRules( const pk::SpriteSheet &sprite_sheet, int32_t swaps_per_turn, int32_t base_turns ):
PlayRules( "Strategy Mode" )
, mSwapsRemainingThisTurn( 0 )
, mSwapsRemaining( swaps_per_turn * base_turns )
, mSwapsAllowedPerTurn( swaps_per_turn )
, mSwapOutline( sprite_sheet.getSprite( "swap-border" ) )
, mTurnBorder( sprite_sheet.getSprite( "turn-border" ) )
, mTurnBorderSingular( sprite_sheet.getSprite( "turn-singular-border" ) )
{
  // multiplayer games get fewer bonus moves than single player
  // mostly so they end faster, in part since you can set up other players
  mWordLengthBonuses[3] = 0;
  mWordLengthBonuses[4] = 0;
  mWordLengthBonuses[5] = 0;
  mWordLengthBonuses[6] = 1;
  mWordLengthBonuses[7] = 2;
  mWordLengthBonuses[8] = 3;
  mWordLengthBonuses[9] = 4;

  for( int i = 0; i < 10; ++i )
  {
    mNumerals[i] = sprite_sheet.getSprite( toString( i ) );
    mNumerals[i].setRegistrationPoint( Vec2f(mNumerals[i].getSize()) * Vec2f( 0, 0.5f ) );  // draw around its center
    mLargeNumerals[i] = sprite_sheet.getSprite( "g" + toString( i ) );
    mLargeNumerals[i].setRegistrationPoint( mLargeNumerals[i].getSize() / 2 );
  }
}

MultiplayerSwapLimitRules::~MultiplayerSwapLimitRules()
{}

void MultiplayerSwapLimitRules::beginTurn()
{
  mSwapsRemainingThisTurn = math<int32_t>::min( mSwapsRemaining, mSwapsAllowedPerTurn );
  SwapStation::instance().appendSwapReceiver( this );
  SwapStation::instance().appendPendingReceiver( this );
}

void MultiplayerSwapLimitRules::endTurn()
{
  SwapStation::instance().removeSwapReceiver( this );
  SwapStation::instance().removePendingReceiver( this );
}

void MultiplayerSwapLimitRules::receive(const cascade::SwapMessage &msg)
{
  if( msg.userInitiated() )
  {
    mSwapsRemainingThisTurn -= msg.getNumSwaps();
    mSwapsRemaining -= msg.getNumSwaps();
    mSwapsMade += msg.getNumSwaps();
    assert( mSwapsRemainingThisTurn >= 0 );
  }
}

void MultiplayerSwapLimitRules::receive(const cascade::PendingSwapMessage &msg)
{
  mSwapsUnderConsideration = msg.getNumSwaps();
}

bool MultiplayerSwapLimitRules::swapAllowed() const
{
  return mSwapsUnderConsideration < mSwapsRemainingThisTurn;
}

void MultiplayerSwapLimitRules::assignCredit(int wordLength)
{
  if( !turnOver() )
  {
    mSwapsRemainingThisTurn += mWordLengthBonuses[wordLength];
    mSwapsRemaining += mWordLengthBonuses[wordLength];
  }
}

int MultiplayerSwapLimitRules::turnsRemaining() const
{
  if( !endReached() )
  {
    return math<float>::ceil( static_cast<float>(mSwapsRemaining) / mSwapsAllowedPerTurn );
  }
  return 0;
}

void MultiplayerSwapLimitRules::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );
  setColor( getAltTextColor() );
  mSwapOutline.draw();

  // swaps left this turn
  const int swaps_remaining = mSwapsRemainingThisTurn - mSwapsUnderConsideration;
  if( swaps_remaining < 3 && !turnOver() )
  {
    setColor( getWarningColor() );
  }

  if( swaps_remaining > 9 )
  {
    int q = swaps_remaining;
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
    mLargeNumerals[swaps_remaining].draw();
  }

  // Draw Turns Remaining
  setColor( getAltTextColor() );
  auto turn_border = (turnsRemaining() != 1)
  ? mTurnBorder
  : mTurnBorderSingular;
  Vec2f loc( -turn_border.getSize().x, 0 );
  gl::pushModelView();
  gl::translate( loc );
  turn_border.draw();
  mLargeNumerals[turnsRemaining()].draw();
  gl::popModelView();

  gl::popModelView();
}


