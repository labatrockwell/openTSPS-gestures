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
    
    int     notUpdatedIn;
    
    Hand(){
        setup();
    }
    
    Hand( ofPoint p ) : ofPoint(p){
        setup();
    }
    
    Hand( int x, int y, int z = 0 ) : ofPoint(x,y,z){
        setup();
    }
    
    void idle(){
        notUpdatedIn++;
    }
    
    void update( ofPoint pos ){
        update( pos.x, pos.y, pos.z );
    }
    
    void update( float x, float y, float z = 0 ){
        notUpdatedIn = 0;
        lastPosition.set( *this );
        velocityHistory.insert(velocityHistory.begin(),velocity);

        if ( velocityHistory.size() > numFramesToAverage ){
            velocityHistory.erase( velocityHistory.end() );
        }
        
        averageVelocity.set(0,0);
        distanceTraveled.set(0,0);
        if ( velocityHistory.size() > 0 ){
            for ( int i=0; i<velocityHistory.size(); i++){
                averageVelocity     += velocityHistory[i];
                distanceTraveled    += velocityHistory[i];
            }
            averageVelocity /= (float) velocityHistory.size();
        }
        
        set(x,y,z);
        velocity = *this - lastPosition;
        age = ofGetElapsedTimeMillis() - timeStarted;
    }
    
    void clearHistory(){
        velocityHistory.clear();
    }
    
private:
    void setup(){
        timeStarted = ofGetElapsedTimeMillis();
        age                 = 0;
        numFramesToAverage  = 10;
        notUpdatedIn        = 0;
    }
    vector<ofVec2f> velocityHistory;
};

enum GestureFactoryMode {
    SEND_ALL,
    SEND_FASTEST,
    SEND_CLOSEST
};

class GestureFactory
{
public:
    
    GestureFactory();
    // probably deprecated
    void setup( ofxOpenNI * context );
    void updateOpenNI();
    
    void updateBlob( int id, int x, int y, int z = 0 );
    void update();
    void draw();
    
    // gesture thresholds
    float   stationaryThreshold;
    float   horizontalThreshold;
    float   verticalThreshold;
    
    // number of frames to average for gestures
    int     averageFrames;
    
    // mode
    GestureFactoryMode mode;
    
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
    
    map<int, ofxSwipeEvent>   lastEvents;
    map<int, ofxSwipeEvent>  toSend;
};

