//
//  HardRules.h
//  WordShift
//
//  Created by David Wicks on 6/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "PlayRules.h"
#include "SwapStation.h"
#include "cinder/Tween.h"

namespace cascade
{
  class HardRules : public PlayRules, public Receiver<SwapMessage>, public Receiver<PendingSwapMessage>
  {
  public:
    HardRules( const pk::SpriteSheet &sprite_sheet, float duration, int swaps );
    ~HardRules();
    void            draw();
    bool            endReached() const { return (mSecondsRemaining <= 0) || (mSwapsRemaining <= 0); }
    int             quantityRemaining() const;
    void            evaluateEnd( float deltaTime );
    void            assignCredit( int wordLength );
    void            addTime( float seconds );
    float           visibleTime() const;
    std::string     getPlayStats() const override;
    uint64_t        getScoreboardContext() const override { return mSecondsPlayed; }
    //! start listening for swaps
    void            beginTurn();
    //! stop listening to swaps
    void            endTurn();
    //! keep track of user-initiated swaps
    void receive( const SwapMessage &msg );
    void receive( const PendingSwapMessage &msg );
    bool            swapAllowed() const;
  private:
    float                         mSecondsRemaining;
    ci::Anim<float>               mEasedSecondsRemaining;
    float                         mSecondsPlayed = 0; // for game end stats
    float                         mSecondsCreditGiven = 0; // keep track of credit for animation
    int32_t                       mSwapsRemaining;
    int32_t                       mSwapsMade = 0;
    mutable int32_t               mSwapsUnderConsideration = 0;
    std::map<int32_t, float>      mWordLengthTimeBonuses;
    std::map<int32_t, int32_t>    mWordLengthSwapBonuses;
    pk::Sprite                    mClockOutline;
    pk::Sprite                    mSwapOutline;
    pk::SpriteData                mPieFilling;
    std::map<int32_t, pk::Sprite> mNumerals;
    std::map<int32_t, pk::Sprite> mLargeNumerals;
    float                         mTileWidth;
  };
}
