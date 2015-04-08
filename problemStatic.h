#ifndef PROBLEMSTATIC_H
#define PROBLEMSTATIC_H

#define NBR_CLASSES 15
#define K 2
#define LAMBDA 20
#define d_max 15

#include <cmath>
#include <array>

using namespace std;

double unary(double pixel_value_x, double pixel_value_y);
double binary(int label1, int label2);
double weights(double pixel_value_1, double pixel_value_2);
int getNodeIdFromCoord(int i, int j, int nbrCol);
array<double,NBR_CLASSES> weightLine(double edge_weight, int first_label);

#endif
