//
//  SoundStation.h
//  WordShift
//
//  Created by David Wicks on 5/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Timer.h"

namespace cascade
{
  class SoundStation
  {
  public:
    enum SoundId
    {
      eButtonHover = 0
      , eButtonSelect
      , eButtonExit
      , eTileSwap
      , eTileSelect
      , eTileDeselect
      , eWordFound
      , eWarnEnd
      , eWarnUsed
    };
    struct Sound
    {
      SoundId id;
      float   time;
      bool operator==(const Sound &other) const { return id == other.id && time == other.time; }
    };
    ~SoundStation();
    //!
    //! recommended playback method; cues sound and avoids duplicate playback
    void cueSound( SoundId id, float delay );
    //! plays sound immediately, no check for duplicates
    void playNow( SoundId id );
    //! play sounds while running (will later just provide an empty sound palette)
    void setEnabled( bool enabled ){ mEnabled = enabled; }
    bool enabled() const { return mEnabled; }
    // start running
    void connect();
    // stop running
    void disconnect();
    void preload();
    void unload();
    static SoundStation& get();
  private:
    SoundStation();
    bool                                mEnabled = true;
    std::vector<Sound>                  mCuedSounds;
    std::map<SoundId, ci::fs::path>     mSoundPaths;
    ci::Timer                           mTimer;
    double                              mLastUpdate = 0;
    ci::signals::connection             mUpdateConnection;
    void update();
    void playSound( SoundId id );
  };

  inline SoundStation& Sound(){ return SoundStation::get(); }
}
