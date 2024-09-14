//
//  UIElement.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "UIElement.h"
#include "CollectionUtilities.hpp"

using namespace cascade;
using namespace cinder;
using namespace pockets;
using namespace std;

UIElement::UIElement():
mSize( 0, 0 )
{}

UIElement::UIElement( const Vec2f &size ):
mSize( size )
{}

void UIElement::appendChild(UIElementRef element)
{
  UIElementRef former_parent = element->getParent();
  if( former_parent ){ former_parent->removeChild( element ); }
  element->setParent( shared_from_this() );
  mChildren.push_back( element );
  childAdded( element );
}

void UIElement::removeChild(UIElementRef element)
{
  vector_remove( &mChildren, element );
  element->setParent( UIElementWeakRef() );
}

void UIElement::setParent(UIElementWeakRef parent)
{
  mParent = parent;
  UIElementRef p = mParent.lock();
  if( p )
  {
    mLocus->setParent( p->getLocus() );
  }
  else
  {
    mLocus->unsetParent();
  }
}

void UIElement::disconnect()
{
  for( signals::connection &connect : mConnections )
  {
    connect.disconnect();
  }
  mConnections.clear();
  customDisconnect();
}

void UIElement::deepDraw()
{
  draw();
  preChildDraw();
  for( UIElementRef &child : mChildren )
  {
    child->deepDraw();
  }
  postChildDraw();
}

void UIElement::deepCancelInteractions()
{
  cancelInteractions();
  for( UIElementRef &child : mChildren )
  {
    child->deepCancelInteractions();
  }
}

void UIElement::deepConnect(ci::app::WindowRef window)
{
  connect( window );
  for( UIElementRef &child : mChildren )
  {
    child->deepConnect( window );
  }
}

void UIElement::deepDisconnect()
{
  disconnect();
  for( UIElementRef &child : mChildren )
  {
    child->deepDisconnect();
  }
}

void UIElement::blockChildren()
{
  for( UIElementRef &child : mChildren )
  {
    child->block();
  }
}

void UIElement::unblockChildren()
{
  for( UIElementRef &child : mChildren )
  {
    child->unblock();
  }
}

void UIElement::block()
{
  customBlock();
  for( signals::connection &connect : mConnections )
  {
    mBlocks.push_back( signals::shared_connection_block( connect ) );
  }
}

void UIElement::unblock()
{
  mBlocks.clear();
  customUnblock();
}

void UIElement::setChildIndex(UIElementRef child, size_t index)
{
  vector_remove( &mChildren, child );
  index = math<int32_t>::min( index, mChildren.size() );
  mChildren.insert( mChildren.begin() + index, child );
}

int UIElement::totalHeight() const
{
  return math<int>::max( getHeight(), childHeight() );
}

int UIElement::childHeight() const
{
  int top = 0;
  int bottom = 0;
  for( auto &child : mChildren )
  {
    int y = child->getLoc().y;
    top = math<int>::min( y, top );
    bottom = math<int>::max( y + child->getHeight(), bottom );
  }

  return bottom - top;
}
















