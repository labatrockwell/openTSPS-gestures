//
//  GestureFactory
//
//  Created by rockwell on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofxSwipeEvent.h"

// this could be more general, just using ofxOpenNI for now
#include "ofxOpenNI.h"

class Hand : public ofPoint
{
public:
    int     age;
    int     timeStarted;
    int     numFramesToAverage;
    ofPoint lastPosition;
    ofPoint velocity, averageVelocity;
    ofPoint distanceTraveled;
    
    Hand(){
        setup();
    }
    
    Hand( ofPoint p ) : ofPoint(p){
        setup();
    }
    
    Hand( int _id, int x, int y ) : ofPoint(x,y){
        setup();
    }
    
    void idle(){
        update( *this );
    }
    
    void update( ofPoint pos ){
        update( pos.x, pos.y );
    }
    
    void update( float x, float y ){
        lastPosition.set( *this );
        velocityHistory.insert(velocityHistory.begin(),velocity);

        if ( velocityHistory.size() > numFramesToAverage ){
            velocityHistory.erase( velocityHistory.end() );
        }
        
        averageVelocity.set(0,0);
        distanceTraveled.set(0,0);
        if ( velocityHistory.size() == numFramesToAverage ){
            for ( int i=0; i<numFramesToAverage; i++){
                averageVelocity     += velocityHistory[i];
                distanceTraveled    += velocityHistory[i];
            }
            averageVelocity /= (float) numFramesToAverage;
        }
        
        set(x,y);
        velocity = *this - lastPosition;
        age = ofGetElapsedTimeMillis() - timeStarted;
    }
    
private:
    void setup(){
        age = 0;
        timeStarted = ofGetElapsedTimeMillis();
        numFramesToAverage = 10;
    }
    vector<ofVec2f> velocityHistory;
};

class GestureFactory
{
public:
    
    GestureFactory();
    void setup( ofxOpenNI * context );
    void update();
    void draw();
    
    // gesture thresholds
    float   stationaryThreshold;
    float   horizontalThreshold;
    float   verticalThreshold;
    
    // number of frames to average for gestures
    int     averageFrames;
    
    // events
    
    ofEvent<ofxSwipeEvent> onSwipeUpEvent;
    ofEvent<ofxSwipeEvent> onSwipeDownEvent;
    ofEvent<ofxSwipeEvent> onSwipeLeftEvent;
    ofEvent<ofxSwipeEvent> onSwipeRightEvent;
    ofEvent<ofxSwipeEvent> onHeldEvent;

    map<int,Hand> hands;
    
private:
    bool            bSetup;
    ofxOpenNI *     context;
    ofxSwipeEvent   lastEvent;
    
};

