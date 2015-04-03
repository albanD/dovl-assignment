#include <png++/png.hpp>
/*
To install png++:
install the package "libpng-dev"
make and install the content of the "png++-0.2.5.tar.gz"
*/

#include <ioUtils.h>


void getImage(vector<vector<array<float,3>>> &data, string fileName) {

    png::image<png::rgb_pixel> image(fileName);

    for(size_t y=0; y<image.get_height(); ++y)
    {
        vector<array<float,3>> tempRow = vector<array<float,3>>();
        for(size_t x=0; x<image.get_width(); ++x)
        {
            array<float,3> tempArray = array<float,3>();
            tempArray[0] = (float)image[y][x].red;
            tempArray[1] = (float)image[y][x].green;
            tempArray[2] = (float)image[y][x].blue;
            tempRow.push_back(tempArray);
        }
        data.push_back(tempRow);
    }
}

void printImage(vector<vector<array<float,3>>> &data, string fileName) {
    png::image<png::rgb_pixel> image(data[0].size(),data.size());

    for(size_t y=0; y<image.get_height(); ++y)
    {
        for(size_t x=0; x<image.get_width(); ++x)
        {
            image[y][x] = png::rgb_pixel(data[y][x][0], data[y][x][1], data[y][x][2]);
        }
    }

    image.write(fileName);
}
