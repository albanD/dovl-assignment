#ifndef IOUTILS_H
#define IOUTILS_H

#include <vector>
#include <array>
#include <string>
#include <iostream>

#include "imageClass.h"

using namespace std;

void getImage(image &data, string fileName);

void printImage(image &data, string fileName);
#endif