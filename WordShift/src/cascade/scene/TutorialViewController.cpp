//
//  TutorialViewController.cpp
//  WordShift
//
//  Created by David Wicks on 2/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TutorialViewController.h"
#include "GameTypography.h"
#include "SpriteButton.h"
#include "cinder/Timeline.h"
#include "cinder/System.h"

using namespace cascade;
using namespace cinder;
using namespace std;

TutorialViewController::TutorialViewController( const pk::SpriteSheet &sprite_sheet ):
mPlayerController( sprite_sheet.getSprite("swap-icon-horizontal"), sprite_sheet.getSprite("swap-icon-vertical") )
{
  addTutorialLines( 2, { "Drag letters above to make words.", "Try spelling \u201CWord\u201D or \u201CGame.\u201D" } );
  addTutorialLines( 3, { "Restrictions:", "- Words must be 4+ letters long.", "- You can\u2019t re-submit words.", "(Make a word to continue)" } );
  addTutorialLines( 4, { "Restrictions:", "- Proper nouns don't count.", "- Nor do slurs and curses." } );
  addTutorialLines( 5, { "That covers the basics!", "Up next: a few tips." } );
  addTutorialLines( 6, { "For score multipliers:", "- Make longer words.", "- Use uncommon letters.", "- Make many words at once." } );
  addTutorialLines( 7, { "Control:", "Put your finger down to", "stop word submission." } );
  addTutorialLines( 8, { "Control:", "Words are submitted in order", "based on when they were found." } );

  if( System::isDeviceIphone() )
  {
    addTutorialLines( 9, { "Strategy mode:", "Longer words give bonus swaps." } );
    addTutorialLines( 10, { "Speed mode:", "Longer words give bonus time." } );
    addTutorialLines( 11, { "Bonuses:", "To get bigger time/swap bonuses,", "make the longest word possible." } );
    addTutorialLines( 12, { "That\u2019s it!" } );
  }
  else
  { // iPad doesn't need strategy/speed tips (already on main screen)
    addTutorialLines( 9, { "Bonuses:", "To get bigger time/swap bonuses,", "make the longest word possible." } );
    addTutorialLines( 10, { "That\u2019s it!" } );
  }
  setup( sprite_sheet );
}

TutorialViewController::~TutorialViewController()
{
  disconnect();
}

void TutorialViewController::addTutorialLines(int index, const std::vector<std::string> &lines)
{
  mLines[index] = lines;
}

void TutorialViewController::setup( const pk::SpriteSheet &sprite_sheet )
{
  mLetterSpring = LetterSpring::create();
	mLetterSpring->setupWithLetters( app::App::getResourcePath( fs::path( "words/tutorial-words-scrambled" ) ) );
  mBoardModel = BoardModel::create( mLetterSpring, 2, 4 );
  mBoardModel->setMinWordLength( 4 ); // only accept 4-letter words in tutorial
  mBoardModel->setConfirmationDelay( 1.25f );
  mBoardModel->appendWordPlayedReceiver( this );
	mBoardView = BoardView::create( mBoardModel, sprite_sheet );
  mPlayerController.setLayer( 2 );
  mPlayerController.setCellSize( mBoardView->getCellSize() );
  mBoardView->addOverlay( &mPlayerController );

  mBackButton = ButtonRef(new SpriteButton( sprite_sheet.getSprite("tm-continue") ));
  mBackButton->expandHitBounds( 6, 6 );
  mBackButton->setSelectFn( [this](){ emitFinish(); } );

  mHeading = sprite_sheet.getSprite( "tu-heading" );

  const float margin = System::isDeviceIpad() ? app::getWindowWidth() / 16.0f : app::getWindowWidth() / 20.0f;
  const int pane_width = app::getWindowWidth() - margin * 2;

  mHeading.setLoc( Vec2f( margin, margin * 2 ) );
  mHeading.setTint( getAltTextColor() );
  Vec2f board_position( app::getWindowCenter().x - mBoardView->getCenteringWidth() / 2
                       , mHeading.getLoc().y + mHeading.getSize().y + margin * 2 + mBoardView->getCellSize().y / 2 );
  mBoardView->setLoc( board_position );

  Vec2f pane_loc( margin, board_position.y + margin + mBoardView->getHeight() - mBoardView->getCellSize().y / 2 );
  for( int i = 0; i < 4; ++i )
  {
    TextPaneRef pane( new TextPane{ getButtonFont(), pane_width } );
    pane->setText( " " );
    pane->setLoc( pane_loc );
    mTextPanes.push_back( pane );
    pane_loc += Vec2f( 0, pane->getHeight() );
  }

  float button_y = app::getWindowHeight() - mBackButton->getSize().y - (margin * 2);
  mBackButton->getLocus()->setLoc( Vec2f( margin
                                         , (int)button_y ) );
  mBoardModel->forceCompleteCheck();
  mTimer.start();
}

void TutorialViewController::connect(ci::app::WindowRef window)
{
  storeConnection( window->getSignalTouchesBegan().connect( [this](app::TouchEvent &event){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this](app::TouchEvent &event){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this](app::TouchEvent &event){ touchesEnded( event ); } ) );
  mBackButton->connect( window );
}

void TutorialViewController::customDisconnect()
{
  mBackButton->disconnect();
}

void TutorialViewController::appear()
{
  mBoardView->appear( 0.66f, Vec2i( 0, 1 ) );
}

void TutorialViewController::touchesBegan(app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
	{
		auto letter = mBoardView->getLetter( touch.getPos() );
    if( letter )
    {
      mPlayerController.updateWithLetter( touch.getId(), letter );
    }
	}
}

void TutorialViewController::touchesMoved( app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
	{
		auto letter = mBoardView->getLetter( touch.getPos() );
    if( letter )
    {
      mPlayerController.updateWithLetter( touch.getId(), letter );
    }
	}
}

void TutorialViewController::touchesEnded( app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
	{
    mPlayerController.apply( touch.getId() );
	}
}

void TutorialViewController::customPause()
{
  mTimer.stop();
}

void TutorialViewController::customResume()
{
  mLastUpdate = 0;
  mTimer.start();
}

void TutorialViewController::receive(const cascade::WordPlayedMessage &msg)
{ // go to the next item in our tutorial
  ++mWordsMade;
  auto lines = mLines[mWordsMade];
  auto prev_lines = mLines[mWordsMade - 1];
  float delay = 0.0f;
  int i = 0;
  for( auto &pane : mTextPanes )
  {
    if( i < lines.size() )
    {
      pane->setTextSoft( lines.at( i ), delay );
    }
    else if( i < prev_lines.size() )
    { // there was a line previously
      pane->setTextSoft( " ", delay );
    }
    ++i;
    delay += 0.1f;
  }
}

void TutorialViewController::update()
{
  float currentTime = mTimer.getSeconds();
  float delta = currentTime - mLastUpdate;
  mLastUpdate = currentTime;
  for( auto &pane : mTextPanes )
  {
    pane->update( delta );
  }

  mBoardView->update( delta );
  if( app::getWindow()->getActiveTouches().empty() && LetterView::timeline().empty() )
  {
    mBoardModel->update( delta, cascade::getWordList() );
  }
  //  mBoardModel->getPlayedWords().clear();
}

void TutorialViewController::draw()
{
  // draw text explanation
  gl::color( getAltTextColor() );
  for( auto &pane : mTextPanes )
  {
    pane->draw();
  }

  // draw interactive bit
  mHeading.render();
  mBoardView->draw();
  mBackButton->draw();
}

