//
//  GestureFactory.cpp
//
//  Created by rockwell on 11/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GestureFactory.h"

//--------------------------------------------------------------
GestureFactory::GestureFactory(){
    bSetup = false;
    
    stationaryThreshold = 10;
    horizontalThreshold = 20;
    verticalThreshold   = 20;
    averageFrames       = 10;
    sendMode            = SEND_CLOSEST;
    detectMode          = DISTANCE;
    gestureWait         = 2000;
    lastGestureSent     = 0;
    verticalDistance    = 100;  // this should eventually be normalized
    horizontalDistance  = 100;  // this should eventually be normalized
    startGesture        = SWIPE_RIGHT;
}

//--------------------------------------------------------------
void GestureFactory::updateBlob( int id, int x, int y, float z ){    
    // loop through current hands
    // do we have this one?
    if ( hands.count( id ) == 0 ){
        hands.insert(make_pair(id, Hand( x, y, z )) );
    }
    
    hands[ id ].update( x, y, z );

    if ( hands[id].age < 1000 ) return;

    // should be some sort of gesture object to check against
    
    ofPoint * checkAgainst  = NULL;
    ofPoint velocity        = hands[ id ].averageVelocity;
    float       hThresh     = horizontalThreshold;
    float       vThresh     = verticalThreshold;
    switch ( detectMode ) {
        case VELOCITY:
            checkAgainst = &hands[ id ].averageVelocity;
            break;
        case DISTANCE:
            checkAgainst = &hands[ id ].distanceTraveled;
            hThresh = horizontalDistance * ofMap(hands[id].z, .95, .5, 1, .25);
            vThresh = verticalDistance * ofMap(hands[id].z, .95, .75, 1, .25);
            break;
    }
    
    if ( checkAgainst == NULL ) return;
    
    // stationary
    if ( checkAgainst->length() < stationaryThreshold ){
        lastEvents[id].position     = hands[id];
        lastEvents[id].velocity.set( 0, 0 );
        
        if ( lastEvents[id].type == STATIONARY ){
            lastEvents[id].duration = ofGetElapsedTimeMillis() - lastEvents[id].timeStarted;
        } else {
            lastEvents[id].duration      = 0;
            lastEvents[id].timeStarted   = ofGetElapsedTimeMillis();
        }
        if ( lastEvents[id].duration > 1000 ){
            toSend.insert(make_pair( id, lastEvents[id]) );
            if ( sendMode == SEND_ALL) ofNotifyEvent(onHeldEvent, lastEvents[id] );
        }
        lastEvents[id].type = STATIONARY;
    } else if ( ofGetElapsedTimeMillis() - lastGestureSent > gestureWait ){
        // left / right
        if ( abs( checkAgainst->x ) > abs( checkAgainst->y ) ){
            if ( abs( checkAgainst->x ) > hThresh ){
                lastEvents[id].angle = checkAgainst->angle(ofVec3f());
                lastEvents[id].velocity.set( velocity.x, 0 );
                lastEvents[id].timeStarted  = ofGetElapsedTimeMillis();
                lastEvents[id].position     = hands[id];
                
                switch ( ofSign( checkAgainst-> x) ) {
                    case 1:
                        lastEvents[id].type = SWIPE_LEFT;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( sendMode == SEND_ALL){
                            lastGestureSent = ofGetElapsedTimeMillis();
                            ofNotifyEvent(onSwipeLeftEvent, lastEvents[id] );
                        }
                        break;
                    case -1:
                        lastEvents[id].type = SWIPE_RIGHT;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        
                        if ( sendMode == SEND_ALL){
                            lastGestureSent = ofGetElapsedTimeMillis();
                            ofNotifyEvent(onSwipeRightEvent, lastEvents[id] );
                        }
                        break;
                } 
                hands[id].clearHistory();
            } else {
                lastEvents[id].duration  = 0;
                lastEvents[id].type      = CUSTOM;
            }
            // up / down
        } else if ( abs( checkAgainst->y ) > abs( checkAgainst->x ) ){
            if ( abs( checkAgainst->y ) > vThresh ){
                lastEvents[id].angle = checkAgainst->angle(ofVec3f());
                lastEvents[id].velocity.set( 0, velocity.y );
                lastEvents[id].timeStarted  = ofGetElapsedTimeMillis();
                lastEvents[id].position     = hands[id];
                
                switch (ofSign( checkAgainst->y )) {
                    case -1:
                        lastEvents[id].type = SWIPE_UP;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( sendMode == SEND_ALL){
                            lastGestureSent = ofGetElapsedTimeMillis();
                            ofNotifyEvent(onSwipeUpEvent, lastEvents[id] );
                        }
                        break;
                    case 1:
                        lastEvents[id].type = SWIPE_DOWN;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( sendMode == SEND_ALL){
                            lastGestureSent = ofGetElapsedTimeMillis();
                            ofNotifyEvent(onSwipeDownEvent, lastEvents[id] );
                        }
                        break;
                }
                hands[id].clearHistory();
            } else {
                lastEvents[id].duration  = 0;
                lastEvents[id].type      = CUSTOM;
            }
        } else {
            lastEvents[id].duration  = 0;
            lastEvents[id].type      = CUSTOM;
        }
    } else {
        hands[id].clearHistory();
        lastEvents[id].duration  = 0;
        lastEvents[id].type      = CUSTOM;
    }
    checkAgainst = NULL;
}

//--------------------------------------------------------------
void GestureFactory::setStartGesture( ofxGestureType startType ){
    startGesture = startType;
}

//--------------------------------------------------------------
ofxGestureType GestureFactory::getStartGesture(){
    return startGesture;
}

//--------------------------------------------------------------
void GestureFactory::update(){
    int closestId               = -1;
    Hand          * closest     = NULL;
    ofxSwipeEvent * eventToSend = NULL;
    map<int, ofxSwipeEvent>::iterator sendIt = toSend.begin();
    
    switch ( sendMode ) {
        case SEND_CLOSEST:
            for (sendIt; sendIt != toSend.end(); ++sendIt){
                if ( hands.count(sendIt->first) > 0 ){
                    if ( (closest == NULL || hands[sendIt->first].z < closest->z) && &hands[sendIt->first].bDidStartGesture ){
                        closest = &hands[sendIt->first];
                        eventToSend = &sendIt->second;
                    }
                } else {
                    cout<<"dead hand!"<<endl;
                }
            }
            
            break;
            
        case SEND_FASTEST:
            for (sendIt; sendIt != toSend.end(); ++sendIt){
                if ( hands.count(sendIt->first) > 0 ){
                    if ( closest == NULL || hands[sendIt->first].averageVelocity.length() < closest->averageVelocity.length() ){
                        closest = &hands[sendIt->first];
                        eventToSend = &sendIt->second;
                    }
                }
            }            
            
        case SEND_ALL:
        default:
            break;
    }
    
    if ( eventToSend != NULL && !closest->bDidStartGesture){
        if ( eventToSend->type == startGesture ){
            cout << "start gesture executed "<<endl;
            closest->bDidStartGesture = true;
        }
        eventToSend = NULL;
    }
    
    if ( eventToSend != NULL ){
        switch ( eventToSend->type ){
            case STATIONARY:
                ofNotifyEvent(onHeldEvent, *eventToSend );
                break;
            case SWIPE_LEFT:
                if ( closest->gestureHappened ){
                    lastGestureSent = ofGetElapsedTimeMillis();
                    ofNotifyEvent(onSwipeLeftEvent, *eventToSend );
                } else {
                    closest->gestureHappened = true;
                }
                break;
            case SWIPE_RIGHT:
                if ( closest->gestureHappened ){
                    lastGestureSent = ofGetElapsedTimeMillis();
                    ofNotifyEvent(onSwipeRightEvent, *eventToSend );
                } else {
                    closest->gestureHappened = true;
                }
                break;
            case SWIPE_UP:
                if ( closest->gestureHappened ){
                    lastGestureSent = ofGetElapsedTimeMillis();
                    ofNotifyEvent(onSwipeUpEvent, *eventToSend );
                } else {
                    closest->gestureHappened = true;
                }
                break;
            case SWIPE_DOWN:
                if ( closest->gestureHappened ){
                    lastGestureSent = ofGetElapsedTimeMillis();
                    ofNotifyEvent(onSwipeDownEvent, *eventToSend );
                } else {
                    closest->gestureHappened = true;
                }
                break;
            default:
                break;
        }
    }
    
    // should have already been sent
    toSend.clear();
    
    // update + erase old dead hands (gross)
    map<int, Hand>::iterator it = hands.begin();
    for (it; it != hands.end(); ++it){
        it->second.idle();
        if ( it->second.notUpdatedIn > 10 ){
            hands.erase(it);
        }
    }
    
}

//--------------------------------------------------------------
void GestureFactory::draw(){
    map<int, Hand>::iterator it = hands.begin();
    for (it; it != hands.end(); ++it){
        if ( it->second.age > 1000 ){
            it->second.draw();
        }
    }
}
