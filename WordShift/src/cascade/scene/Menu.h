//
//  Menu.h
//  WordShift
//
//  Created by David Wicks on 2/21/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "SceneController.h"
#include "ButtonBase.h"

#include "BoardView.h"
#include "PlayerMoveController.h"

namespace cascade
{
  typedef std::shared_ptr<class Menu> MenuRef;
  /**
  Menu showing list of choices
  Can add buttons and automatically lay them out.
  TODO: optionally animate new buttons into position (say, when game center is authenticated)
  */
	class Menu : public SceneController
	{
	public:
		~Menu();
    ButtonRef addButton( const pk::Sprite &sprite, ButtonBase::SelectFn select_fn );
    UIElementRef addRow();
    ButtonRef createButton( const pk::Sprite &sprite, ButtonBase::SelectFn select_fn );
    //! move a button to new index (requires relayout to take effect)
    void setButtonIndex( ButtonRef button, uint32_t index );
    void connect( ci::app::WindowRef window );
    void customDisconnect();

    void touchesBegan( ci::app::TouchEvent &event );
    void touchesMoved( ci::app::TouchEvent &event );
    void touchesEnded( ci::app::TouchEvent &event );

    void autoLayout( int margin );
    void appear();
    void update();
    void draw();
    // animate menu positions to put button here
    void showButtonAtIndex( ButtonRef button, uint32_t index );
    // move button offscreen and ignore for flow
    void hideButton( ButtonRef button );
    //! call from other thread, sets thing to do in main thread when time is right
    void setGameCenterHandler( std::function<void ()> handler );
    static MenuRef create( const pk::SpriteSheet &sprite_sheet ){ return MenuRef( new Menu( sprite_sheet ) ); }
	private:
		Menu( const pk::SpriteSheet &sprite_sheet );
    UIElementRef                        mRootElement = UIElementRef{ new UIElement };
    std::set<UIElementRef>              mHiddenButtons;
    PlayerMoveController                mPlayerController;
    BoardModelRef                       mBoardModel;
    BoardViewRef                        mBoardView;
    bool                                mGameCenterHandled = false;
    bool                                mGameCenterInitialized = false; // set on other thread
    std::function<void ()>              mGameCenterHandler;             // set on other thread
	};
}
