//
//  CinderDenshion.h
//  Cascade
//
//  Created by David Wicks on 11/12/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <OpenAL/al.h>
#include "cinder/Filesystem.h"

namespace cinder
{
namespace sansumbrella
{
	class CinderDenshion
	{
	public:
		~CinderDenshion();
		static CinderDenshion& instance();
		bool isOtherAudioPlaying();
		void playBackgroundMusic( const fs::path &file );
		ALuint playEffect( const fs::path &file, float pitch=1.0f, float pan=0.0f, float gain=1.0f );
    void preloadEffect( const fs::path &file );
    void unloadEffect( const fs::path &file );
		void stopEffect( ALuint soundId );
	private:
		CinderDenshion();
	};

	inline CinderDenshion& Denshion(){ return CinderDenshion::instance(); }
} // sansumbrella
namespace su = sansumbrella;
} // cinder

