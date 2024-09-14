//
//  BoardView.h
//  Cascade
//
//  Created by David Wicks on 11/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

/*
 Renders the current state of the playing board
 Animates transitions on state change
 */

#include "BoardModel.h"
#include "LetterView.h"
#include "WordView.h"
#include "SwapView.h"
#include "pockets/Locus2d.h"
#include "pockets/SimpleRenderer.h"

namespace cascade
{
  typedef std::shared_ptr<class BoardView>	BoardViewRef;

  class BoardView : public Receiver<WordFoundMessage>
  {
  public:
    typedef std::function<float (LetterViewRef letter)> DelayFn;
    ~BoardView();
    void      update( float deltaTime );
    void      draw();
    void      receive( const WordFoundMessage &message );
    LetterRef getLetter( const ci::Vec2f &loc );
    void      updateFromModel();
    //! set the top-left point of the board, where the center of tile 0,0 is drawn
    void      setLoc( const ci::Vec2f &loc ){ mLocus->setLoc( loc ); mOffsetAnim = loc; }
    void      setLocSoft( const ci::Vec2f &loc );
    ci::Vec2f getLoc() const { return mLocus->getLoc(); }
    pk::Locus2dRef  getLocus(){ return mLocus; }
    //! returns the full width of the board view
    float     getWidth() const { return mCellSize.x * mModel->getColumnCount(); }
    //! returns the full height of the board view
    float     getHeight() const { return mCellSize.y * mModel->getRowCount(); }
    //! returns distance between centers of left- and right-most tiles
    float     getCenteringWidth() const { return mCellSize.x * (mModel->getColumnCount() - 1); }
    //! returns distance between centers of top- and bottom-most tiles
    float     getCenteringHeight() const { return mCellSize.y * (mModel->getRowCount() - 1); }
    //! set the rotation of letters on the board
    void      setLetterRotation( float radians );
    //! add an overlay to renderer
    void      addOverlay( pk::SimpleRenderer::IRenderable *overlay );
    //! animate letters in
    void      appear( float duration, const ci::Vec2f &from_direction );
    //! pulse letters a warning color
    void      pulse( DelayFn fn );
    DelayFn   leftRightDelay( float total_delay );
    DelayFn   topBottomDelay( float total_delay );
    DelayFn   rightLeftDelay( float total_delay );
    DelayFn   bottomTopDelay( float total_delay );
    DelayFn   centerOutDelay( float total_delay );
    DelayFn   closeOnCenterDelay( float total_delay );
    //! delay goes from 0 at white to total_delay at black
    DelayFn   channelDelay( ci::Channel8u channel, float total_delay );
    static float noDelay( LetterViewRef letter ){ return 0; }
    //
    ci::Rectf getTileBounds() const;
    ci::Rectf getCellBounds() const;
    ci::Vec2i getTileSize() const { return mTileSize; }
    ci::Vec2i getCellSize() const { return mCellSize; }
    static BoardViewRef create( BoardModelRef model, const pk::SpriteSheet &sprite_sheet )
    {
      BoardViewRef view( new BoardView( model, sprite_sheet ) );
      model->appendWordFoundReceiver( view.get() );
      return view;
    }
  private:
    BoardView( BoardModelRef model, const pk::SpriteSheet &sprite_sheet );
    pk::Locus2dRef              mLocus = pk::Locus2dRef( new pk::Locus2d );
    BoardModelRef               mModel;
    std::vector<LetterViewRef>	mLetters;
    std::vector<WordViewRef>    mWords;
    std::vector<SwapViewRef>    mSwapViews;
    pk::SimpleRenderer          mOverlayRenderer;
    pk::SpriteData              mCornerSpriteData;
    pk::SpriteData              mLineSpriteData;
    ci::Vec2i                   mTileSize;
    ci::Vec2i                   mCellSize;
    ci::Anim<ci::Vec2f>         mOffsetAnim;
  };
}
