//
//  BoardModel.cpp
//  Cascade
//
//  Created by David Wicks on 11/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "BoardModel.h"
#include "CollectionUtilities.hpp"
#include "cinder/Timeline.h"
#include "SwapStation.h"
#include <string>

using namespace cascade;
using namespace pockets;
using namespace std;
using namespace ci;

BoardModel::BoardModel( LetterSpringRef spring, int rows, int columns ):
mLetterSpring( spring )
, mRows( rows )
, mColumns( columns )
{
	for( int y = 0; y < mRows; ++y )
	{
		for( int x = 0; x < mColumns; ++x )
		{
			LetterRef letter = Letter::create( mLetterSpring->nextLetter(), y, x );
			mGrid.push_back( letter );
		}
	}
  for( LetterRef letter : mGrid )
  {
    letter->setNeighbor( NORTH, letterAt(letter->getRow(), letter->getColumn() - 1) );
    letter->setNeighbor( SOUTH, letterAt(letter->getRow(), letter->getColumn() + 1) );
    letter->setNeighbor( EAST, letterAt(letter->getRow() + 1, letter->getColumn()) );
    letter->setNeighbor( WEST, letterAt(letter->getRow() - 1, letter->getColumn()) );
    letter->appendReceiver( this );
  }
}

BoardModel::BoardModel( int rows, int columns ):
mRows( rows )
, mColumns( columns )
{}

BoardModelRef BoardModel::createLogoModel()
{ // build an empty board with space to fit "WORD SHIFT"
  BoardModelRef board( new BoardModel( 2, 5 ) );
  board->mGrid.clear();
  string word = "word";
  string shift = "shift";

  int column = 0;
  for( char c : word )
  {
    LetterRef letter = Letter::create( c, 0, column++ );
    board->mGrid.push_back( letter );
  }
  // store an empty LetterRef at end of first row so letterAt works properly
  board->mGrid.push_back( LetterRef() );
  column = 0;
  for( char c : shift )
  {
    LetterRef letter = Letter::create( c, 1, column++ );
    board->mGrid.push_back( letter );
  }

  for( LetterRef letter : board->mGrid )
  {
    if( letter )
    {
      letter->setNeighbor( NORTH, board->letterAt(letter->getRow(), letter->getColumn() - 1) );
      letter->setNeighbor( SOUTH, board->letterAt(letter->getRow(), letter->getColumn() + 1) );
      letter->setNeighbor( EAST, board->letterAt(letter->getRow() + 1, letter->getColumn()) );
      letter->setNeighbor( WEST, board->letterAt(letter->getRow() - 1, letter->getColumn()) );
    }
  }

  return board;
}

void BoardModel::removeWords(const WordList &wordList)
{
  int words_removed = 0;
  for( int column = 0; column < mColumns; ++column )
  {
    GridIndex index( 0, column, false );
    auto indices = wordList.findWords( getString( index ), mMinWordLength );
    for( auto &letter_indices : indices )
    {
      words_removed++;
      WordRef word = Word::create( getLetters(index, letter_indices), 0 );
      while( wordList.isWord( word->getString() ) )
      {
        rebuildWord( word );
      }
    }
  }
  for( int row = 0; row < mColumns; ++row )
  {
    GridIndex index( row, 0, true );
    auto indices = wordList.findWords( getString( index ), mMinWordLength );
    for( auto &letter_indices : indices )
    {
      words_removed++;
      WordRef word = Word::create( getLetters(index, letter_indices), 0 );
      while( wordList.isWord( word->getString() ) )
      {
        rebuildWord( word );
      }
    }
  }
}

void BoardModel::rebuildWord(WordRef word)
{
  for( LetterRef letter : *word )
  {
    letter->assignChar( mLetterSpring->nextLetter() );
  }
}

LetterRef BoardModel::letterAt( size_t row, size_t column )
{
  size_t index = indexOf( row, column );
  if( row < mRows && column < mColumns && index < mGrid.size() )
  {
    return mGrid.at( index );
  }
  return LetterRef();
}

LetterRef BoardModel::letterAt( const GridIndex &index )
{
  return letterAt( index.row(), index.column() );
}

BoardModel::~BoardModel()
{
  for( auto l : mGrid )
  {
    if( l )
    {
      l->clearNeighbors();
    }
  }
}

void BoardModel::setReadingDirections(cascade::ReadingDirection left_right, cascade::ReadingDirection top_bottom)
{
  if( left_right != mRight || top_bottom != mDown )
  {
    mRight = left_right;
    mDown = top_bottom;
    forceCompleteCheck();
  }
}

void BoardModel::forceCompleteCheck()
{
  mChangedSequences.clear();
  for( int r = 0; r < mRows; ++r )
  {
    GridIndex row( r, 0, true );
    mChangedSequences.push_back( row );
  }
  for( int c = 0; c < mColumns; ++c )
  {
    GridIndex column( 0, c, false );
    mChangedSequences.push_back( column );
  }
}

void BoardModel::receive( const LetterMessage &message )
{
	if( message.getEvent() == LetterMessage::CHANGE )
	{
    int r = message.getSubject().getRow();
    int c = message.getSubject().getColumn();
    GridIndex row( r, c, true );
    GridIndex column( r, c, false );
    if( find( mChangedSequences.begin(), mChangedSequences.end(), row ) == mChangedSequences.end() )
    { // if we don't have it yet
      mChangedSequences.push_back( row );
    }
    if( find( mChangedSequences.begin(), mChangedSequences.end(), column ) == mChangedSequences.end() )
    { // if we don't have it yet
      mChangedSequences.push_back( column );
    }
	}
}

void BoardModel::update( float delta, const WordList &wordList )
{
  mTime += delta;
  for( auto &pair : mPendingWords )
  {
    pair.second->update( delta );
  }
	// look for words in changed sequences
  for( auto index : mChangedSequences )
  {
    // Note: if the longest word was already found but shorter ones exist, the
    // player will need to unmake that longer word.
    // We could remove all already-made words here, but then there would be no
    // visual feedback to the user if they tried to re-make a word.
    auto words = wordList.findWords( getString( index ), mMinWordLength );
    if( !words.empty() )
    {
      auto letter_indices = WordList::longestWord( words );
      WordRef word = Word::create( getLetters(index, letter_indices), mConfirmationDelay );
      // must dereference to trigger Word::operator== (otherwise checks pointer equality)
      // if the found word isn't already pending, add it to our pending collection
      if( !vector_contains( mPendingWords, [&](const IndexedWord &p ){ return *p.second == *word; } ) )
      {
        // remove any existing words at this index
        vector_erase_if( &mPendingWords, [&](const IndexedWord &p){ return p.first == index; } );
        mPendingWords.push_back( IndexedWord( index, word ) );
        mWordFoundMessenger.deliver( WordFoundMessage( word ) );
      }
    }
  }
  mChangedSequences.clear();

  // create list of words that are valid and have hung around longer than confirmation delay
  vector<WordRef> confirmed_words;
  for( auto &pair : mPendingWords )
  {
    if( pair.second->confirmed() )
    {
      confirmed_words.push_back( pair.second );
    }
  }

  if( ! confirmed_words.empty() )
  {
    // remove confirmed words from pending list
    auto in_confirmed_words = [&](const IndexedWord &w){ return std::find(confirmed_words.begin(), confirmed_words.end(), w.second) != confirmed_words.end(); };
    vector_erase_if( &mPendingWords, in_confirmed_words );
    // sort confirmed words so swapping happens in proper order (edge-nearest first)
    auto distance_to_edge_compare = [=]( const WordRef &lhs, const WordRef &rhs )
    {
      if( lhs->isInRow() != rhs->isInRow() )
      { // columns before rows (arbitrary)
        return lhs->isInRow();
      } else if( lhs->isInRow() )
      { // compare on rows (rows will shift along column direction)
        return (mDown == Positive) ? lhs->getRow() < rhs->getRow()
        : lhs->getRow() > rhs->getRow();
      } else
      { // compare on columns (columns will shift along row direction)
        return (mRight == Positive) ? lhs->getColumn() < rhs->getColumn()
        : lhs->getColumn() > rhs->getColumn();
      }
    };
    sort( confirmed_words.begin(), confirmed_words.end(), distance_to_edge_compare );
    // submit confirmed words
    int match_count = confirmed_words.size() - 1;
    int sequence_count = 0; // TODO: figure out multiplier for cascading matches
    for( auto &word : confirmed_words )
    {
      submitWord( word, match_count, sequence_count );
    }

    // swap letter values
    float total_delay = 0.0f;
    while( !confirmed_words.empty() )
    {
      WordRef word = confirmed_words.back();
      confirmed_words.pop_back();
      total_delay = cascadeChanges( word, confirmed_words, total_delay );
    }
  }
	// remove any invalidated words (when letters are swapped, the string changes)
  vector_erase_if( &mPendingWords, [&](const IndexedWord &w){ return !wordList.isWord( w.second->getString() ); } );
  // remove any already-submitted words
  vector_erase_if( &mPendingWords, [&](const IndexedWord &w)
                  {
                    if( mSubmittedWords.find( w.second->getString() ) != mSubmittedWords.end() )
                      { w.second->setAlreadyPlayed(); }
                    return w.second->getAlreadyPlayed();
                  } );
}

void BoardModel::setConfirmationDelay(float seconds)
{
  mConfirmationDelay = seconds;
  for( auto &pair : mPendingWords )
  {
    pair.second->setConfirmationDelay( seconds );
  }
}

vector<LetterRef> BoardModel::getLetters( const GridIndex &index, const LetterIndices &indices )
{
  ReadingDirection direction = index.isRow() ? mRight : mDown;
  vector<LetterRef> letters;
  int start = indices.getPosition();
  if( direction == Negative )
  { // if the string was flipped around, we need to walk backwards
    start = index.isRow() ? mColumns - start - 1
    : mRows - start - 1;
  }
  int end = start + indices.getLength() * direction;
  if( index.isRow() )
  {
    assert( end >= -1 && end <= (int)mColumns );
    assert( start >= 0 && start < mColumns );
    for( int column = start; column != end; column += direction )
    {
      auto letter = letterAt( index.row(), column );
      assert( letter );
      letters.push_back( letter );
    }
  }
  else
  {
    assert( end >= -1 && end <= (int)mRows );
    assert( start >= 0 && start < mRows );
    for( int row = start; row != end; row += direction )
    {
      auto letter = letterAt( row, index.column() );
      assert( letter );
      letters.push_back( letter );
    }
  }
  return letters;
}

std::string BoardModel::getString( const GridIndex &index )
{
  string ret = "";
  if( index.isRow() )
  {
    int c_start = mRight > 0 ? 0 : mColumns - 1;
    int c_end = mRight > 0 ? mColumns : -1;
    int row = index.row();
    for( int column = c_start; column != c_end; column += mRight )
    {
      ret += letterAt( row, column )->getChar();
    }
  }
  else
  {
    int r_start = mDown > 0 ? 0 : mRows - 1;
    int r_end = mDown > 0 ? mRows : -1;
    int column = index.column();
    for( int row = r_start; row != r_end; row += mDown )
    {
      ret += letterAt( row, column )->getChar();
    }
  }
  return ret;
}

void BoardModel::submitWord( WordRef word, int match_count, int sequence_count )
{
  string submitted_word = word->getString();
	mSubmittedWords.insert( submitted_word );
  mWordPlayedMessenger.deliver( WordPlayedMessage( submitted_word, match_count ) );
}

float BoardModel::cascadeChanges( WordRef word, const vector<WordRef> &other_words, float initial_delay )
{
  bool in_row = word->isInRow();
  auto used_in_other_word = [&]( LetterRef letter )->bool
  { for( WordRef w : other_words )
    {
      if( w->containsLetter( letter ) ){ return true; }
    }
    return false;
  };

  float max_delay = initial_delay;
  vector<SwapStation::Swap> letter_swaps;
	for( LetterRef letter : *word )
	{	// swap each letter with its neighbors
    float delay = initial_delay;
    if( !used_in_other_word(letter) )
    {
      LetterRef next_letter = in_row ? letterAt( letter->getRow() + mDown, letter->getColumn() )
                                     : letterAt( letter->getRow(), letter->getColumn() + mRight );
      while( next_letter )
      { // swap with neighbors in turn until we reach the edge
        if( !used_in_other_word( next_letter ) )
        {
          letter_swaps.push_back( SwapStation::createSwap( letter, next_letter, delay ) );
          letter = next_letter;
          float inc = math<float>::clamp( lmap( delay, 0.0f, 0.4f, 0.084f, 0.05f ), 0.05f, 0.084f );
          delay += inc;
          max_delay = max( delay, max_delay );
        }
        next_letter = in_row ? letterAt( next_letter->getRow() + mDown, next_letter->getColumn() )
                             : letterAt( next_letter->getRow(), next_letter->getColumn() + mRight );
      }
      assert( letter );
      if( !used_in_other_word( letter ) )
      { // add a new letter to the board at edge
        float y = in_row ? mDown : 0;
        float x = in_row ? 0 : mRight;
        float theta = math<float>::atan2( y, x );
        letter_swaps.push_back( SwapStation::createSwap( letter, mLetterSpring->nextLetter(), theta, delay ) );
      }
    }
  }
  SwapStation::instance().performSwaps( letter_swaps, false );
  return max_delay;
}

bool BoardModel::hasNothingToDo()
{ // nothing in the queue, nothing to check for placing in queue
  return mPendingWords.empty() && mChangedSequences.empty();
}
