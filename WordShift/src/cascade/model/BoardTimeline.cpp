//
//  GameTimeline.cpp
//  WordShift
//
//  Created by David Wicks on 4/27/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "BoardTimeline.h"

using namespace cinder;

Timeline& cascade::boardTimeline()
{
  static ci::TimelineRef sTimeline = ci::Timeline::create();
  return *sTimeline;
}
