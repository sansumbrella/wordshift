//
//  Menu.cpp
//  WordShift
//
//  Created by David Wicks on 2/21/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Menu.h"


#include "SpriteButton.h"
#include "GameColor.h"
#include "GameTypography.h"
#include "CollectionUtilities.hpp"
#include "cinder/System.h"

using namespace std;
using namespace ci;
using namespace pockets;
using namespace cascade;

Menu::Menu( const pk::SpriteSheet &sprite_sheet ):
mPlayerController( sprite_sheet.getSprite("swap-icon-horizontal"), sprite_sheet.getSprite("swap-icon-vertical") )
{
  mBoardModel = BoardModel::createLogoModel();
  mBoardView = BoardView::create( mBoardModel, sprite_sheet );
  mPlayerController.setLayer( 2 );
  mPlayerController.setCellSize( mBoardView->getCellSize() );
  mBoardView->addOverlay( &mPlayerController );
}

Menu::~Menu()
{
  disconnect();
}

ButtonRef Menu::addButton( const pk::Sprite &sprite, ButtonBase::SelectFn select_fn )
{
  auto button = createButton( sprite, select_fn );
  mRootElement->appendChild( button );
  return button;
}

UIElementRef Menu::addRow()
{
  UIElementRef row{ new UIElement };
  mRootElement->appendChild( row );
  return row;
}

ButtonRef Menu::createButton(const pk::Sprite &sprite, ButtonBase::SelectFn select_fn)
{
  ButtonRef button( new SpriteButton{ sprite } );
  button->expandHitBounds( 6, 2 );
  button->setSelectFn( select_fn );
  return button;
}

void Menu::setButtonIndex(ButtonRef button, uint32_t index)
{
  mRootElement->setChildIndex( button, index );
}

void Menu::connect(ci::app::WindowRef window)
{
  storeConnection( window->getSignalTouchesBegan().connect( [this](app::TouchEvent &event){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this](app::TouchEvent &event){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this](app::TouchEvent &event){ touchesEnded( event ); } ) );
  mRootElement->deepConnect( window );
}

void Menu::customDisconnect()
{
  mRootElement->deepDisconnect();
}

void Menu::touchesBegan(app::TouchEvent &event)
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

void Menu::touchesMoved( app::TouchEvent &event)
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

void Menu::touchesEnded( app::TouchEvent &event)
{
  for( auto &touch : event.getTouches() )
	{
    mPlayerController.apply( touch.getId() );
	}
}

void Menu::autoLayout( int margin )
{
  int y = 0;
  mBoardView->setLoc( Vec2f( margin, margin ) + mBoardView->getTileSize() / 2 );
  int smidgen = mBoardView->getCellSize().x - mBoardView->getTileSize().x;
  Vec2f top_left( margin, mBoardView->getHeight() + margin * 2 - smidgen );
  mRootElement->setLoc( top_left );

  for( UIElementRef button : mRootElement->getChildren() )
  {
    if( mHiddenButtons.count( button ) )
    { // move off screen
      button->setLoc( -button->getSize() - top_left );
    }
    else
    {
      button->setLoc( Vec2f( 0, y ) );
      y += button->totalHeight() + margin / 4;
    }
  }
}

void Menu::hideButton(ButtonRef button)
{
  mHiddenButtons.insert( button );
}

void Menu::showButtonAtIndex(ButtonRef button, uint32_t index)
{
  mHiddenButtons.erase( button );
  setButtonIndex( button, index );
}

void Menu::appear()
{
  Vec2f direction( 1.1f, 0.0f );
  direction.rotate( Rand::randFloat( M_PI * 2 ) );
  mBoardView->appear( 0.66f, direction );
}

void Menu::setGameCenterHandler(std::function<void ()> handler)
{
  mGameCenterHandler = handler;
  mGameCenterInitialized = true;
}

void Menu::update()
{
  if( mGameCenterInitialized && !mGameCenterHandled )
  {
    // thing to do
    mGameCenterHandler();
    mGameCenterHandler = nullptr; // in case it references this
    mGameCenterHandled = true;
  }
  mBoardView->update( 1.0f / 60.0f );
}

void Menu::draw()
{
  mRootElement->deepDraw();
  mBoardView->draw();
}
