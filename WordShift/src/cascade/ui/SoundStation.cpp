//
//  SoundStation.cpp
//  WordShift
//
//  Created by David Wicks on 5/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SoundStation.h"
#include "CinderDenshion.h"
#include "CollectionUtilities.hpp"

using namespace cascade;
using namespace pockets;
using namespace cinder;
using namespace std;

SoundStation::SoundStation()
{
  mSoundPaths = {
    { eButtonHover,     app::App::getResourcePath("sound/button-hover.aif") }
    , { eButtonExit,    app::App::getResourcePath("sound/button-exit.aif") }
    , { eButtonSelect,  app::App::getResourcePath("sound/button-select.aif") }
    , { eTileSelect,    app::App::getResourcePath("sound/tile-select.aif") }
    , { eTileDeselect,  app::App::getResourcePath("sound/tile-deselect.aif") }
    , { eTileSwap,      app::App::getResourcePath("sound/tile-swap.aif") }
    , { eWordFound,     app::App::getResourcePath("sound/word-found.aif") }
    , { eWarnEnd,       app::App::getResourcePath("sound/warning.aif") }
    , { eWarnUsed,      app::App::getResourcePath("sound/warning.aif") }
  };
  preload();
}

SoundStation::~SoundStation()
{
  unload();
  disconnect();
}

void SoundStation::preload()
{
  for( auto &pair : mSoundPaths )
  {
    sansumbrella::Denshion().preloadEffect( pair.second );
  }
}

void SoundStation::unload()
{
  for( auto &pair : mSoundPaths )
  {
    sansumbrella::Denshion().unloadEffect( pair.second );
  }
}

void SoundStation::connect()
{
  mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this](){ update(); } );
  mLastUpdate = 0;
  mTimer.start();
}

void SoundStation::disconnect()
{
  mUpdateConnection.disconnect();
  mTimer.stop();
}

void SoundStation::update()
{
  double now = mTimer.getSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;
  for( auto &sound : mCuedSounds )
  {
    sound.time -= deltaTime;
    if( sound.time <= 0 )
    {
      playSound( sound.id );
    }
  }
  vector_erase_if( &mCuedSounds, [](const Sound &sound){ return sound.time <= 0; } );
}

void SoundStation::cueSound(SoundId id, float delay)
{
  Sound sound{ id, delay };
  if( ! vector_contains( mCuedSounds, sound ) )
  {
    mCuedSounds.push_back( sound );
  }
}

void SoundStation::playNow(SoundId id)
{
  if( mSoundPaths.count(id) && mEnabled )
  {
    sansumbrella::Denshion().playEffect( mSoundPaths[id] );
  }
}

void SoundStation::playSound(SoundId id)
{
  if( mSoundPaths.count(id) && mEnabled )
  {
    sansumbrella::Denshion().playEffect( mSoundPaths[id] );
  }
}

SoundStation& SoundStation::get()
{
  static SoundStation sStation;
  return sStation;
}
