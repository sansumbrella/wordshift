//
//  SwapView.cpp
//  WordShift
//
//  Created by David Wicks on 4/27/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SwapView.h"
#include "GameColor.h"

using namespace cascade;
using namespace cinder;
using namespace pockets;

SwapView::SwapView( const Vec2i &cell_size, const Sprite &horizontal, const Sprite &vertical ):
mCellSize( cell_size )
, mHorizontalSprite( horizontal )
, mVerticalSprite( vertical )
{}

SwapView::~SwapView()
{}

void SwapView::receive(const SwapControllerMessage &msg)
{
  if( msg.getEvent() == SwapController::CHANGED )
  {
    mSprites.clear();
    auto current = msg.getSubject().mLetters.begin();
    auto next = current + 1;
    while( next < msg.getSubject().mLetters.end() )
    {
      Vec2i a = (*current)->getPosition();
      Vec2i b = (*next)->getPosition();
      Vec2i intersection = (a + b) * mCellSize / 2;
      Sprite sprite = (a.x != b.x) ? mHorizontalSprite : mVerticalSprite;
      sprite.setLoc( intersection );
      sprite.setTint( getBackgroundColor() );
      mSprites.push_back( sprite );
      current = next;
      ++next;
    }
  }
  else if ( msg.getEvent() == SwapController::DESTRUCTED )
  {
    mIsDead = true;
    mSprites.clear();
  }
}

void SwapView::render()
{
  for( auto &sprite : mSprites )
  {
    sprite.render();
  }
}

