#include "cinder/app/AppNative.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Filesystem.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"

#include "Game.h"
#include "MultiplayerSetupScene.h"
#include "HouseRules.h"
#include "TimeLimitrules.h"
#include "SwapLimitRules.h"
#include "HardRules.h"
#include "Menu.h"
#include "TutorialViewController.h"
#include "AboutPage.h"
#include "GameColor.h"
#include "CollectionUtilities.hpp"

#include "CascadeGameCenterDelegate.h"
#include "SoundStation.h"
#include "LocalData.h"
#include "GameTypography.h"

#include <boost/algorithm/string/case_conv.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;
using namespace cascade;

/*
 A word game

 App connects different components to the render/ui framework as needed.
 */

class WordShiftApp : public AppNative
{
public:
  void prepareSettings( Settings *settings );
	void setup();
  //! build and start tutorial
  void showTutorial();
  //! build and start a new game; pass by value for replay lambda
  typedef function<vector<PlayerModelRef> (const pk::SpriteSheet&)> PlayerFn;
  void startGame( PlayerFn players, ci::Vec2i entry_direction );
  //! display screen for configuring multiplayer
  void showMultiplayerOptions();
  //! present apple's Game Center page in app
  void showGameCenter();
  //! build and display main menu
  void showMainMenu();
  //! build and display about page
  void showAboutPage();
  //! show a view and make it active
  void showView( SceneControllerRef view );
  //! hide all views; destroying on completion
  void hideViews();
  // TODO: check whether full game was purchased
  bool purchased(){ return false; }
  //! load content needed for play if it's not already in memory
  void loadData();
  // called by loadData
  void loadSpriteSheet();
  // unloads content needed for play if game is in background
  void unloadData();
  // load sound and color preferences from disk
  void      loadPreferences();
  // default sound and color settings
  JsonTree  defaultPreferences();
private:
  vector<SceneControllerRef>  mActiveSceneControllers;
  vector<SceneControllerRef>  mVanishingSceneControllers;
  SpriteSheetRef              mSpriteSheet;
  std::function<void ()>      mOnGameCenter = nullptr;
  bool                        mPalettesLoaded = false;
  bool                        mActive = true;
};

void WordShiftApp::prepareSettings( Settings *settings )
{
  settings->enableMultiTouch( true );
  settings->setResizable( false );
  settings->enableHighDensityDisplay();
}

void WordShiftApp::setup()
{
  mOnGameCenter = nullptr;
  getGameCenter().authenticatePlayer([this]( bool success ){
    getGameCenter().loadPlayerData(); // loading on a separate thread
    if( success )
    { // if there is an authenticated player, show GameCenter info (does not mean we're online)
      if( mOnGameCenter )
      {
        mOnGameCenter();
        mOnGameCenter = nullptr;
      }
    }
  });
  loadData();
  loadPreferences();
  { // create and destroy a tutorial to get some memory from the OS
    // I'm guessing this is why some folks write their own allocators
    TutorialViewControllerRef tutorial( new TutorialViewController( *mSpriteSheet ) );
  }
  getSignalDidBecomeActive().connect( [this]()
                                     { // whenever we gain focus
                                       mActive = true;
                                       // reload data in case it was ditched in low-memory warning
                                       loadData();
                                       for( auto scene : mActiveSceneControllers ){ scene->resume(); }
                                       Sound().connect();
                                     } );
  getSignalWillResignActive().connect( [this]()
                                      { // Whenever we lose focus
                                        mActive = false;
                                        for( auto scene : mActiveSceneControllers ){ scene->pause(); }
                                        getGameCenter().resignActive();
                                        Sound().disconnect();
                                      } );
  getSignalMemoryWarning().connect( [this]()
                                   { // unload whatever data we can (about 1MB of dictionary and 9MB of sprites)
                                     unloadData();
                                   });
  // clear background before drawing any scene
  getWindow()->getSignalDraw().connect( -1
                                       , [this](){
                                         gl::clear( getBackgroundColor() );
                                         mSpriteSheet->enableAndBind();
                                       } );
  showMainMenu();
}

void WordShiftApp::loadData()
{
  if( getWordList().empty() )
  {
    getWordList().loadWordData( getResourcePath( fs::path("words") ), 4, 9 );
  }
  if( !mSpriteSheet )
  {
    loadSpriteSheet();
  }
  if( !mPalettesLoaded )
  {
    mPalettesLoaded = true;
    ColorStation::get().loadPalettes( getResourcePath("img/colors.json") );
  }
  getHeadingTextureFont();
  getBodyTextureFont();
  SoundStation::get().preload();
}

void WordShiftApp::unloadData()
{
  if( !mActive )
  { // release the word list
    getWordList().clear();
    mSpriteSheet.reset();
    unloadHeadingTextureFont();
    unloadBodyTextureFont();
    SoundStation::get().unload();
  }
}

void WordShiftApp::loadSpriteSheet()
{
  if( System::isDeviceIpad() )
  {
    mSpriteSheet = pk::SpriteSheet::load( getResourcePath("sprites/sprite_sheet_ipad.json") );
  }
  else if( System::isDeviceIphone() )
  {
    mSpriteSheet = pk::SpriteSheet::load( getResourcePath("sprites/sprite_sheet_iphone.json") );
  }
  string alphabet = "abcdefghijklmnopqrstuvwxyz";
  for( char c : alphabet )
  {
    string s;
    s.assign( 1, c );
    boost::algorithm::to_upper( s );
    LetterView::letterSprites()[c] = mSpriteSheet->getSpriteData( s );
  }
}

void WordShiftApp::loadPreferences()
{
  LocalData data;
  data.read( AboutPage::getPreferencesPath(), AboutPage::defaultPreferences() );
  ColorStation::get().setPalette( data.getDataValue<string>( "color-palette" ) );
  Sound().setEnabled( data.getDataValue<bool>("sound") );
}

void WordShiftApp::showView(SceneControllerRef view)
{
  view->connect( getWindow() );
  view->show( getWindow() );
  mActiveSceneControllers.push_back( view );
}

void WordShiftApp::hideViews()
{
  for( SceneControllerRef v : mActiveSceneControllers )
  {
    // put into vanishing controllers, to be removed after hide completes...
    mVanishingSceneControllers.push_back( v );
    v->disconnect();
    v->hide( [this, v](){ vector_remove( &mVanishingSceneControllers, v ); });
  }
  mActiveSceneControllers.clear();
}

void WordShiftApp::showTutorial()
{
  TutorialViewControllerRef tutorial( new TutorialViewController( *mSpriteSheet ) );
  tutorial->setFinishFn( [this](){ showMainMenu(); } );

  hideViews();
  showView( tutorial );
}

void WordShiftApp::startGame( PlayerFn player_fn, Vec2i entry_direction )
{
  auto players = player_fn( *mSpriteSheet );
  int rows = 9;
  int columns = 6;
  // prepare major game objects
  GameRef game( new Game( *mSpriteSheet, rows, columns, players ) );
  game->setAppearDirection( entry_direction );
  game->setGameOverFn( [this](){ showMainMenu(); } );
  // call this function with the same arguments to replay the same game
  game->setReplayFn( [=](){ startGame( player_fn, entry_direction ); } );

  hideViews();
  showView( game );
}

void WordShiftApp::showMainMenu()
{
  auto speed_fn = [this](const pk::SpriteSheet &sprite_sheet) -> vector<PlayerModelRef>
  {
    TimeLimitRulesRef time_limit( new TimeLimitRules( sprite_sheet, 45 ) );  // 45
    return { PlayerModelRef( new PlayerModel( "", Positive, Positive, time_limit ) ) };
  };
  auto strategy_fn = [this](const pk::SpriteSheet &sprite_sheet) -> vector<PlayerModelRef>
  {
    SwapLimitRulesRef move_limit( new SwapLimitRules( sprite_sheet, 50 ) ); // 50
    return { PlayerModelRef( new PlayerModel( "", Positive, Positive, move_limit ) ) };
  };
  auto hard_fn = [this](const pk::SpriteSheet &sprite_sheet) -> vector<PlayerModelRef>
  {
    shared_ptr<HardRules> hard_rules( new HardRules( sprite_sheet, 45, 50 ) );
    return { PlayerModelRef( new PlayerModel( "", Positive, Positive, hard_rules ) ) };
  };

  MenuRef mainMenu = Menu::create( *mSpriteSheet );
  const int margin = System::isDeviceIpad() ? getWindowWidth() / 16 : getWindowWidth() / 20;
  mainMenu->addButton( mSpriteSheet->getSprite("mm-tutorial")   , [this](){ showTutorial(); } );
  mainMenu->addButton( mSpriteSheet->getSprite("mm-strategy") , [=](){ startGame( strategy_fn, Vec2i( 0, -1 ) ); } );
  mainMenu->addButton( mSpriteSheet->getSprite("mm-speed")    , [=](){ startGame( speed_fn, Vec2i( -1, 0 ) ); } );
  mainMenu->addButton( mSpriteSheet->getSprite("mm-hard")   , [=](){ startGame( hard_fn, Vec2i( -1, -1 ) ); } );
  mainMenu->addButton( mSpriteSheet->getSprite("mm-multiplayer")   , [this](){ showMultiplayerOptions(); } );
  auto settings = mainMenu->createButton( mSpriteSheet->getSprite("mm-settings")         , [this](){ showAboutPage(); } );
  auto gc = mainMenu->createButton( mSpriteSheet->getSprite("mm-gamecenter"), [this](){ showGameCenter(); } );
  auto bottom_row = mainMenu->addRow();
  bottom_row->appendChild( settings );
  gc->setLoc( Vec2f{ settings->getSize().x + margin / 4, 0 } );
  // game center
  if( getGameCenter().isEnabled() )
  { // add button to menu flow
    bottom_row->appendChild( gc );
  }
  else
  {
    weak_ptr<Menu> menu = mainMenu;
    mOnGameCenter = [menu, bottom_row, gc]()
    {
      auto m = menu.lock();
      if( m )
      { // this is a bit ugly in order to make sure it's thread safe
        m->setGameCenterHandler( [bottom_row, gc]() -> void
                                {
                                  // connect game center to UI events
                                  bottom_row->appendChild( gc );
                                  gc->connect( app::getWindow() );
                                }
                                );
      }
    };
  }
  mainMenu->setBounds( getWindowBounds() );
  mainMenu->autoLayout( margin );

  hideViews();
  showView( mainMenu );
}

void WordShiftApp::showMultiplayerOptions()
{
  MultiplayerSetupSceneRef scene( new MultiplayerSetupScene( *mSpriteSheet ) );
  scene->setGameStartFn( [this]( PlayerFn player_fn ){ startGame( player_fn, Vec2i( 0, 1 ) ); } );
  scene->setMenuFn( [this](){ showMainMenu(); } );
  hideViews();
  showView( scene );
}

void WordShiftApp::showAboutPage()
{
  AboutPageRef about( new AboutPage( *mSpriteSheet ) );
  about->setFinishFn( [this](){ showMainMenu(); } );
  hideViews();
  showView( about );
}

void WordShiftApp::showGameCenter()
{
  // game center only shows if enabled
  getGameCenter().showGameCenter();
}

CINDER_APP_NATIVE( WordShiftApp, RendererGl( RendererGl::AA_NONE ) )
