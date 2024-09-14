//
//  Drawing.cpp
//  WordShift
//
//  Created by David Wicks on 5/7/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Drawing.h"

using namespace pockets;
using namespace cinder;
using namespace std;

void cascade::drawPie( const SpriteData &texture_info, float time, int segments )
{
  vector<Vec2f> positions;
  vector<Vec2f> texcoords;
  Rectf texture_bounds = texture_info.getTextureBounds();
  Vec2f inside( texture_bounds.getX1(), texture_bounds.getCenter().y );
  Vec2f outside( texture_bounds.getX2(), texture_bounds.getCenter().y );
  float radius = texture_info.getSize().x;
  float t_begin = -M_PI / 2;
  float t_end = t_begin + 2 * M_PI * time;
  // build the pie
  positions.emplace_back( 0, 0 );
  texcoords.push_back( inside );
  for( int i = 0; i < segments; ++i )
  {
    float theta = lmap<float>( i, 0, segments - 1, t_begin, t_end );
    float x = radius * math<float>::cos( theta );
    float y = radius * math<float>::sin( theta );
    positions.emplace_back( x, y );
    texcoords.push_back( outside );
  }

  glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, &positions[0] );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, &texcoords[0] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, positions.size() );
	glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

