#include "ofApp.h"

#include "ofxPlotterTools.h"

ofParameter<int> e1 = 7, e2 = 3, e3 = 5, e4 = 1;
ofParameter<bool> clamp;

//for each y from top to bottom
//for each x from left to right
//oldpixel  := pixel[x][y]
//newpixel  := find_closest_palette_color(oldpixel)
//pixel[x][y]  := newpixel
//quant_error  := oldpixel - newpixel
//pixel[x+1][y  ] := pixel[x+1][y  ] + quant_error * 7/16
//pixel[x-1][y+1] := pixel[x-1][y+1] + quant_error * 3/16
//pixel[x  ][y+1] := pixel[x  ][y+1] + quant_error * 5/16
//pixel[x+1][y+1] := pixel[x+1][y+1] + quant_error * 1/16

void dither(const ofImage &img, ofImage &result) {
    
    if (img.type != OF_IMAGE_GRAYSCALE) {
        ofLogError() << "image type needs to grayscale";
    }
    

    
    int w = img.getWidth(), h = img.getHeight();
    unsigned char *pix = new unsigned char[w * h];
    
    copy(img.getPixels().getData(), img.getPixels().getData() + w * h, pix);
    
    unsigned char *rpix = result.getPixels().getData();

    unsigned char op, np;
    for (int y = 0; y < h-1; y++) {
        for (int x = 0; x < w-1; x++) {
            op = pix[y * w + x];
            np = op > 128 ? 255 : 0;
            rpix[y * w + x] = np;
//            if (np == 0) points.push_back(ofVec2f(x, y));
            char error = op - np;

            if (clamp) {
                pix[y * w + x + 1] = MIN(MAX(pix[y * w + x + 1] + error * e1 / 16.0, 0), 255);
                pix[(y + 1) * w + x - 1] = MIN(MAX(pix[(y + 1) * w + x - 1] + error * e2 / 16.0, 0), 255);
                pix[(y + 1) * w + x] = MIN(MAX(pix[(y + 1) * w + x] + error * e3 / 16.0, 0), 255);
                pix[(y + 1) * w + x + 1] = MIN(MAX(pix[(y + 1) * w + x + 1] + error * e4 / 16.0, 0), 255);

            }
            else {
                pix[y * w + x + 1] = pix[y * w + x + 1] + error * e1 / 16.0;
                pix[(y + 1) * w + x - 1] = pix[(y + 1) * w + x - 1] + error * e2 / 16.0;
                pix[(y + 1) * w + x] = pix[(y + 1) * w + x] + error * e3 / 16.0;
                pix[(y + 1) * w + x + 1] = pix[(y + 1) * w + x + 1] + error * e4 / 16.0;
            }
        }
    }
    
    result.setFromPixels(rpix, w, h, OF_IMAGE_GRAYSCALE);
    delete[] pix;
}




void conwaysGameOfLife(ofImage &input, ofImage &output) {
    
    unsigned char *inputPixels = input.getPixels().getData();
    unsigned char *outputPixels = output.getPixels().getData();
    
    int w = input.getWidth(), h = input.getHeight();
    
    int t = 127, index;
    int numAlive;
    const unsigned char dead = 0, alive = 255;
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            index = y * w + x;
            
            numAlive = 0;
            if (x > 0 && y > 0) numAlive+= inputPixels[(y-1) * w + x - 1] > t;
            if (x > 0) numAlive+= inputPixels[y * w + x - 1] > t;
            if (x > 0 && y < h-1) numAlive+= inputPixels[(y+1) * w + x - 1] > t;
            
            if (x < w-1) numAlive+= inputPixels[y * w + x + 1] > t;
            if (x < w-1 && y > 0) numAlive+= inputPixels[(y-1) * w + x + 1] > t;
            if (x < w-1 && y < h-1) numAlive+= inputPixels[(y+1) * w + x + 1] > t;

            if (y > 0) numAlive+= inputPixels[(y-1) * w + x] > t;
            if (y < h-1) numAlive+= inputPixels[(y+1) * w + x] > t;
            
            outputPixels[index] = dead;
            if (inputPixels[index] > t) { //alive
                if (numAlive < e1) outputPixels[index] = dead;
                else if (numAlive <= e2) outputPixels[index] = alive;
                else if (numAlive > e2) outputPixels[index] = dead;
            }
            else {
                if (numAlive == e4) outputPixels[index] = alive;
            }
                
            
        }
    }
    
    output.setFromPixels(outputPixels, w, h, OF_IMAGE_GRAYSCALE);
    output.update();
    
}

void ofApp::setup() {

//    img.load("/Users/whg/Desktop/-/cat400.jpg");
    img.load("/Users/whg/Desktop/-/zand.jpg");
//    img.load("/Users/whg/Desktop/gray.png");
    img.setImageType(OF_IMAGE_GRAYSCALE);
    

//
    panel.setup("gui");
    panel.add(e1.set("e1", 7, 0, 16));
    panel.add(e2.set("e2", 3, 0, 16));
    panel.add(e3.set("e3", 5, 0, 16));
    panel.add(e4.set("e4", 1, 0, 16));
    panel.add(clamp.set("clamp", true));
  
    e1.addListener(this, &ofApp::refresh);
    e2.addListener(this, &ofApp::refresh);
    e3.addListener(this, &ofApp::refresh);
    e4.addListener(this, &ofApp::refresh);
    
    clamp.addListener(this, &ofApp::refreshb);
//    keyPressed(' ');
    
    ofSetFrameRate(25);
    
//    grabber.setup(640, 480);
    
//    img.allocate(grabber.getWidth(), grabber.getHeight(), OF_IMAGE_COLOR);
    result.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_GRAYSCALE);
    
//    result = img;
//    conwaysGameOfLife(img, result);

    dither(img, result);
    

    routesFromImage(result, lines);
    
    hideImage = false;
}


void ofApp::update() {

//    grabber.update();
//    if (grabber.isFrameNew()) {
//        img.setFromPixels(grabber.getPixels());
//        img.setImageType(OF_IMAGE_GRAYSCALE);
//    }
}


void ofApp::draw() {

    ofBackground(255);
    ofSetColor(255);
    
    ofPushMatrix();
    ofTranslate(50, 50);
    
    //dither(img, result);

//    ofScale(2, 2);
    if (!hideImage) {
        result.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        float d = 2;
        result.draw(0, 0, result.getWidth()*d, result.getHeight()*d);
    }
    
//    for (ofVec2f &p : points) {
//        ofDrawRectangle(p, 1, 1);
//    }

    if (ofGetKeyPressed()) {
        ofTranslate(1, 1);
        ofSetColor(255, 0, 150);
        for (auto &line : lines) {
            if (line.size() == 1) {
//                ofVec3f p = line.getVertices()[0];
//                ofDrawRectangle(p.x, p.y, 1, 1);
            }
            else {
                ofSetLineWidth(0.5);
                line.draw();
            }
        }
    }
    
    ofPopMatrix();
    
    panel.draw();
    
//    ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
}

void ofApp::refresh(int &b) {

    dither(img, result);
}

void ofApp::keyPressed(int key) {

//    ofImage input = result;
//    conwaysGameOfLife(input, result);
    
    if (key == 'r') {
        result.load("/Users/whg/Desktop/m.png");
        result.setImageType(OF_IMAGE_GRAYSCALE);
    }
    
    if (key == 'g') {
        cout << "starting... ";
        routesFromImage(result, lines);
        cout << "done" << endl;
    }
    if (key == 'h') hideImage = !hideImage;
}


void ofApp::keyReleased(int key) {

}


void ofApp::mouseMoved(int x, int y) {

}


void ofApp::mouseDragged(int x, int y, int button) {

}


void ofApp::mousePressed(int x, int y, int button) {

}


void ofApp::mouseReleased(int x, int y, int button) {

}


void ofApp::windowResized(int w, int h) {

}


void ofApp::gotMessage(ofMessage msg) {

}


void ofApp::dragEvent(ofDragInfo dragInfo) { 

}