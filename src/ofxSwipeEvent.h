//
//  ofxSwipeEvent.h
//  openTSPS
//
//  Created by rockwell on 11/30/12.
//
//

#pragma once
#include "ofPoint.h"

enum ofxGestureType {
    SWIPE_UP,
    SWIPE_DOWN,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    STATIONARY,
    CUSTOM
};

class ofxSwipeEvent
{
public:    
    ofxGestureType  type;
    ofPoint         position;
    ofPoint         velocity;
    float           angle;
    float           duration;
    int             timeStarted;
    
    ofxSwipeEvent(){
        timeStarted = 0;
        type        = CUSTOM;
        angle       = 0;
        duration    = 0;
    }
};
