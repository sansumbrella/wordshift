//
//  LetterView.h
//  Cascade
//
//  Created by David Wicks on 12/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "LetterModel.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Tween.h"
#include "pockets/SpriteSheet.h"
#include "GameColor.h"

namespace cascade
{
  typedef std::shared_ptr<class LetterView> LetterViewRef;

  class LetterView : public Receiver<LetterMessage>
  {
    struct LetterSwap
    {
      LetterSwap( const char l, float d, float t ):
      letter( l )
      , direction( d )
      , time( t )
      {}
      const char  letter;
      float       time;
      float       direction;
    };
  public:
    // queues any necessary swaps
    void      update();
    void      draw();
    void      setLoc( const ci::Vec2f &loc ){ mLoc = loc; }
    void      setSize( const ci::Vec2f &size ){ mBounds = ci::Rectf( -size/2, size/2 ); }
    //! set rotation in radians, with a possible delay for animation
    void      setRotation( float radians, float delay=0.0f );
    //! returns rotation in radians
    float     getRotation() const { return mRotation; }
    //! moves glyph character into view
    void      appear( const ci::Vec2f &from_direction, float duration, float delay );
    void      receive( const LetterMessage &message );
    void      updateFromModel();
    LetterRef getModel(){ return mModel; }
    bool      contains( const ci::Vec2f &loc ){ return (mBounds + mLoc).contains( loc ); }
    ci::Vec2f getLoc() const { return mLoc; }
    void      pulse( float delay );
    static LetterViewRef create( LetterRef model, const pk::Sprite &backing ){ LetterViewRef view( new LetterView( model, backing ) ); model->appendReceiver( view.get() ); return view; }
    static ci::Timeline& timeline();
    // sprite data for letterviews, should only be written to by someone who has the master sprite sheet
    static std::map<char, pk::SpriteData>&  letterSprites();
  private:
    LetterView( LetterRef model, const pk::Sprite &backing_sprite );
    void            updateGlyph( char glyph, float direction, float delay );
    void            flip( const char letter, float direction, float at_time, float out_duration, float back_duration, ci::EaseFn back_ease=ci::EaseNone() );
    ci::Vec2f               mLoc;
    LetterRef               mModel;
    pk::Sprite              mGlyphSprite;
    pk::Sprite              mBacking;
    ci::Color               mColor = getBackingColor();
    ci::Anim<ci::Color>     mTextColor = getTextColor();
    ci::Rectf               mBounds;
    ci::Anim<float>         mRotation = 0.0f;
    // rotation of tile in degrees
    ci::Anim<float>         mTileRotation = 0.0f;
    ci::Anim<ci::Vec2f>     mOffset = ci::Vec2f::zero();
    std::vector<LetterSwap> mSwaps;
  };
}
