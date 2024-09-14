//
//  AboutPage.cpp
//  WordShift
//
//  Created by David Wicks on 2/26/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "AboutPage.h"
#include "cinder/System.h"
#include "cinder/Text.h"
#include "GameTypography.h"
#include "GameColor.h"
#include "SoundStation.h"
#include "SpriteNode.h"

using namespace cascade;
using namespace cinder;
using namespace std;

AboutPage::AboutPage( const pk::SpriteSheet &sprite_sheet )
{
  mData.read( getPreferencesPath(), defaultPreferences() );
  Font font = getButtonFont();
  mBackButton = SpriteButtonRef( new SpriteButton( sprite_sheet.getSprite("back-icon") ) );
  mBackButton->expandHitBounds( 6, 6 );
  mBackButton->setSelectFn( [this](){ emitFinish(); } );
  mBackButton->getLocus()->setLoc( Vec2f( 0, 0 ) );

  const int margin = System::isDeviceIpad() ? app::getWindowWidth() / 16 : app::getWindowWidth() / 20;
  int top = mBackButton->getHeight() + margin * 2;

  mScrollPane = ScrollPaneRef( new ScrollPane( app::getWindowSize() - Vec2i( margin * 2, top + margin ) ) );
  mScrollPane->setLoc( Vec2f( margin, top ) );
  mSoundToggle = SpriteToggle::create( sprite_sheet.getSprite("sm-sounds-on")
                                      , sprite_sheet.getSprite("sm-sounds-off")
                                      , [](){ return Sound().enabled(); }
                                      , [this](bool value)->void
                                      { Sound().setEnabled( value );
                                        mData.appendData( "sound", value );
                                      } );
  mSoundToggle->setLoc( Vec2f( 0, 0 ) );
  mScrollPane->appendChild( mSoundToggle );

  auto colors = SpriteNode::create( sprite_sheet.getSprite( "sm-colors" ) );
  colors->setTint( getAltTextColor() );
  colors->setLoc( mSoundToggle->getLoc() + Vec2f( 0, margin + mSoundToggle->getSize().y ) );
  mScrollPane->appendChild( colors );

  int button_width = System::isDeviceIpad() ? margin * 14 : margin * 18;
  Vec2i button_loc( 0, colors->getLoc().y + colors->getHeight() + margin / 2 );

  for( const GamePalette &palette : ColorStation::get().getPalettes() )
  {
    ButtonRef button( new ColorThemeButton( button_width, palette, sprite_sheet ) );
    button->setLoc( button_loc );
    button->setSelectFn( [=](){
      ColorStation::get().setPalette( palette );
      mData.appendData( "color-palette", palette.getName() );
      updateColors();
      colors->setTint( getAltTextColor() );
    } );
    mScrollPane->appendChild( button );

    button_loc.y += button->getHeight() + margin / 8;
  }

  // TODO: set up a toggle button for the sound, set current state from Sound()
  //  mScrollPane->appendChild( sound_toggle );
}

JsonTree AboutPage::defaultPreferences()
{
  JsonTree ret;
  ret.pushBack( JsonTree( "color-palette", "wordshift" ) );
  ret.pushBack( JsonTree( "sound", true ) );
  return ret;
}

fs::path AboutPage::getPreferencesPath()
{
  return getDocumentsDirectory() / "preferences";
}

AboutPage::~AboutPage()
{
  disconnect();
  // TODO: write values into settings
  mData.write( getPreferencesPath() );
}

void AboutPage::connect(ci::app::WindowRef window)
{
  mBackButton->connect( window );
  mScrollPane->deepConnect( window );
}

void AboutPage::customDisconnect()
{
  mBackButton->disconnect();
  mScrollPane->deepDisconnect();
}

void AboutPage::updateColors()
{
  mBackButton->updateColor();
  mSoundToggle->updateColor();
}

void AboutPage::update()
{
  mScrollPane->update();
}

void AboutPage::draw()
{
  mScrollPane->deepDraw();
  mBackButton->draw();
}
