//
//  SwipeDetectorONI.cpp
//  openTSPS
//
//  Created by rockwell on 10/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "SwipeDetectorONI.h"

SwipeDetectorONI::SwipeDetectorONI(){
    detector = new XnVSwipeDetectorONI( false );
    g_pSessionManager = NULL;
    m_pInnerFlowRouter = NULL;
}

void SwipeDetectorONI::setup( ofxOpenNI & _openNI ){
    openNI = &_openNI;
    detector->RegisterSwipeUp(this, &callbackOnSwipeUp);
    detector->RegisterSwipeDown(this, &callbackOnSwipeDown);
    detector->RegisterSwipeLeft(this, &callbackOnSwipeLeft);
    detector->RegisterSwipeRight(this, &callbackOnSwipeRight);    
    g_pSessionManager = new XnVSessionManager;
    m_pInnerFlowRouter = new XnVFlowRouter;
    m_Broadcaster.AddListener(m_pInnerFlowRouter);
    g_pSessionManager->Initialize(&openNI->getContext(), "RaiseHand", "RaiseHand");
    g_pSessionManager->AddListener(m_pInnerFlowRouter);    
    m_pInnerFlowRouter->SetActive(detector);
    openNI->setMaxNumHands(10);
}

void SwipeDetectorONI::update(){
    if ( g_pSessionManager != NULL && openNI->getNumDevices() > 0 ){
        g_pSessionManager->Update(&openNI->getContext());
    }
}

void XN_CALLBACK_TYPE SwipeDetectorONI::callbackOnSwipeUp(XnFloat fVelocity, XnFloat fAngle, void* pUserCxt){
    cout << "swipe up" <<endl;
    SwipeDetectorONI* SwipeDetectorONI = static_cast<SwipeDetectorONI*>(pUserCxt);
    ofxSwipeEvent event;
    event.velocity.set( 0, fVelocity );
    event.angle = fAngle;
    ofNotifyEvent(SwipeDetectorONI->onSwipeUpEvent, event );
}

void XN_CALLBACK_TYPE SwipeDetectorONI::callbackOnSwipeDown(XnFloat fVelocity, XnFloat fAngle, void* pUserCxt){
    cout << "swipe down" <<endl;
    SwipeDetectorONI* SwipeDetectorONI = static_cast<SwipeDetectorONI*>(pUserCxt);
    ofxSwipeEvent event;
    event.velocity.set( 0, fVelocity );
    event.angle = fAngle;
    ofNotifyEvent(SwipeDetectorONI->onSwipeDownEvent, event);
    
}

void XN_CALLBACK_TYPE SwipeDetectorONI::callbackOnSwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* pUserCxt){
    cout << "swipe left" <<endl;
    SwipeDetectorONI* SwipeDetectorONI = static_cast<SwipeDetectorONI*>(pUserCxt);
    ofxSwipeEvent event;
    event.velocity.set( fVelocity, 0 );
    event.angle = fAngle;
    ofNotifyEvent(SwipeDetectorONI->onSwipeLeftEvent, event);
}

void XN_CALLBACK_TYPE SwipeDetectorONI::callbackOnSwipeRight(XnFloat fVelocity, XnFloat fAngle, void* pUserCxt){
    cout << "swipe right" <<endl;
    SwipeDetectorONI* SwipeDetectorONI = static_cast<SwipeDetectorONI*>(pUserCxt);
    ofxSwipeEvent event;
    event.velocity.set( fVelocity, 0 );
    event.angle = fAngle;
    ofNotifyEvent(SwipeDetectorONI->onSwipeRightEvent, event);
    
}