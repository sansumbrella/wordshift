//
//  HouseRules.cpp
//  WordShift
//
//  Created by David Wicks on 3/5/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "HouseRules.h"
#include "SpriteButton.h"

using namespace cascade;
using namespace cinder;
using namespace std;

HouseRules::HouseRules( const pk::SpriteSheet &sprite_sheet ):
PlayRules( "House Rules" )
, mButton( new SpriteButton( sprite_sheet.getSprite("end-turn-icon") ) )
{
  mButton->setSelectFn( [this](){ mButtonPressed = true; mButton->disconnect(); } );
  mButton->getLocus()->setParent( getLocus() );
  mButton->expandHitBounds( 6, 6 );
}

HouseRules::~HouseRules()
{}

void HouseRules::beginTurn()
{
  mButtonPressed = false;
  auto window = app::getWindow();
  mButton->connect( window );
}

void HouseRules::endTurn()
{
  mButton->disconnect();
}

void HouseRules::draw()
{
  mButton->draw();
}
