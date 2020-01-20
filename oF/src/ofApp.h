/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxNetwork.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	enum{
		WINDOW_WIDTH = 300,
		WINDOW_HEIGHT = 300,
	};
	enum{
		UDP_BUF_SIZE = 100000,
	};

	ofxUDPManager udp_Send;
	ofxUDPManager udp_Receive;
	
	void setup_udp();
	
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
