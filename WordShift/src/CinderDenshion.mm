//
//  CinderDenshion.cpp
//  Cascade
//
//  Created by David Wicks on 11/12/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CinderDenshion.h"
#import "SimpleAudioEngine.h"

using namespace ci;
using namespace sansumbrella;

CinderDenshion& CinderDenshion::instance()
{
	static CinderDenshion instance;
	return instance;
}

bool CinderDenshion::isOtherAudioPlaying()
{
	return [[CDAudioManager sharedManager] isOtherAudioPlaying];
}

void CinderDenshion::playBackgroundMusic( const fs::path &file )
{
	[[SimpleAudioEngine sharedEngine] playBackgroundMusic: [NSString stringWithUTF8String:file.c_str()]];
}

ALuint CinderDenshion::playEffect( const fs::path &file, float pitch, float pan, float gain )
{
	return [[SimpleAudioEngine sharedEngine] playEffect:[NSString stringWithCString:file.c_str() encoding:[NSString defaultCStringEncoding]]
									  pitch:pitch
									  pan:pan
									  gain:gain ];
}

void CinderDenshion::preloadEffect(const fs::path &file)
{
  [[SimpleAudioEngine sharedEngine] preloadEffect:[NSString stringWithCString:file.c_str() encoding:[NSString defaultCStringEncoding]]];
}

void CinderDenshion::unloadEffect(const fs::path &file)
{
  [[SimpleAudioEngine sharedEngine] unloadEffect:[NSString stringWithCString:file.c_str() encoding:[NSString defaultCStringEncoding]]];
}

void CinderDenshion::stopEffect( ALuint soundId )
{
	[[SimpleAudioEngine sharedEngine] stopEffect: soundId];
}

CinderDenshion::CinderDenshion()
{
	[SimpleAudioEngine sharedEngine];
}

CinderDenshion::~CinderDenshion()
{
  [SimpleAudioEngine end];
}

