//
//  HouseRules.h
//  WordShift
//
//  Created by David Wicks on 3/5/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "PlayRules.h"
#include "ButtonBase.h"

namespace cascade
{
  typedef std::shared_ptr<class HouseRules> HouseRulesRef;
	class HouseRules : public PlayRules
	{
	public:
		HouseRules( const pk::SpriteSheet &sprite_sheet );
		~HouseRules();
    void draw();
    //! start button listening
    void beginTurn();
    //! stop button listening
    void endTurn();
    bool endReached() const { return false; }
    bool turnOver() const { return mButtonPressed; }
    std::string getPlayStats() const { return ""; }
	private:
    ButtonRef   mButton;
    bool        mButtonPressed = false;
	};
}
