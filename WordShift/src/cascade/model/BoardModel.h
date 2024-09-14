//
//  BoardModel.h
//  Cascade
//
//  Created by David Wicks on 11/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


/*
 Maintains the current state of the game
 Essentially, a grid of letters

 Sends a ScoreMessage whenever a word is submitted and applied to player's score.
 Sends a WordFoundMessage whenever a word is found in the grid.
 Receives messages from Letters and updates overall state

 Letter -> LetterSequence -> BoardModel (checks for words)
 BoardModel sends out notices on word formation
 */

#pragma once
#include <vector>
#include "LetterSpring.h"
#include "WordList.h"
#include "Word.h"

namespace cascade
{
	typedef std::shared_ptr<class BoardModel> BoardModelRef;

  class GridIndex;
  class WordFoundMessage;
  class WordPlayedMessage;

	class BoardModel : public Receiver<LetterMessage>
	{
	public:
		~BoardModel();
		std::string                   getString( const GridIndex &index );
    const std::vector<LetterRef>& getLetters () const { return mGrid; }
		void                          receive( const LetterMessage &message );
		void                          update( float deltaTime, const WordList &wordList );
    LetterRef                     letterAt( size_t row, size_t column );
    LetterRef                     letterAt( const GridIndex &index );
    size_t                        indexOf( int row, int column ){ return row * mColumns + column; }
    std::vector<LetterRef>        getLetters( const GridIndex &index, const LetterIndices &indices );
    int                           getRowCount() const { return mRows; }
    int                           getColumnCount() const { return mColumns; }
    //! set the number of characters in the shortest allowable words
    void                          setMinWordLength( int length ){ mMinWordLength = length; }
    //! set how long to wait once a word is found before submitting it
    void                          setConfirmationDelay( float seconds );
    //! remove all words from the board (before starting play)
    void                          removeWords( const WordList &wordList );
    //! change the letters in a word until it's not a word
    void                          rebuildWord( WordRef word );
    //! returns true if there are no pending words
    bool                          hasNothingToDo();
    //! Create a BoardModel suitable for playing a game of WordShift
		static BoardModelRef create( LetterSpringRef spring, int rows, int columns )
		{
			BoardModelRef board( new BoardModel( spring, rows, columns ) );
			return board;
		}
    //! returns a board model for the logo; swappable but not playable
    static BoardModelRef createLogoModel();
    //! receive messages when a word is played (removed from board)
    void appendWordPlayedReceiver( Receiver<WordPlayedMessage> *receiver ){ mWordPlayedMessenger.appendReceiver( receiver ); }
    //! receive notifications when a word is found
    void appendWordFoundReceiver( Receiver<WordFoundMessage> *receiver ){ mWordFoundMessenger.appendReceiver( receiver ); }
    //! stop hearing about played words
    void removeWordPlayedReceiver( Receiver<WordPlayedMessage> *receiver ){ mWordPlayedMessenger.removeReceiver( receiver ); }
    //! when reading direction changes, it forces a complete check
    void setReadingDirections( ReadingDirection left_right, ReadingDirection top_bottom );
    //! get reading directions
    ReadingDirection getLeftRight() const { return mRight; }
    ReadingDirection getTopBottom() const { return mDown; }
    //! set all rows and columns as having changed
    void forceCompleteCheck();
    //! for grabbing the list of played words at end of game and allowing replays during tutorial
    std::set<std::string>&        getPlayedWords(){ return mSubmittedWords; }
	private:
    typedef std::pair<GridIndex, WordRef> IndexedWord;
		void                          submitWord( WordRef word, int match_count, int sequence_count );
    float                         cascadeChanges( WordRef word, const std::vector<WordRef> &other_words, float initial_delay=0.0f );
    // Construct a usable board model
		BoardModel( LetterSpringRef spring, int rows, int columns );
    // Prepare an empty board model with rows and columns
    BoardModel( int rows, int columns );

		std::vector<LetterRef>				mGrid;
		LetterSpringRef               mLetterSpring;
		std::vector<GridIndex>        mChangedSequences;
    std::vector<IndexedWord>      mPendingWords;
		std::set<std::string>         mSubmittedWords;
    //! the direction of reading in rows (right) and columns (down)
    ReadingDirection              mRight = ReadingDirection::Positive;
    ReadingDirection              mDown = ReadingDirection::Positive;
    //! the number of rows and columns
    const size_t                  mRows;
    const size_t                  mColumns;
    float                         mConfirmationDelay = 2.0f;
    double                        mTime = 0.0f;
    int                           mMinWordLength = 4;
    Messenger<WordFoundMessage>   mWordFoundMessenger;
    Messenger<WordPlayedMessage>  mWordPlayedMessenger;
  };

  // Dispatched when a word is found on the board, so it can be highlighted
  class WordFoundMessage
  {
  public:
    WordFoundMessage( WordRef word ):
    mWord( word )
    {}
    WordRef getWord() const { return mWord; }
  private:
    WordRef mWord;
  };

  // Dispatched when a word is played, so you can get points for it
  class WordPlayedMessage
  {
  public:
    WordPlayedMessage( const std::string &str, int matches ):
    mWord( str )
    ,mMatches( matches )
    {}
    std::string getWord() const { return mWord; }
    int         getSimultaneousMatches() const { return mMatches; }
  private:
    std::string mWord;
    int         mMatches;
  };

  class GridIndex
  {
  public:
    GridIndex( int row, int column, bool is_row ):
    mRow( row )
    , mColumn( column )
    , mIsRow( is_row )
    {}
    int row() const { return mRow; }
    int column() const { return mColumn; }
    bool isRow() const { return mIsRow; }
    bool operator==(const GridIndex &rhs) const
    { if( mIsRow && rhs.mIsRow ){ return mRow == rhs.mRow; }
    else if( !mIsRow && !rhs.mIsRow ){ return mColumn == rhs.mColumn; }
      return false;
    }
  private:
    int   mRow;
    int   mColumn;
    bool  mIsRow;
  };
}
