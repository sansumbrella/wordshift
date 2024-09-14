//
//  PlayerDetailScene.cpp
//  WordShift
//
//  Created by David Wicks on 3/7/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "PlayerDetailScene.h"
#include "cinder/Text.h"
#include "OrientationHelpers.h"
#include "GameColor.h"
#include "SpriteButton.h"
#include "TextureNode.h"
#include "cinder/System.h"
#include "cinder/ip/Fill.h"
#include "DynamicTypeNode.h"
#include "CascadeGameCenterDelegate.h"

using namespace cascade;
using namespace cinder;
using namespace std;
using pk::Sprite;

PlayerDetailScene::PlayerDetailScene( vector<PlayerModelRef> players, const pk::SpriteData &resume_icon, const pk::SpriteData &quit_icon )
{
  mBackButton = ButtonRef( new SpriteButton( Sprite( resume_icon ) ) );
  mQuitButton = ButtonRef( new SpriteButton( Sprite( quit_icon ) ) );

  // base the view off of the frontmost player (winner or active pauser)
  const auto model = *players.front();

  setBounds( app::getWindowBounds() );
  bool landscape = (model.getLeftRight() != model.getTopBottom());
  // find screen positions relative to player
  mUpperLeft = getOrientedUpperLeft( getBounds(), model.getLeftRight(), model.getTopBottom() );
  mLowerLeft = getOrientedLowerLeft( getBounds(), model.getLeftRight(), model.getTopBottom() );
  mLocus->setLoc( mUpperLeft );
  mLocus->setRotation( readingDirectionAngle( model.getLeftRight(), model.getTopBottom() ) );
  // set up the text box size and margins for buttons
  const float margin = System::isDeviceIpad() ? getBounds().getWidth() / 16.0f : getBounds().getWidth() / 20.0f;
  Vec2f text_margins = System::isDeviceIpad() ? Vec2f( margin, margin ) : Vec2f( margin, margin * 2 );
  mScreenSize = landscape ? getBounds().getSize().yx() : getBounds().getSize();
  const float inv_scale = 1.0f / app::getWindow()->getContentScale();

  // Set up text box/scroll region
  Vec2f text_box_size = landscape ? getBounds().getSize().yx() : getBounds().getSize();
  text_box_size -= text_margins * 2 + Vec2f( 0, mBackButton->getHeight() * 2 );

  mScrollPane = ScrollPaneRef( new ScrollPane( text_box_size ) );
  mScrollPane->getLocus()->setParent( mLocus );
  mScrollPane->setLoc( text_margins + Vec2f( 0, mBackButton->getHeight() ) );

  vector<TextBlock>  text_blocks;
  // start with game header
  const int paragraph_spacing = players.size() > 1 ? text_margins.y : 0;
  text_blocks.push_back( { FontType::eHeading, createGameDescription( players ), paragraph_spacing } );
  for( auto player : players )
  {
    text_blocks.push_back( { FontType::eHeading, createPlayerHeading( player ), 0 } );
    text_blocks.push_back( { FontType::eBody, createPlayerBody( player, text_box_size.x, inv_scale ), paragraph_spacing } );
  }
  Vec2i pos{ 0, 0 };
  for( auto &t : text_blocks )
  {
    auto text = DynamicTypeNode::create( t.type, t.text );
    text->setLoc( pos );
    pos.y += text->getHeight() + t.spacing;
    mScrollPane->appendChild( move( text ) );
  }

  // allow scrolling a little above the bottom button
  mScrollPane->setBottomScrollPercent( (text_box_size.y - margin) / text_box_size.y );

  mBackButton->expandHitBounds( 6, 6 );
  mBackButton->setSelectFn( [=](){ emitBack(); } );
  mBackButton->setLoc( Vec2f( 0, text_box_size.y ) + mScrollPane->getLoc() );
  mBackButton->getLocus()->setParent( mLocus );

  mQuitButton->expandHitBounds( 6, 6 );
  mQuitButton->setSelectFn( [=](){ emitQuit(); } );
  mQuitButton->setLoc( Vec2f( 0, 0 ) );
  mQuitButton->getLocus()->setParent( mLocus );
}

PlayerDetailScene::~PlayerDetailScene()
{
  disconnect();
}

void PlayerDetailScene::connect(ci::app::WindowRef window)
{
  mBackButton->connect( window );
  mQuitButton->connect( window );
  mScrollPane->connect( window );
}

void PlayerDetailScene::customDisconnect()
{
  mBackButton->disconnect();
  mQuitButton->disconnect();
  mScrollPane->disconnect();
}

string PlayerDetailScene::createGameDescription( const vector<PlayerModelRef> &players )
{
  auto rules = players.front()->getRules();
  string player_prefix = [](int count){
    switch (count) {
      case 1:
        return "";
        break;
      case 2:
        return "Two-Player ";
        break;
      case 3:
        return "Three-Player ";
        break;
      case 4:
        return "Four-Player ";
        break;
      default:
        return "";
        break;
    }
  }( players.size() );


  string full_line = player_prefix + rules->getName();
  string stats = rules->getPlayStats();
  if( !stats.empty() ){ full_line += ": " + stats; }

  return full_line;
}

string PlayerDetailScene::createPlayerHeading( PlayerModelRef player )
{
  string name = player->getName();
  string score = toString( player->getScore() ) + " points";
  if( !player->getScoreboardId().empty() )
  {
    int64_t high_score = getGameCenter().getPlayerHighScore( player->getScoreboardId() );
    if( high_score > player->getScore() )
    {
      score += " (Best: " + toString( high_score ) + ")";
    }
    else if( player->getScore() > 0 )
    {
      score += " (Personal Best)";
    }
  }
  if( !name.empty() )
  {
    return name + "\n" + score;
  }
  return score;
}

string PlayerDetailScene::createPlayerBody(PlayerModelRef player, int text_width, float scale )
{
  // sort function for played words
  const auto by_score = []( const PlayerModel::ScoredWord &lhs, const PlayerModel::ScoredWord &rhs ){
    return lhs.score < rhs.score;
  };

  string played_words("");
  auto word_list = player->getPlayedWords();
  stable_sort( word_list.begin(), word_list.end(), by_score );
  int word_value = 0;
  string current_line("");
  for( auto &word : word_list )
  {
    if( word.score != word_value )
    { // start a new line
      word_value = word.score;
      played_words += current_line;
      current_line = toString( word.score ) + ": ";
      // remove last comma and start a new line for current score
      if( !played_words.empty() )
      {
        played_words = played_words.substr( 0, played_words.size() - 2 ) + "\n";
      }
    }

    if( getBodyTextureFont().measureString( current_line + word.word ).x * scale > text_width )
    {
      played_words += current_line + "\n";
      current_line.clear();
    }
    current_line += word.word + ", ";
  }
  played_words += current_line;
  played_words = played_words.empty()
  ? "No words played."
  : played_words.substr( 0, played_words.size() - 2 );

  return played_words;
}

void PlayerDetailScene::update()
{
  mScrollPane->update();
  mTimeline->step( 1.0f / 60.0f );
}

void PlayerDetailScene::appear()
{
  mLocus->setLoc( mLowerLeft );
  mTimeline->apply( &mSlidePosition, mLowerLeft, mUpperLeft, 0.48f, EaseOutQuint() )
  .updateFn( [this]()->void
            {
              mLocus->setLoc( mSlidePosition() );
            } );
}

void PlayerDetailScene::vanish(Callback finishFn)
{
  mTimeline->apply( &mSlidePosition, mLowerLeft, 0.4f, EaseInQuint() )
  .updateFn( [this]()->void
            {
              mLocus->setLoc( mSlidePosition() );
            } )
  .finishFn( finishFn );
}

void PlayerDetailScene::draw()
{
  gl::disable( GL_TEXTURE_2D );
  gl::pushModelView();
  gl::multModelView( *mLocus );
  gl::color( getBackgroundColor() );
  gl::drawSolidRect( Rectf( 0, 0, mScreenSize.x, mScreenSize.y ) );
  gl::popModelView();

  gl::enable( GL_TEXTURE_2D );
  gl::color( getAltTextColor() );
  mScrollPane->deepDraw();

  mBackButton->draw();
  mQuitButton->draw();
}
