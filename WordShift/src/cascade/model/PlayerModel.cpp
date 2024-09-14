//
//  PlayerModel.cpp
//  WordShift
//
//  Created by David Wicks on 3/4/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "PlayerModel.h"

using namespace cascade;
using namespace std;

PlayerModel::PlayerModel( const string &name, ReadingDirection left_right, ReadingDirection top_bottom, PlayRulesRef end_condition ):
mName( name )
, mRight( left_right )
, mDown( top_bottom )
, mRules( end_condition )
{}

PlayerModel::~PlayerModel()
{}

void PlayerModel::setTurn( bool isTurn )
{
  if( isTurn )
  {
    mRules->enable();
  }
  else
  {
    mRules->disable();
  }
}

bool PlayerModel::swapAllowed() const
{
  return mRules->swapAllowed();
}

void PlayerModel::creditWithWord(const ScoredWord &word)
{
  mScore += word.score;
  mPlayedWords.push_back( word );
  mScoreMessenger.deliver( PlayerScoreMessage( word.word, word.score, mScore ) );
  mRules->assignCredit( word.word.size() );
}

void PlayerModel::update( float deltaTime )
{
  mRules->update( deltaTime );
}

