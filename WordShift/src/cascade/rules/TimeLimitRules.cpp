//
//  TimeLimitRules.cpp
//  WordShift
//
//  Created by David Wicks on 2/18/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TimeLimitRules.h"
#include "GameColor.h"
#include "Drawing.h"
#include "BoardTimeline.h"

using namespace cascade;
using namespace cinder;
using namespace pockets;
using namespace std;

TimeLimitRules::TimeLimitRules( const pk::SpriteSheet &sprite_sheet, float duration ):
PlayRules( "Speed Mode" )
, mTimeRemaining( duration )
, mEasedTimeRemaining( duration )
, mClockOutline( sprite_sheet.getSprite( "clock-border" ) )
, mPieFilling( sprite_sheet.getSpriteData( "pie-fill" ) )
{
  setScoreboardId( "wordshift.speed_mode_score" );
  setScoreAchievementPrefix( "wordshift.speed.score." );
  mWordLengthBonuses[3] = 0.0f;
  mWordLengthBonuses[4] = 1.25f;  // 0.3125
  mWordLengthBonuses[5] = 4.0f;   // 0.8  + 0
  mWordLengthBonuses[6] = 10.0f;  // 1.5  + 1
  mWordLengthBonuses[7] = 16.0f;  // 2    + 2
  mWordLengthBonuses[8] = 24.0f;  // 3    + 0
  mWordLengthBonuses[9] = 30.0f;  // 3    + 3
  // DON'T CHANGE BONUSES, YOU GOT 16K WITH HARD WORK AND IT FELT AWESOME
  // CONSIDER ADDING A 20K OR 25K ACHIEVEMENT LATER

  for( int i = 0; i < 6; ++i )
  {
    mNumerals[i] = sprite_sheet.getSprite( "g" + toString( i ) );
    mNumerals[i].setRegistrationPoint( mNumerals[i].getSize() / 2 );  // draw around its center
  }
}

TimeLimitRules::~TimeLimitRules()
{}

string TimeLimitRules::getPlayStats() const
{
  if( endReached() )
  {
    if( mSecondsPlayed > 90.99f )
    {
      int seconds = fmodf( mSecondsPlayed, 60 );
      int minutes = mSecondsPlayed / 60;
      string second_string = toString(seconds);
      if( second_string.size() == 1 ){ second_string = "0" + second_string; }
      return toString( minutes ) + ":" + second_string + " played";
    }
    return toString( static_cast<int>(mSecondsPlayed) ) + " seconds played";
  }
  return toString( static_cast<int>(mTimeRemaining) ) + " seconds left";
}

void TimeLimitRules::assignCredit(int wordLength)
{
  if( !endReached() )
  {
    addTime( mWordLengthBonuses[wordLength] );
  }
}

void TimeLimitRules::evaluateEnd( float deltaTime )
{
  mTimeRemaining = math<float>::max( mTimeRemaining - deltaTime, 0 );
  mSecondsPlayed += deltaTime;
  mCreditGiven = 0;
}

int TimeLimitRules::quantityRemaining() const
{
  return math<float>::ceil( mTimeRemaining );
}

void TimeLimitRules::addTime(float seconds)
{
  if( mEasedTimeRemaining.isComplete() )
  {
    mEasedTimeRemaining = mTimeRemaining;
  }
  mTimeRemaining += seconds;
  mCreditGiven += seconds;
  boardTimeline().apply( &mEasedTimeRemaining, mTimeRemaining, mCreditGiven * 0.1f, EaseInOutQuad() );
}

float TimeLimitRules::visibleTime() const
{
  float t = math<float>::min( mTimeRemaining, mEasedTimeRemaining() );
  return math<float>::clamp( t / 60.0f );
}

void TimeLimitRules::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  gl::color( getAltTextColor() );
  mClockOutline.draw();
  if( mTimeRemaining < 15.0f )
  {
    gl::color( getWarningColor() );
  }

  drawPie(  mPieFilling
          , visibleTime()
          , math<float>::ceil( mTimeRemaining + 1 ) );
  if( mTimeRemaining < 5.0f )
  { // add numeral at very end, since wedge is hard to see then
    gl::color( getAltTextColor() );
    int seconds = math<float>::ceil(mTimeRemaining);
    mNumerals[seconds].draw();
  }
  gl::popModelView();
}

