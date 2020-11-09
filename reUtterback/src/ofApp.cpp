#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    // User choice constants
    num_painters = 800;
    p_size = 4;
    seed = 13;
    threshold_val = 30; // Tweak based on ambient lighting and background
    draw_full_cam = true; // Set Screen Width to be 1280
    negative = false; // If false push away from body, if true trap inside
    num_colors = 18; // Must match color list in function build sample colors
    brightness_scaler = 150;
    
    // Physics constants
    if (negative) {
        max_speed = 8.0;
        min_speed = 3.0;
        max_accel = 8.0;
        accel = 1.0;
    } else {
        max_speed = 8.0;
        min_speed = 3.0;
        max_accel = 40.0;
        accel = 4.0;
    }
    
    // Invariant Constants
    if (draw_full_cam) {
        screen_w = 1024;
        screen_h = ofGetHeight();
    } else {
        screen_w = ofGetWidth();
        screen_h = ofGetHeight();
    }
    
    fbo.allocate(screen_w, screen_h, GL_RGBA);
    
    // Initialize painters, directions and white counts
    for (int i = 0; i < num_painters; i++) {
        
        float rand_x = ofRandom(ofGetWidth());
        float rand_y = ofRandom(ofGetHeight());
        
        float dir_x = ofRandom(-max_speed, max_speed);
        float dir_y = ofRandom(-max_speed, max_speed);
        
        // CAP MIN SPEEDS
        if (dir_x < 0 && dir_x > -min_speed) {
            dir_x = -min_speed;
        }
        if (dir_y < 0 && dir_y > -min_speed) {
            dir_y = -min_speed;
        }
        if (dir_x > 0 && dir_x < min_speed) {
            dir_x = min_speed;
        }
        if (dir_y > 0 && dir_y < min_speed) {
            dir_y = min_speed;
        }
        
        ofPoint cur_point = ofPoint(rand_x,rand_y,0);
        ofVec2f cur_dir = ofVec2f(dir_x, dir_y);
        
        painters.push_back(cur_point);
        directions.push_back(cur_dir);
        intial_directions.push_back(cur_dir);
        white_count.push_back(0);
    }
    
    // Set initial background to black
    ofBackground(0);
    
    // Build colors
    build_sample_colors();
    
    // Set up grabber and required images
//    grabber.listDevices();
    grabber.setDeviceID(1);
    cout << screen_w << endl;
    cout << screen_h << endl;
    grabber.initGrabber(screen_w, screen_h);
    
    cam_grey.allocate(grabber.getWidth(), grabber.getHeight(), OF_IMAGE_GRAYSCALE);
    bg_grey.allocate(grabber.getWidth(), grabber.getHeight(), OF_IMAGE_GRAYSCALE);
    diff_img.allocate(grabber.getWidth(), grabber.getHeight(), OF_IMAGE_GRAYSCALE);
    thresh_img.allocate(grabber.getWidth(), grabber.getHeight(), OF_IMAGE_GRAYSCALE);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Update painters
    update_painters();
    
    // Update grabber
    update_grabber();
    
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
    ofSeedRandom(seed);
    
    ofPushMatrix();
    
    if(draw_full_cam) {
        // Draw all cameras
        ofPushMatrix();
        ofSetColor(255);
        ofScale(0.25, 0.25);
        grabber.draw(0, 0);
        cam_grey.draw(0, grabber.getHeight());
        diff_img.draw(0, grabber.getHeight() * 2);
        thresh_img.draw(0, grabber.getHeight() * 3);
        ofPopMatrix();
        
        ofTranslate(256,0);
    }
    // Create trails through repeated background drawing
    ofSetColor(0,0,0,5);
    ofDrawRectangle(0, 0, screen_w, screen_h);
    
    // Draw each painter
    for (int i = 0; i < num_painters; i++) {
        
        // Get random color
        int rand_color = (int)ofRandom(num_colors - 1);
        ofSetColor(sample_colors[rand_color]);
        
        // Draw painter
        ofDrawCircle(painters[i], p_size);
        
    }
    ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::update_grabber(){
 
    grabber.update();
    if (grabber.isFrameNew()) {
        convertColor(grabber, cam_grey, CV_RGB2GRAY);
        cam_grey.update();

        if (b_record_bg) {
            bg_grey.setFromPixels(cam_grey.getPixels());
            b_record_bg = false;
        }
        
        absdiff(cam_grey, bg_grey, diff_img);
        diff_img.update();
        
        thresh_img.setFromPixels(diff_img.getPixels());
        threshold(thresh_img, threshold_val);
        thresh_img.update();
        
    }
           
}

//--------------------------------------------------------------
void ofApp::update_painters(){
    
    for (int i = 0; i < num_painters; i++) {
     
        ofPoint cur_point = painters[i];
        cur_point.x = cur_point.x + directions[i][0];
        cur_point.y = cur_point.y + directions[i][1];
        
        // Check if current point is off the screen
        if (cur_point.x > screen_w) {
            //cur_point.x = cur_point.x - screen_w;
            cur_point.x = 0;
        }
        if (cur_point.x < 0) {
            //cur_point.x = cur_point.x + screen_w;
            cur_point.x = screen_w;
        }
        if (cur_point.y > screen_h) {
            //cur_point.y = cur_point.y - screen_h;
            cur_point.y = 0;
        }
        if (cur_point.y < 0) {
            //cur_point.y = cur_point.y + screen_h;
            cur_point.y = screen_h;
        }
        
        // Check if the current point hits a white pixel -
        // If its been on a white pixel 3 times in a row accelerate off
        int brightness = thresh_img.getColor(cur_point.x, cur_point.y).getBrightness();
        if (negative) {
            if (brightness < 120) {
                if (white_count[i] > 2) {
                    directions[i][0] = directions[i][0] * accel;
                    directions[i][1] = directions[i][1] * accel;
                    if (directions[i][0] > max_accel) {
                        directions[i][0] = max_accel;
                    }
                    if (directions[i][1] > max_accel) {
                        directions[i][1] = max_accel;
                    }
                } else {
                    directions[i][0] = directions[i][0] * -1;
                    directions[i][1] = directions[i][1] * -1;
                }
                white_count[i] = white_count[i] + 1;
            } else {
                if (white_count[i] > 2) {
                    directions[i][0] = intial_directions[i][0];
                    directions[i][1] = intial_directions[i][1];
                }
                white_count[i] = 0;
            }
        } else {
            if (brightness > 120) {
                if (white_count[i] > 2) {
                    directions[i][0] = directions[i][0] * accel;
                    directions[i][1] = directions[i][1] * accel;
                    if (directions[i][0] > max_accel) {
                        directions[i][0] = max_accel;
                    }
                    if (directions[i][1] > max_accel) {
                        directions[i][1] = max_accel;
                    }
                } else {
                    directions[i][0] = directions[i][0] * -1;
                    directions[i][1] = directions[i][1] * -1;
                }
                white_count[i] = white_count[i] + 1;
            } else {
                if (white_count[i] > 2) {
                    directions[i][0] = intial_directions[i][0];
                    directions[i][1] = intial_directions[i][1];
                }
                white_count[i] = 0;
            }
        }
        
        // Check for min speed and accelerate up if needs be
        if (directions[i][0] > -min_speed && directions[i][0] < min_speed) {
            directions[i][0] = directions[i][0] * accel;
        }
        if (directions[i][1] > -min_speed && directions[i][1] < min_speed) {
            directions[i][1] = directions[i][1] * accel;
        }
        
        painters[i] = cur_point;
        
    }
}

//--------------------------------------------------------------
void ofApp::build_sample_colors(){
 
    // List of available colors
    int reds[18] = {203, 217, 115, 218, 228, 135, 217, 231, 116, 156, 88, 1, 82, 150, 187, 28, 196, 220};
    int greens[18] = {58, 118, 167, 150, 194, 35, 150, 215, 29, 45, 170, 13, 147, 179, 189, 76, 116, 153};
    int blues[18] = {43, 89, 202, 120, 76, 46, 127, 157, 48, 91, 161, 58, 49, 65, 70, 146, 165, 63};
 
    // Build colors
    for (int i = 0; i < 18; i++) {
        ofColor cur_color = ofColor(reds[i], greens[i], blues[i]);
        float h;
        float s;
        float b;
        cur_color.getHsb(h, s, b);
        cur_color.setHsb(h, s, fmin(b + brightness_scaler, 255.0));
        sample_colors.push_back(cur_color);
    }
        
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
         screen_grab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
         screen_grab.save(ofGetTimestampString() + "screenshot.png");
    }
    
    if (key == 'b') {
        b_record_bg = true;
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
