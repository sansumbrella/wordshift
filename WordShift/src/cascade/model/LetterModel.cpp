//
//  LetterModel.cpp
//  Cascade
//
//  Created by David Wicks on 12/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "LetterModel.h"

using namespace cascade;
using namespace ci;
using namespace std;

void Letter::swapWith( LetterRef other, float delay )
{
	char temp_letter = mLetter;
  Vec2f neighbor( other->getPosition() - getPosition() );
  float theta = math<float>::atan2( neighbor.y, neighbor.x );

  other->getPosition();
	mLetter = other->mLetter;
  setChar( other->mLetter, theta, delay );
  other->setChar( temp_letter, theta + M_PI, delay );
}

void Letter::setChar( char c, float from_direction, float delay )
{
	mLetter = c;
	deliver( LetterMessage( *this, LetterMessage::CHANGE, from_direction, delay ) );
}

bool Letter::isNeighbor(LetterRef candidate)
{
	return std::find( mNeighbors.begin(), mNeighbors.end(), candidate ) != mNeighbors.end();
}

void Letter::clearNeighbors()
{
  for( auto &n : mNeighbors )
  {
    n.reset();
  }
}

void Letter::setSelected( bool val )
{
  mSelected = val;
  deliver( LetterMessage( *this, LetterMessage::SELECT ) );
}

vector<LetterRef> Letter::getNeighbors() const
{
  vector<LetterRef> ret = { mNeighbors[NORTH], mNeighbors[SOUTH], mNeighbors[EAST], mNeighbors[WEST] };
  return ret;
}
