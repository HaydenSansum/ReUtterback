#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    screen_w = ofGetWidth();
    screen_h = ofGetHeight();
    
    fbo.allocate(screen_w, screen_h, GL_RGBA);
    
    num_painters = 300;
    
    for (int i = 0; i < num_painters; i++) {
        
        float rand_x = ofRandom(ofGetWidth());
        float rand_y = ofRandom(ofGetHeight());
        
        float dir_x = ofRandom(-2.0, 2.0);
        float dir_y = ofRandom(-2.0, 2.0);
        
        ofPoint cur_point = ofPoint(rand_x,rand_y,0);
        ofVec2f cur_dir = ofVec2f(dir_x, dir_y);
        
        painters.push_back(cur_point);
        directions.push_back(cur_dir);
    }
    
    
    ofBackground(0);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Update pixels
    fbo.readToPixels(p);
    
    for (int i = 0; i < num_painters; i++) {
     
        ofPoint cur_point = painters[i];
        cur_point.x = cur_point.x + directions[i][0];
        cur_point.y = cur_point.y + directions[i][1];
        
        // Check if current point is off the screen
        if (cur_point.x > screen_w) {
            cur_point.x = cur_point.x - screen_w;
        }
        if (cur_point.x < 0) {
            cur_point.x = cur_point.x + screen_w;
        }
        if (cur_point.y > screen_h) {
            cur_point.y = cur_point.y - screen_h;
        }
        if (cur_point.y < 0) {
            cur_point.y = cur_point.y + screen_h;
        }
        
        // Check if the current point hits a white pixel
        ofPoint heading_point;
        
        int brightness = p.getColor(cur_point.x, cur_point.y).getBrightness();
        if (brightness > 120) {
            directions[i][0] = directions[i][0] * -1;
            directions[i][1] = directions[i][1] * -1;
        }
        
        painters[i] = cur_point;
        
    }
    
    // Set backgrounds
    fbo.begin();
    ofSetColor(0);
    ofDrawRectangle(0, 0, screen_w, screen_h);
    ofSetColor(255);
    ofDrawRectangle(300, 300, 200, 200);
    fbo.end();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetBackgroundAuto(false);
    ofSeedRandom(13);
    
    ofSetColor(0,0,0,5);
    ofDrawRectangle(0, 0, screen_w, screen_h);
    
    for (int i = 0; i < num_painters; i++) {
     
        ofColor color;
        color.setHsb(ofRandom(255), 255, 255);
        ofSetColor(color);
        
        
        ofDrawCircle(painters[i], 8);
        
    }
    
    ofSetColor(0);
    ofDrawRectangle(300, 300, 200, 200);


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
   if (key == ' ') {
        screen_grab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        screen_grab.save(ofGetTimestampString() + "screenshot.png");
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
