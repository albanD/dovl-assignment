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
    double forward_backward_min_marginals();
    void forward(int until_nodeId);
    void backward(int until_nodeId);
    bool proper_min_marginals();
    double get_min_marginal_for_id(int node_id);
};


#endif
