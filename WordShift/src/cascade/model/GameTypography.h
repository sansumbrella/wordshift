//
//  GameTypography.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Text.h"
#include "cinder/gl/TextureFont.h"

/**
Font generators for the game.
Uses Avenir if it's available, otherwise falls back to Helvetica.
Game tiles are always in Avenir.
*/

namespace cascade
{
  enum FontType
  {
    eHeading = 0,
    eBody
  };

  ci::Font  getButtonFont();
  ci::Font  getBodyFont();
  ci::Font  getLargeButtonFont();

  void unloadHeadingTextureFont();
  void unloadBodyTextureFont();
  ci::gl::TextureFont&  getHeadingTextureFont();
  ci::gl::TextureFont&  getBodyTextureFont();
  ci::gl::TextureFont&  getTextureFont( FontType type );
}
