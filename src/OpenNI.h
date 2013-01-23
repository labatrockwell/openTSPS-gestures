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
            tiltAmount = 0;
            setDepthColoring( COLORING_GREY );
        }
        
        /** Tilt amount (-1 to 1)  */
        float tiltAmount;
        
        // core
        bool available(){
            return (getNumDevices() >= 1);
        }
        
        void update(){
            ofxOpenNI::update();
            if ( isDepthOn() ){
                const XnDepthPixel* depth = getDepthMetaData().Data();
                
                // build inverted pixels + do shift
                int dims        = invertedPixels.getWidth() * invertedPixels.getHeight();
                float shift     = 0.0f;
                int index       = 0;
                
                for (XnUInt16 y = getDepthMetaData().YOffset(); y < getDepthMetaData().YRes() + getDepthMetaData().YOffset(); y++){
                    for (XnUInt16 x = 0; x < getDepthMetaData().XRes(); x++, depth++){
                        
                        if ( tiltAmount > 0 ){
                            float div = (float) y/getHeight();
                            shift = (div * tiltAmount);
                        } else if ( tiltAmount < 0) {
                            float div = ((float) (getHeight()-y)/getHeight() );
                            shift = div * (-tiltAmount);
                        }
                        // invert
                        XnUInt8 a = (XnUInt8)(((*depth) / (getMaxDepth() / -255.00)));
                        
                        // shift
                        a *= (1.0-shift);
                        invertedPixels[index] = a;
                        index++;
                    }
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
                addDepthGenerator();
                setUseDepthRawPixels(true);
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

