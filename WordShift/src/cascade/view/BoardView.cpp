//
//  BoardView.cpp
//  Cascade
//
//  Created by David Wicks on 11/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "BoardView.h"
#include "cinder/Text.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/System.h"
#include "cinder/Timeline.h"
#include "CollectionUtilities.hpp"
#include "BoardTimeline.h"

using namespace cascade;
using namespace ci;
using namespace pockets;
using namespace std;

BoardView::BoardView( BoardModelRef model, const pk::SpriteSheet &sprite_sheet ):
mModel( model )
, mCornerSpriteData( sprite_sheet.getSpriteData("corner") )
, mLineSpriteData( sprite_sheet.getSpriteData( "line-arrow" ) )
{
  if( System::isDeviceIpad() )
  {
    mCellSize.set( 86, 86 );
    mTileSize.set( 80, 80 );
  }
  else
  {
    mCellSize.set( 48, 48 );
    mTileSize.set( 46, 46 );
  }

  auto backing = sprite_sheet.getSprite( "tile-backing" );
  for( auto letter : mModel->getLetters() )
  {
    if( letter )
    {
      Vec2f loc( mCellSize.x * letter->getColumn()
               , mCellSize.y * letter->getRow() );
      auto letterView = LetterView::create( letter, backing );
      letterView->setLoc( loc );
      letterView->setSize( mTileSize );
      mLetters.push_back( letterView );
    }
  }

  boardTimeline().clear();
  boardTimeline().stepTo( 0 );
  LetterView::timeline().clear();
  LetterView::timeline().stepTo( 0 );
}

BoardView::~BoardView()
{}

void BoardView::appear( float duration, const ci::Vec2f &from_direction )
{
  float letter_duration = duration * 0.75f;
  float min_delay = 0.0f;
  float max_delay = duration - letter_duration;
  Vec2f size( getCenteringWidth(), getCenteringHeight() );
  Vec2f center = size / 2;
  float maxDistanceSquared = size.distanceSquared( center );
  for( LetterViewRef letter : mLetters )
  {
    float t = letter->getLoc().distanceSquared( center ) / maxDistanceSquared;
    float delay = lerp( min_delay, max_delay, easeOutCubic( t ) );
    letter->appear( from_direction, letter_duration, delay );
  }
}

void BoardView::pulse( DelayFn delay_fn )
{
  for( LetterViewRef &letter : mLetters )
  {
    letter->pulse( delay_fn( letter ) );
  }
}

BoardView::DelayFn BoardView::channelDelay(ci::Channel8u channel, float total_delay)
{
  return [=]( LetterViewRef letter )->float
  {
    float value = 1.0f - *channel.getData(letter->getModel()->getColumn(), letter->getModel()->getRow()) / 255.0f;
    return lerp( 0.0f, total_delay, value );
  };
}

BoardView::DelayFn BoardView::leftRightDelay(float total_delay)
{
  int low = 0;
  int high = mModel->getColumnCount() - 1;
  return [=]( LetterViewRef letter )->float
  {
    return lmap<float>( letter->getModel()->getColumn(), low, high, 0.0f, total_delay );
  };
}

BoardView::DelayFn BoardView::rightLeftDelay(float total_delay)
{
  int low = mModel->getColumnCount() - 1;
  int high = 0;
  return [=]( LetterViewRef letter )->float
  {
    return lmap<float>( letter->getModel()->getColumn(), low, high, 0.0f, total_delay );
  };
}

BoardView::DelayFn BoardView::topBottomDelay(float total_delay)
{
  int low = 0;
  int high = mModel->getRowCount() - 1;
  return [=]( LetterViewRef letter )->float
  {
    return lmap<float>( letter->getModel()->getRow(), low, high, 0.0f, total_delay );
  };
}

BoardView::DelayFn BoardView::bottomTopDelay(float total_delay)
{
  int low = mModel->getRowCount() - 1;
  int high = 0;
  return [=]( LetterViewRef letter ){
    return lmap<float>( letter->getModel()->getRow(), low, high, 0.0f, total_delay );
  };
}

BoardView::DelayFn BoardView::centerOutDelay(float total_delay)
{
  Vec2f center = Vec2f( mModel->getColumnCount() - 1, mModel->getRowCount() - 1 ) / 2;
  float max_distance = center.length();
  return [=]( LetterViewRef letter ){
    Vec2f pos( letter->getModel()->getColumn(), letter->getModel()->getRow() );
    return lmap<float>( pos.distance( center ), 0, max_distance, 0.0f, total_delay );
  };
}

BoardView::DelayFn BoardView::closeOnCenterDelay(float total_delay)
{
  Vec2f center = Vec2f( mModel->getColumnCount() - 1, mModel->getRowCount() - 1 ) / 2;
  float max_distance = center.length();
  return [=]( LetterViewRef letter ){
    Vec2f pos( letter->getModel()->getColumn(), letter->getModel()->getRow() );
    return lmap<float>( pos.distance( center ), max_distance, 0, 0.0f, total_delay );
  };
}

LetterRef BoardView::getLetter( const Vec2f &loc )
{
	for( LetterViewRef letter : mLetters )
	{
		if( letter->contains( loc - getLoc() ) )
		{
			return letter->getModel();
		}
	}
	return LetterRef();
}

void BoardView::receive( const WordFoundMessage &message )
{
  WordViewRef view( new WordView( message.getWord(), mModel->getLeftRight(), mModel->getTopBottom(), mCellSize, mCornerSpriteData, mLineSpriteData ) );
  mWords.push_back( view );
  view->connectToRenderer( &mOverlayRenderer );
  mOverlayRenderer.sort();
}

void BoardView::addOverlay(pk::SimpleRenderer::IRenderable *overlay)
{
  mOverlayRenderer.add( overlay );
  mOverlayRenderer.sort();
}

void BoardView::update( float deltaTime )
{
  boardTimeline().step( deltaTime );
  LetterView::timeline().step( deltaTime );
  for( LetterViewRef letter : mLetters )
  {
    letter->update();
  }
  for( WordViewRef word : mWords )
  {
    word->update();
  }
  vector_erase_if( &mWords, [](WordViewRef w){ return w->isDead(); } );
  vector_erase_if( &mSwapViews, [](SwapViewRef v){ return v->isDead(); } );
}

void BoardView::updateFromModel()
{
  for( LetterViewRef letter : mLetters )
  {
    letter->updateFromModel();
  }
}

void BoardView::setLetterRotation(float radians)
{
  float minDelay = 0.0f;
  float maxDelay = 0.22f;
  Vec2f size( getCenteringWidth(), getCenteringHeight() );
  Vec2f center = size / 2;
  float maxDistanceSquared = size.distanceSquared( center );
  for( LetterViewRef letter : mLetters )
  {
    float t = letter->getLoc().distanceSquared( center ) / maxDistanceSquared;
    float delay = lerp( minDelay, maxDelay, easeOutCubic( t ) );
    letter->setRotation( radians, delay );
  }
}

void BoardView::setLocSoft(const ci::Vec2f &loc)
{
  boardTimeline().appendTo( &mOffsetAnim, loc, 0.5f )
                    .easeFn( EaseInOutQuart() )
                    .updateFn( [this](){ mLocus->setLoc( mOffsetAnim ); } );
}

Rectf BoardView::getTileBounds() const
{
  return Rectf( getLoc() - mTileSize / 2, getLoc() + (mTileSize / 2) + Vec2f( getCenteringWidth(), getCenteringHeight() ) );
}

Rectf BoardView::getCellBounds() const
{
  return Rectf( getLoc() - mCellSize / 2, getLoc() + (mCellSize / 2) + Vec2f( getWidth(), getHeight() ) );
}

void BoardView::draw()
{
  gl::pushModelView();
  gl::multModelView( *mLocus );

	for( LetterViewRef letter : mLetters )
	{
		letter->draw();
	}
  mOverlayRenderer.render();

  gl::popModelView();
}



