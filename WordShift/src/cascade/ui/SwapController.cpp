//
//  SwapController.cpp
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SwapController.h"
#include "SwapStation.h"

using namespace cascade;
using namespace ci;
using namespace std;

SwapController::SwapController()
{}

SwapController::~SwapController()
{
  deliver( Message( *this, DESTRUCTED ) );
}

bool SwapController::apply()
{
  for( LetterRef l : mLetters )
  {
    l->setSelected( false );
  }
  if( mLetters.size() > 1 )
  {
    vector<SwapStation::Swap> swaps;
    auto current = mLetters.begin();
    auto next = current + 1;
    float delay = 0.0f;
    while( next != mLetters.end() )
    {
      swaps.push_back( SwapStation::createSwap( *current, *next, delay ) );
      current++;
      next++;
      float inc = math<float>::clamp( lmap( delay, 0.0f, 0.32f, 0.084f, 0.05f ), 0.05f, 0.084f );
      delay += inc;
    }
    SwapStation::instance().performSwaps( swaps, true );
  }
  return mLetters.size() > 1;
}

bool SwapController::containsLetter( LetterRef letter ) const
{
  return std::find( mLetters.begin(), mLetters.end(), letter ) != mLetters.end();
}

void SwapController::updateWithLetter(LetterRef letter, bool canUseLetter)
{
  bool changed = false;
  if( mLetters.empty() )
  { // kick things off if we have no letters
    if( canUseLetter && !letter->isSelected() )
    {
      letter->setSelected( true );
      mLetters.push_back( letter );
      changed = true;
    }
  }
  else if( containsLetter( letter ) )
  { // check for backtracking if we have more than one letter
    if( mLetters.size() > 1 && letter == mLetters.at( mLetters.size() - 2 ) )
    { // remove letter at end of list
      mLetters.back()->setSelected( false );
      mLetters.pop_back();
      changed = true;
    }
  }
  else if( !letter->isSelected() )
  { // if we advanced onto a neighbor
    if( mLetters.back()->isNeighbor( letter ) && canUseLetter )
    {
      letter->setSelected( true );
      mLetters.push_back( letter );
      changed = true;
    }
  }

  if( changed )
  {
    deliver( Message( *this, CHANGED ) );
  }
}

