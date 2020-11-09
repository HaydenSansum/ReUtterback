#pragma once

#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void update_painters();
        void update_grabber();
        void build_sample_colors();

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
		
    // Painter variables
    vector <ofPoint> painters;
    vector <ofVec2f> directions;
    vector <ofVec2f> intial_directions;
    vector <int> white_count;
    vector <ofColor> sample_colors;
    int num_painters;
    float max_speed;
    float min_speed;
    float accel;
    float max_accel;
    int seed;
    float p_size;
    float brightness_scaler;
    int num_colors;
    
    // Body variables
    ofVideoGrabber grabber;
    ofImage cam_grey;
    ofImage bg_grey;
    ofImage diff_img;
    ofImage thresh_img;
    bool draw_cam;
    bool draw_full_cam;
    bool negative;
    
    bool b_record_bg;
    float threshold_val;
    
    // Screen variables
    ofFbo fbo;
    ofPixels p;
    
    int screen_w;
    int screen_h;
    
    // Save image
    ofImage screen_grab;
    
};
