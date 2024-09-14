//
//  GameColor.h
//  WordShift
//
//  Created by David Wicks on 2/25/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Json.h"

namespace cascade
{
  class GamePalette
  {
  public:
    GamePalette() = default;
    GamePalette( const cinder::JsonTree &json );
    ci::Color 	getBackground() const { return mBackground; }
    ci::Color   getBacking() const { return mBacking; }
    ci::Color 	getBackingHighlight() const { return mBackingHighlight; }
    ci::Color   getText() const { return mText; }
    ci::Color   getAltText() const { return mAltText; }
    ci::Color   getWarn() const { return mWarn; }
    std::string getName() const { return mName; }
  private:
    std::string mName = "";
    ci::Color mBackground =       ci::Color( 1.0f, 1.0f, 1.0f );
    ci::Color mBacking =          ci::Color( 0.4f, 0.4f, 0.4f );
    ci::Color mBackingHighlight = ci::Color( 0.1f, 0.1f, 0.1f );
    ci::Color mText =             ci::Color( 1.0f, 1.0f, 1.0f );
    ci::Color mAltText =          ci::Color( 0.3f, 0.3f, 0.3f );
    ci::Color mWarn =             ci::Color( 1.0f, 0.8f, 0.0f );
  };

	class ColorStation
	{
	public:
		ColorStation();
		~ColorStation();
    //! Accessors for palette colors
    ci::Color getBackground() const
    { return mPalette.getBackground(); }
    ci::Color getBacking() const
    { return mPalette.getBacking(); }
    ci::Color getBackingHighlight() const
    { return mPalette.getBackingHighlight(); }
    ci::Color getText() const
    { return mPalette.getText(); }
    ci::Color getAltText() const
    { return mPalette.getAltText(); }
    ci::Color getWarn() const
    { return mPalette.getWarn(); }

    //! load palettes from JSON file
    void loadPalettes( const ci::fs::path &color_file );
    //! set the current palette
    void setPalette( const GamePalette &palette ){ mPalette = palette; }
    //! set the current palette by name
    void setPalette( const std::string &name );
    //! returns a reference to the full list of palettes
    const std::vector<const GamePalette>& getPalettes() const { return mPalettes; }
    static ColorStation& get();
	private:
    std::vector<const GamePalette>  mPalettes;
    GamePalette                     mPalette;
	};

  inline ci::Color getBackgroundColor(){ return ColorStation::get().getBackground(); }
  inline ci::Color getBackingColor(){ return ColorStation::get().getBacking(); }
  inline ci::Color getBackingHighlightColor(){ return ColorStation::get().getBackingHighlight(); }
  inline ci::Color getTextColor(){ return ColorStation::get().getText(); }
  inline ci::Color getAltTextColor(){ return ColorStation::get().getAltText(); }
  inline ci::Color getWarningColor(){ return ColorStation::get().getWarn(); }
}
