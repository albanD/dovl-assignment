#ifndef IMAGECLASS_H
#define IMAGECLASS_H


#include <vector>

using namespace std;

typedef vector<float> imageRow;
typedef vector<imageRow> imageData;

class image {

public:
    imageData data;
    int width;
    int height;
};

#endif