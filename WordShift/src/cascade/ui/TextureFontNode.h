//
//  TextureFontNode.h
//  WordShift
//
//  Created by David Wicks on 5/31/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "UIElement.h"
#include "cinder/gl/TextureFont.h"

namespace cascade
{
  class TextureFontNode : public UIElement
  {
  public:
    TextureFontNode( ci::gl::TextureFontRef font, const std::string &text );
    ~TextureFontNode();
    static std::unique_ptr<TextureFontNode> create(ci::gl::TextureFontRef font, const std::string &text);
    void draw();
  private:
    ci::gl::TextureFontRef  mFont;
    std::string             mText;
  };
}
