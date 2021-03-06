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
    bUseWave = bOldUseWave = true;
    deviceUri = "";
}

//--------------------------------------------------------------
void CustomDelegate::setup(){
    peopleTracker.setUseKinect(false);
    
    // setup OpenNI source
    source.setSourceIndex(id);
    source.openSource(320,240, deviceUri );
    
    ofxTSPS::Delegate::setup();
    
    // add your custom source
    peopleTracker.setSource(source);
    
    // ofxOpenNI is :( about > 1 gesture generator...
    if ( numGenerators == 0 ){
        // add gestures + hands, which we will send as custom data=
        numGenerators++;
    }
    
    // setup swipe detector
    //SwipeDetectorONI.setup( source );
    //gestureGenerator.setup( &source );
    ofAddListener(gestureGenerator.onSwipeUpEvent, this, &CustomDelegate::onSwipeUp);
    ofAddListener(gestureGenerator.onSwipeDownEvent, this, &CustomDelegate::onSwipeDown);
    ofAddListener(gestureGenerator.onSwipeLeftEvent, this, &CustomDelegate::onSwipeLeft);
    ofAddListener(gestureGenerator.onSwipeRightEvent, this, &CustomDelegate::onSwipeRight);
    ofAddListener(gestureGenerator.onHeldEvent, this, &CustomDelegate::onHeld);
    
    ofAddListener( source.getTracker().calibrationStarted, this, &CustomDelegate::onCalibrationStarted );
    ofAddListener( source.getTracker().calibrationComplete, this, &CustomDelegate::onCalibrationEnded );
    ofAddListener( source.getTracker().handLost, this, &CustomDelegate::onHandLost );
    
    autoThreshold   = 255.0f;
    thresholdBuffer = .9f;
    minimumThreshold = 100;
    maximumThreshold = 255;
    handSendTime    = 500;
    lastHandSent    = 0;
    bUseDistance    = bOldUseDistance = true;
    
    // add stuff to gui
    peopleTracker.addToggle("Use wave gesture", &bUseWave);
    peopleTracker.addSlider("Minimum Threshold", &minimumThreshold, 0, 250);
    peopleTracker.addSlider("Maximum Threshold", &maximumThreshold, 0, 250);
    peopleTracker.addSlider("Threshold buffer", &thresholdBuffer, 0.0f, 1.0f);
    peopleTracker.addSlider("Gesture Sensitivity Horz", &gestureGenerator.horizontalThreshold, 0, 100);
    peopleTracker.addSlider("Gesture Sensitivity Vert", &gestureGenerator.verticalThreshold, 0, 100);
    peopleTracker.addSlider("Gesture Distance Horz", &gestureGenerator.horizontalDistance, 0.0, 1.0);
    peopleTracker.addSlider("Gesture Distance Vert", &gestureGenerator.verticalDistance, 0.0, 1.0);
    peopleTracker.addSlider("Number of frames to avg", &gestureGenerator.averageFrames, 0, 100);
    peopleTracker.addSlider("Time to wait btw gestures", &gestureGenerator.gestureWait, 0, 2000);
    peopleTracker.addSlider("How long (millis) until valid hand", &gestureGenerator.handWait, 0, 2000);
    peopleTracker.addToggle("Send hand position?", &bSendHand);
    peopleTracker.addSlider("Rate at which to send hands", &handSendTime, 0, 500);
    
    peopleTracker.addSlider("Camera tilt adjust", &source.tiltAmount, -1.0f, 1.0f);
    peopleTracker.addToggle("Distance-based gestures (velocity-based if off)", &bUseDistance);
    peopleTracker.addToggle("Trigger gesture on reverse direction", &gestureGenerator.bTriggerOnNegative);
    
}

//--------------------------------------------------------------
void CustomDelegate::update(){
    
    // a-mazing auto thresholding
    cv::Point minLoc, maxLoc;
    double minVal = 0, maxVal = 0;
    
    ofImage tempImage;
    tempImage.setFromPixels( peopleTracker.getWarpedImage()->getPixelsRef() );
    
    cv::Mat cameraMat = ofxCv::toCv( tempImage );
    cv::Mat maxedMat = ofxCv::toCv( tempImage );
    cv::min(cameraMat, maximumThreshold, maxedMat);
    cv::minMaxLoc( maxedMat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    
    ofColor c = tempImage.getColor( maxLoc.x, maxLoc.y );
    autoThreshold = autoThreshold * .9 + c.r * .1;
    
    peopleTracker.setThreshold( fmax(minimumThreshold, autoThreshold * thresholdBuffer) );
    
    // normal update
    peopleTracker.update();
    
    map<int, ofPoint> hands = source.getTracker().getHands();
    map<int, ofPoint>::iterator it = hands.begin();
    
    bool sendHands = bSendHand && ofGetElapsedTimeMillis() - lastHandSent > handSendTime;
    if ( sendHands ){
        lastHandSent = ofGetElapsedTimeMillis();
    }
    
    for ( it; it != hands.end(); it++){
        gestureGenerator.updateBlob( it->first, it->second.x / 320.0, it->second.y / 240.0, 0);//source.getTracker().getRawHand(it->first).z / 2000.0 );
        
        if ( sendHands ){
            map<string,string> params;
            params["id"]     = ofToString(it->first);
            params["x"]      = ofToString(it->second.x / 320.0, 3);
            params["y"]      = ofToString(it->second.y / 240.0, 3);
            peopleTracker.triggerCustomEvent( "hand", params );
        }
    }
    
    // check on gesture stuff
    if ( bUseWave != bOldUseWave ){
        bOldUseWave = bUseWave;
        if ( bUseWave ) {
            source.getTracker().addStartGesture(nite::GESTURE_WAVE);
            source.getTracker().removeStartGesture(nite::GESTURE_HAND_RAISE);
        } else {
            source.getTracker().addStartGesture(nite::GESTURE_HAND_RAISE);
            source.getTracker().removeStartGesture(nite::GESTURE_WAVE);
        }
    }
    if ( bUseDistance != bOldUseDistance ){
        gestureGenerator.detectMode = bUseDistance ? DISTANCE : VELOCITY;
    }
    gestureGenerator.update();
}

//--------------------------------------------------------------
void CustomDelegate::draw(){
    ofxTSPS::Delegate::draw();
    gestureGenerator.draw( 350, 20, 640, 480 );
}

//--------------------------------------------------------------
void CustomDelegate::setUri( string uri ){
    deviceUri = uri;
}

//--------------------------------------------------------------
string CustomDelegate::getUri(){
    return deviceUri;
}

#pragma mark gesture events

//--------------------------------------------------------------
void CustomDelegate::onSwipeUp( ofxSwipeEvent & e ){
    map<string,string> params;
    params["strength"]      = ofToString(e.velocity.y);
    params["angle"]         = ofToString(e.angle);
    params["positionX"]     = ofToString(e.position.x);
    params["positionY"]     = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "swipeUp", params );
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
}


//--------------------------------------------------------------
void CustomDelegate::onHeld( ofxSwipeEvent & e ){
    /*map<string,string> params;
    params["duration"] = ofToString( e.duration );
    params["positionX"]      = ofToString(e.position.x);
    params["positionY"]      = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "held", params );*/
}

//--------------------------------------------------------------
void CustomDelegate::onCalibrationStarted( ofxNiteCalibrationEvent & e ){
    map<string,string> params;
    params["positionX"]     = ofToString(e.position.x);
    params["positionY"]     = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "calibrationStarted", params );
    cout << "started" << endl;
}

//--------------------------------------------------------------
void CustomDelegate::onCalibrationEnded( ofxNiteCalibrationEvent & e ){
    map<string,string> params;
    params["positionX"]     = ofToString(e.position.x);
    params["positionY"]     = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "calibrationEnded", params );
}

//--------------------------------------------------------------
void CustomDelegate::onHandLost( ofxNiteHandEvent & e ){
    map<string,string> params;
    params["positionX"]     = ofToString(e.position.x);
    params["positionY"]     = ofToString(e.position.y);
    params["camera"]        = ofToString(id);
    peopleTracker.triggerCustomEvent( "handLost", params );
    cout << "LOST" << endl;
}
