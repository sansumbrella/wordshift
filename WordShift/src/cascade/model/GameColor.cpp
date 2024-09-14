//
//  GameColor.cpp
//  WordShift
//
//  Created by David Wicks on 2/25/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "GameColor.h"

using namespace cascade;
using namespace cinder;

ColorStation::ColorStation()
{}

ColorStation::~ColorStation()
{}

ColorStation& ColorStation::get()
{
  static ColorStation gc;
  return gc;
}

void ColorStation::loadPalettes(const ci::fs::path &color_file)
{
  JsonTree color_json( loadFile( color_file ) );
  JsonTree palettes = color_json["palettes"];
  for( const auto &p : palettes )
  {
    mPalettes.push_back( GamePalette(p) );
  }
}

inline Color toColor( const JsonTree &json )
{
  return Color8u( json["r"].getValue<int>(), json["g"].getValue<int>(), json["b"].getValue<int>() );
}

void ColorStation::setPalette(const std::string &name)
{
  for( const auto &palette : mPalettes )
  {
    if( palette.getName() == name )
    {
      mPalette = palette;
      break;
    }
  }
}

GamePalette::GamePalette( const JsonTree &json )
{
  mName = json["name"].getValue();
  mBackground = toColor( json["background"] );
  mBacking = toColor( json["backing"] );
  mBackingHighlight = toColor( json["backing-highlight"] );
  mText = toColor( json["text"] );
  mAltText = toColor( json["text-alt"] );
  if( json.hasChild("warn") )
  { mWarn = toColor( json["warn"] ); }
}
