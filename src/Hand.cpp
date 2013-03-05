//
//  Hand.cpp
//  openTSPS
//
//  Created by BRenfer on 12/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Hand.h"

//--------------------------------------------------------------
Hand::Hand(){
    setup();
}

//--------------------------------------------------------------
Hand::Hand( ofPoint p ) : ofPoint(p){
    setup();
}

//--------------------------------------------------------------
Hand::Hand( int x, int y, int z ) : ofPoint(x,y,z){
    setup();
}

//--------------------------------------------------------------
void Hand::idle(){
    notUpdatedIn++;
}

//--------------------------------------------------------------
void Hand::update( ofPoint pos ){
    update( pos.x, pos.y, pos.z );
}

//--------------------------------------------------------------
void Hand::update( float x, float y, float z ){
    notUpdatedIn = 0;
    lastPosition.set( *this );
    velocityHistory.insert(velocityHistory.begin(),velocity);
    positionHistory.insert(positionHistory.begin(),lastPosition);
    
    if ( velocityHistory.size() > numFramesToAverage ){
        velocityHistory.erase( velocityHistory.end() );
        positionHistory.erase( positionHistory.end() );
    }
    
    averageVelocity.set(0,0);
    distanceTraveled.set(0,0);
    if ( velocityHistory.size() > 0 ){
        for ( int i=0; i<velocityHistory.size(); i++){
            averageVelocity     += velocityHistory[i];
        }
        averageVelocity /= (float) velocityHistory.size();
        distanceTraveled.set( x,y );
        distanceTraveled -= positionHistory[ positionHistory.size() - 1];
    }
    
    set(x,y,z);
    velocity = *this - lastPosition;
    age = ofGetElapsedTimeMillis() - timeStarted;
}

//--------------------------------------------------------------
void Hand::clearHistory(){
    velocityHistory.clear();
    positionHistory.clear();
}

//--------------------------------------------------------------
void Hand::setup(){
    gestureHappened     = false;
    bDidStartGesture    = true;
    timeStarted         = ofGetElapsedTimeMillis();
    age                 = 0;
    numFramesToAverage  = 10;
    notUpdatedIn        = 0;
}

//--------------------------------------------------------------
void Hand::draw( int width, int height ){
    static ofPoint dims( width, height);
    
    ofPushStyle();{
        ofNoFill();
        ofSetColor( 255 );
        ofCircle( *this * dims, 20);
        ofLine( *this * dims, *this * dims + velocity * dims );
        ofSetColor(255,0,0);
        ofLine( *this * dims, *this * dims + averageVelocity * dims );
        ofDrawBitmapString( ofToString( distanceTraveled * dims ), *this * dims + ofPoint(20,20));
        ofSetColor(255);
        ofPoint * last = NULL;
        ofFill();
        for ( int i=0; i<positionHistory.size(); i++){
            if ( last != NULL ){
                ofLine(*last * dims, positionHistory[i] * dims);
            }
            ofCircle(positionHistory[i] * dims, 2);
            last = &positionHistory[i];
        }
    } ofPopStyle();
}