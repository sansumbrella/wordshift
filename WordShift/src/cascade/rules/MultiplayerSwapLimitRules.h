//
//  MultiplayerSwapLimitRules.h
//  WordShift
//
//  Created by David Wicks on 5/6/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "PlayRules.h"
#include "SwapStation.h"

namespace cascade
{
  typedef std::shared_ptr<class MultiplayerSwapLimitRules> MultiplayerSwapLimitRulesRef;
  class MultiplayerSwapLimitRules : public PlayRules, public Receiver<SwapMessage>, public Receiver<PendingSwapMessage>
  {
  public:
    MultiplayerSwapLimitRules( const pk::SpriteSheet &sprite_sheet, int32_t swaps_per_turn, int32_t base_turns );
    ~MultiplayerSwapLimitRules();
    void  draw();
    bool  endReached() const { return mSwapsRemaining <= 0; }
    bool  turnOver() const { return mSwapsRemainingThisTurn <= 0; }
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
    int  turnsRemaining() const;
    std::string getPlayStats() const { return ""; }
  private:
    int32_t       	mSwapsRemaining;
    int32_t         mSwapsRemainingThisTurn;
    int32_t         mSwapsAllowedPerTurn;
    int32_t         mSwapsMade = 0; // nice stat for later
    mutable int32_t mSwapsUnderConsideration = 0;
    std::map<int32_t, int32_t>  mWordLengthBonuses;
    pk::Sprite        mSwapOutline;
    pk::Sprite        mTurnBorder;
    pk::Sprite        mTurnBorderSingular;
    std::map<int32_t, pk::Sprite> mNumerals;
    std::map<int32_t, pk::Sprite> mLargeNumerals;
  };
}
