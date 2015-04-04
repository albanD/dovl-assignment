#ifndef IMAGECLASS_H
#define IMAGECLASS_H


#include <vector>

using namespace std;

class image {

public:
    vector<vector<float>> data;
    int width;
    int height;
};

#endif