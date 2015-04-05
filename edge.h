#ifndef EDGE_H
#define EDGE_H

#include <vector>


using namespace std;

class edge {

public:
    vector<vector<float>> weights;
    // Use the coordinates in the same order as the Nodes in the corresponding vector of unaries.

    edge();
    void addLineWeights(vector<float> weightLine);
};
#endif