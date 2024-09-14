//
//  WordList.h
//  Cascade
//
//  Created by David Wicks on 11/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include "cinder/Filesystem.h"

namespace cascade
{
  class LetterIndices
  {
  public:
    LetterIndices( size_t position, size_t length ):
    mPosition( position )
    , mLength( length )
    {}
    size_t  getPosition() const { return mPosition; }
		size_t  getLength() const { return mLength; }
  private:
    size_t mPosition, mLength;
  };

	class WordList
	{
	public:
		WordList();
		~WordList();
		void loadWordData( const ci::fs::path &folder, int minLength, int maxLength );
		//! finds words as substrings of str
		std::vector<LetterIndices>	findWords( const std::string &str, int min_length ) const;
		bool isWord( const std::string &str ) const;
    bool empty(){ return mWords.empty(); }
    void clear(){ mWords.clear(); }
		static LetterIndices longestWord( const std::vector<LetterIndices> &data );
    static WordList& getSharedList();
	private:
		mutable std::unordered_map<int, std::unordered_set<std::string> > mWords;
	};

  inline WordList& getWordList(){ return WordList::getSharedList(); }
}
