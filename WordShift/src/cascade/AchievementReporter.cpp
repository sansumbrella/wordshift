//
//  AchievementReporter.cpp
//  WordShift
//
//  Created by David Wicks on 5/4/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "AchievementReporter.h"
#include "CollectionUtilities.hpp"
#include "CascadeGameCenterDelegate.h"
#include "cinder/Utilities.h"
#include "PlayerModel.h"
#include "BoardModel.h"

using namespace cascade;
using namespace cinder;
using namespace std;

AchievementReporter::AchievementReporter():
mAchievements( validAchievementList() )
{}

AchievementReporter::~AchievementReporter()
{}

AchievementReporter& AchievementReporter::get()
{
  static AchievementReporter sReporter;
  return sReporter;
}

set<string> AchievementReporter::validAchievementList()
{
  return {
    "wordshift.length.6",
    "wordshift.length.7",
    "wordshift.length.8",
    "wordshift.length.9"
  };
}

void AchievementReporter::receive(const cascade::WordPlayedMessage &msg)
{
  const string achievement = "wordshift.length." + toString( msg.getWord().size() );
  if( mAchievements.count(achievement) != 0 )
  {
    getGameCenter().reportAchievement( achievement, 100.0f );
    mAchievements.erase( achievement );
  }
}

void AchievementReporter::reportGameEndAchievements(const PlayerModelRef player)
{
  const int64_t score = player->getScore();
  string prefix = player->getRules()->getScoreAchievementPrefix();
  if( !prefix.empty() )
  {
    if( score == 0 )
    {
      getGameCenter().reportAchievement( prefix + "moon" );
    }
    if( score >= 5000 )
    {
      getGameCenter().reportAchievement( prefix + "5k" );
    }
    if( score >= 10000 )
    {
      getGameCenter().reportAchievement( prefix + "10k" );
    }
    if( score >= 15000 )
    {
      getGameCenter().reportAchievement( prefix + "15k" );
    }
    if( score >= 25000 )
    {
      getGameCenter().reportAchievement( prefix + "25k" );
    }
  }
}

