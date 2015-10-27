//
//  ofxPlotterTools.h
//  emptyExample
//
//  Created by Will Gallia on 27/10/2015.
//
//

#pragma once

#include "ofMain.h"

struct Location {
    int x, y;
    Location(): x(0), y(0) {}
    Location(int x, int y): x(x), y(y) {}
};

struct PixelPoint {
    Location location;
    deque<int> neighbours;
    
};

typedef map<pair<int, int>, int> pointMapType;
typedef map<int, PixelPoint> locationMap;

// from a grayscale image, return the black points
inline bool img2Points(const ofImage &img, pointMapType &points) {
    
    if (img.type != OF_IMAGE_GRAYSCALE) {
        ofLogError() << "img2Points needs a grayscale image";
        return false;
    }
    
    points.clear();
    const unsigned char *pix = img.getPixels().getData();
    int w = img.getWidth(), h = img.getHeight();
    int d = w * h;
    int threshold = 10;
    int j = 0;

    for (int i = 0; i < d; i++) {
        if (pix[i] < threshold) {
            points[pair<int, int>(i % w, i / w)] = j++; //Location(i % w, i / h);
        }
    }
    
    return true;
}



inline bool createLocationMap(const pointMapType &points, locationMap &pixelPoints) {
    
    pixelPoints.clear();
    
    int x, y, id;
    for (auto &loc : points) {
        id = loc.second;
        x = loc.first.first;
        y = loc.first.second;
        PixelPoint pp;
        pp.location = Location(x, y);
        
        if (points.count(pair<int, int>(x+1, y))) pp.neighbours.push_back(points.at(pair<int, int>(x+1, y)));
        if (points.count(pair<int, int>(x-1, y))) pp.neighbours.push_back(points.at(pair<int, int>(x-1, y)));
        if (points.count(pair<int, int>(x, y+1))) pp.neighbours.push_back(points.at(pair<int, int>(x, y+1)));
        if (points.count(pair<int, int>(x, y-1))) pp.neighbours.push_back(points.at(pair<int, int>(x, y-1)));
        
        pixelPoints[id] = pp;
    }
    
    return true;
}

inline bool createRoutesFromMap(const locationMap &data, vector<ofPolyline> &lines) {
    
    locationMap localMap(data), erasableLocalMap(data);
    
    lines.clear();
    

    while (erasableLocalMap.size() > 0) {
    
        int nextId = erasableLocalMap.begin()->first;
        
        ofPolyline line;
        PixelPoint *pp = &localMap[nextId];
        line.addVertex(pp->location.x, pp->location.y);
        
        erasableLocalMap.erase(nextId);
        
        int currentId;
        while (pp->neighbours.size() > 0) {
            currentId = nextId;
            nextId = pp->neighbours.front();
            pp->neighbours.pop_front();
            
            if (erasableLocalMap.count(nextId)) {
                pp = &localMap[nextId];
                line.addVertex(pp->location.x, pp->location.y);
                erasableLocalMap.erase(nextId);
            }

        }
//        line.simplify();
        lines.push_back(line);
//        cout << "added line of length " << line.size() << endl;
    }
    
    int m = 3;
    for (auto &line : lines) {
        for (auto &vertex : line.getVertices()) {
            vertex.x *= m;
            vertex.y *= m;
        }
    }
    
    cout << lines.size() << endl;
    
    return true;
}

inline bool routesFromImage(const ofImage &img, vector<ofPolyline> &lines) {
    pointMapType points;
    if(img2Points(img, points)) {
        locationMap pixelPoints;
        if (createLocationMap(points, pixelPoints)) {
            return createRoutesFromMap(pixelPoints, lines);
        }
    }
    
    return false;
}

inline void lines2hpgl(const vector<ofPolyline> &lines, string &output) {
    
    stringstream ss;
    
    for (auto &line : lines) {
        ofVec3f p = line.getVertices()[0];
        ss << "PU" << p.x << "," << p.y << ";";
        for (auto &vertex : line.getVertices()) {
            ss << "PD" << p.x << "," << p.y << ";";
        }
    }
    
    output = ss.str();
}

inline void lines2file(const vector<ofPolyline> &lines, const string &filename) {
    string hpgl;
    lines2hpgl(lines, hpgl);
    
    ofstream outfile;
    outfile.open(ofToDataPath("output.hpgl").c_str());
    outfile << hpgl;
    outfile.close();
}
