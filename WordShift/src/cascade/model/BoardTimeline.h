//
//  GameTimeline.h
//  WordShift
//
//  Created by David Wicks on 4/27/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Timeline.h"

namespace cascade
{
  // a shared timeline that is managed by the active BoardView instance
  // reset whenever a new boardmodel is created
  // stepped forward when that boardmodel is updated
  ci::Timeline& boardTimeline();
}
