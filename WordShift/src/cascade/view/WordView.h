//
//  WordView.h
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "Word.h"
#include "ExpandedLine2d.h"
#include "cinder/Tween.h"
#include "pockets/SimpleRenderer.h"
#include "pockets/SpriteSheet.h"

/*
 Highlight around words that are scheduled for submission
 */
namespace cascade
{
  class ClockOutline : public pockets::SimpleRenderer::IRenderable
  {
  public:
    void setCoordinates( const ci::Vec2f &tl, const ci::Vec2f &tr, const ci::Vec2f &br, const ci::Vec2f &bl );
    void setTime( float t );
    void setTimeInverse( float t );
    void render();
    void matchSprite( const pk::SpriteData &sprite );
    void setTint( const ci::Color &tint ){ mTint = tint; }
  private:
    std::array<pk::ExpandedLine2dRef, 4>  mLines;
    std::array<float, 4>                  mPortions;
    ci::Color                             mTint = ci::Color::white();
  };

  typedef std::shared_ptr<class WordView> WordViewRef;
	class WordView : public Receiver<WordMessage>
	{
	public:
		WordView( WordRef word, ReadingDirection left_right, ReadingDirection top_bottom, const ci::Vec2f &tile_size, const pk::SpriteData &corner, const pk::SpriteData &line );
		~WordView();
    void receive( const WordMessage &msg );
    void update();
    bool isDead() const { return mIsDead; }
    void connectToRenderer( pk::SimpleRenderer *renderer );
    void vanish();
    void warnAlreadyPlayed();
	private:
    bool                                mIsDead = false;
    bool                                mSounded = false;
    WordWeakRef                         mModel;
    std::array<pk::SpriteRef, 4>        mCorners;
    std::array<ci::Anim<ci::Vec2f>, 4>  mCornerLocs;
    ci::Anim<ci::ColorA>                mColorAnim;
    ClockOutline                        mOutline;
	};
}
