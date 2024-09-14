//
//  CascadeGameCenterDelegate.h
//  WordShift
//
//  Created by David Wicks on 3/17/13.
//
//


/**
 C++ facade for performing basic Game Center management tasks
 - Display Apple's GameCenter ViewController (and automatically dismiss)
 - Authenticate player with a custom callback
 - Submit a score to a scoreboard
 - Submit an achievement with percent completion
*/

#ifdef __OBJC__
  @class CascadeGameCenterDelegate;
#else
  class CascadeGameCenterDelegate;
#endif

#include <map>
#include "model/LocalData.h"

namespace cascade
{
typedef std::shared_ptr<class CascadeGameCenter> CascadeGameCenterRef;
class CascadeGameCenter
{
public:
  struct Achievement{
    Achievement() = default;
    explicit Achievement( int pct ):
    percent_complete( pct )
    {}
    int percent_complete = 0;
  };
  struct Score
  {
    Score() = default;
    Score( int64_t v, uint64_t c ):
    value( v )
    , context( c )
    {}
    int64_t     value = 0;
    uint64_t    context = 0;
  };
  ~CascadeGameCenter();
  //! presents game center view over app; automatically pausing main Cinder program execution
  //! dismisses game center view when it's finished, restoring control back to Cinder program.
  void showGameCenter();
  //! Set up callback for player authentication; call once in setup()
  //! passed true to \a callback on success, false on failure
  void authenticatePlayer( std::function<void (bool)> callback=0 );
  //! Returns true if a player was successfully authenticated
  bool isEnabled() const { return mGameCenterEnabled; }
  //! Call when resigning active (when iOS sends you to background)
  //! Writes the player's data to disk and disables game center
  void resignActive();

  //! Loads achievement and high score progress for the player
  //! reports achievements and scores that are not yet in game center
  void loadPlayerData();

  //! Reports the user's score to the named scoreboard and makes that board the default in gamecenter
  void reportScore( const std::string &scoreboard_id, int64_t value, uint64_t context=0 );
  //! Reports user's progress on the named achievement (defaults to 100%)
  void reportAchievement( const std::string &achievement_id, double percentComplete=100.0 );
  //! Write achievements to disk; stores reported and unreported achievements
  void savePlayerData();
  //! If logged into game center, returns true if player is under age. Otherwise false.
  bool isPlayerUnderage() const;
  //! Returns the player's all-time high score on the given board
  int64_t getPlayerHighScore( const std::string &scoreboard_id ) const;
  static CascadeGameCenter& getShared();
  static ci::fs::path localFile();
  static ci::JsonTree defaultJson();
private:
  CascadeGameCenter();
  //! Pointer to implementation
  CascadeGameCenterDelegate *mImpl;
  std::atomic<bool>                   mGameCenterEnabled = {false};
  mutable std::mutex                  mLocalDataMutex;
  HashedLocalData                     mLocalData;
  mutable std::mutex                  mLocalAchievementMutex;
  std::map<std::string, Achievement>  mLocalAchievements;
  mutable std::mutex                  mLocalScoreMutex;
  std::map<std::string, Score>        mLocalScores; // scores that may not have been reported; clear on receipt of scores from gamecenter
};

//! Use for access to the shared game center object
inline CascadeGameCenter& getGameCenter(){ return CascadeGameCenter::getShared(); }

} // cascade::