#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "CustomDelegate.h"

class tspsApp : public ofBaseApp {
    
    public:
    
        /**
         * @param {int}         numCameras  How many cameras to connect to
         * @param {std::string} host        (optional) Host of websocket server to send to
         * @param {std::string} port        (optional) Port of websocket server to send to
         */
        tspsApp( int numCameras=1, int startIndex=0, string settingsFile="", string host="", int port=0, string channel="", string deviceUri="" );
        
		void setup();
		void update();
        void draw();
        void exit();
    
        ofMutex delegateMutex;
    
        void mouseReleased(int x, int y, int button);
    
        int     startIndex;
        string  wsHost, wsChannel;
        int     wsPort;
    
        int currentDelegate; // which one you are drawing
        vector<CustomDelegate *> delegates;
        
        // buttons for switching between cameras + adding more cameras
        map<string, guiTypeButton *> buttons;
        void onButtonPressed( string & button );
};

#endif