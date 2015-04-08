#ifndef NODE_H
#define NODE_H

#include "problemStatic.h"
#include <array>


using namespace std;

class node {

public:
    array<double, NBR_CLASSES> unaries;
    int id;
    int next_unary;

    node(){};
    node(int id);
    void addUnary(double value);
    double get_min_unary();
};

#endif
