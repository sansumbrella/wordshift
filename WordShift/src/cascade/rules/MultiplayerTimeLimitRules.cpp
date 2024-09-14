//
//  MultiplayerTimeLimitRules.cpp
//  WordShift
//
//  Created by David Wicks on 5/5/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "MultiplayerTimeLimitRules.h"
#include "GameColor.h"
#include "Drawing.h"
#include "BoardTimeline.h"

using namespace cascade;
using namespace cinder;
using namespace std;

MultiplayerTimeLimitRules::MultiplayerTimeLimitRules( const pk::SpriteSheet &sprite_sheet, float seconds_per_turn, int num_turns ):
PlayRules( "Speed Mode" )
, mSecondsRemaining( seconds_per_turn * num_turns )
, mSecondsPerTurn( seconds_per_turn )
, mEasedTimeRemaining( seconds_per_turn )
, mClockOutline( sprite_sheet.getSprite( "clock-border" ) )
, mPieFilling( sprite_sheet.getSpriteData( "pie-fill" ) )
, mTurnBorder( sprite_sheet.getSprite( "turn-border" ) )
, mTurnBorderSingular( sprite_sheet.getSprite( "turn-singular-border" ) )
{
  mWordLengthBonuses[3] = 0.0f;
  mWordLengthBonuses[4] = 0.0f;
  mWordLengthBonuses[5] = 0.0f;
  mWordLengthBonuses[6] = 2.0f;
  mWordLengthBonuses[7] = 3.0f;
  mWordLengthBonuses[8] = 4.0f;
  mWordLengthBonuses[9] = 5.0f;

  for( int i = 0; i < 10; ++i )
  {
    mNumerals[i] = sprite_sheet.getSprite( "g" + toString( i ) );
    mNumerals[i].setRegistrationPoint( mNumerals[i].getSize() / 2 );  // draw around its center
  }
}

MultiplayerTimeLimitRules::~MultiplayerTimeLimitRules()
{}

void MultiplayerTimeLimitRules::beginTurn()
{
  mSecondsRemainingThisTurn = math<float>::min( mSecondsRemaining, mSecondsPerTurn );
  mEasedTimeRemaining = mSecondsRemainingThisTurn;
}

void MultiplayerTimeLimitRules::evaluateEnd( float deltaTime )
{
  mSecondsRemaining = math<float>::max( mSecondsRemaining - deltaTime, 0 );
  mSecondsRemainingThisTurn = math<float>::max( mSecondsRemainingThisTurn - deltaTime, 0 );
  mSecondsPlayed += deltaTime;
  mCreditGiven = 0;
}

void MultiplayerTimeLimitRules::assignCredit(int wordLength)
{
  if( !turnOver() )
  {
    addTime( mWordLengthBonuses[wordLength] );
  }
}

void MultiplayerTimeLimitRules::addTime(float seconds)
{
  if( mEasedTimeRemaining.isComplete() )
  {
    mEasedTimeRemaining = mSecondsRemainingThisTurn;
  }
  mSecondsRemainingThisTurn += seconds;
  mSecondsRemaining += seconds;
  mCreditGiven += seconds;
  boardTimeline().apply( &mEasedTimeRemaining, mSecondsRemainingThisTurn, mCreditGiven * 0.1f, EaseInOutQuad() );
}

int MultiplayerTimeLimitRules::turnsRemaining() const
{
  if( !endReached() )
  {
    return math<float>::ceil( mSecondsRemaining / mSecondsPerTurn );
  }
  return 0;
}

float MultiplayerTimeLimitRules::visibleTime() const
{
  float t = math<float>::min( mSecondsRemainingThisTurn, mEasedTimeRemaining() );
  return math<float>::clamp( t / mSecondsPerTurn );
}

void MultiplayerTimeLimitRules::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  // Draw Time
  gl::color( getAltTextColor() );
  mClockOutline.draw();
  if( mSecondsRemainingThisTurn < mSecondsPerTurn / 2 )
  {
    setColor( getWarningColor() );
  }

  drawPie(  mPieFilling
          , visibleTime()
          , math<float>::ceil( mSecondsRemainingThisTurn * 3 + 1 ) );
  if( mSecondsRemainingThisTurn < 5.0f )
  { // add numeral at very end, since wedge is hard to see then
    gl::color( getAltTextColor() );
    int seconds = math<float>::ceil(mSecondsRemainingThisTurn);
    mNumerals[seconds].draw();
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
  mNumerals[turnsRemaining()].draw();
  gl::popModelView();

  gl::popModelView();
}
