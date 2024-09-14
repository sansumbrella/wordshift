//
//  LetterModel.h
//  Cascade
//
//  Created by David Wicks on 12/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Messenger.hpp"
#include <array>

namespace cascade
{
  // the directions in which we can move around a letter
  enum Direction {
    NORTH=0
    , EAST
    , SOUTH
    , WEST
    , NUM_DIRECTIONS
  };

	typedef std::shared_ptr<class Letter> LetterRef;

  class LetterMessage
  {
  public:
		enum Type
    {
     	SELECT
      , CHANGE
      , ROTATE
    };
    LetterMessage( const Letter &subject, Type type, float direction=0.0f, float offset=0.0f ):
    mEvent( type )
    , mDirection( direction )
    , mOffset( offset )
    , mSubject( subject )
    {}
    //! what type of thing happened?
    Type getEvent() const { return mEvent; }
    //! a delay for when the event should be evaluated
    float getOffset() const { return mOffset; }
    //! what direction the CHANGE came from
    float getChangeDirection() const { return mDirection; }
    const Letter& getSubject() const { return mSubject; }
  private:
  	Type 	mEvent;
    float mDirection = 0; // direction of change in radians
    float mOffset;
    const Letter &mSubject;
  };

	class Letter : public Messenger<LetterMessage>
	{
	public:
		void swapWith( LetterRef other, float delay );
    //! assign letter without sending notifications
    void assignChar( char c ){ mLetter = c; }
		void setChar( char c, float from_direction, float with_delay );
		char getChar() const { return mLetter; }
		//! row index in portrait view
		int getRow() const { return mRow; }
		//! column index in portrait view
		int getColumn() const { return mColumn; }
		LetterRef getNeighbor( Direction dir ) const { return mNeighbors[dir]; }
		void setNeighbor( Direction dir, LetterRef neighbor ){ mNeighbors[dir] = neighbor; }
    std::vector<LetterRef> getNeighbors() const;
		bool isNeighbor( LetterRef candidate );
    void clearNeighbors();
		//! is this letter selected by a user action?
		bool isSelected() const { return mSelected; }
		//! Tell this letter whether it's selected by a user. Delivers a SELECT message.
		void setSelected( bool val );
    //! return grid position (column, row)
    ci::Vec2i getPosition() const { return ci::Vec2i( mColumn, mRow ); }
		static LetterRef create( char letter, int row, int column )
    {
      LetterRef ret( new Letter( letter, row, column ) );
      return ret;
    }
	private:
		Letter( char letter, int row, int column ):
		mLetter( letter )
		, mRow ( row )
		, mColumn( column )
		{}
		char 		mLetter;
    std::array<LetterRef, NUM_DIRECTIONS> mNeighbors;
		//! has the user selected us?
		bool		mSelected = false;
		//! positional information in grid
		int			mRow;
		int 		mColumn;
	};
}
