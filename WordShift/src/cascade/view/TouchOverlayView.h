//
//  TouchOverlayView.h
//  WordShift
//
//  Created by David Wicks on 5/9/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

namespace cascade
{
  class TouchOverlayView
  {
  public:
    struct TouchView {
      ci::Vec2f       loc;
      ci::Anim<float> scale;
      ci::Anim<float> alpha;
      bool            alive;
    };
    enum{
      NO_GROUP = INT_MIN
    };
    TouchOverlayView();
    ~TouchOverlayView();
    //! display touch overlays on \a window, optionally connecting to signalDraw in \a draw_group
    void connect( ci::app::WindowRef window, int draw_group=NO_GROUP );
    //! stop running/displaying the touch overlays (called automatically on destruction)
    void disconnect();
    void setGraphic( const ci::Surface &surface );
  private:
    ci::gl::Texture mTouchTexture;
    ci::Rectf       mTextureSize = ci::Rectf( -32, -32, 32, 32 );
    ci::TimelineRef mTimeline = ci::Timeline::create();
    ci::Timer       mTimer;
    typedef std::shared_ptr<TouchView> TouchViewRef;
    std::map<uint32_t, TouchViewRef>      mTouches;
    std::vector<TouchViewRef>             mDyingTouches;
    std::vector<ci::signals::connection>  mConnections;

    void storeConnection( ci::signals::connection connection ){ mConnections.push_back( connection ); }
    void update();
    void draw();
    void touchesBegan( const ci::app::TouchEvent &event );
    void touchesMoved( const ci::app::TouchEvent &event );
    void touchesEnded( const ci::app::TouchEvent &event );
  };
}
