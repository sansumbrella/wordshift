//
//  Game.h
//  WordShift
//
//  Created by David Wicks on 2/21/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "SceneController.h"
#include "PlayerMoveController.h"
#include "cascade/model/LetterSpring.h"
#include "cascade/model/WordList.h"
#include "cascade/model/BoardModel.h"
#include "cascade/view/BoardView.h"
#include "cascade/view/PlayerView.h"
#include "cascade/model/PlayerModel.h"
#include "SimpleButton.h"
#include "SwapStation.h"
#include "WordEvaluator.h"

namespace cascade
{
  typedef std::shared_ptr<class Game>             GameRef;

  /**
  A game being played.
  Composes an EndCondition object to control how gameplay is evaluated.

  Variants: one and two-player
  */
	class Game : public SceneController, public Receiver<GameEndMessage>, public Receiver<GameWarningMessage>, public Receiver<PlayerTurnMessage>, public Receiver<WordPlayedMessage>
	{
	public:
		Game( const pockets::SpriteSheet &sprite_sheet, int rows, int columns, const std::vector<PlayerModelRef> &players );
		~Game();
    void layoutViews();
    void appear();
    void update();
    void draw();
    void connect( ci::app::WindowRef window );
    void customDisconnect();
    void touchesBegan( const ci::app::TouchEvent &event );
    void touchesMoved( const ci::app::TouchEvent &event );
    void touchesEnded( const ci::app::TouchEvent &event );
    void receive( const GameEndMessage &msg );
    void receive( const GameWarningMessage &msg );
    void receive( const WordPlayedMessage &msg );
    void receive( const PlayerTurnMessage &msg );
    void showPlayerDetails( PlayerModelRef player );
    //! set function called when the game is completely finished (player has viewed score and said 'done')
    void setGameOverFn( std::function<void ()> fn ){ mGameOverFn = fn; }
    //! set function called to start game over
    void    setReplayFn( std::function<void ()> fn ){ mReplayFn = fn; }
    void    showGameEndScene();
    void    customPause();
    void    customResume();
    bool    swapAllowed();
    //! set the position from which letters slide into view
    void    setAppearDirection( const ci::Vec2i &direction ){ mAppearDirection = direction; }
	private:
    void            emitGameOver(){ if( mGameOverFn ){ mGameOverFn(); } }
    void            createPlayerViews();
    //! returns the currently active player; returns last player when between turns
    PlayerModelRef  currentPlayer() {
      if( !hasActivePlayer() ) { return mLatestPlayer; }
      else { return mActivePlayers.at( mCurrentPlayerIndex ); }
    }
    //! returns true if it is a player's turn
    bool            hasActivePlayer(){
      for( auto p : mActivePlayers )
      { if( p->isTurn() ){ return true; } }
      return false;
    }
    //! increments currentPlayerIndex and returns player at new position
    PlayerModelRef  nextPlayer();
    void            enablePlayer( PlayerModelRef player );
    void            disablePlayer( PlayerModelRef player );
    void            flushTouches();
    //! returns locus on side of board nearest player with given reading directions
    pk::Locus2dRef  readingDirectionBottomLocus( ReadingDirection left_right, ReadingDirection top_bottom );
    SceneControllerRef                  mOverlaidScene;
    // touch IDs mapped to controllers gathering points
    PlayerMoveController                mPlayerController;
    LetterSpringRef                     mLetterSpring;
    BoardModelRef                       mBoardModel;
    BoardViewRef                        mBoardView;
    int32_t                             mCurrentPlayerIndex = 0;
    WordEvaluator                       mWordEvaluator;
    // players still in the game (with moves/time remaining)
    std::vector<PlayerModelRef>         mActivePlayers;
    // players who have played in this game
    std::vector<PlayerModelRef>         mPlayers;
    std::vector<PlayerViewRef>          mPlayerViews;
    // the player who was most recently active (for when active players is empty)
    PlayerModelRef                      mLatestPlayer;
    std::vector<ButtonRef>              mPauseButtons;
    ci::Vec2i                           mAppearDirection = ci::Vec2i::zero();
    ci::Timer                           mTimer;
    pk::Locus2dRef                      mNorthLocus = pk::Locus2dRef( new pk::Locus2d() );
    pk::Locus2dRef                      mSouthLocus = pk::Locus2dRef( new pk::Locus2d() );
    pk::Locus2dRef                      mEastLocus = pk::Locus2dRef( new pk::Locus2d() );
    pk::Locus2dRef                      mWestLocus = pk::Locus2dRef( new pk::Locus2d() );
    pk::SpriteData                      mContinueSpriteData;
    pk::SpriteData                      mReplaySpriteData;
    pk::SpriteData                      mBackSpriteData;
    pk::SpriteData                      mQuitSpriteData;
    pk::SpriteData                      mPauseSpriteData;
    //! game end condition was met and we're playing out remaining matches
    bool                                mWaitingForEnd = false;
    //! player's turn has ended and we're playing out remaining matches (multiplayer only)
    bool                                mTurnEnded = false;
    std::function<void ()>              mGameOverFn;
    std::function<void ()>              mReplayFn;
    double                              mLastUpdate;
    const bool                          mMultiplayer;
    std::map<PlayerModelRef, ci::Vec2f> mBoardShift;
    std::vector<ci::Channel8u>          mWarningChannels;
	};
}
