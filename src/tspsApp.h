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
        tspsApp( int numCameras=1, string host="", int port=0 );
        
		void setup();
		void update();
		void draw();
    
        void mouseReleased(int x, int y, int button);
    
        string  wsHost;
        int     wsPort;
    
        int currentDelegate; // which one you are drawing
        vector<CustomDelegate *> delegates;
        
        // buttons for switching between cameras + adding more cameras
        map<string, guiTypeButton *> buttons;
        void onButtonPressed( string & button );
};

#endif