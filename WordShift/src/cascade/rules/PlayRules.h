//
//  PlayRules.h
//  WordShift
//
//  Created by David Wicks on 2/18/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "BoardModel.h"
#include "pockets/SpriteSheet.h"
#include "pockets/Locus2d.h"

namespace cascade
{
  class PlayerModel;
  class GameEndMessage;
  class GameWarningMessage;
  class PlayerTurnMessage;

  typedef std::shared_ptr<class PlayRules> PlayRulesRef;
  /**
  Base PlayRules
  conditionMet() returns true when the game should be considered over.
  Handles display of own state, since it's strongly-coupled to the type of condition.
  */
	class PlayRules
	{
	public:
    PlayRules( const std::string &name );
		virtual ~PlayRules();
    //! update the play restriction with info about the current player and play time
    void            update( float deltaTime );
    virtual void    draw(){}
    //! returns true if gameplay has reached its end
    virtual bool    endReached() const = 0;
    //! returns quantity of resource remaining, as relevant for warning
    //! this quantity is not used to evaluate the game/turn end at all
    virtual int     quantityRemaining() const { return 100; }
    //! called once per update until condition is met
    virtual void    evaluateEnd( float deltaTime ){}
    //! called once per update until turn is over
    virtual bool    turnOver() const { return false; }
    //! called by player when a word is credited to them
    virtual void    assignCredit( int wordLength ){}
    //! returns the GameCenter id of the scoreboard for this mode (if any)
    std::string     getScoreboardId() const { return mScoreboardId; }
    //! returns the GameCenter achievement prefix for this mode (if any)
    std::string     getScoreAchievementPrefix() const { return mScoreAchievementPrefix; }
    //! returns the mode's name in human-readable format
    std::string     getName() const { return mName; }
    //! override to return information about gameplay duration/etc
    virtual std::string     getPlayStats() const = 0;
    virtual uint64_t        getScoreboardContext() const { return 0; }
    //! returns true if it is the current PlayRules' turn
    bool            isTurn() const { return mIsTurn; }
    void            setColor( const ci::Color &color ) const;

    void            appendGameEndReceiver( Receiver<GameEndMessage> *receiver ){ mEndMessenger.appendReceiver( receiver ); }
    void            removeGameEndReceiver( Receiver<GameEndMessage> *receiver ){ mEndMessenger.removeReceiver( receiver ); }
    void            appendWarningReceiver( Receiver<GameWarningMessage> *receiver ){ mWarningMessenger.appendReceiver( receiver ); }
    void            removeWarningReceiver( Receiver<GameWarningMessage> *receiver ){ mWarningMessenger.removeReceiver( receiver ); }
    void            appendTurnReceiver( Receiver<PlayerTurnMessage> *receiver ){ mTurnMessenger.appendReceiver( receiver ); }
    void            removeTurnReceiver( Receiver<PlayerTurnMessage> *receiver ){ mTurnMessenger.appendReceiver( receiver ); }
    void            enable();
    void            disable();

    //! should the player be allowed to include next tile in swap?
    virtual bool    swapAllowed() const { return mIsTurn && !mEndReached; }
    //! for positioning graphics on screen, allow the rules to be parented to something else
    pk::Locus2dRef  getLocus(){ return mLocus; }
  protected:
    void setScoreboardId( const std::string &id ){ mScoreboardId = id; }
    void setScoreAchievementPrefix( const std::string &p ){ mScoreAchievementPrefix = p; }
    //! called when rules are enabled
    virtual void    beginTurn(){}
    //! called when rules are disabled
    virtual void    endTurn(){}
  private:
    pk::Locus2dRef  mLocus = pk::Locus2dRef( new pk::Locus2d );
    std::string     mName;
    std::string     mScoreboardId = "";
    std::string     mScoreAchievementPrefix = "";
    int             mPrevQuantityRemaining = 100;
    Messenger<GameEndMessage>       mEndMessenger;
    Messenger<GameWarningMessage>   mWarningMessenger;
    Messenger<PlayerTurnMessage>    mTurnMessenger;
    bool            mIsTurn = false;
    bool            mEndReached = false;

    void            checkTurnEnd();
    void            checkQuantityRemaining();
	};

  // Dispatched when the rules reach their end condition
  class GameEndMessage
  {};

  // Dispatched when the rules are close to their end condition
  class GameWarningMessage
  {
  public:
    explicit GameWarningMessage( int quantity_remaining ):
    mQuantityRemaining( quantity_remaining )
    {}
    int getQuantityRemaining() const { return mQuantityRemaining; }
  private:
    int mQuantityRemaining;
  };

  // Dispatched when a player's turn ends or begins
  class PlayerTurnMessage
  {
  public:
    explicit PlayerTurnMessage( bool isTurn ):
    mIsTurn( isTurn )
    {}
    bool isTurnBeginning() const { return mIsTurn; }
    bool isTurnEnding() const { return !mIsTurn; }
  private:
    bool mIsTurn = true;
  };
}
