//
//  GameTypography.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "GameTypography.h"
#include "cinder/System.h"

using namespace cinder;

gl::TextureFontRef  bodyTextureFont;
gl::TextureFontRef  headingTextureFont;

void cascade::unloadBodyTextureFont()
{
  bodyTextureFont.reset();
}

void cascade::unloadHeadingTextureFont()
{
  headingTextureFont.reset();
}

gl::TextureFont& cascade::getTextureFont(cascade::FontType type)
{
  switch ( type ) {
    case eHeading:
      return getHeadingTextureFont();
      break;
    case eBody:
      return getBodyTextureFont();
      break;
    default:
      break;
  }
}

gl::TextureFont& cascade::getHeadingTextureFont()
{
  if( !headingTextureFont )
  {
    gl::TextureFont::Format fmt;
    fmt.premultiply();
    fmt.enableMipmapping( false );
    headingTextureFont = gl::TextureFont::create( getButtonFont(), fmt );
  }
  return *headingTextureFont;
}

gl::TextureFont& cascade::getBodyTextureFont()
{
  if( !bodyTextureFont )
  {
    gl::TextureFont::Format fmt;
    fmt.premultiply();
    fmt.enableMipmapping( false );
    bodyTextureFont = gl::TextureFont::create( getBodyFont(), fmt );
  }
  return *bodyTextureFont;
}

Font  cascade::getBodyFont()
{
  float font_size = System::isDeviceIpad() ? app::toPixels(18.0f) : app::toPixels(14.0f);
  try
  {
    return Font( "Avenir Next Medium", font_size );
  } catch (FontInvalidNameExc &exc)
  {
    return Font( "Helvetica Neue", font_size );
  }
}

Font  cascade::getButtonFont()
{
  float font_size = System::isDeviceIpad() ? app::toPixels(24.0f) : app::toPixels(18.0f);
  try
  {
    return Font( "Avenir Next Demi Bold", font_size );
  } catch (FontInvalidNameExc &exc)
  {
    return Font( "Helvetica Neue Medium", font_size );
  }
}

Font  cascade::getLargeButtonFont()
{
  float font_size = System::isDeviceIpad() ? app::toPixels(36.0f) : app::toPixels(24.0f);
  try
  {
    return Font( "Avenir Next Bold", font_size );
  } catch (FontInvalidNameExc &exc)
  {
    return Font( "Helvetica Neue Bold", font_size );
  }
}


