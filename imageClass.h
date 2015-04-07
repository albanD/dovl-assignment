#ifndef IMAGECLASS_H
#define IMAGECLASS_H

#include "problemStatic.h"

#include <vector>

using namespace std;

typedef vector<float> imageRow;
typedef vector<imageRow> imageData;

class image {

public:
    imageData data;
    int width;
    int height;

    image();
    image(vector<vector<int>> &input_array);
};

#endif