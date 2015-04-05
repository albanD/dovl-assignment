#include "node.h"

node::node(int id) {
    this->id = id;
}


void node::addUnary(float value) {
    this->unaries.push_back(value);
}