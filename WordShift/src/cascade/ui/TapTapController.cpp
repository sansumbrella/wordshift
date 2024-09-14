//
//  TapTapController.cpp
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TapTapController.h"
#include "CollectionUtilities.hpp"
#include "SwapStation.h"

using namespace cascade;
using namespace ci;
using namespace std;
using namespace pockets;

TapTapController::TapTapController()
{}

TapTapController::~TapTapController()
{}

void TapTapController::updateWithLetter(LetterRef letter, bool canUseLetter )
{
  if( !containsLetter( letter ) && mLastLetter != letter )
  {
    if( canUseLetter )
    {
      if( mLetters.empty() || letterIsNeighborly( letter ) )
      {
        letter->setSelected( true );
        mLetters.push_back( letter );
      }
    }
    else if ( letterWillSwap( letter ) )
    {
      letter->setSelected( true );
      mLetters.push_back( letter );
    }
  }
  else
  {
    removeLetter( letter );
  }
  mLastLetter.reset();
}

bool TapTapController::letterIsNeighborly(LetterRef letter) const
{
  for( LetterRef l : mLetters )
  {
    if( l->isNeighbor( letter ) )
    {
      return true;
    }
  }
  for( LetterRef l : sharedNeighbors() )
  {
    if( letter->isNeighbor( l ) )
    {
      return true;
    }
  }
  return false;
}

bool TapTapController::letterWillSwap( LetterRef letter ) const
{
  for( LetterRef l : mLetters )
  {
    if( l->isNeighbor( letter ) )
    {
      return true;
    }
  }
  return false;
}

vector<LetterRef> TapTapController::sharedNeighbors() const
{
  vector<LetterRef> neighbors;
  auto iter = mLetters.begin();
  if( iter != mLetters.end() )
  {
    neighbors = (*iter)->getNeighbors();
    auto null_ptr = [](LetterRef letter){ return !letter; };
    vector_erase_if( &neighbors, null_ptr );
    while( ++iter != mLetters.end() )
    {
      vector<LetterRef> other_neighbors = (*iter)->getNeighbors();
      auto neg_compare = [=](LetterRef letter){ return !vector_contains(other_neighbors, letter); };
      vector_erase_if( &neighbors, neg_compare );
    }
  }
  return neighbors;
}

bool TapTapController::containsLetter( LetterRef letter ) const
{
  return std::find( mLetters.begin(), mLetters.end(), letter ) != mLetters.end();
}

void TapTapController::removeLetter( LetterRef letter )
{
  if( containsLetter( letter ) )
  {
    letter->setSelected( false );
    vector_remove( &mLetters, letter );
    mLastLetter = letter;
  }
}

bool TapTapController::apply()
{
  float delay = 0.0f;
  vector<SwapStation::Swap> swaps;
  for( int i = mLetters.size() - 1; i > 0; --i )
  {
    for( int j = i - 1; j >= 0; --j )
    {
      if( mLetters.at( i )->isNeighbor( mLetters.at( j ) ) )
      {
        swaps.push_back( SwapStation::createSwap( mLetters[i], mLetters[j], delay ) );
        float inc = math<float>::clamp( lmap( delay, 0.0f, 0.32f, 0.084f, 0.05f ), 0.05f, 0.084f );
        delay += inc;
      }
    }
  }
  if( !swaps.empty() )
  {
    SwapStation::instance().performSwaps( swaps, true );
    clear();
  }
  return !swaps.empty();
}

void TapTapController::clear()
{
  for( LetterRef letter : mLetters )
  {
    letter->setSelected( false );
  }
  mLetters.clear();
  mLastLetter.reset();
}

