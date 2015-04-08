#include "edge.h"


edge::edge() {
    next_line = 0;
}

void edge::addLineWeights(array<float,NBR_CLASSES> weightLine) {
    weights[next_line] = weightLine;
    ++next_line;
}
