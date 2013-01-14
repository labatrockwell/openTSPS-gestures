//
//  CustomDelegate.h
//  openTSPS
//
//  Created by BRenfer on 1/14/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "TSPSDelegate.h"

// include OpenNI source
#include "OpenNI.h"

// include Swipe Gesture
//#include "SwipeDetectorONI.h"
#include "GestureFactory.h"

class CustomDelegate : public ofxTSPS::Delegate
{
public:
    
    CustomDelegate( int _id = 1 );
    
    void setup();
    void update();
    void draw();
    
    // custom openNI source
    ofxTSPS::OpenNI     source;
    
    // gesture detector
    //SwipeDetectorONI   SwipeDetectorONI;
    GestureFactory       gestureGenerator;
    
    // custom event
    void onOpenNIGesture( ofxOpenNIGestureEvent & e );
    void onOpenNIHand( ofxOpenNIHandEvent & e );
    void onSwipeUp( ofxSwipeEvent & e );
    void onSwipeDown( ofxSwipeEvent & e );
    void onSwipeLeft( ofxSwipeEvent & e );
    void onSwipeRight( ofxSwipeEvent & e );
    void onHeld( ofxSwipeEvent & e );
    
    // auto threshold
    float autoThreshold;
    float thresholdBuffer;
};

