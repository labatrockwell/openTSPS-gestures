//
//  GestureFactory.cpp
//
//  Created by rockwell on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GestureFactory.h"

//--------------------------------------------------------------
GestureFactory::GestureFactory(){
    context = NULL;
    bSetup = false;
    
    stationaryThreshold = 10;
    horizontalThreshold = 20;
    verticalThreshold   = 20;
    averageFrames       = 10;
}

//--------------------------------------------------------------
void GestureFactory::setup( ofxOpenNI * ctx ){
    context = ctx;
    bSetup = true;
}

//--------------------------------------------------------------
void GestureFactory::update(){
    if ( context == NULL || !bSetup ) return;
    
    map<int, Hand> toErase = hands;
    map<int, Hand>::iterator it = hands.begin();
    for (it; it != hands.end(); ++it){
        it->second.numFramesToAverage = averageFrames;
    }
    
    // loop through current hands
    for (int i=0; i<context->getNumTrackedHands(); i++){
        ofxOpenNIHand & h = context->getTrackedHand(i);
        
        int ID = (int) h.getID();
        
        // do we have this one?
        if ( hands.count( ID ) == 0 ){
            hands.insert(make_pair(ID, Hand( h.getPosition() )) );
        }
        
        hands[ ID ].update( h.getPosition() );
        toErase.erase( ID );
        
        // should be some sort of gesture object to check against
        
        // stationary
        if ( hands[ ID ].averageVelocity.length() < stationaryThreshold ){
            if ( lastEvent.type == STATIONARY ){
                lastEvent.duration = ofGetElapsedTimeMillis() - lastEvent.timeStarted;
            } else {
                lastEvent.duration      = 0;
                lastEvent.timeStarted   = ofGetElapsedTimeMillis();
            }
            if ( lastEvent.duration > 1000 ){
                ofNotifyEvent(onHeldEvent, lastEvent );
            }
            lastEvent.type = STATIONARY;
            
        // left / right
        } else if ( abs(hands[ ID ].averageVelocity.x) > abs(hands[ ID ].averageVelocity.y) ){
            if ( abs(hands[ ID ].averageVelocity.x) > horizontalThreshold ){
                lastEvent.angle = hands[ ID ].averageVelocity.angle(ofVec3f());
                lastEvent.velocity.set( hands[ ID ].averageVelocity.x, 0 );
                
                switch (ofSign(hands[ ID ].averageVelocity.x)) {
                    case 1:
                        ofNotifyEvent(onSwipeLeftEvent, lastEvent );
                        lastEvent.type = SWIPE_LEFT;
                        break;                        
                    case -1:
                        ofNotifyEvent(onSwipeRightEvent, lastEvent );
                        lastEvent.type = SWIPE_RIGHT;
                        break;
                }
            } else {
                lastEvent.duration  = 0;
                lastEvent.type      = CUSTOM;
            }
        // up / down
        } else if ( abs(hands[ ID ].averageVelocity.y) > abs(hands[ ID ].averageVelocity.x) ){
            if ( abs(hands[ ID ].averageVelocity.y) > verticalThreshold ){
                lastEvent.angle = hands[ ID ].averageVelocity.angle(ofVec3f());
                lastEvent.velocity.set( 0, hands[ ID ].averageVelocity.y );
                                
                switch (ofSign(hands[ ID ].averageVelocity.y)) {
                    case 1:
                        ofNotifyEvent(onSwipeUpEvent, lastEvent );
                        lastEvent.type = SWIPE_UP;
                        break;                        
                    case -1:
                        ofNotifyEvent(onSwipeDownEvent, lastEvent );
                        lastEvent.type = SWIPE_DOWN;
                        break;
                }
            } else {
                lastEvent.duration  = 0;
                lastEvent.type      = CUSTOM;
            }
        } else {
            lastEvent.duration  = 0;
            lastEvent.type      = CUSTOM;
        }
    }
    
    if ( context->getNumTrackedHands() == 0 ){
        lastEvent.duration  = 0;
        lastEvent.type      = CUSTOM;
    }
    
    map<int,Hand>::iterator eit = toErase.begin();
    for ( toErase; eit != toErase.end(); ++eit ){
        cout<<"erase "<<eit->first<<endl;
        //it->second.idle();
        hands.erase(eit->first);
    }
}

//--------------------------------------------------------------
void GestureFactory::draw(){
    map<int, Hand>::iterator it = hands.begin();
    ofPushStyle();
    ofNoFill();
    for (it; it != hands.end(); ++it){
        ofSetColor( 255 );
        ofCircle( it->second, 20);
        ofLine( it->second, it->second + it->second.velocity );
        ofSetColor(255,0,0);
        ofLine( it->second, it->second + it->second.averageVelocity );
        ofDrawBitmapString( ofToString( it->second.distanceTraveled ) + "\n" + ofToString( it->first ), it->second + ofPoint(20,20));
    }
    
    ofSetColor(255);
    ofDrawBitmapString("Event: "+ofToString(lastEvent.type) +" : "+ ofToString(lastEvent.duration), 20, 20 );
    
    ofPopStyle();
}
