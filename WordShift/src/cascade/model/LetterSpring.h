//
//  LetterSpring.h
//  Cascade
//
//  Created by David Wicks on 11/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

/*
 A source of shuffled letters.
 */

#pragma once

#include <vector>
#include "cinder/Filesystem.h"
#include "cinder/Rand.h"
#include <ctime>

namespace cascade
{
	typedef std::shared_ptr<class LetterSpring> LetterSpringRef;
	class LetterSpring
	{
	public:
		LetterSpring();
		~LetterSpring();
    //! set up with letter frequencies from file and approximately deckSize letters
		void setup( const ci::fs::path &letterFile );
    //! set up with the letters in letterFile, in order
    void setupWithLetters( const ci::fs::path &letterFile );
		char nextLetter();
		static LetterSpringRef create(){ return LetterSpringRef( new LetterSpring() ); }
	private:
		typedef std::vector<char> LetterList;
    std::function<void ()>    mIteratorResetFn;
		LetterList                mLetters;
		LetterList::iterator      mNextLetter;
		ci::Rand                  mRand = ci::Rand( time(NULL) );
    void                      shuffleLetters();
	};
}
