//
//  SpriteNode.h
//  WordShift
//
//  Created by David Wicks on 5/3/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "UIElement.h"
#include "pockets/Sprite.h"

namespace cascade
{
  typedef std::shared_ptr<class SpriteNode> SpriteNodeRef;
  class SpriteNode : public UIElement
  {
  public:
    SpriteNode( const pk::Sprite &sprite );
    ~SpriteNode();
    void draw();
    void setTint( const ci::ColorA &color ){ mSprite.setTint( color ); }
    static SpriteNodeRef create( const pk::Sprite &sprite ){ return SpriteNodeRef( new SpriteNode( sprite ) ); }
  private:
    pk::Sprite  mSprite;
  };
}
