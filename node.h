#ifndef NODE_H
#define NODE_H

#include "problemStatic.h"
#include <array>


using namespace std;

class node {

public:
    array<float, NBR_CLASSES> unaries;
    int id;
    int next_unary;

    node(){};
    node(int id);
    void addUnary(float value);
    float get_min_unary();
};

#endif
