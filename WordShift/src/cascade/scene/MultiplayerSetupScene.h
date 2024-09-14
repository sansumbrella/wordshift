//
//  MultiplayerSetupScene.h
//  WordShift
//
//  Created by David Wicks on 3/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "SceneController.h"
#include "OptionSelector.h"
#include "ButtonBase.h"
#include "PlayerModel.h"
#include "pockets/SpriteSheet.h"

namespace cascade
{
  /**
  Presents options for different types of multiplayer games.
  On completion, displays player position selector
  */
  typedef std::shared_ptr<class MultiplayerSetupScene> MultiplayerSetupSceneRef;
	class MultiplayerSetupScene : public SceneController
	{
	public:
    typedef std::function<std::vector<PlayerModelRef> (const pk::SpriteSheet&)> PlayerCollectionFn;
    typedef std::function<void (PlayerCollectionFn)> GameStartFn;
    typedef std::function<void ()> MenuFn;
		MultiplayerSetupScene( const pk::SpriteSheet &sprite_sheet );
		~MultiplayerSetupScene();
    void    setGameStartFn( GameStartFn fn ){ mGameStartFn = fn; }
    void    setMenuFn( MenuFn fn ){ mMenuFn = fn; }
    void    draw();
    void    startGame();
    void    connect( ci::app::WindowRef window );
    void    customDisconnect();
    // animation functions called by SceneController
    void    appear();
    void    vanish( Callback finishFn );
    void    layoutViews();
	private:
    //! takes a name and returns a player model
    typedef std::function<PlayerModelRef(const std::string &, ReadingDirection right, ReadingDirection down, const pk::SpriteSheet&)> PlayerFn;
    OptionSelectorRef     mNumPlayersSelector;
    OptionSelectorRef     mGameTypeSelector;
    ButtonRef             mStartButton;
    ButtonRef             mBackButton;
    GameStartFn           mGameStartFn;
    MenuFn                mMenuFn;
	};
}
