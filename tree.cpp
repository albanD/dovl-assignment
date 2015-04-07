#include "tree.h"
#include <limits>
#include <algorithm>
#include <iostream>

tree::tree(vector<node> nodes, vector<edge> edges) {
    this->nodes = nodes;
    this->edges = edges;
}

float tree::forward_backward_min_marginals(){
    int nb_nodes = nodes.size();

    // Do the forward
    for(int node_pos=0; node_pos < nb_nodes-1; ++node_pos){
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
    }
    // Do the backward
    for(int node_pos=nb_nodes-1; node_pos > 0; --node_pos){
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
    }

    float dual_value = *min_element(nodes[0].unaries.begin(), nodes[0].unaries.end());
    // Verify that all the min marginals agree and we didn't fuck up.
    bool do_verification_check = false;
    if(do_verification_check){
        for(int node = 1; node<nb_nodes; ++node){
            float new_dual_value = *min_element(nodes[node].unaries.begin(), nodes[node].unaries.end());
            if(fabs(new_dual_value - dual_value) > 1e-3) {
                cout<< "We have an error boyz; "<< dual_value
                    <<" vs. " << new_dual_value
                    << " difference of "<< new_dual_value-dual_value<<endl;
            }
        }
    }

    return dual_value;

}
