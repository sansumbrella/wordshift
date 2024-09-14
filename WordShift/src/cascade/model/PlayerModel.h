//
//  PlayerModel.h
//  WordShift
//
//  Created by David Wicks on 3/4/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "Word.h"
#include "BoardModel.h"
#include "cinder/Utilities.h"
#include "SwapStation.h"
#include "PlayRules.h"

namespace cascade
{
  class PlayerScoreMessage;

/**
 Model of a player, governed by a PlayRestriction
*/

  typedef std::shared_ptr<class PlayerModel> PlayerModelRef;
	class PlayerModel
	{
	public:
    struct ScoredWord
    {
      ScoredWord( const std::string &aWord, uint32_t aScore ):
      word( aWord )
      , score( aScore )
      {}
      std::string word;
      int32_t     score;
    };
    PlayerModel( const std::string &name, ReadingDirection left_right, ReadingDirection top_bottom, PlayRulesRef end_condition );
		~PlayerModel();
    void              setTurn( bool isTurn=true );
    void              creditWithWord( const ScoredWord &word );
    //! listen for when the player scores points
    void              appendScoreReceiver( Receiver<PlayerScoreMessage> *receiver ){ mScoreMessenger.appendReceiver( receiver ); }
    void              removeScoreReceiver( Receiver<PlayerScoreMessage> *receiver ){ mScoreMessenger.removeReceiver( receiver ); }
    //! listen to when the rules say the turn is over
    void              appendTurnReceiver( Receiver<PlayerTurnMessage> *receiver ){ mRules->appendTurnReceiver( receiver ); }
    void              removeTurnReceiver( Receiver<PlayerTurnMessage> *receiver ){ mRules->removeTurnReceiver( receiver ); }
    //! listen to when the rules say the game is over
    void              appendGameEndReceiver( Receiver<GameEndMessage> *receiver ){ mRules->appendGameEndReceiver( receiver ); }
    void              removeGameEndReceiver( Receiver<GameEndMessage> *receiver ){ mRules->removeGameEndReceiver( receiver ); }
    //! listen for when the rules say we are close the the end of a turn or a game
    void              appendWarningReceiver( Receiver<GameWarningMessage> *receiver ){ mRules->appendWarningReceiver( receiver ); }
    void              removeWarningReceiver( Receiver<GameWarningMessage> *receiver ){ mRules->removeWarningReceiver( receiver ); }
    void              update( float deltaTime );

    std::string       getName() const { return mName; }
    std::string       getScoreboardId() const { return mRules->getScoreboardId(); }
    uint64_t          getScoreboardContext() const { return mRules->getScoreboardContext(); }
    int64_t           getScore() const { return mScore; }
    //! collection of words played by this player during game
    const std::vector<ScoredWord>& getPlayedWords() const { return mPlayedWords; }
    void              setReadingDirections( ReadingDirection left_right, ReadingDirection top_bottom ){ mRight = left_right; mDown = top_bottom; }
    ReadingDirection  getLeftRight() const { return mRight; }
    ReadingDirection  getTopBottom() const { return mDown; }
    bool              swapAllowed() const;
    //! returns true if the player thinks it is their turn
    bool              isTurn() const { return mRules->isTurn(); }
    //! get the rules (so it can be parented to things and drawn)
    PlayRulesRef      getRules() const { return mRules; }
	private:
    //! the directions in which this player reads
    //! defaults are for vertical portrait orientation
    ReadingDirection              mRight = ReadingDirection::Positive;
    ReadingDirection              mDown = ReadingDirection::Positive;
    std::string                   mName = "Player";
    int64_t                       mScore = 0;
    std::vector<ScoredWord>       mPlayedWords;
    Messenger<PlayerScoreMessage> mScoreMessenger;
    PlayRulesRef                  mRules;
	};

  class PlayerScoreMessage
  {
  public:
    PlayerScoreMessage( const std::string &word, uint32_t value, uint64_t total_score ):
    mWord( word )
    , mWordValue( value )
    , mScore( total_score )
    {}
    int32_t        getWordValue() const { return mWordValue; }
    std::string    getWord() const { return mWord; }
    int64_t        getTotalScore() const { return mScore; }
  private:
    std::string    mWord;
    int32_t        mWordValue;
    int64_t        mScore;
  };
}
