//
//  SwapStation.cpp
//  WordShift
//
//  Created by David Wicks on 4/17/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SwapStation.h"
#include "SoundStation.h"
#include "LetterView.h"
#include "cinder/Timeline.h"

using namespace cascade;

SwapStation::SwapStation()
{}

SwapStation::~SwapStation()
{}

void SwapStation::performSwaps(const std::vector<Swap> &swaps, bool user_initiated)
{
  // always start from zero
  if( LetterView::timeline().empty() ){ LetterView::timeline().stepTo( 0 ); }
  // if there are swaps in progress, we should start after they are finished
  float start_offset = std::max( LetterView::timeline().getEndTime() - LetterView::timeline().getCurrentTime(), 0.0f );
  // if there was simply a twitch that resulted in a delay between touchesEnded events, don't delay
  if( LetterView::timeline().getCurrentTime() < 1.0f / 30.0f ){ start_offset = 0; }
  for( const Swap &swap : swaps )
  {
    swap( start_offset );
  }
  // inform listeners
  mSwapMessenger.deliver( SwapMessage( swaps.size(), user_initiated ) );
}

SwapStation::Swap SwapStation::createSwap(LetterRef lhs, LetterRef rhs, float delay)
{
  return [=](float start_offset){
    lhs->swapWith( rhs, start_offset + delay );
    Sound().cueSound( SoundStation::eTileSwap, start_offset + delay );
  };
}

SwapStation::Swap SwapStation::createSwap(LetterRef letter, char glyph, float theta, float delay)
{
  return [=](float start_offset){
    letter->setChar( glyph, theta, start_offset + delay );
    Sound().cueSound( SoundStation::eTileSwap, start_offset + delay );
  };
}

void SwapStation::setPendingUserSwaps(int32_t swaps)
{
  mPendingMessenger.deliver( PendingSwapMessage{ swaps } );
}

SwapStation& SwapStation::instance()
{
  static SwapStation sStation;
  return sStation;
}

