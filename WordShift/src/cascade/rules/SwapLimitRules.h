//
//  SwapLimitRules.h
//  WordShift
//
//  Created by David Wicks on 3/19/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "PlayRules.h"
#include "SwapStation.h"

namespace cascade
{
  typedef std::shared_ptr<class SwapLimitRules> SwapLimitRulesRef;
  class SwapLimitRules : public PlayRules, public Receiver<SwapMessage>, public Receiver<PendingSwapMessage>
  {
  public:
    SwapLimitRules( const pk::SpriteSheet &sprite_sheet, int32_t max_swaps );
    ~SwapLimitRules();
    void  draw();
    bool  endReached() const { return mSwapsRemaining <= 0; }
    int   quantityRemaining() const { return mSwapsRemaining - mSwapsUnderConsideration; }
    //! keep track of user-initiated swaps
    void receive( const SwapMessage &msg );
    void receive( const PendingSwapMessage &msg );
    //! award more swaps when words are played
    void assignCredit( int wordLength );
    //! start listening for swaps
    void beginTurn();
    //! stop listening to swaps
    void endTurn();
    //! swaps are allowed if we have some remaining
    bool swapAllowed() const;
    std::string getPlayStats() const;
    virtual uint64_t        getScoreboardContext() const { return mSwapsMade; }
  private:
    int32_t           mSwapsRemaining;
    int32_t         	mSwapsMade = 0;
    mutable int32_t   mSwapsUnderConsideration = 0;
    std::map<int32_t, int32_t>  mWordLengthBonuses;
    pk::Sprite        mSwapOutline;
    std::map<int32_t, pk::Sprite> mNumerals;
    std::map<int32_t, pk::Sprite> mLargeNumerals;
  };
}
