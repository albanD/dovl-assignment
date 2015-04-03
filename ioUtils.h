#ifndef IOUTILS_H
#define IOUTILS_H

#include <vector>
#include <array>
#include <string>
#include <iostream>

using namespace std;

void getImage(vector<vector<array<float,3>>> &data, string fileName);

void printImage(vector<vector<array<float,3>>> &data, string fileName);
#endif