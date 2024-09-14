//
//  Word.cpp
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Word.h"

using namespace cascade;
using namespace std;

Word::Word( const std::vector<LetterRef> &letters, float confirmationDelay ):
mLetters( letters )
, mConfirmationDelay( confirmationDelay )
{
  mIsInRow = firstLetter()->getRow() == lastLetter()->getRow();
  if( mIsInRow )
  {
    mReadingDirection = (firstLetter()->getColumn() < lastLetter()->getColumn()) ? Positive : Negative;
  }
  else
  {
    mReadingDirection = (firstLetter()->getRow() < lastLetter()->getRow()) ? Positive : Negative;
  }
}

Word::~Word()
{
  auto message = getAlreadyPlayed() ? WordMessage::ALREADY_PLAYED : WordMessage::VANISH;
  deliver( WordMessage( message ) );
}

void Word::setConfirmationDelay(float delay)
{
  mConfirmationDelay = delay;
}

bool Word::containsLetter(LetterRef letter) const
{
  return std::find( mLetters.begin(), mLetters.end(), letter ) != mLetters.end();
}

bool Word::operator==(const cascade::Word &other)
{
  bool equal = mLetters.size() == other.mLetters.size();
  if( equal )
  {
    for( int i = 0; i < mLetters.size(); ++i )
    {
      if( mLetters.at( i ) != other.mLetters.at( i ) )
      {
        equal = false;
        break;
      }
    }
  }
  return equal;
}

string Word::getString() const
{
	string ret = "";
	for( auto letter : mLetters )
	{
		ret += letter->getChar();
	}
	return ret;
}
