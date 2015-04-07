#include "node.h"
#include <algorithm>

node::node(int id) {
    this->id = id;
}


void node::addUnary(float value) {
    this->unaries.push_back(value);
}

float node::get_min_unary() {
    return *min_element(unaries.begin(), unaries.end());
}
