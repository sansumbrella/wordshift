//
//  PlayerView.h
//  WordShift
//
//  Created by David Wicks on 2/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "PlayerModel.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "pockets/Locus2d.h"

namespace cascade
{
  typedef std::shared_ptr<class PlayerView> PlayerViewRef;
  /**
   A scrolling image view
   Constructs images from text to show score and played words
  */
	class PlayerView : public Receiver<PlayerScoreMessage>, public Receiver<PlayerTurnMessage>
	{
  struct ScoreUpdate
    {
      std::string word;
      std::string delta;
      std::string new_score;
    };
	public:
		PlayerView( const ci::Font &font, const int width, PlayRulesRef end_condition );
		~PlayerView();
    void  receive( const PlayerScoreMessage &msg );
    void  receive( const PlayerTurnMessage &msg );
    void  update( float deltaTime );
    void  changeDisplay( const std::string &new_message );
    void  draw();
    int   getHeight(){ return mScreenBounds.getHeight(); }
    void  setIntensity( float value );
    pk::Locus2dRef    getLocus(){ return mLocus; }
    pk::Locus2dRef    getEndConditionLocus(){ return mEndCondition->getLocus(); }
	private:
    pk::Locus2dRef            mLocus = pk::Locus2dRef( new pk::Locus2d() );
    ci::Font                  mFont;
    std::string               mScoreText = "0";
    ci::gl::Texture           mScoreTexture;
    ci::Surface               mScoreSurface;
    ci::Rectf                 mScreenBounds;  // in points
    ci::Area                  mTextureRegion; // in pixels
    ci::Anim<ci::Vec2f>       mTextureOffset = ci::Vec2f::zero(); // in pixels
    std::vector<ScoreUpdate>  mUpdates;
    ci::Anim<float>           mIntensity = 0.4f;
    ci::TimelineRef           mTimeline = ci::Timeline::create();
    PlayRulesRef              mEndCondition;

    void changeDisplaySoft( const std::string &update, float delay );
  };
}
