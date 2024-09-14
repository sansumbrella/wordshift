//
//  AboutPage.h
//  WordShift
//
//  Created by David Wicks on 2/26/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "SceneController.h"
#include "cinder/gl/Texture.h"
#include "SpriteButton.h"
#include "pockets/SpriteSheet.h"
#include "ColorThemeButton.h"
#include "ScrollPane.h"
#include "SpriteToggle.h"
#include "LocalData.h"

/**

 About page and settings for the application
 Choose color palette

*/

namespace cascade
{
  typedef std::shared_ptr<class AboutPage> AboutPageRef;
	class AboutPage : public SceneController
	{
	public:
		AboutPage( const pk::SpriteSheet &sprite_sheet );
		~AboutPage();
    void                  update();
    void                  draw();
    void                  connect( ci::app::WindowRef window );
    void                  customDisconnect();
    void                  setFinishFn( std::function<void ()> fn ){ mFinishFn = fn; }
    void                  updateColors();
    static ci::JsonTree   defaultPreferences();
    static ci::fs::path   getPreferencesPath();
	private:
    SpriteButtonRef               mBackButton;
    SpriteToggleRef               mSoundToggle;
    std::function<void ()>        mFinishFn;
    ScrollPaneRef                 mScrollPane;
    LocalData                     mData;
    void emitFinish(){ if( mFinishFn ){ mFinishFn(); } }
	};
}
