#include "problemStatic.h"
#include "tree.h"
#include <limits>
#include <array>
#include <algorithm>
#include <iostream>
#include <stdexcept>

tree::tree(vector<node> nodes, vector<edge> edges) {
    this->nodes = nodes;
    this->edges = edges;
}

double tree::forward_backward_min_marginals(){
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
    int target_label, source_label;
    double reparam_constant;
    array<double, NBR_CLASSES> reparam_candidate;

    while(nodes[node_pos].id != until_nodeId) {
        node& from_node = nodes[node_pos];
        edge& linking_edge = edges[node_pos];

        for(target_label=0; target_label < NBR_CLASSES; ++target_label) {

            // Compute the reparametrization constant
            for(source_label=0; source_label< NBR_CLASSES; ++source_label) {
                reparam_candidate[source_label] = from_node.unaries[source_label];
            }
            for(source_label=0; source_label<NBR_CLASSES; ++source_label){
                reparam_candidate[source_label] += linking_edge.weights[source_label][target_label];
            }
            reparam_constant = *min_element(reparam_candidate.begin(), reparam_candidate.end());

            // Reparametrize that shit!
            nodes[node_pos+1].unaries[target_label] += reparam_constant;
            for(source_label=0; source_label< NBR_CLASSES; ++source_label) {
                edges[node_pos].weights[source_label][target_label] -= reparam_constant;
            }
        }
        ++node_pos;
    }
}

void tree::backward(int until_nodeId) {
    int nb_nodes = nodes.size();
    int node_pos = nb_nodes-1;
    int target_label, source_label;
    double reparam_constant;
    array<double, NBR_CLASSES> reparam_candidate;

    while(nodes[node_pos].id != until_nodeId) {
        node& from_node = nodes[node_pos];
        edge& linking_edge = edges[node_pos-1];

        for(target_label=0; target_label < NBR_CLASSES; ++target_label) {

            // Compute the reparametrization constant
            for(source_label=0; source_label< NBR_CLASSES; ++source_label) {
                reparam_candidate[source_label] = from_node.unaries[source_label];
            }
            for(source_label=0; source_label<NBR_CLASSES; ++source_label){
                reparam_candidate[source_label] += linking_edge.weights[target_label][source_label];
            }
            reparam_constant = *min_element(reparam_candidate.begin(), reparam_candidate.end());
            // Reparametrize that shit!
            nodes[node_pos-1].unaries[target_label] += reparam_constant;
            for(source_label=0; source_label< NBR_CLASSES; ++source_label) {
                edges[node_pos-1].weights[target_label][source_label] -= reparam_constant;
            }
        }
        --node_pos;
    }
}

bool tree::proper_min_marginals() {
    // Verify that all the unary are in min-marginal condition
    int nb_nodes = nodes.size();
    double dual_value = *min_element(nodes[0].unaries.begin(), nodes[0].unaries.end());
    for(int node = 1; node<nb_nodes; ++node){
        double new_dual_value = *min_element(nodes[node].unaries.begin(), nodes[node].unaries.end());
        if(fabs(new_dual_value - dual_value) > 1e-3) {
            return false;
        }
    }
    return true;
}


double tree::get_min_marginal_for_id(int node_id) {
    // There should be no reason but verification to use this
    for(vector<node>::iterator n_iter=nodes.begin(),n_end=nodes.end();
        n_iter<n_end; ++n_iter) {
        if (n_iter->id == node_id) {
            return *min_element(n_iter->unaries.begin(), n_iter->unaries.end());
        }
    }
    throw invalid_argument("Doesn't belong to this tree");
}
