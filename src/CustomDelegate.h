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
#include "NiteSource.h"

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
    ofxTSPS::Nite   source;
    
    // gesture detector
    //SwipeDetectorONI   SwipeDetectorONI;
    GestureFactory       gestureGenerator;
    
    // custom event
    void onSwipeUp( ofxSwipeEvent & e );
    void onSwipeDown( ofxSwipeEvent & e );
    void onSwipeLeft( ofxSwipeEvent & e );
    void onSwipeRight( ofxSwipeEvent & e );
    void onHeld( ofxSwipeEvent & e );
    
    // calibration event
    void onCalibrationStarted( ofxNiteCalibrationEvent & e );
    void onCalibrationEnded( ofxNiteCalibrationEvent & e );
//    void onHandUpdated( ofxNiteHandEvent & e );
    
    // auto threshold
    float autoThreshold, minimumThreshold;
    float thresholdBuffer;
    
    // which gesture to use
    bool bUseWave, bOldUseWave;
    
};

