#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofImage img, result;
    vector<ofVec2f> points;
    ofxPanel panel;
    
    ofVideoGrabber grabber;
    
    void refresh(int &b);
    void refreshb(bool &b) { int i = int(b); refresh(i); }
    
    vector<ofPolyline> lines;
    bool hideImage;
};

