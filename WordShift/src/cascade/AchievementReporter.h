//
//  AchievementReporter.h
//  WordShift
//
//  Created by David Wicks on 5/4/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "Receiver.hpp"
#include <set>

namespace cascade
{
  class WordPlayedMessage;
  typedef std::shared_ptr<class PlayerModel> PlayerModelRef;
  /**
   Reports word-length based achievements to Game Center.
   */

  class AchievementReporter : public Receiver<WordPlayedMessage>
  {
  public:
    ~AchievementReporter();
    //! check whether word is an unearned achievement; if so, reports to gamecenter
    void receive( const WordPlayedMessage &msg );
    void reportGameEndAchievements( const PlayerModelRef player );
    bool unearned( const std::string &achievement );
    //! connect to game center and get list of achievements not yet earned
    void loadUnearnedAchievements();
    static AchievementReporter& get();
  private:
    AchievementReporter();
    std::set<std::string>  mAchievements;
    static std::set<std::string> validAchievementList();
  };
}
