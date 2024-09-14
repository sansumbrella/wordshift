//
//  TapTapController.h
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "LetterModel.h"

/**
Captures two letter selections and attempts to make a match
*/

namespace cascade
{
	class TapTapController
	{
	public:
		TapTapController();
		~TapTapController();
    void updateWithLetter( LetterRef letter, bool canUseLetter );
    bool containsLetter( LetterRef letter ) const;
    //! returns true iff the given letter a neighbor or it shares the sharedNeighbor()
    bool letterIsNeighborly( LetterRef letter ) const;
    //! returns true iff the given letter is a direct neighbor
    bool letterWillSwap( LetterRef letter ) const;
    //! returns the letter that is shared as a neighbor by all letters
    std::vector<LetterRef> sharedNeighbors() const;
    void removeLetter( LetterRef letter );
    bool apply();
    void clear();
    int32_t pendingSwaps() const { return mLetters.size(); }
	private:
    std::vector<LetterRef>  mLetters;
    // the most recently removed letter, for figuring out tap order
    LetterRef               mLastLetter;
	};
}
