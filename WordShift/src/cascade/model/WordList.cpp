//
//  WordList.cpp
//  Cascade
//
//  Created by David Wicks on 11/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "WordList.h"
#include "CascadeLogger.h"
#include "cinder/Utilities.h"
#include "cinder/DataSource.h"
#include <boost/algorithm/string/trim.hpp>
#include <algorithm>

using namespace cascade;
using namespace ci;
using namespace std;
using boost::algorithm::trim_copy;

WordList::WordList()
{}

WordList::~WordList()
{}

void WordList::loadWordData( const ci::fs::path &folder, int minLength, int maxLength )
{
	for( int letters = minLength; letters < maxLength + 1; ++letters )
	{
		auto path = folder / (toString(letters) + "-letters");
		if( fs::exists( path ) )
		{
			auto dataSource = loadFile( path );
			auto stream = dataSource->createStream();
			mWords.insert( make_pair( letters, unordered_set<string>() ) );
			while( ! stream->isEof() )
			{
				mWords[letters].insert( trim_copy( stream->readLine() ) );
			}
		}
		else
		{
      CascadeLog( "Unable to load words: " + path.string() );
		}
	}
}

vector<LetterIndices> WordList::findWords( const std::string &str, int min_length ) const
{
	vector<LetterIndices> foundWords;
  for( int start = 0; start <= static_cast<int>(str.size()) - min_length; ++start )
  {
    for ( int len = static_cast<int>(str.size()) - start; len >= min_length; --len )
    {
      string wordy = str.substr( start, len );
      if( isWord( wordy ) )
      {
        foundWords.push_back( LetterIndices( start, len ) );
      }
    }
  }
	return foundWords;
}

bool WordList::isWord( const string &str ) const
{
	size_t len = str.size();
  if( mWords.count(len) == 0 ){ return false; }
	return mWords[len].find( str ) != mWords[len].end();
}

LetterIndices WordList::longestWord( const vector<LetterIndices> &data )
{
	assert( !data.empty() );
	return *max_element( data.begin(), data.end(), []( const LetterIndices &lhs, const LetterIndices &rhs ){ return lhs.getLength() < rhs.getLength(); } );
}

WordList& WordList::getSharedList()
{
  static WordList sList;
  return sList;
}

