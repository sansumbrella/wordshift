//
//  WordEvaluator.cpp
//  WordShift
//
//  Created by David Wicks on 4/19/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "WordEvaluator.h"

using namespace cascade;

WordEvaluator::WordEvaluator()
{
  mLetterValues['a'] = 1;
  mLetterValues['b'] = 1;
  mLetterValues['c'] = 1;
  mLetterValues['d'] = 1;
  mLetterValues['e'] = 1;
	mLetterValues['f'] = 1;
  mLetterValues['g'] = 1;
  mLetterValues['h'] = 1;
  mLetterValues['i'] = 1;
  mLetterValues['j'] = 4;
  mLetterValues['k'] = 1;
  mLetterValues['l'] = 1;
  mLetterValues['m'] = 1;
  mLetterValues['n'] = 1;
  mLetterValues['o'] = 1;
  mLetterValues['p'] = 1;
  mLetterValues['q'] = 5;
  mLetterValues['r'] = 1;
  mLetterValues['s'] = 1;
  mLetterValues['t'] = 1;
  mLetterValues['u'] = 1;
  mLetterValues['v'] = 2;
  mLetterValues['w'] = 1;
  mLetterValues['x'] = 3;
  mLetterValues['y'] = 1;
  mLetterValues['z'] = 2;

	mLengthMultipliers[0] = 	1.0f;
	mLengthMultipliers[1] = 	1.0f;
	mLengthMultipliers[2] = 	1.0f;
	mLengthMultipliers[3] = 	1.0f;
	// Actual gameplay word lengths
	mLengthMultipliers[4] = 	1.0f;   // 100
	mLengthMultipliers[5] = 	1.2f;   // 125 -> 150
	mLengthMultipliers[6] = 	1.66f;  // 150 -> 250
	mLengthMultipliers[7] = 	2.0f;   // 175 -> 350
	mLengthMultipliers[8] = 	2.5f;   // 200 -> 500
	mLengthMultipliers[9] = 	3.33f;  // 225 -> 750
	// Maybe someday we'll have a bigger board
	mLengthMultipliers[10] = 	4.0f;   // 250 -> 1000
	mLengthMultipliers[11] = 	4.0f;
	mLengthMultipliers[12] = 	4.0f;
	mLengthMultipliers[13] = 	4.0f;
	mLengthMultipliers[14] = 	4.0f;

	mMatchCountMultipliers[0] = 	1.0f;
	mMatchCountMultipliers[1] = 	1.2f;
	mMatchCountMultipliers[2] = 	1.4f;
	mMatchCountMultipliers[3] = 	1.6f;
	mMatchCountMultipliers[4] = 	1.8f;
	mMatchCountMultipliers[5] = 	2.0f;
	mMatchCountMultipliers[6] = 	2.2f;
	mMatchCountMultipliers[7] = 	2.4f;
	mMatchCountMultipliers[8] = 	2.6f;
	mMatchCountMultipliers[9] = 	2.8f;
	mMatchCountMultipliers[10] = 	3.0f;
	for( int i = 11; i < 55; ++i )
	{	// pad out the remaining possibilities (however unlikely)
		mMatchCountMultipliers[i] = 3.6f;
	}
}

WordEvaluator::~WordEvaluator()
{}

uint32_t WordEvaluator::getWordValue( const std::string &word, int num_matches )
{
	uint32_t value = 0;
	for( const char &c : word )
	{
		value += mLetterValues[c];
	}
	// big up the word
	value = value * mLengthMultipliers[word.size()] * mMatchCountMultipliers[num_matches] * 25;
  // make sure we're a multiple of 5
  uint32_t remainder = value % 5;
  if( remainder != 0 )
  { // round up
    value += 5 - remainder;
  }

	return value;
}

