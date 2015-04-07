#include <png++/png.hpp>
/*
To install png++:
install the package "libpng-dev"
make and install the content of the "png++-0.2.5.tar.gz"
*/

#include <ioUtils.h>
#include "imageClass.h"


void getImage(image &myImage, string fileName) {

    png::image<png::rgb_pixel> image(fileName);

    myImage.width = image.get_width();
    myImage.height = image.get_height();

    for(int y=0; y<myImage.height; ++y)
    {
        imageRow tempRow = imageRow();
        for(int x=0; x<myImage.width; ++x)
        {
            float grayScaleValue = 0;
            grayScaleValue += 0.21*(float)image[y][x].red;
            grayScaleValue += 0.72*(float)image[y][x].green;
            grayScaleValue += 0.07* (float)image[y][x].blue;
            tempRow.push_back(grayScaleValue);
        }
        myImage.data.push_back(tempRow);
    }
}

void printImage(image &myImage, string fileName) {
    png::image<png::gray_pixel> image(myImage.width,myImage.height);

    for(int y=0; y<(int)image.get_height(); ++y)
    {
        for(int x=0; x<(int)image.get_width(); ++x)
        {
            image[y][x] = png::gray_pixel(myImage.data[y][x]);
        }
    }

    image.write(fileName);
}
