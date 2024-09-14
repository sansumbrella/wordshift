//
//  Drawing.h
//  WordShift
//
//  Created by David Wicks on 5/7/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "pockets/Sprite.h"

namespace cascade
{
  //! draw pie with normalized \a time completion
  void  drawPie( const pk::SpriteData &texture_info, float time, int segments );
}
