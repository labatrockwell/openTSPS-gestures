//
//  CustomDelegate.cpp
//  openTSPS
//
//  Created by BRenfer on 1/14/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "CustomDelegate.h"

static int numGenerators = 0;

//--------------------------------------------------------------
CustomDelegate::CustomDelegate( int _id ) :
ofxTSPS::Delegate(_id)
{
}

//--------------------------------------------------------------
void CustomDelegate::setup(){
    ofxTSPS::Delegate::setup();
    
    // add your custom source
    peopleTracker.setSource(source);
    
    // setup OpenNI source
    source.openSource(320,240);
    
    // ofxOpenNI is :( about > 1 gesture generator...
    if ( numGenerators == 0 ){
        // add gestures + hands, which we will send as custom data=
        numGenerators++;
        source.addGestureGenerator();
        source.addAllGestures();
        source.addHandsGenerator();
        ofAddListener(source.gestureEvent, this, &CustomDelegate::onOpenNIGesture);
        ofAddListener(source.handEvent, this, &CustomDelegate::onOpenNIHand);
        
        // customize openNI to make it deal better with hands
        source.setMaxNumHands(3);
        source.setMinTimeBetweenHands(1000);
    }
    
    // setup swipe detector
    //SwipeDetectorONI.setup( source );
    //gestureGenerator.setup( &source );
    ofAddListener(gestureGenerator.onSwipeUpEvent, this, &CustomDelegate::onSwipeUp);
    ofAddListener(gestureGenerator.onSwipeDownEvent, this, &CustomDelegate::onSwipeDown);
    ofAddListener(gestureGenerator.onSwipeLeftEvent, this, &CustomDelegate::onSwipeLeft);
    ofAddListener(gestureGenerator.onSwipeRightEvent, this, &CustomDelegate::onSwipeRight);
    ofAddListener(gestureGenerator.onHeldEvent, this, &CustomDelegate::onHeld);
    
    autoThreshold   = 255.0f;
    thresholdBuffer = .9f;
    
    // add stuff to gui
    peopleTracker.addSlider("Gesture Sensitivity Horz", &gestureGenerator.horizontalThreshold, 0, 100);
    peopleTracker.addSlider("Gesture Sensitivity Vert", &gestureGenerator.verticalThreshold, 0, 100);
    peopleTracker.addSlider("Gesture Distance Horz", &gestureGenerator.horizontalDistance, 0, 640);
    peopleTracker.addSlider("Gesture Distance Vert", &gestureGenerator.verticalDistance, 0, 480);
    peopleTracker.addSlider("Number of frames to avg", &gestureGenerator.averageFrames, 0, 100);
    peopleTracker.addSlider("Threshold buffer", &thresholdBuffer, 0.0f, 1.0f);
    peopleTracker.addSlider("Time to wait btw gestures", &gestureGenerator.gestureWait, 0, 10000);
}

//--------------------------------------------------------------
void CustomDelegate::update(){
    //SwipeDetectorONI.update();
    
    // a-mazing auto thresholding
    cv::Point minLoc, maxLoc;
    double minVal = 0, maxVal = 0;
    
    ofImage tempImage;
    tempImage.setFromPixels( peopleTracker.getWarpedImage()->getPixelsRef() );
    cv::Mat cameraMat = ofxCv::toCv( tempImage );
    cv::minMaxLoc( cameraMat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    
    ofColor c = tempImage.getColor( maxLoc.x, maxLoc.y );
    autoThreshold = autoThreshold * .9 + c.r * .1;
    
    peopleTracker.setThreshold( autoThreshold * thresholdBuffer );
    
    // normal update
    peopleTracker.update();
    
    // update gesture tracker from blobs
    for ( int i=0; i<peopleTracker.totalPeople(); i++){
        ofxTSPS::Person * p = peopleTracker.personAtIndex(i);
        gestureGenerator.updateBlob( p->pid, p->highest.x, p->highest.y );
    }
    
    gestureGenerator.update();
}

//--------------------------------------------------------------
void CustomDelegate::draw(){
    ofxTSPS::Delegate::draw();
    
    ofPushMatrix();
    ofTranslate(350, 20);
    gestureGenerator.draw();
    ofPopMatrix();
}

#pragma mark gesture events

//--------------------------------------------------------------
void CustomDelegate::onOpenNIGesture( ofxOpenNIGestureEvent & e ){
    // this is a little clunky right now!
    // you can either make your own string, pass in a vector of strings,
    // or pass in a map that will get passed into a message. open to suggestions!
    
    map<string,string> params;
    params["timestampMillis"]   = ofToString(e.timestampMillis);
    params["progress"]          = ofToString(e.progress);
    params["worldPositionX"]    = ofToString(e.worldPosition.x);
    params["worldPositionY"]    = ofToString(e.worldPosition.y);
    params["positionX"]         = ofToString((float) e.position.x / ofGetWidth());
    params["positionY"]         = ofToString((float) e.position.y / ofGetHeight());
    
    // trigger custom event!
    //peopleTracker.triggerCustomEvent( "openNI"+e.gestureName, params );
}

//--------------------------------------------------------------
void CustomDelegate::onOpenNIHand( ofxOpenNIHandEvent & e ){
    // this is a little clunky right now!
    // you can either make your own string, or pass in a vector of strings
    // that will get passed into a message. open to suggestions!
    
    // only send it if it has a good status
    if ( e.handStatus == HAND_TRACKING_STARTED || e.handStatus == HAND_TRACKING_UPDATED){
        
        map<string,string> params;
        params["timestampMillis"]   = ofToString(e.timestampMillis);
        params["id"]                = ofToString(e.id);
        params["worldPositionX"]    = ofToString(e.worldPosition.x);
        params["worldPositionY"]    = ofToString(e.worldPosition.y);
        params["positionX"]         = ofToString((float) e.position.x / ofGetWidth());
        params["positionY"]         = ofToString((float) e.position.y / ofGetHeight());
        
        // trigger custom event!
        //peopleTracker.triggerCustomEvent( "openNIHand", params );
    } else if ( e.handStatus == HAND_TRACKING_STOPPED ){
        map<string,string> params;
        params["timestampMillis"]   = ofToString(e.timestampMillis);
        params["id"]                = ofToString(e.id);
        params["worldPositionX"]    = ofToString(e.worldPosition.x);
        params["worldPositionY"]    = ofToString(e.worldPosition.y);
        params["positionX"]         = ofToString((float) e.position.x / ofGetWidth());
        params["positionY"]         = ofToString((float) e.position.y / ofGetHeight());
        
        // trigger custom event!
        //peopleTracker.triggerCustomEvent( "openNIHandStopped", params );
    }
}


//--------------------------------------------------------------
void CustomDelegate::onSwipeUp( ofxSwipeEvent & e ){
    map<string,string> params;
    params["strength"]      = ofToString(e.velocity.y);
    params["angle"]         = ofToString(e.angle);
    params["positionX"]     = ofToString(e.position.x);
    params["positionY"]     = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "swipeUp", params );
    //cout<< id << " up" << endl;
}

//--------------------------------------------------------------
void CustomDelegate::onSwipeDown( ofxSwipeEvent & e ){
    map<string,string> params;
    params["strength"] = ofToString(e.velocity.y);
    params["angle"]     = ofToString(e.angle);
    params["positionX"]      = ofToString(e.position.x);
    params["positionY"]      = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "swipeDown", params );
    //cout<< id << " down" << endl;
}

//--------------------------------------------------------------
void CustomDelegate::onSwipeLeft( ofxSwipeEvent & e ){
    map<string,string> params;
    params["strength"] = ofToString(e.velocity.x);
    params["angle"]     = ofToString(e.angle);
    params["positionX"]      = ofToString(e.position.x);
    params["positionY"]      = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "swipeLeft", params );
    //cout<< id << " left" << endl;
}

//--------------------------------------------------------------
void CustomDelegate::onSwipeRight( ofxSwipeEvent & e ){
    map<string,string> params;
    params["strength"] = ofToString(e.velocity.x);
    params["angle"]     = ofToString(e.angle);
    params["positionX"]      = ofToString(e.position.x);
    params["positionY"]      = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "swipeRight", params );
    //cout<< id << " right" << endl;
}


//--------------------------------------------------------------
void CustomDelegate::onHeld( ofxSwipeEvent & e ){
    map<string,string> params;
    params["duration"] = ofToString( e.duration );
    params["positionX"]      = ofToString(e.position.x);
    params["positionY"]      = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "held", params );
    //cout<< id << " held" << endl;
}