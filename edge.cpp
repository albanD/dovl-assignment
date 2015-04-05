#include "edge.h"



edge::edge() {}

void edge::addLineWeights(vector<float> weightLine) {
    this->weights.push_back(weightLine);
}