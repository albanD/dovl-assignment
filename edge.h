#ifndef EDGE_H
#define EDGE_H

#include "problemStatic.h"
#include <array>


using namespace std;

class edge {

public:
    array<array<double,NBR_CLASSES>, NBR_CLASSES> weights;
    // Use the coordinates in the same order as the Nodes in the corresponding vector of unaries.
    int next_line;

    edge();
    void addLineWeights(array<double, NBR_CLASSES> weightLine);
};
#endif
