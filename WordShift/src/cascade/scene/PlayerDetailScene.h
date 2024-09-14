//
//  PlayerDetailScene.h
//  WordShift
//
//  Created by David Wicks on 3/7/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "SceneController.h"
#include "ButtonBase.h"
#include "PlayerModel.h"
#include "cinder/Timeline.h"
#include "GameTypography.h"
#include "ScrollPane.h"

namespace cascade
{
  typedef std::shared_ptr<class PlayerDetailScene> PlayerDetailSceneRef;
  /**
  Shows details about player's score and played words.
  Displayed when game is paused and at game end.
  */
	class PlayerDetailScene : public SceneController
	{
	public:
		PlayerDetailScene( std::vector<PlayerModelRef> players, const pk::SpriteData &resume_icon, const pk::SpriteData &quit_icon );
		~PlayerDetailScene();
    void connect( ci::app::WindowRef window );
    void customDisconnect();
    void setBackFunction( std::function<void ()> fn ){ mBackFn = fn; }
    void setQuitFunction( std::function<void ()> fn ){ mQuitFn = fn; }
    void appear();
    void vanish( Callback finishFn );
    void update();
    void draw();
	private:
    struct TextBlock
    {
      FontType                type;
      std::string             text;
      int                     spacing;
    };
    std::string createPlayerHeading( PlayerModelRef player );
    std::string createPlayerBody( PlayerModelRef player, int text_width, float scale );
    std::string createGameDescription( const std::vector<PlayerModelRef> &players );
    pk::Locus2dRef          mLocus = pk::Locus2dRef( new pk::Locus2d() );
    ButtonRef               mBackButton;
    ButtonRef               mQuitButton;
    ScrollPaneRef           mScrollPane;
    ci::Anim<ci::Vec2f>     mSlidePosition;
    ci::Vec2f               mUpperLeft;
    ci::Vec2f               mLowerLeft;
    ci::Vec2f               mScreenSize;
    ci::TimelineRef         mTimeline = ci::Timeline::create();
    std::function<void ()>  mBackFn;
    std::function<void ()>  mQuitFn;
    void emitBack(){ if( mBackFn ){ mBackFn(); } }
    void emitQuit(){ if( mQuitFn ){ mQuitFn(); } }
	};
}
