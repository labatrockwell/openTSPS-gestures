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
    
    // set URI of source
    void setUri( string uri );
    string getUri();
    
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
    void onHandLost( ofxNiteHandEvent & e );
//    void onHandUpdated( ofxNiteHandEvent & e );
    
    // auto threshold
    float autoThreshold, minimumThreshold, maximumThreshold;
    float thresholdBuffer;
    
    // which gesture to use
    bool bUseWave, bOldUseWave, bSendHand;
    bool bUseDistance, bOldUseDistance;
    
    // send hands at set interval
    int lastHandSent, handSendTime;
    
protected:
    
    string deviceUri;
    
};

