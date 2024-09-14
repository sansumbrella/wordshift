//
//  TimeLimitRules.h
//  WordShift
//
//  Created by David Wicks on 2/18/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "PlayRules.h"
#include "cinder/Tween.h"

namespace cascade
{
  typedef std::shared_ptr<class TimeLimitRules> TimeLimitRulesRef;
	class TimeLimitRules : public PlayRules
	{
	public:
		TimeLimitRules( const pk::SpriteSheet &sprite_sheet, float duration );
		~TimeLimitRules();
    void  draw();
    bool  endReached() const { return mTimeRemaining <= 0; }
    int   quantityRemaining() const;
    void  evaluateEnd( float deltaTime );
    void  assignCredit( int wordLength );
    void  addTime( float seconds );
    float visibleTime() const;
    std::string getPlayStats() const;
    virtual uint64_t        getScoreboardContext() const { return mSecondsPlayed; }
	private:
    float                     mTimeRemaining;
    ci::Anim<float>           mEasedTimeRemaining;
    float                     mSecondsPlayed = 0; // for game end stats
    float                     mCreditGiven = 0; // keep track of credit for animation
    std::map<int32_t, float>  mWordLengthBonuses;
    pk::Sprite                    mClockOutline;
    pk::SpriteData                mPieFilling;
    std::map<int32_t, pk::Sprite> mNumerals;
	};
}
