//
//  SwapStation.h
//  WordShift
//
//  Created by David Wicks on 4/17/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Messenger.hpp"
#include "LetterModel.h"

namespace cascade
{
  class SwapMessage
  {
  public:
    SwapMessage( int32_t num_swaps, bool user_initiated ):
    mNumSwaps( num_swaps )
    , mUserInitiated( user_initiated )
    {}
    int32_t getNumSwaps() const { return mNumSwaps; }
    bool    userInitiated() const { return mUserInitiated; }
  private:
    int32_t mNumSwaps;
    bool    mUserInitiated;
  };

  class PendingSwapMessage
  {
  public:
    explicit PendingSwapMessage( int32_t swaps ):
    mNumSwaps( swaps )
    {}
    int32_t getNumSwaps() const { return mNumSwaps; }
  private:
    int32_t mNumSwaps;
  };

  /**
  Central dispatch location for tile swaps.
  Register here to know when swaps occur.
  */

  class SwapStation
  {
  public:
    typedef std::function<void (float)> Swap;
    ~SwapStation();
    //! perform a series of swaps and notify listeners
    void performSwaps( const std::vector<Swap> &swaps, bool user_initiated );
    //! register receiver to hear when swaps are performed
    void appendSwapReceiver( Receiver<SwapMessage> *receiver ){ mSwapMessenger.appendReceiver( receiver ); }
    void removeSwapReceiver( Receiver<SwapMessage> *receiver ){ mSwapMessenger.removeReceiver( receiver ); }
    void appendPendingReceiver( Receiver<PendingSwapMessage> *receiver ){ mPendingMessenger.appendReceiver( receiver ); }
    void removePendingReceiver( Receiver<PendingSwapMessage> *receiver ){ mPendingMessenger.removeReceiver( receiver ); }
    void setPendingUserSwaps( int32_t swaps );
    //! returns a swap object
    static Swap createSwap( LetterRef lhs, LetterRef rhs, float delay );
    static Swap createSwap( LetterRef letter, char glyph, float theta, float delay );
    static SwapStation& instance();
  private:
    SwapStation();
    Messenger<SwapMessage>        mSwapMessenger;
    Messenger<PendingSwapMessage> mPendingMessenger;
  };
}
