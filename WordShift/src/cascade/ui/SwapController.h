//
//  SwapController.h
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "LetterModel.h"
#include "MessageType.hpp"

/**

Gathers a sequence of letters for swapping

*/

namespace cascade
{
  // received by SwapView's (the little dots between letters)
  typedef MessageT<class SwapController> SwapControllerMessage;

	class SwapController : public Messenger<SwapControllerMessage>
	{
	public:
    enum Event
    {
      CHANGED = 0
      , DESTRUCTED
    };
		SwapController();
		~SwapController();
    bool    apply();
    bool    containsLetter( LetterRef letter ) const;
    bool    hasContent() const { return mLetters.size() > 1; }
    void    updateWithLetter( LetterRef letter, bool canUseLetter );
    int32_t pendingSwaps() const { return ci::math<int32_t>::max( mLetters.size() - 1, 0 ); }
	private:
    friend class SwapView;
    std::vector<LetterRef>  mLetters;
	};
}
