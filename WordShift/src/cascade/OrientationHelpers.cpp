//
//  OrientationHelpers.cpp
//  WordShift
//
//  Created by David Wicks on 3/7/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "OrientationHelpers.h"

using namespace cascade;
using namespace cinder;
using namespace std;

Vec2f cascade::getOrientedUpperLeft(const Rectf &rect, ReadingDirection right, ReadingDirection down)
{
  if( right == Negative && down == Positive )
  {
    return rect.getUpperRight();
  }
  else if( right == Negative && down == Negative )
  {
    return rect.getLowerRight();
  }
  else if( right == Positive && down == Negative )
  {
    return rect.getLowerLeft();
  }
  // ( right == Positive && down == Positive )
  return rect.getUpperLeft();
}

Vec2f cascade::getOrientedLowerLeft(const Rectf &rect, ReadingDirection right, ReadingDirection down)
{
  if( right == Negative && down == Positive )
  { // rotate clockwise
    return rect.getUpperLeft();
  }
  else if( right == Negative && down == Negative )
  { // rotate 180
    return rect.getUpperRight();
  }
  else if( right == Positive && down == Negative )
  { // rotate counter-clockwise
    return rect.getLowerRight();
  }
  // ( right == Positive && down == Positive )
  return rect.getLowerLeft();
}

float cascade::readingDirectionAngle( ReadingDirection left_right, ReadingDirection top_bottom )
{
  if( left_right == Negative && top_bottom == Positive )
  {
    return M_PI / 2;
  }
  else if( left_right == Negative && top_bottom == Negative )
  {
    return M_PI;
  }
  else if( left_right == Positive && top_bottom == Negative )
  {
    return 3 * M_PI / 2;
  }
  // ( left_right == Positive && top_bottom == Positive )
  return 0;
}

vector<ReadingDirectionPair> cascade::getPlayerReadingDirections(int total_players, GameOrientation orientation)
{
  vector<ReadingDirectionPair> directions;
  if( orientation == GameOrientation::Portrait )
  {
    switch (total_players)
    {
      case 2:
        directions.push_back( ReadingDirectionPair( Positive, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Negative ) );
        break;
      case 3:
        directions.push_back( ReadingDirectionPair( Positive, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Negative ) );
        break;
      case 4:
        directions.push_back( ReadingDirectionPair( Positive, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Negative ) );
        directions.push_back( ReadingDirectionPair( Positive, Negative ) );
        break;
      default:
        break;
    }
  }
  else
  {
    switch (total_players)
    {
      case 2:
        directions.push_back( ReadingDirectionPair( Negative, Positive ) );
        directions.push_back( ReadingDirectionPair( Positive, Negative ) );
        break;
      case 3:
        directions.push_back( ReadingDirectionPair( Negative, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Negative ) );
        directions.push_back( ReadingDirectionPair( Positive, Negative ) );
        break;
      case 4:
        directions.push_back( ReadingDirectionPair( Negative, Positive ) );
        directions.push_back( ReadingDirectionPair( Negative, Negative ) );
        directions.push_back( ReadingDirectionPair( Positive, Negative ) );
        directions.push_back( ReadingDirectionPair( Positive, Positive ) );
        break;
      default:
        break;
    }
  }

  return directions;
}
