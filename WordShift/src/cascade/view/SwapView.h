//
//  SwapView.h
//  WordShift
//
//  Created by David Wicks on 4/27/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//


#pragma once

#include "SwapController.h"
#include "pockets/SpriteSheet.h"

/**
 View of swap controllers.
 Receives messages from a swap controller and draws an icon at the
 intersection of any two letters that in might swap.
*/

namespace cascade
{
  typedef std::shared_ptr<class SwapView> SwapViewRef;
  class SwapView : public Receiver<SwapControllerMessage>, public pk::SimpleRenderer::IRenderable
  {
  public:
    SwapView( const ci::Vec2i &cell_size, const pk::Sprite &horizontal, const pk::Sprite &vertical );
    ~SwapView();
    void  receive( const SwapControllerMessage &msg );
    void  render();
    bool  isDead(){ return mIsDead; }
  private:
    pk::Sprite              mHorizontalSprite;
    pk::Sprite              mVerticalSprite;
    ci::Vec2i               mCellSize;
    std::vector<pk::Sprite> mSprites;
    bool                    mIsDead = false;
  };
}
