#include "tree.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <stdexcept>

tree::tree(vector<node> nodes, vector<edge> edges) {
    this->nodes = nodes;
    this->edges = edges;
}

float tree::forward_backward_min_marginals(){
    int nb_nodes = nodes.size();

    // Do the forward
    forward(nodes[nb_nodes-1].id);
    // Do the backward
    backward(nodes[0].id);

    if(not proper_min_marginals()) {
        cout << "We fucked up boyz!" <<endl;
    }

    return get_min_marginal_for_id(nodes[0].id);
}



void tree::forward(int until_nodeId) {
    int node_pos = 0;
    while(nodes[node_pos].id != until_nodeId) {
        node from_node = nodes[node_pos];
        edge linking_edge = edges[node_pos];

        int nb_labels = from_node.unaries.size();

        for(int target_label=0; target_label < nb_labels; ++target_label) {

            // Compute the reparametrization constant
            float reparam_constant = numeric_limits<float>::infinity();
            for(int source_label=0; source_label< nb_labels; ++source_label) {
                reparam_constant = min(reparam_constant,
                                       from_node.unaries[source_label] + linking_edge.weights[source_label][target_label]);
            }

            // Reparametrize that shit!
            nodes[node_pos+1].unaries[target_label] += reparam_constant;
            for(int source_label=0; source_label< nb_labels; ++source_label) {
                edges[node_pos].weights[source_label][target_label] -= reparam_constant;
            }
        }
        ++node_pos;
    }
}

void tree::backward(int until_nodeId) {
    int nb_nodes = nodes.size();
    int node_pos = nb_nodes-1;
    while(nodes[node_pos].id != until_nodeId) {
        node from_node = nodes[node_pos];
        edge linking_edge = edges[node_pos-1];

        int nb_labels = from_node.unaries.size();

        for(int target_label=0; target_label < nb_labels; ++target_label) {

            // Compute the reparametrization constant
            float reparam_constant = numeric_limits<float>::infinity();
            for(int source_label=0; source_label< nb_labels; ++source_label) {
                reparam_constant = min(reparam_constant,
                                       from_node.unaries[source_label] + linking_edge.weights[target_label][source_label]);
            }

            // Reparametrize that shit!
            nodes[node_pos-1].unaries[target_label] += reparam_constant;
            for(int source_label=0; source_label< nb_labels; ++source_label) {
                edges[node_pos-1].weights[target_label][source_label] -= reparam_constant;
            }
        }
        --node_pos;
    }
}

bool tree::proper_min_marginals() {
    // Verify that all the unary are in min-marginal condition
    int nb_nodes = nodes.size();
    float dual_value = *min_element(nodes[0].unaries.begin(), nodes[0].unaries.end());
    for(int node = 1; node<nb_nodes; ++node){
        float new_dual_value = *min_element(nodes[node].unaries.begin(), nodes[node].unaries.end());
        if(fabs(new_dual_value - dual_value) > 1e-3) {
            return false;
        }
    }
    return true;
}


float tree::get_min_marginal_for_id(int node_id) {
    // There should be no reason but verification to use this
    for(vector<node>::iterator n_iter=nodes.begin(),n_end=nodes.end();
        n_iter<n_end; ++n_iter) {
        if (n_iter->id == node_id) {
            return *min_element(n_iter->unaries.begin(), n_iter->unaries.end());
        }
    }
    throw invalid_argument("Doesn't belong to this tree");
}
