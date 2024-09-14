//
//  LetterSpring.cpp
//  Cascade
//
//  Created by David Wicks on 11/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "LetterSpring.h"

#include "cinder/DataSource.h"
#include "cinder/Utilities.h"
#include "cinder/CinderMath.h"
#include <boost/algorithm/string/trim.hpp>
#include <algorithm>
#include "CascadeLogger.h"

using namespace cascade;
using namespace ci;
using namespace std;

LetterSpring::LetterSpring()
{}

LetterSpring::~LetterSpring()
{}

void LetterSpring::setup( const fs::path &letterFile )
{
  if( fs::exists( letterFile ) && fs::is_regular_file( letterFile ) )
  {
    auto dataSource = loadFile( letterFile );
    auto stream = dataSource->createStream();
    while( ! stream->isEof() )
    {
      string line = boost::algorithm::trim_copy( stream->readLine() );
      auto parts = split( line, ":" );
      if( parts.size() == 2 )
      {
        char letter = parts.at( 0 ).at( 0 );
        size_t count = fromString<int>( parts.at( 1 ) );
        mLetters.insert( mLetters.end(), count, letter );
      }
    }
  }
  mIteratorResetFn = [this](){ shuffleLetters(); };
  mIteratorResetFn();
  mNextLetter = mLetters.begin();
}

void LetterSpring::setupWithLetters(const ci::fs::path &letterFile)
{
  if( fs::exists( letterFile ) && fs::is_regular_file( letterFile ) )
  {
    auto dataSource = loadFile( letterFile );
    auto stream = dataSource->createStream();
    vector<string> letterCombinations;
    while( ! stream->isEof() )
    {
      string line = boost::algorithm::trim_copy( stream->readLine() );
      letterCombinations.push_back( line );
    }

    mIteratorResetFn = [this, letterCombinations]() mutable -> void
                      { // shuffle words, but always keep the first four fixed
                        random_shuffle( letterCombinations.begin() + 4, letterCombinations.end()
                                       , [this]( int val ){ return mRand.nextUint( val ); } );
                        // put words into letters
                          mLetters.clear();
                          for( const string &str : letterCombinations )
                          {
                            for( const char c : str )
                            {
                              mLetters.push_back( c );
                            }
                          }
                      };
    mIteratorResetFn();
    mNextLetter = mLetters.begin();
  }
  else
  {
    CascadeLog( "WARNING: unable to set up with letter file: " + letterFile.string() );
  }
}

char LetterSpring::nextLetter()
{
  if( mNextLetter == mLetters.end() )
  {
    mIteratorResetFn();
    mNextLetter = mLetters.begin();
  }
  return *mNextLetter++;
}

void LetterSpring::shuffleLetters()
{
  random_shuffle( mLetters.begin(), mLetters.end(), [this]( int val ){ return mRand.nextUint( val ); } );
}

