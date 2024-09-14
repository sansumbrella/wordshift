//
//  MultiplayerSetupScene.cpp
//  WordShift
//
//  Created by David Wicks on 3/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "MultiplayerSetupScene.h"
#include "MultiplayerTimeLimitRules.h"
#include "MultiplayerSwapLimitRules.h"
#include "HouseRules.h"
#include "OrientationHelpers.h"
#include "SimpleButton.h"
#include "SpriteButton.h"
#include "CascadeGameCenterDelegate.h"

#include "cinder/Timeline.h"
#include "cinder/System.h"
#include "GameTypography.h"
#include "cinder/System.h"
#include "GameColor.h"

using namespace cascade;
using namespace cinder;
using namespace std;

struct PlayersWithOrientation
{
  GameOrientation orientation;
  uint32_t        players;
};

MultiplayerSetupScene::MultiplayerSetupScene( const pk::SpriteSheet &sprite_sheet )
{
  mNumPlayersSelector = OptionSelectorRef( new OptionSelector( sprite_sheet.getSprite("mp-players") ) );
  mGameTypeSelector = OptionSelectorRef( new OptionSelector( sprite_sheet.getSprite("mp-rules") ) );

  PlayerFn score_rules_fn = [](const string &name, ReadingDirection right, ReadingDirection down, const pk::SpriteSheet &sprite_sheet)->PlayerModelRef
  { // six turns, 20 seconds
    MultiplayerTimeLimitRulesRef rules( new MultiplayerTimeLimitRules( sprite_sheet, 20.0f, 6 ) );
    return PlayerModelRef( new PlayerModel( name, right, down, rules ) );
  };

  PlayerFn strategy_rules_fn = [](const string &name, ReadingDirection right, ReadingDirection down, const pk::SpriteSheet &sprite_sheet)->PlayerModelRef
  { // nine turns, nine moves
    MultiplayerSwapLimitRulesRef rules( new MultiplayerSwapLimitRules( sprite_sheet, 9, 9 ) );
    return PlayerModelRef( new PlayerModel( name, right, down, rules ) );
  };
  PlayerFn house_rules_fn = [sprite_sheet](const string &name, ReadingDirection right, ReadingDirection down, const pk::SpriteSheet &sprite_sheet)->PlayerModelRef
  { // No limit to play
    HouseRulesRef rules( new HouseRules( sprite_sheet ) );
    return PlayerModelRef( new PlayerModel( name, right, down, rules ) );
  };

  mNumPlayersSelector->addOption( PlayersWithOrientation( { GameOrientation::Portrait, 2 } ), sprite_sheet.getSprite("mp-two-portrait") );
  if( System::isDeviceIpad() )
  {
    mNumPlayersSelector->addOption( PlayersWithOrientation( { GameOrientation::Landscape, 2 } ), sprite_sheet.getSprite("mp-two-landscape") );
    mNumPlayersSelector->addOption( PlayersWithOrientation( { GameOrientation::Portrait, 3 } ), sprite_sheet.getSprite("mp-three") );
    mNumPlayersSelector->addOption( PlayersWithOrientation( { GameOrientation::Portrait, 4 } ), sprite_sheet.getSprite("mp-four") );
  }
  mNumPlayersSelector->setDefaultOptionIndex( mNumPlayersSelector->numOptions() - 1 );

  mGameTypeSelector->addOption( strategy_rules_fn, sprite_sheet.getSprite("mp-strategy"), true );
  mGameTypeSelector->addOption( score_rules_fn, sprite_sheet.getSprite("mp-race"), true );
  mGameTypeSelector->addOption( house_rules_fn, sprite_sheet.getSprite("mp-house-rules"), true );
  mGameTypeSelector->setDefaultOptionIndex( 0 );

  const bool is_ipad = System::isDeviceIpad();
  const float padding = is_ipad ? app::getWindowWidth() / 16 : app::getWindowWidth() / 20;
  const OptionSelector::Layout option_layout = is_ipad ? OptionSelector::HEADER_STACKED : OptionSelector::HEADER_INLINE;
  float right_edge = app::getWindowWidth() - padding;
  float left_edge = is_ipad ? 0 : padding * 7;
  mNumPlayersSelector->layoutHorizontallyWithPadding( padding, padding / 2, left_edge, right_edge, option_layout );
  mGameTypeSelector->layoutHorizontallyWithPadding( padding / 2, padding / 2, left_edge, right_edge, option_layout );

  mBackButton = ButtonRef( new SpriteButton( sprite_sheet.getSprite("back-icon") ) );
  mBackButton->expandHitBounds( 6, 6 );
  mBackButton->setSelectFn( [this](){ if( mMenuFn ){ mMenuFn(); } } );
  mStartButton = ButtonRef( new SpriteButton( sprite_sheet.getSprite("mp-start") ) );
  mStartButton->expandHitBounds( 6, 6 );
  mStartButton->setSelectFn( [this](){ startGame(); } );

}

MultiplayerSetupScene::~MultiplayerSetupScene()
{
  disconnect();
}

void MultiplayerSetupScene::connect(ci::app::WindowRef window)
{
  mStartButton->connect( window );
  mBackButton->connect( window );
  mGameTypeSelector->connect( window );
  mNumPlayersSelector->connect( window );
}

void MultiplayerSetupScene::customDisconnect()
{
  mStartButton->disconnect();
  mBackButton->disconnect();
  mGameTypeSelector->disconnect();
  mNumPlayersSelector->disconnect();
}

void MultiplayerSetupScene::appear()
{
  layoutViews();
}

void MultiplayerSetupScene::vanish(Callback finishFn)
{
  finishFn();
}

void MultiplayerSetupScene::layoutViews()
{
  Rectf bounds = getBounds();

  //  float padding = System::isDeviceIpad() ? 48.0f : 36.0f;
  float padding = System::isDeviceIpad() ? bounds.getWidth() / 16 : bounds.getWidth() / 20;
  float top_y = mBackButton->getHeight() + padding * 2;
  Vec2f loc( padding, top_y );

  mNumPlayersSelector->setLoc( loc );
  loc.y += mNumPlayersSelector->getHeight() + padding;
  mGameTypeSelector->setLoc( loc );
  loc.y += mGameTypeSelector->getHeight() + padding * 2;
  mStartButton->setLoc( loc );

  //  float remaining_height = bounds.getY2() - loc.y - mStartButton->getHeight();
  //  loc.y = loc.y + remaining_height * 0.5f;
}

void MultiplayerSetupScene::draw()
{
  mNumPlayersSelector->draw();
  mGameTypeSelector->draw();
  mStartButton->draw();
  mBackButton->draw();
}

void MultiplayerSetupScene::startGame()
{
  PlayersWithOrientation pwo = mNumPlayersSelector->getValue<PlayersWithOrientation>();
  PlayerFn make_player = mGameTypeSelector->getValue<PlayerFn>();

  PlayerCollectionFn fn = [pwo, make_player](const pk::SpriteSheet &sprite_sheet){
    vector<PlayerModelRef> players;
    const GameOrientation orientation = pwo.orientation;
    const int num_players = pwo.players;
    vector<ReadingDirectionPair> directions = getPlayerReadingDirections( num_players, orientation );
    vector<string> names = { "Player One", "Player Two", "Player Three", "Player Four" };
    for( int i = 0; i < num_players; ++i )
    {
      PlayerModelRef player = make_player( names[i], directions[i].right, directions[i].down, sprite_sheet );
      players.push_back( player );
    }
    return players;
  };
  
  mGameStartFn( fn );
}
