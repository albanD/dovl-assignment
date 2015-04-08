#include "node.h"
#include <algorithm>

node::node(int id) {
    this->id = id;
    this->next_unary = 0;
}


void node::addUnary(float value) {
   unaries[next_unary] = value;
   ++next_unary;
}

float node::get_min_unary() {
    return *min_element(unaries.begin(), unaries.end());
}
