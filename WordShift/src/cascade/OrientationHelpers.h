//
//  OrientationHelpers.h
//  WordShift
//
//  Created by David Wicks on 3/7/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

namespace cascade
{
  enum ReadingDirection
  {
    Positive = 1
    , Negative = -1
  };

  enum GameOrientation
  {
    Portrait = 1
    , Landscape
  };

  struct ReadingDirectionPair
  {
    ReadingDirectionPair( ReadingDirection aRight, ReadingDirection aDown ):
    right( aRight )
    , down( aDown )
    {}
    ReadingDirection right;
    ReadingDirection down;
  };

  //! returns perceptual upper left of rectangle based on reading direction
  ci::Vec2f getOrientedUpperLeft( const ci::Rectf &rect, ReadingDirection right, ReadingDirection down );
  ci::Vec2f getOrientedLowerLeft( const ci::Rectf &rect, ReadingDirection right, ReadingDirection down );
  //! returns angle in degrees of device rotation for reading directions
  float readingDirectionAngle( ReadingDirection left_right, ReadingDirection top_bottom );
  std::vector<ReadingDirectionPair> getPlayerReadingDirections( int total_players, GameOrientation orientation );
}
