#ifndef PROBLEMSTATIC_H
#define PROBLEMSTATIC_H

#define NBR_CLASSES 15
#define K 2
#define lambda 20
#define d_max 15

#include <cmath>

using namespace std;

float unary(float pixel_value_x, float pixel_value_y);
float binary(int label1, int label2);
float weights(float pixel_value_1, float pixel_value_2);
int getNodeIdFromCoord(int i, int j, int nbrCol);

#endif