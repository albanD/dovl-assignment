#include "imageClass.h"

image::image() {
    this->data = imageData();
}

image::image(vector<vector<int>> &input_array) {
    this->data = imageData();

    this->width = input_array[0].size();
    this->height = input_array.size();

    for(int y=0; y<this->height; ++y)
    {
        imageRow tempRow = imageRow();
        for(int x=0; x<this->width; ++x)
        {
            tempRow.push_back((double)255*input_array[y][x]/NBR_CLASSES);
        }
        this->data.push_back(tempRow);
    }
}
