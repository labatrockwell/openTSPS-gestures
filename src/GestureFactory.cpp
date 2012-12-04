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
    mode                = SEND_CLOSEST;
}

//--------------------------------------------------------------
void GestureFactory::setup( ofxOpenNI * ctx ){
    context = ctx;
    bSetup = true;
}

//--------------------------------------------------------------
void GestureFactory::updateBlob( int id, int x, int y, int z ){    
    // loop through current hands
    // do we have this one?
        if ( hands.count( id ) == 0 ){
            hands.insert(make_pair(id, Hand( x, y, z )) );
        }
        
        hands[ id ].update( x, y, z );
        
        // should be some sort of gesture object to check against
        
        // stationary
        if ( hands[ id ].averageVelocity.length() < stationaryThreshold ){
            if ( lastEvents[id].type == STATIONARY ){
                lastEvents[id].duration = ofGetElapsedTimeMillis() - lastEvents[id].timeStarted;
            } else {
                lastEvents[id].duration      = 0;
                lastEvents[id].timeStarted   = ofGetElapsedTimeMillis();
            }
            if ( lastEvents[id].duration > 1000 ){
                toSend.insert(make_pair( id, lastEvents[id]) );
                if ( mode == SEND_ALL) ofNotifyEvent(onHeldEvent, lastEvents[id] );
            }
            lastEvents[id].type = STATIONARY;
            
            // left / right
        } else if ( abs(hands[ id ].averageVelocity.x) > abs(hands[ id ].averageVelocity.y) ){
            if ( abs(hands[ id ].averageVelocity.x) > horizontalThreshold ){
                lastEvents[id].angle = hands[ id ].averageVelocity.angle(ofVec3f());
                lastEvents[id].velocity.set( hands[ id ].averageVelocity.x, 0 );
                
                switch (ofSign(hands[ id ].averageVelocity.x)) {
                    case 1:
                        lastEvents[id].type = SWIPE_LEFT;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( mode == SEND_ALL) ofNotifyEvent(onSwipeLeftEvent, lastEvents[id] );
                        break;
                    case -1:
                        lastEvents[id].type = SWIPE_RIGHT;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( mode == SEND_ALL) ofNotifyEvent(onSwipeRightEvent, lastEvents[id] );
                        break;
                }
                hands[id].clearHistory();
            } else {
                lastEvents[id].duration  = 0;
                lastEvents[id].type      = CUSTOM;
            }
            // up / down
        } else if ( abs(hands[ id ].averageVelocity.y) > abs(hands[ id ].averageVelocity.x) ){
            if ( abs(hands[ id ].averageVelocity.y) > verticalThreshold ){
                lastEvents[id].angle = hands[ id ].averageVelocity.angle(ofVec3f());
                lastEvents[id].velocity.set( 0, hands[ id ].averageVelocity.y );
                
                switch (ofSign(hands[ id ].averageVelocity.y)) {
                    case -1:
                        lastEvents[id].type = SWIPE_UP;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( mode == SEND_ALL)
                            ofNotifyEvent(onSwipeUpEvent, lastEvents[id] );
                        break;
                    case 1:
                        lastEvents[id].type = SWIPE_DOWN;
                        toSend.insert(make_pair( id, lastEvents[id]) );
                        if ( mode == SEND_ALL)
                            ofNotifyEvent(onSwipeDownEvent, lastEvents[id] );
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
}

//--------------------------------------------------------------
void GestureFactory::update(){
    int closestId               = -1;
    Hand          * closest     = NULL;
    ofxSwipeEvent * eventToSend = NULL;
    map<int, ofxSwipeEvent>::iterator sendIt = toSend.begin();
    
    switch ( mode ) {
        case SEND_CLOSEST:
            for (sendIt; sendIt != toSend.end(); ++sendIt){
                if ( hands.count(sendIt->first) > 0 ){
                    if ( closest == NULL || hands[sendIt->first].z < closest->z ){
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
    
    if ( eventToSend != NULL ){
        switch ( eventToSend->type ){
            case STATIONARY:
                ofNotifyEvent(onHeldEvent, *eventToSend );
                break;
            case SWIPE_LEFT:
                ofNotifyEvent(onSwipeLeftEvent, *eventToSend );
                break;
            case SWIPE_RIGHT:
                ofNotifyEvent(onSwipeRightEvent, *eventToSend );
                break;
            case SWIPE_UP:
                ofNotifyEvent(onSwipeUpEvent, *eventToSend );
                break;
            case SWIPE_DOWN:
                ofNotifyEvent(onSwipeDownEvent, *eventToSend );
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
void GestureFactory::updateOpenNI(){
    if ( context == NULL || !bSetup ){
        return;
    }
    
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
            if ( lastEvents[ID].type == STATIONARY ){
                lastEvents[ID].duration = ofGetElapsedTimeMillis() - lastEvents[ID].timeStarted;
            } else {
                lastEvents[ID].duration      = 0;
                lastEvents[ID].timeStarted   = ofGetElapsedTimeMillis();
            }
            if ( lastEvents[ID].duration > 1000 ){
                ofNotifyEvent(onHeldEvent, lastEvents[ID] );
            }
            lastEvents[ID].type = STATIONARY;
            
        // left / right
        } else if ( abs(hands[ ID ].averageVelocity.x) > abs(hands[ ID ].averageVelocity.y) ){
            if ( abs(hands[ ID ].averageVelocity.x) > horizontalThreshold ){
                lastEvents[ID].angle = hands[ ID ].averageVelocity.angle(ofVec3f());
                lastEvents[ID].velocity.set( hands[ ID ].averageVelocity.x, 0 );
                
                switch (ofSign(hands[ ID ].averageVelocity.x)) {
                    case 1:
                        ofNotifyEvent(onSwipeLeftEvent, lastEvents[ID] );
                        lastEvents[ID].type = SWIPE_LEFT;
                        break;                        
                    case -1:
                        ofNotifyEvent(onSwipeRightEvent, lastEvents[ID] );
                        lastEvents[ID].type = SWIPE_RIGHT;
                        break;
                }
            } else {
                lastEvents[ID].duration  = 0;
                lastEvents[ID].type      = CUSTOM;
            }
        // up / down
        } else if ( abs(hands[ ID ].averageVelocity.y) > abs(hands[ ID ].averageVelocity.x) ){
            if ( abs(hands[ ID ].averageVelocity.y) > verticalThreshold ){
                lastEvents[ID].angle = hands[ ID ].averageVelocity.angle(ofVec3f());
                lastEvents[ID].velocity.set( 0, hands[ ID ].averageVelocity.y );
                                
                switch (ofSign(hands[ ID ].averageVelocity.y)) {
                    case 1:
                        ofNotifyEvent(onSwipeUpEvent, lastEvents[ID] );
                        lastEvents[ID].type = SWIPE_UP;
                        break;                        
                    case -1:
                        ofNotifyEvent(onSwipeDownEvent, lastEvents[ID] );
                        lastEvents[ID].type = SWIPE_DOWN;
                        break;
                }
            } else {
                lastEvents[ID].duration  = 0;
                lastEvents[ID].type      = CUSTOM;
            }
        } else {
            lastEvents[ID].duration  = 0;
            lastEvents[ID].type      = CUSTOM;
        }
    }
    
    map<int,Hand>::iterator eit = toErase.begin();
    for ( toErase; eit != toErase.end(); ++eit ){
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
        ofSetColor(255);
//        ofDrawBitmapString("Event: "+ofToString(lastEvent.type) +" : "+ ofToString(lastEvent.duration), 20, 20 );
    }
    
    
    ofPopStyle();
}
