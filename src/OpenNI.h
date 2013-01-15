//
//  OpenNI.h
//  openTSPS
//
//  Created by Brett Renfer on 9/4/12.
//
//

#pragma once

#include "ofxOpenNI.h"
#include "ofxTSPS/source/Source.h"

namespace ofxTSPS {
    class OpenNI : public Source, public ofxOpenNI {
    public:
        
        OpenNI() : Source(), ofxOpenNI()
        {
            // type defaults to CAMERA_CUSTOM
            bCanTrackHaar = false;
            invertedPixels.allocate( 320,240, OF_IMAGE_GRAYSCALE);
        }
        
        // core
        bool available(){
            return (getNumDevices() >= 1);
        }
        
        void update(){
            ofxOpenNI::update();
            if ( isDepthOn() ){
                //this is pretty annoying, but ofxOpenNI colors are backwards
                ofPixelsRef pix = getDepthPixels();
                int channels    = pix.getNumChannels();
                int dims        = invertedPixels.getWidth() * invertedPixels.getHeight();
                for ( int i=0; i<dims; i++){
                    invertedPixels[i] = pix[2+i * channels] == 0 ? 0 : abs(255-pix[2+i * channels]);
                }
            }
        }
        
        bool doProcessFrame(){
            return isNewFrame();
        }
        
        // required bc ofxOpenNI doesn't declare it
        unsigned char*  getPixels(){
            return invertedPixels.getPixels();
        }
        
        ofPixelsRef getPixelsRef(){
            return invertedPixels;
        }
        
        bool openSource( int width, int height, string etc="" ){
            bool bSetup = isContextReady();
            if(!bSetup) bIsOpen = setup(true, width, height);
            if ( bIsOpen ){
                if ( invertedPixels.getWidth() != width || invertedPixels.getHeight() != height ){
                    invertedPixels.clear();
                    invertedPixels.allocate( width, height, OF_IMAGE_GRAYSCALE);
                }
                setDepthColoring(COLORING_BLUES);
                addDepthGenerator();
                start();                
            }
            return bIsOpen;
        }
        
        void closeSource(){
            stop();
        }
    private:
        ofPixels invertedPixels;
    };
}

