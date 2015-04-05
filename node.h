#ifndef NODE_H
#define NODE_H

#include <vector>

using namespace std;

class node {

public:
    vector<float> unaries;
    int id;

    node(int id);
    void addUnary(float value);
};

#endif