#ifndef TREE_H
#define TREE_H

#include <vector>

#include "node.h"
#include "edge.h"

class tree {

public:
    vector<node> nodes;
    vector<edge> edges;

    tree(vector<node> nodes, vector<edge> edges);
};


#endif