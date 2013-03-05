//
//  GestureFactory
//
//  Created by rockwell on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxSwipeEvent.h"
#include "Hand.h"

enum GestureSendMode {
    SEND_ALL = 0,
    SEND_FASTEST,
    SEND_CLOSEST
};

enum GestureDetectMode {
    VELOCITY = 0,
    DISTANCE
};

class GestureFactory
{
public:
    
    GestureFactory();
    
    void updateBlob( int id, float x, float y, float z = 0 );
    void update();
    void draw( int x, int y, int width, int height );
    
    // gesture thresholds
    float   stationaryThreshold;
    float   horizontalThreshold;
    float   verticalThreshold;
    float   horizontalDistance;
    float   verticalDistance;
    
    // how long to wait between gestures
    int     gestureWait;
    
    // number of frames to average for gestures
    int     averageFrames;
    
    // how long before "valid" hand
    int     handWait;
    
    // what gesture to start with
    void setStartGesture( ofxGestureType startType );
    ofxGestureType getStartGesture();
    
    // mode
    GestureSendMode         sendMode;
    GestureDetectMode       detectMode;
    
    // events
    
    ofEvent<ofxSwipeEvent> onSwipeUpEvent;
    ofEvent<ofxSwipeEvent> onSwipeDownEvent;
    ofEvent<ofxSwipeEvent> onSwipeLeftEvent;
    ofEvent<ofxSwipeEvent> onSwipeRightEvent;
    ofEvent<ofxSwipeEvent> onHeldEvent;

    map<int,Hand> hands;
    
private:
    ofxGestureType  startGesture;
    bool            bSetup;
    ofPoint         centroid;
    
    map<int, ofxSwipeEvent>   lastEvents;
    map<int, ofxSwipeEvent>  toSend;
    int             lastGestureSent;
};

