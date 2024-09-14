//
//  PlayRules.cpp
//  WordShift
//
//  Created by David Wicks on 2/18/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "PlayRules.h"

using namespace cascade;
using namespace std;

PlayRules::PlayRules( const string &name ):
mName( name )
{}

PlayRules::~PlayRules()
{}

void PlayRules::update( float deltaTime )
{
  // update the condition information until end condition is met
  if( !mEndReached && mIsTurn )
  {
		evaluateEnd( deltaTime );
    if( endReached() )
    {
      mEndReached = true;
      mEndMessenger.deliver( GameEndMessage() );
    }
    else
    {
      checkTurnEnd();
      checkQuantityRemaining();
    }
  }
}

void PlayRules::checkTurnEnd()
{
  if( mIsTurn && turnOver() )
  {
    mIsTurn = false;
    mTurnMessenger.deliver( PlayerTurnMessage( mIsTurn ) );
  }
}

void PlayRules::checkQuantityRemaining()
{
  int remaining = quantityRemaining();
  if( remaining != mPrevQuantityRemaining )
  {
    if( remaining < 6 && remaining < mPrevQuantityRemaining )
    {
      mWarningMessenger.deliver( GameWarningMessage( remaining ) );
    }
    mPrevQuantityRemaining = remaining;
  }
}

void PlayRules::setColor(const ci::Color &color) const
{
  if( mIsTurn )
  {
    cinder::gl::color( color );
  }
  else
  {
    cinder::gl::color( ci::ColorA( color, 0.4f ) );
  }
}

void PlayRules::enable()
{
  if( !mIsTurn )
  {
    mIsTurn = true;
    mTurnMessenger.deliver( PlayerTurnMessage( mIsTurn ) );
  }
  beginTurn();
}

void PlayRules::disable()
{
  if( mIsTurn )
  { // if this ends our turn, report it
    mIsTurn = false;
    mTurnMessenger.deliver( PlayerTurnMessage( mIsTurn ) );
  }
  endTurn();
}

