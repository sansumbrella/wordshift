//
//  UIElement.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "pockets/Locus2d.h"

namespace cascade
{
  /*
  A thing positioned on screen that can connect to a window.
  Uses UI signals for events, does not connect to draw signal.
  Has no default interactions, instead lets any interested children connect
  or disconnect to signals they care about.
  
  The simplest little scene graph, modeled after AS3 Sprites.
  */
  typedef std::shared_ptr<class UIElement> UIElementRef;
  typedef std::weak_ptr<class UIElement> UIElementWeakRef;
	class UIElement : public std::enable_shared_from_this<UIElement>
	{
	public:
    UIElement();
    UIElement( const ci::Vec2f &size );
		virtual ~UIElement(){ disconnect(); }
    void            deepDraw();
    void            deepConnect( ci::app::WindowRef window );
    void            deepDisconnect();
    //! call block() on all child objects
    void            blockChildren();
    //! call unblock() on all child objects
    void            unblockChildren();
    virtual void    draw(){}
    //! called in deepdraw before drawing children
    virtual void    preChildDraw(){}
    //! called in deepdraw after drawing children
    virtual void    postChildDraw(){}
    //! block all stored signals
    void            block();
    //! called immediately before stored signals are blocked
    virtual void    customBlock(){}
    //! unblock all stored signals
    void            unblock();
    //! called immediately after signals are unblocked
    virtual void    customUnblock(){}
    //! connect to any UI signals of interest here
    virtual void    connect( ci::app::WindowRef window ){}
    //! disconnects all stored connections
    void            disconnect();
    //! disconnect from any non-stored signals here
    virtual void    customDisconnect(){}
    //! stop whatever event-related tracking this object was doing
    virtual void    cancelInteractions(){}
    void            deepCancelInteractions();
    //! set top-left of element
    void            setLoc( const ci::Vec2f &loc ){ mLocus->setLoc( loc ); }
    ci::Vec2f       getLoc() const { return mLocus->getLoc(); }
    //! set rotation around z-axis
    void            setRotation( float radians ){ mLocus->setRotation( radians ); }
    void            setRegistrationPoint( const ci::Vec2f &loc ){ mLocus->setRegistrationPoint( loc ); }
    pk::Locus2dRef  getLocus(){ return mLocus; }
    ci::Vec2f       getSize() const { return mSize; }
    int             getWidth() const { return mSize.x; }
    int             getHeight() const { return mSize.y; }
    int             totalHeight() const;
    int             childHeight() const;
    //! add a UIElement as a child; will receive connect/disconnect events and have its locus parented
    void            appendChild( UIElementRef element );
    //! called when a child is added to this uielement
    virtual void    childAdded( UIElementRef element ){}
    void            removeChild( UIElementRef element );
    UIElementRef    getParent(){ return mParent.lock(); }

    // Child Manipulation
    size_t          numChildren() const { return mChildren.size(); }
    UIElementRef    getChildAt( size_t index ){ return mChildren.at( index ); }
    void            setChildIndex( UIElementRef child, size_t index );
    //! return child vector, allowing manipulation of each child, but not the vector
    const std::vector<UIElementRef>& getChildren() const { return mChildren; }
  protected:
    void            setHeight( float height ){ mSize.y = height; }
    void            setWidth( float width ){ mSize.x = width; }
    //! store a connection so it can be blocked/unblocked/disconnected later
    void            storeConnection( const ci::signals::connection &connection ){ mConnections.push_back( connection ); }
  private:
    pk::Locus2dRef            mLocus = pk::Locus2dRef( new pk::Locus2d );
    ci::Vec2i                 mSize;
    UIElementWeakRef          mParent;
    std::vector<UIElementRef> mChildren;

    void setParent(UIElementWeakRef parent);
    std::vector<ci::signals::connection>              mConnections;
    std::vector<ci::signals::shared_connection_block> mBlocks;
	};
}
