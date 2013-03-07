//
//  NiteSource.h
//  openTSPS
//
//  Created by BRenfer on 2/28/13.
//
//

#ifndef openTSPS_NiteSource_h
#define openTSPS_NiteSource_h

#include "ofxTSPS/source/Source.h"
#include "ofxNiteHandTracker.h"

namespace ofxTSPS {
    class Nite : public Source {
    public:
        
        Nite() : Source()
        {
            // type defaults to CAMERA_CUSTOM
            bCanTrackHaar = false;
            tiltedPixels.allocate( 320,240, OF_IMAGE_GRAYSCALE);
            tiltAmount = 0;
        }
        
        /** Tilt amount (-1 to 1)  */
        float tiltAmount;
        
        // core
        bool available(){
            return (niteSource.enumerateDevices().size() >= 1);
        }
        
        void update(){
            if (!bIsOpen) return;
            niteSource.update();
            
            int dims        = niteSource.getWidth() * niteSource.getHeight();
            float shift     = 0.0f;
            int index       = 0;
            
            unsigned char * pix = niteSource.getDepthPixels();
            
            for (int y = 0; y < niteSource.getHeight(); y++){
                for (int x = 0; x < niteSource.getWidth(); x++){
                    
                    if ( tiltAmount > 0 ){
                        float div = (float) y/niteSource.getHeight();
                        shift = (div * tiltAmount);
                    } else if ( tiltAmount < 0) {
                        float div = ((float) (niteSource.getHeight()-y)/niteSource.getHeight() );
                        shift = div * (-tiltAmount);
                    }
                    
                    tiltedPixels[x + y * niteSource.getWidth()] = pix[x + y * niteSource.getWidth()] *  (1.0-shift);
                }
            }
        }
        
        void draw(){
            niteSource.draw(0,0);
        }
        
        bool doProcessFrame(){
            return niteSource.isFrameNew();
        }
        
        unsigned char*  getPixels(){
            return tiltedPixels.getPixels();
        }
        
        ofPixelsRef getPixelsRef(){
            return tiltedPixels;
        }
        
        bool openSource( int width, int height, string etc="" ){
            // sorry, flexibility
            width = 320;
            height = 240;
            niteSource.setUseTexture( false );
            
            string uri = "";
            
            // get device ID
            if ( niteSource.enumerateDevices().size() > sourceIndex ){
                uri = niteSource.enumerateDevices()[sourceIndex].getUri();
            }
            
            openni::Status rc = niteSource.setup( uri );
            cout << (rc == openni::STATUS_OK) << endl;
            bIsOpen = true;//rc == openni::STATUS_OK;
            if ( bIsOpen ){
                if ( tiltedPixels.getWidth() != width || tiltedPixels.getHeight() != height ){
                    tiltedPixels.clear();
                    tiltedPixels.allocate( width, height, OF_IMAGE_GRAYSCALE);
                }
                niteSource.start();
            }
            return bIsOpen;
        }
        
        ofxNiteHandTracker & getTracker(){
            return niteSource;
        }
        
        void closeSource(){
            niteSource.stop();
        }
    private:
        ofxNiteHandTracker  niteSource;
        ofPixels            tiltedPixels;
    };
}

#endif
