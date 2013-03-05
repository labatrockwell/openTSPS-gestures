//
//  Hand.h
//  openTSPS
//
//  Created by BRenfer on 12/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class Hand : public ofPoint
{
public:
    int     age;
    int     timeStarted, lastGesture;
    int     numFramesToAverage;
    ofPoint lastPosition;
    ofPoint velocity, averageVelocity;
    ofPoint distanceTraveled;
    
    int     notUpdatedIn;
    bool    gestureHappened;
    
    Hand();
    Hand( ofPoint p );
    Hand( int x, int y, int z = 0 );
    
    void idle();void update( ofPoint pos );
    void update( float x, float y, float z = 0 );
    void draw( int width, int height );
    
    void clearHistory();
    
    bool bDidStartGesture;
    
private:
    
    void setup();
    vector<ofPoint> velocityHistory;
    vector<ofPoint> positionHistory;
};

