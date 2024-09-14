//
//  CascadeGameCenterDelegate.m
//  WordShift
//
//  Created by David Wicks on 3/17/13.
//
//

#import "CascadeGameCenterDelegate.h"
#include "CollectionUtilities.hpp"
using namespace cascade;
using namespace cinder;
using namespace pockets;
using namespace std;

typedef std::function<void (NSArray *achievements, NSError *error)> AchievementFn;
typedef std::function<void (NSArray *scores, NSError *error)> ScoreFn;

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface CascadeGameCenterDelegate : NSObject<GKGameCenterControllerDelegate>
@property (retain) GKLocalPlayer *localPlayer;
- (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController;
- (void) showGameCenter;
- (void) authenticateLocalPlayerWithCallback:(std::function<void (bool)>) callback;
- (bool) isPlayerUnderage;
// returns the time since reference date for the score creation
- (void) reportScore:(const std::string &)scoreboard withPoints:(int64_t)score andContext:(uint64_t)context;
- (void) reportAchievement:(const std::string &)achievement_id percentComplete:(double)percent withBanner:(BOOL)showsBanner;
- (void) loadPlayerAchievements:(AchievementFn)callback;
// loads scores for all categories and accumulates to send to callback
- (void) loadPlayerScores:(ScoreFn)callback inCategories:(const std::vector<std::string> &)categories;
@end

@implementation CascadeGameCenterDelegate
@synthesize localPlayer;

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
  [[gameCenterViewController presentingViewController] dismissViewControllerAnimated:YES completion:nil];
}

- (void)showGameCenter
{
  UIViewController* root = ci::app::getWindow()->getNativeViewController();
  GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
  if (gameCenterController != nil)
  {
    gameCenterController.gameCenterDelegate = self;
    [root presentViewController: gameCenterController animated: YES completion:nil];
  }
}

- (void)authenticateLocalPlayerWithCallback:(std::function<void (bool)>)callback
{
  GKLocalPlayer *player = [GKLocalPlayer localPlayer];
  [self setLocalPlayer: player];
  player.authenticateHandler = ^(UIViewController *controller, NSError *error)
  {
    if( error )
    { // we might still be authenticated despite any error
      NSLog(@"GameKit ERROR: %@", [[error userInfo] description] );
    }

    if( controller != nil )
    { // show login controller for player to sign in
      UIViewController* root = ci::app::getWindow()->getNativeViewController();
      [root presentViewController:controller animated:YES completion:nil];
    }
    else if ( player.authenticated )
    { // player is ready to go (not necessarily online)
      callback( true );
    }
    else
    { // player is not authenticated
      callback( false );
    }
  };
}

- (void)loadPlayerAchievements:(AchievementFn)callback
{
  [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *error) {
    callback( achievements, error );
  }];
}

- (void)loadPlayerScores:(ScoreFn)callback inCategories:(const std::vector<std::string> &)categories
{ // accumulate high scores for local player from each scoreboard and pass to callback
  int total_size = categories.size();
  NSMutableArray *all_scores = [NSMutableArray array];

  for( auto &c : categories )
  {
    GKLeaderboard *board = [[[GKLeaderboard alloc] initWithPlayerIDs:[NSArray arrayWithObject:[[self localPlayer] playerID]]] autorelease];
    board.timeScope = GKLeaderboardTimeScopeAllTime;
    board.category = [NSString stringWithCString:c.c_str() encoding:[NSString defaultCStringEncoding]];
    [board loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error) {
      @synchronized(all_scores)
      { // get player's score if it exists
        GKScore *score = board.localPlayerScore;
        if( score == nil )
        { // if the player has never played this mode before, give them zero points
          score = [[[GKScore alloc] initWithCategory:board.category] autorelease];
          score.value = 0;
          score.context = 0;
        }
        // add the remote score or the new score to the collection
        [all_scores addObject:score];
        if( [all_scores count] == total_size )
        {
          callback( all_scores, error );
        }
      }
    }];
  }
}

- (void)reportScore:(const std::string &)scoreboard withPoints:(int64_t)score andContext:(uint64_t)context
{
  NSString *category = [NSString stringWithCString:scoreboard.c_str() encoding:[NSString defaultCStringEncoding]];
  GKScore *score_reporter = [[[GKScore alloc] initWithCategory:category] autorelease];
  score_reporter.value = score;
  score_reporter.context = context;
  score_reporter.shouldSetDefaultLeaderboard = YES;
  NSLog( @"Reporting score to category: %@, with value: %lld", score_reporter.category, score_reporter.value );
  [score_reporter reportScoreWithCompletionHandler:^(NSError *error) {
    if( error != nil )
    {
      NSLog( @"Error reporting score: %@", error );
    }
  }];
}

- (void)reportAchievement:(const std::string &)achievement percentComplete:(double)percent withBanner:(BOOL)showsBanner
{
  NSString *achievement_id = [NSString stringWithCString:achievement.c_str() encoding:[NSString defaultCStringEncoding]];
  GKAchievement *achievement_reporter = [[[GKAchievement alloc] initWithIdentifier:achievement_id] autorelease];
  if( achievement_reporter )
  {
    achievement_reporter.percentComplete = percent;
    achievement_reporter.showsCompletionBanner = showsBanner;
    NSLog( @"Reporting achievement to category: %@, with completion; %f", achievement_reporter.identifier, achievement_reporter.percentComplete );
    [achievement_reporter reportAchievementWithCompletionHandler:^(NSError *error) {
      if( error != nil )
      {
        NSLog( @"Error reporting achievement %@", error );
      }
    }];
  }
}

- (bool)isPlayerUnderage
{
  if( [self localPlayer] != nil )
  {
    return [self localPlayer].underage;
  }
  return false;
}

@end

//
//  C++ Implementation (facade for Objective-C method calls)
//

CascadeGameCenter::CascadeGameCenter()
{
  mImpl = [[CascadeGameCenterDelegate alloc] init];
  [mImpl retain];
}

CascadeGameCenter::~CascadeGameCenter()
{
  [mImpl release];
}

void CascadeGameCenter::authenticatePlayer( function<void (bool)> callback )
{
  mGameCenterEnabled = false; // set to false now, will get overwritten in callback
  auto fn = [this, callback](bool success)
  {
    mGameCenterEnabled = success;
    if( callback ){ callback( success ); }
  };
  [mImpl authenticateLocalPlayerWithCallback:fn];
}

bool CascadeGameCenter::isPlayerUnderage() const
{
  return [mImpl isPlayerUnderage];
}

fs::path CascadeGameCenter::localFile()
{
  return getDocumentsDirectory() / "gk";
}

ci::JsonTree CascadeGameCenter::defaultJson()
{
  JsonTree json;
  JsonTree a = JsonTree::makeObject("achievements");
  JsonTree s = JsonTree::makeObject("scores");
  json.pushBack( a );
  json.pushBack( s );
  return json;
}

void CascadeGameCenter::loadPlayerData()
{
  // load local achievements and scores from disk
  mLocalDataMutex.lock();
  mLocalData.read( localFile(), defaultJson() );
  mLocalAchievementMutex.lock();
  for( auto &a : mLocalData.getData( "achievements" ).getChildren() )
  {
    mLocalAchievements[a.getKey()] = Achievement{ a.getChild("percent-complete").getValue<int>() };
  }
  auto local_achievements = mLocalAchievements;
  mLocalAchievementMutex.unlock();
  mLocalScoreMutex.lock();
  for( auto &s : mLocalData.getData("scores").getChildren() )
  {
    mLocalScores[s.getKey()] = Score{ s["value"].getValue<int64_t>(), s["context"].getValue<uint64_t>() };
  }
  auto local_scores = mLocalScores;
  mLocalScoreMutex.unlock();
  mLocalDataMutex.unlock();

  // Create callbacks for Obj-C achievement- and score-loading methods
  AchievementFn achievement_fn = [this, local_achievements](NSArray *descriptions, NSError *error){
    map<string, Achievement> local_only;
    map<string, Achievement> remote;
    map<string, Achievement> remote_only;
    for( GKAchievement *a in descriptions )
    {
      string name = [[a identifier] UTF8String];
      double completion = a.percentComplete;
      remote[name] = Achievement( completion );
    }
    // build local only list
    for( auto &pair : local_achievements )
    { // only in local, or local is more complete
      if( remote.count(pair.first) == 0
         || remote[pair.first].percent_complete < pair.second.percent_complete )
      {
        local_only.insert( pair );
      }
    }
    for( auto &pair : remote )
    { // only in the remote, or remote is more complete
      if( local_achievements.count(pair.first) == 0
         || local_achievements.find(pair.first)->second.percent_complete < pair.second.percent_complete )
      { // can't use operator[] on local, since it's immutable within the lambda
        remote_only.insert( pair );
      }
    }

    for( auto &pair : local_only )
    { // report achievements earned while offline
      [mImpl reportAchievement:pair.first percentComplete:pair.second.percent_complete withBanner:NO];
    }

    if( !remote_only.empty() )
    { // store achievements earned on another device/previously
      lock_guard<std::mutex> lck( mLocalAchievementMutex );
      for( auto &pair : remote_only )
      { // add remote-only achievements to our local store (e.g. if they switched devices or had removed app)
        // if they are more complete than our current local achievement
        if( (mLocalAchievements.count(pair.first) == 0)
           ||  (mLocalAchievements[pair.first].percent_complete < pair.second.percent_complete) )
        { // replace local pair with more complete remote
          mLocalAchievements[pair.first] = pair.second;
        }
      }
    }
    if( error != nil )
    {
      NSLog( @"GameKit Achievement Error: %@", error );
    }
  };

  ScoreFn score_fn = [this, local_scores](NSArray *scores, NSError *error){
    // report any local scores not on server
    // remove any scores from local found on server
    map<string, Score> remote;
    for( GKScore *score in scores )
    {
      remote[ [score.category UTF8String] ] = Score{ score.value, score.context };;
    }

    for( auto &pair : local_scores )
    { // report local scores earned while offline

      if( remote.count(pair.first) == 0
         ||  remote[pair.first].value < pair.second.value )
      { // report local score if it is higher than the remote score
        [mImpl reportScore:pair.first withPoints:pair.second.value andContext:pair.second.context];
      }
    }
    if( !remote.empty() )
    {
      lock_guard<std::mutex> lck( mLocalScoreMutex );
      for( auto &pair : remote )
      {
        if( mLocalScores.count( pair.first ) == 0
           ||  mLocalScores[pair.first].value < pair.second.value )
        { // store remote score if it
          mLocalScores[pair.first] = pair.second;
        }
      }
    }
    if( error != nil )
    {
      NSLog( @"GameKit Score Error: %@", error );
    }
  };
  // TODO: make categories a parameter for this function for generic use
  vector<string> scoreboard_categories = {"wordshift.strategy_mode_score", "wordshift.speed_mode_score", "wordshift.hard_mode_score"};

  if( isEnabled() )
  {
    [mImpl loadPlayerAchievements:achievement_fn];
    [mImpl loadPlayerScores:score_fn inCategories:scoreboard_categories ];
  }
}

int64_t CascadeGameCenter::getPlayerHighScore(const std::string &scoreboard_id) const
{
  std::lock_guard<std::mutex> lck(mLocalScoreMutex);
  if( mLocalScores.count( scoreboard_id ) )
  {
    return mLocalScores.find( scoreboard_id )->second.value;
  }
  return 0;
}

void CascadeGameCenter::reportScore(const std::string &scoreboard_id, int64_t value, uint64_t context)
{ // always report scores
  if( isEnabled() )
  {
    [mImpl reportScore:scoreboard_id withPoints:value andContext:context];
  }
  mLocalScoreMutex.lock();
  if( mLocalScores.count( scoreboard_id ) == 0
     ||  mLocalScores[scoreboard_id].value < value )
  { // cache on disk if higher than our previous score
    mLocalScores[scoreboard_id] = Score{ value, context };
  }
  mLocalScoreMutex.unlock();
}

void CascadeGameCenter::reportAchievement(const std::string &achievement_id, double percent)
{ // only report achievements that are improvements over past
  mLocalAchievementMutex.lock();
  if( (mLocalAchievements.count( achievement_id ) == 0) // no achievement yet
     ||  (percent > mLocalAchievements[achievement_id].percent_complete) ) // achievement is more complete
  {
    mLocalAchievements[achievement_id] = Achievement( percent );
    if( isEnabled() )
    {
      [mImpl reportAchievement:achievement_id percentComplete:percent withBanner:YES];
    }
  }
  mLocalAchievementMutex.unlock();
}

void CascadeGameCenter::showGameCenter()
{
  if( isEnabled() )
  {
    [mImpl showGameCenter];
  }
}

CascadeGameCenter& CascadeGameCenter::getShared()
{
  static CascadeGameCenter gc;
  return gc;
}

void CascadeGameCenter::resignActive()
{
  savePlayerData();
  mGameCenterEnabled = false;
}

void CascadeGameCenter::savePlayerData()
{
  // write directly to local data tree
  std::lock_guard<std::mutex> data_lock( mLocalDataMutex );
  auto &achievements = mLocalData.getData( "achievements" );
  auto &scores = mLocalData.getData( "scores" );
  { // write achievements
    std::lock_guard<std::mutex> lck( mLocalAchievementMutex );
    for( auto &pair : mLocalAchievements )
    {
      JsonTree a = JsonTree::makeObject(pair.first);
      a.pushBack( JsonTree("percent-complete", pair.second.percent_complete) );
      achievements.pushBack( a );
    }
  }
  { // write scores
    std::lock_guard<std::mutex> lck( mLocalScoreMutex );
    for( auto &pair : mLocalScores )
    {
      JsonTree score = JsonTree::makeObject(pair.first);
      score.pushBack( JsonTree("value", pair.second.value) );
      score.pushBack( JsonTree("context", pair.second.context) );
      scores.pushBack( score );
    }
  }
  mLocalData.write( localFile() );
}

