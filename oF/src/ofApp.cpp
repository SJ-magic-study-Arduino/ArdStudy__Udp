#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("udp Send");
	
	ofSetWindowShape(WINDOW_WIDTH, WINDOW_HEIGHT);
	/*
	ofSetVerticalSync(false);
	ofSetFrameRate(0);
	/*/
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
    //*/
	
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	setup_udp();
}

/******************************
******************************/
void ofApp::setup_udp(){
	/********************
	********************/
	{
		ofxUDPSettings settings;
		// settings.sendTo("127.0.0.1", 12345);
		settings.sendTo("10.0.0.10", 12345);
		settings.blocking = false;
		
		udp_Send.Setup(settings);
	}
	
	/********************
	********************/
	{
		ofxUDPSettings settings;
		settings.receiveOn(12346);
		settings.blocking = false;
		
		udp_Receive.Setup(settings);
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	char udpMessage[UDP_BUF_SIZE];
	udp_Receive.Receive(udpMessage, UDP_BUF_SIZE);
	string message=udpMessage;
	
	if(message!=""){
		string LastMessage = message;
		
		do{ // 巻き取り
			udp_Receive.Receive(udpMessage, UDP_BUF_SIZE);
			message = udpMessage;
			if(message != "") LastMessage = message;
			
		}while(message != "");
		
		vector<string> strMessages = ofSplitString(LastMessage, "[p]");
		
		if(strMessages.size() == 3){
			printf("%s : ", strMessages[0].c_str());
			printf("%s ", strMessages[1].c_str());
			printf("[%s th message]\n", strMessages[2].c_str());
			fflush(stdout);
		}
	}


}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	string message="";
	
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			enum {BUF_SIZE = 100,};
			char buf[BUF_SIZE];
			sprintf(buf, "%d", key - '0');
			
			message += "Button|";
			message += buf;
			
			udp_Send.Send(message.c_str(),message.length());
			
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
