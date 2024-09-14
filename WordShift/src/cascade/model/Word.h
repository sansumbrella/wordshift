//
//  Word.h
//  WordShift
//
//  Created by David Wicks on 2/11/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "LetterModel.h"
#include "OrientationHelpers.h"

namespace cascade
{
	const char NOT_A_LETTER = '-';
	const char WILDCARD = '*';
	typedef std::shared_ptr<class Word> WordRef;
  typedef std::weak_ptr<class Word> WordWeakRef;

  class WordMessage
  {
  public:
		enum Type
    {
      VANISH
      , ALREADY_PLAYED
    };
    WordMessage( Type type ):
    mEvent( type )
    {}
    //! what type of thing happened?
    Type getEvent() const { return mEvent; }
  private:
  	Type 	mEvent;
  };

	class Word : public Messenger<WordMessage>
	{
	public:
		~Word();
		std::vector<LetterRef>::const_iterator begin() const { return mLetters.begin(); }
		std::vector<LetterRef>::const_iterator end() const { return mLetters.end(); }
    std::vector<LetterRef>::iterator begin() { return mLetters.begin(); }
		std::vector<LetterRef>::iterator end() { return mLetters.end(); }
    LetterRef         firstLetter() const { return mLetters.front(); }
    LetterRef         lastLetter() const { return mLetters.back(); }
		std::string       getString() const;
    bool              isInRow() const { return mIsInRow; }
    //! returns whether we have waited out the confirmation delay
    bool              confirmed() const { return mTime >= mConfirmationDelay; }
    int               getColumn() const { return firstLetter()->getColumn(); }
    int               getRow() const { return firstLetter()->getRow(); }
    ReadingDirection  getReadingDirection() const { return mReadingDirection; }
    bool              containsLetter( LetterRef letter ) const;
    void              update(float deltaTime){ mTime += deltaTime; }
    void              setConfirmationDelay( float delay );
    void              setAlreadyPlayed( bool played=true ){ mInvalid = played; }
    bool              getAlreadyPlayed() const { return mInvalid; }
    //! returns how much time is left before confirmation
    float             getRemainingTime() const { return mConfirmationDelay - mTime; }
    //! returns time elapsed over time allowed
    float             getNormalizedTime() const { return mTime / mConfirmationDelay; }
    bool operator==(const Word &other);
		static WordRef create( const std::vector<LetterRef> &letters, float currentTime )
    { return WordRef( new Word( letters, currentTime ) ); }
	private:
		Word( const std::vector<LetterRef> &letters, float confirmationDelay );
		std::vector<LetterRef>	mLetters;
    bool                    mIsInRow;
    bool                    mInvalid = false;
    ReadingDirection        mReadingDirection;
    float                   mTime = 0.0f;
    float                   mConfirmationDelay;
	};
}
