//
//  MultiplayerTimeLimitRules.h
//  WordShift
//
//  Created by David Wicks on 5/5/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "PlayRules.h"
#include "cinder/Tween.h"

namespace cascade
{
  typedef std::shared_ptr<class MultiplayerTimeLimitRules> MultiplayerTimeLimitRulesRef;
  class MultiplayerTimeLimitRules : public PlayRules
  {
  public:
    MultiplayerTimeLimitRules( const pk::SpriteSheet &sprite_sheet, float seconds_per_turn, int num_turns );
    ~MultiplayerTimeLimitRules();
    void  draw();
    bool  endReached() const { return mSecondsRemaining <= 0; }
    bool  turnOver() const { return mSecondsRemainingThisTurn <= 0; }
    //! return current turns time so we get warning at end of every turn, don't report at zero
    int   quantityRemaining() const { return ci::math<int>::max( ci::math<float>::ceil( mSecondsRemainingThisTurn ), 1 ); }
    void  evaluateEnd( float deltaTime );
    //! award more time for this turn when words are played
    void  assignCredit( int wordLength );
    //! use some bonus time
    void  addTime( float seconds );
    //! amount of time displayed (eases up to higher values)
    float visibleTime() const;
    //! reset seconds per turn
    void  beginTurn();
    int   turnsRemaining() const;
    std::string getPlayStats() const { return ""; }
  private:
    float                     mSecondsRemaining;
    float                     mSecondsRemainingThisTurn = 0;
    float                     mSecondsPerTurn;
    float                     mSecondsPlayed = 0; // for game end stats
    ci::Anim<float>           mEasedTimeRemaining;
    float                     mCreditGiven = 0; // keep track of credit for animation
    std::map<int32_t, float>  mWordLengthBonuses;
    pk::Sprite                mClockOutline;
    pk::Sprite                mTurnBorder;
    pk::Sprite                mTurnBorderSingular;
    pk::SpriteData                mPieFilling;
    std::map<int32_t, pk::Sprite> mNumerals;
  };
}
