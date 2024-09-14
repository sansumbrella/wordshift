//
//  TutorialViewController.h
//  WordShift
//
//  Created by David Wicks on 2/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "SceneController.h"
#include "cascade/model/WordList.h"
#include "PlayerMoveController.h"
#include "cascade/model/LetterSpring.h"
#include "cascade/model/BoardModel.h"
#include "cascade/view/BoardView.h"
#include "TextPane.h"
#include "ButtonBase.h"

namespace cascade
{
  typedef std::shared_ptr<class TutorialViewController> TutorialViewControllerRef;
	class TutorialViewController : public SceneController, public Receiver<WordPlayedMessage>
	{
	public:
		TutorialViewController( const pk::SpriteSheet &sprite_sheet );
		~TutorialViewController();
    void appear();
    void update();
    void draw();
    void receive( const WordPlayedMessage &msg );
    void connect( ci::app::WindowRef window );
    void customDisconnect();
    void customPause();
    void customResume();
    void touchesBegan( ci::app::TouchEvent &event );
    void touchesMoved( ci::app::TouchEvent &event );
    void touchesEnded( ci::app::TouchEvent &event );
    void setFinishFn( std::function<void ()> fn ){ mFinishFn = fn; }
	private:
    // touch IDs mapped to controllers gathering points
    PlayerMoveController                mPlayerController;
    LetterSpringRef                     mLetterSpring;
    BoardModelRef                       mBoardModel;
    BoardViewRef                        mBoardView;
    ButtonRef                           mBackButton;
    std::function<void ()>              mFinishFn;
    pk::Sprite                          mHeading;
    ci::Timer                           mTimer;
    float                               mLastUpdate = 0;
    int                                 mWordsMade = 0;
    void emitFinish(){ if( mFinishFn ){ mFinishFn(); } }
    std::map<int, std::vector<std::string>>   mLines;
    std::vector<TextPaneRef>                  mTextPanes;
    void addTutorialLines( int index, const std::vector<std::string> &lines );
    void setup( const pk::SpriteSheet &sprite_sheet );
	};
}
