//
//  Game.cpp
//  WordShift
//
//  Created by David Wicks on 2/21/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Game.h"
#include "cinder/Timeline.h"
#include "cinder/System.h"
#include "cinder/ip/Fill.h"
#include "PlayerDetailScene.h"
#include "GameTypography.h"
#include "CascadeGameCenterDelegate.h"
#include "AchievementReporter.h"
#include "SpriteButton.h"
#include "SoundStation.h"

using namespace cascade;
using namespace pockets;
using namespace cinder;
using namespace std;

Game::Game( const pk::SpriteSheet &sprite_sheet, int rows, int columns, const vector<PlayerModelRef> &players ):
mPlayerController( sprite_sheet.getSprite("swap-icon-horizontal"), sprite_sheet.getSprite("swap-icon-vertical") )
, mPlayers( players )
, mActivePlayers( players )
, mLatestPlayer( players.front() )
, mMultiplayer( players.size() > 1 )
, mReplaySpriteData( sprite_sheet.getSpriteData( "replay-icon" ) )
, mBackSpriteData( sprite_sheet.getSpriteData( "back-icon" ) )
, mContinueSpriteData( sprite_sheet.getSpriteData( "continue-icon" ) )
, mQuitSpriteData( sprite_sheet.getSpriteData( "quit-icon" ) )
, mPauseSpriteData( sprite_sheet.getSpriteData( "pause-icon" ) )
{
  mLetterSpring = LetterSpring::create();
  mLetterSpring->setup( app::App::getResourcePath( fs::path( "words/letter-counts" ) ) );
  // in future, we might pass these images in from outside (only takes ~2ms to load all three, though)
  // right now, it's important to load these in order (so indices match up)
  mWarningChannels.emplace_back( loadImage( app::App::getResourcePath("img/countdown-0.png") ) );
  mWarningChannels.emplace_back( loadImage( app::App::getResourcePath("img/countdown-1.png") ) );
  mWarningChannels.emplace_back( loadImage( app::App::getResourcePath("img/countdown-2.png") ) );
  mWarningChannels.emplace_back( loadImage( app::App::getResourcePath("img/countdown-3.png") ) );
  mBoardModel = BoardModel::create( mLetterSpring, rows, columns );
  mBoardModel->setConfirmationDelay( 1.25f );
  mBoardModel->removeWords( getWordList() );
  mBoardModel->appendWordPlayedReceiver( this );
  mBoardModel->appendWordPlayedReceiver( &AchievementReporter::get() );
  mBoardView = BoardView::create( mBoardModel, sprite_sheet );
  mPlayerController.setLayer( 2 );
  mPlayerController.setCellSize( mBoardView->getCellSize() );
  mBoardView->addOverlay( &mPlayerController );
  setBounds( app::getWindowBounds() );

  createPlayerViews();
  
  layoutViews();
}

Game::~Game()
{
  disconnect();
}

void Game::appear()
{
  enablePlayer( currentPlayer() );

  mBoardView->appear( 0.66f, mAppearDirection );
  mLastUpdate = 0;
  mTimer.start();
}

void Game::createPlayerViews()
{
  float score_width = (mBoardView->getCellSize().x * 4) - (mBoardView->getCellSize().x - mBoardView->getTileSize().x);
  Font font = getButtonFont();
  for( PlayerModelRef player : mPlayers )
  {
    PlayerViewRef view( new PlayerView( font, score_width, player->getRules() ) );
    player->appendScoreReceiver( view.get() );
    player->appendTurnReceiver( view.get() );

    const float smidgen = (mBoardView->getCellSize().x + mBoardView->getCellSize().x - mBoardView->getTileSize().x) / 2;
    bool horizontal = (player->getLeftRight() != player->getTopBottom());
    int cell_index = horizontal ? 8 : 5;
    view->getEndConditionLocus()->setLoc( Vec2f(mBoardView->getCellSize().x * cell_index - smidgen, view->getHeight() / 2 ) );

    mPlayerViews.push_back( view );
    view->getLocus()->setParent( readingDirectionBottomLocus( player->getLeftRight(), player->getTopBottom() ) );

    ButtonRef pause = ButtonRef( new SpriteButton( Sprite{ mPauseSpriteData } ) );
    pause->expandHitBounds( 6, 6 );
    pause->setSelectFn( [=](){ showPlayerDetails( player ); } );
    mPauseButtons.push_back( pause );
    pause->getLocus()->setParent( view->getLocus() );

    int pause_cell_index = horizontal ? 9 : 6;
    Vec2f end_loc( mBoardView->getCellSize().x * pause_cell_index - smidgen, view->getHeight() / 2 );
    pause->setLoc( end_loc );
  }
}

void Game::flushTouches()
{
  // end touches for this scene
  app::WindowRef window = app::getWindow();
  app::TouchEvent event( window, window->getActiveTouches() );
  touchesEnded( event );
}

void Game::customPause()
{
  mTimer.stop();
  // stop the game
  for( auto &btn : mPauseButtons ){ btn->block(); }
  block();
  // ditch any current touches
  flushTouches();
}

void Game::customResume()
{
  unblock();
  for( auto &btn : mPauseButtons ){ btn->unblock(); }
  mLastUpdate = 0;
  mTimer.start();
}

void Game::showPlayerDetails( PlayerModelRef player )
{
  pause();

  // make a sorted copy of players
  vector<PlayerModelRef> players;
  auto iter = std::find( mPlayers.begin(), mPlayers.end(), player );
  const auto end = iter;
  do
  { // put players into vector in their natural turn order
    players.push_back( *iter++ );
    if( iter == mPlayers.end() )
    {
      iter = mPlayers.begin();
    }
  }
  while( iter != end );

  PlayerDetailSceneRef details( new PlayerDetailScene{ players, mContinueSpriteData, mQuitSpriteData } );
  details->connect( app::getWindow() );
  details->show( app::getWindow() );
  auto back = [this](){
    // stop receiving events
    mOverlaidScene->disconnect();
    // resume game once scene is hidden
    mOverlaidScene->hide( [this](){ resume(); } );
  };
  details->setBackFunction( back );
  details->setQuitFunction( [this](){ emitGameOver(); } );
  mOverlaidScene = details;
}

void Game::showGameEndScene()
{
  auto by_player_score = []( const PlayerModelRef &lhs, const PlayerModelRef &rhs ){
    return lhs->getScore() > rhs->getScore();
  };
  // put winner up top
  auto players = mPlayers;
  sort( players.begin(), players.end(), by_player_score );

  PlayerDetailSceneRef details( new PlayerDetailScene{ players, mReplaySpriteData, mBackSpriteData } );
  details->connect( app::getWindow() );
  details->show( app::getWindow() );
  details->setQuitFunction( [this]()
  { // hide overlay and emit game over
    mOverlaidScene->disconnect();
    emitGameOver();
  } );
  details->setBackFunction( [this](){
    if( mReplayFn ){ mReplayFn(); }
  });
  mOverlaidScene = details;

  // report score since we have now finished cascading all last-minute changes
  string scoreboard = mPlayers.front()->getScoreboardId();
  int context = mPlayers.front()->getScoreboardContext();
  if( !scoreboard.empty() )
  {
    int64_t total_score = 0;
    for( auto p : mPlayers )
    {
      total_score += p->getScore();
    }
    getGameCenter().reportScore( scoreboard, total_score, context );
    AchievementReporter::get().reportGameEndAchievements( mPlayers.front() );
  }
}

PlayerModelRef Game::nextPlayer()
{
  assert( !mActivePlayers.empty() );  // worse will happen if we get a bad ref
  mCurrentPlayerIndex = (mCurrentPlayerIndex + 1) % mActivePlayers.size();
  return mActivePlayers.at( mCurrentPlayerIndex );
}

void Game::disablePlayer(PlayerModelRef player)
{
  player->removeGameEndReceiver( this );
  player->removeTurnReceiver( this );
  player->removeWarningReceiver( this );
  player->setTurn( false );
}

void Game::enablePlayer(PlayerModelRef player)
{
  player->setTurn(true);
  player->appendGameEndReceiver( this );
  player->appendTurnReceiver( this );
  player->appendWarningReceiver( this );
  mLatestPlayer = player;  // give this person credit for points

  mBoardModel->setReadingDirections( player->getLeftRight(), player->getTopBottom() );
  mBoardView->setLetterRotation( readingDirectionAngle( player->getLeftRight(), player->getTopBottom() ) );
  if( !mBoardShift.empty() )
  { // move the board to display the player's view (on 3.5-inch iPhone displays)
    Vec2f board_offset = mBoardShift[player];
    mBoardView->setLocSoft( board_offset );
  }
}

void Game::layoutViews()
{
  Vec2f board_offset( app::getWindowCenter() - Vec2f( mBoardView->getCenteringWidth(), mBoardView->getCenteringHeight() ) / 2 );
  if( board_offset.y < (mBoardView->getCellSize().y * 1.5) )
  { // shift down the view on pre-5 iPhones (store player offsets in a map)
    board_offset.y = mBoardView->getCellSize().y * 1.5;
    if( mPlayers.size() > 1 )
    {
      mBoardShift[mPlayers.at(0)] = Vec2f( board_offset.x, mBoardView->getCellSize().y * 0.5f );
      mBoardShift[mPlayers.at(1)] = board_offset;
    }
  }
  mBoardView->setLoc( board_offset );

  Rectf board_bounds = mBoardView->getTileBounds();
  board_bounds.offset( -board_bounds.getUpperLeft() );

  const float tile_width = mBoardView->getTileSize().x;
  const float tile_height = mBoardView->getTileSize().y;
  const float view_height = mPlayerViews.front()->getHeight();
  const float h_gap = board_offset.x - view_height * 2;// - tile_width / 2;
  const float v_gap = board_offset.y - view_height * 2;// - tile_height / 2;
  const Vec2f corner( -tile_width / 2, -tile_height / 2 );

  // center in remaining space
  Vec2f north = board_bounds.getUpperRight() + corner - Vec2f( 0, v_gap / 2 );
  Vec2f south = board_bounds.getLowerLeft() + corner + Vec2f( 0, v_gap / 2 );
  Vec2f west = board_bounds.getUpperLeft() + corner - Vec2f( h_gap / 2, 0 );
  Vec2f east = board_bounds.getLowerRight() + corner + Vec2f( h_gap / 2, 0 );
  mNorthLocus->setLoc( north );
  mNorthLocus->setRotation( M_PI );
  mSouthLocus->setLoc( south );
  mEastLocus->setLoc( east );
  mEastLocus->setRotation( -M_PI / 2 );
  mWestLocus->setLoc( west );
  mWestLocus->setRotation( M_PI / 2 );

  mNorthLocus->setParent( mBoardView->getLocus() );
  mSouthLocus->setParent( mBoardView->getLocus() );
  mEastLocus->setParent( mBoardView->getLocus() );
  mWestLocus->setParent( mBoardView->getLocus() );

  PlayerViewRef first_player = mPlayerViews.front();
  if( !mMultiplayer )
  { // place single player view on top of board
    first_player->getLocus()->setParent( mNorthLocus );
    north = corner - Vec2f( 0, v_gap / 2 + view_height );
    mNorthLocus->setLoc( north );
    mNorthLocus->setRotation( 0 );
  }
}

void Game::connect(ci::app::WindowRef window)
{
  storeConnection( window->getSignalTouchesBegan().connect( [this](const app::TouchEvent &event){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this](const app::TouchEvent &event){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this](const app::TouchEvent &event){ touchesEnded( event ); } ) );
  for( auto &btn : mPauseButtons )
  {
    btn->connect( window );
  }
}

void Game::customDisconnect()
{
  for( auto &btn : mPauseButtons )
  {
    btn->disconnect();
  }
  flushTouches();
}

void Game::touchesBegan(const ci::app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
  {
    auto letter = mBoardView->getLetter( touch.getPos() );
    if( letter )
    {
      mPlayerController.updateWithLetter( touch.getId(), letter, swapAllowed() );
    }
  }
}

void Game::touchesMoved( const app::TouchEvent &event )
{
  for( auto &touch : event.getTouches() )
	{
		auto letter = mBoardView->getLetter( touch.getPos() );
    if( letter )
    {
      mPlayerController.updateWithLetter( touch.getId(), letter, swapAllowed() );
    }
	}
}

void Game::touchesEnded( const app::TouchEvent &event )
{
  for( auto &touch : event.getTouches() )
	{
    auto letter = mBoardView->getLetter( touch.getPos() );
    mPlayerController.apply( touch.getId(), letter, swapAllowed() );
	}
}

bool Game::swapAllowed()
{
  return currentPlayer()->swapAllowed();
}

void Game::receive( const GameEndMessage &msg )
{
  // This is game end for the current player
  // remove the current player from the game
  disablePlayer( currentPlayer() );
  vector_remove( &mActivePlayers, currentPlayer() );
  if( mActivePlayers.empty() )
  { // End the game entirely if all players' games have ended
    // stop receiving user input
    disconnect();
    mBoardModel->setConfirmationDelay( 0.25f );
    mWaitingForEnd = true;
  }
  else
  { // set index back one so when the turn message comes through it goes to the right player
    --mCurrentPlayerIndex;
    if( mCurrentPlayerIndex < 0 )
    {
      mCurrentPlayerIndex = mActivePlayers.size() - 1;
    }
    // We are no longer listening to the player whose game ended, so end the turn
    mTurnEnded = true;
  }
}

void Game::receive( const GameWarningMessage &msg )
{
  if( msg.getQuantityRemaining() < mWarningChannels.size() )
  {
    mBoardView->pulse( mBoardView->channelDelay( mWarningChannels.at(msg.getQuantityRemaining()), 0.133f ) );
    Sound().cueSound( SoundStation::eWarnEnd, 0.0f );
  }
}

void Game::receive( const WordPlayedMessage &msg )
{
  uint32_t value = mWordEvaluator.getWordValue( msg.getWord(), msg.getSimultaneousMatches() );
  PlayerModel::ScoredWord word( msg.getWord(), value );
  // give points to the latest player (currentPlayer() is invalid in between turns)
  mLatestPlayer->creditWithWord( word );
}

void Game::receive( const PlayerTurnMessage &msg )
{
  if( msg.isTurnEnding() )
  {
    mTurnEnded = true;
    flushTouches();
  }
}

void Game::update()
{
  float currentTime = mTimer.getSeconds();
  float delta = currentTime - mLastUpdate;
  mLastUpdate = currentTime;
  for( auto player : mPlayers ){ player->update( delta ); }
  mBoardView->update( delta );
  if( app::getWindow()->getActiveTouches().empty() && LetterView::timeline().empty() )
  {
    mBoardModel->update( delta, getWordList() );
  }

  for( PlayerViewRef view : mPlayerViews )
  {
    view->update( delta );
  }
  // End of Game
  if( mWaitingForEnd && mBoardModel->hasNothingToDo() && LetterView::timeline().empty() )
  { // if no changes remain to be made and no letters are animating, we are truly finished
    mWaitingForEnd = false;
    // don't advance to next turn if the game is over
    mTurnEnded = false;
    showGameEndScene();
  }
  // End of Turn (multiplayer)
  if( mTurnEnded && mBoardModel->hasNothingToDo() && LetterView::timeline().empty() )
  {
    mTurnEnded = false;
    disablePlayer( currentPlayer() );
    enablePlayer( nextPlayer() );
  }
}

void Game::draw()
{
  mBoardView->draw();

  for( const auto &btn : mPauseButtons )
  {
    btn->draw();
  }

  for( PlayerViewRef view : mPlayerViews )
  {
    view->draw();
  }
}

pk::Locus2dRef Game::readingDirectionBottomLocus(ReadingDirection left_right, ReadingDirection top_bottom)
{
  if( left_right == Negative && top_bottom == Positive )
  {
    return mWestLocus;
  }
  else if( left_right == Negative && top_bottom == Negative )
  {
    return mNorthLocus;
  }
  else if( left_right == Positive && top_bottom == Negative )
  {
    return mEastLocus;
  }
  // ( left_right == Positive && top_bottom == Positive )
  return mSouthLocus;
}

