#include "trw.h"
#include "problemStatic.h"
#include "tree.h"
#include <functional>


#include <iostream>

// Functions declaration

void generateTrees(image &Ldata, image &Rdata, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup);
void divideUnaries(vector<vector<reference_wrapper<node>>> &nodeLookup);
float computeDual( vector<tree> &trees);

void trw(image &Ldata, image &Rdata, vector<vector<int>> label) {
    // label should be initialized with the same size as data.

    vector<vector<reference_wrapper<node>>> nodeLookup;
    vector<vector<reference_wrapper<tree>>> treeLookup;
    vector<tree> trees;

    // Initialization
    cout<<"Generating the trees"<<endl;
    generateTrees(Ldata, Rdata, trees, treeLookup, nodeLookup);
    cout<<"Dividing the unaries"<<endl;
    divideUnaries(nodeLookup);

    // Check
    cout << "Computation of a dual" << endl;
    float dual_value = computeDual(trees);
    cout << "Initial Value of the dual: " << dual_value << endl;

    // TRW
    cout<< "Starting the TRW" <<endl;
    int max_id = Ldata.width * Ldata.height;
    bool making_progress = true;
    while(making_progress) {
        float old_dual_value = dual_value;
        float improvement = 0;
        making_progress = false;
        for(int curr_id = 0; curr_id < max_id; ++curr_id) {
            vector<reference_wrapper<tree>> concerned_trees = treeLookup[curr_id];
            vector<reference_wrapper<node>> concerned_nodes = nodeLookup[curr_id];
            float dual_increase = 0;

            // Compute the min marginals at this node for all the trees that contain it.
            for(vector<reference_wrapper<tree>>::iterator t_iter = concerned_trees.begin(), t_end=concerned_trees.end();
                t_iter < t_end; ++t_iter) {
                t_iter->get().forward(curr_id);
                t_iter->get().backward(curr_id);
            }

            // Get the value of the dual for these nodes, and sum all the unaries in a buffer
            vector<float> unary_buffer = vector<float>(NBR_CLASSES, 0);
            for(vector<reference_wrapper<node>>::iterator n_iter = concerned_nodes.begin(), n_end=concerned_nodes.end();
                n_iter < n_end; ++n_iter) {
                // could compute the min in the same pass as going
                // over the vector to do the sum
                dual_increase -= n_iter->get().get_min_unary();
                int buffer_pos = 0;
                for(vector<float>::iterator unary_iter = n_iter->get().unaries.begin(), unary_end = n_iter->get().unaries.end();
                    unary_iter < unary_end; ++unary_iter) {
                    unary_buffer[buffer_pos] += *unary_iter;
                    ++buffer_pos;
                }
            }

            // Average the min-marginals and replace in the various trees
            int nb_trees_concerned = concerned_nodes.size();
            for(vector<reference_wrapper<node>>::iterator n_iter = concerned_nodes.begin(), n_end=concerned_nodes.end();
                n_iter < n_end; ++n_iter) {
                int buffer_pos = 0;
                for(vector<float>::iterator unary_iter = n_iter->get().unaries.begin(), unary_end = n_iter->get().unaries.end();
                    unary_iter < unary_end; ++unary_iter) {
                    *unary_iter = unary_buffer[buffer_pos] / nb_trees_concerned;
                    ++buffer_pos;
                }
            }

            // Compute the dual increase that this operation brought
            for(vector<reference_wrapper<tree>>::iterator t_iter = concerned_trees.begin(), t_end=concerned_trees.end();
                t_iter < t_end; ++t_iter) {
                t_iter->get().forward(curr_id);
                t_iter->get().backward(curr_id);
            }
            for(vector<reference_wrapper<node>>::iterator n_iter = concerned_nodes.begin(), n_end=concerned_nodes.end();
                n_iter < n_end; ++n_iter) {
                dual_increase += n_iter->get().get_min_unary();
            }

            if(dual_increase > 0){
                making_progress = true;
            }
            improvement += dual_increase;
        }
        dual_value = computeDual(trees);
        cout << "Augmented the global dual by "<< dual_value - old_dual_value
             <<", incremental improvement report "<< improvement<< endl;
    }

}



void generateTrees(image &Ldata, image &Rdata, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup){

    int i, j, label;
    int nbrCol = Ldata.width;

    // Initialize lookup tables
    for(i=0; i<Ldata.width; ++i) {
        for(j=0; j<Ldata.height; ++j) {
            treeLookup.push_back( vector<reference_wrapper<tree>>());
            nodeLookup.push_back( vector<reference_wrapper<node>>());
        }
    }

    // generate a tree for each column
    for(i=0; i<Ldata.width; ++i) {
        vector<node> treeNodes = vector<node>();
        vector<edge> treeEdges = vector<edge>();

        for(j=0; j<Ldata.height; ++j) {
            int node_id = getNodeIdFromCoord(j, i, nbrCol);
            node tempNode = node(node_id);
            for(label=0; label<NBR_CLASSES; ++label) {
                if(i-label > 0) {
                    tempNode.addUnary(unary(Ldata.data[j][i], Rdata.data[j][i-label]));
                } else {
                    tempNode.addUnary(0);
                }
            }
            if( j+1 < Ldata.height){
                edge tempEdge = edge();
                float edge_weight = weights(Ldata.data[j][i], Ldata.data[j+1][i]);
                for( label=0; label<NBR_CLASSES; ++label){
                    tempEdge.addLineWeights( weightLine(edge_weight, label));
                }
                treeEdges.push_back(tempEdge);
            }
            treeNodes.push_back(tempNode);
        }

        tree col_tree = tree(treeNodes, treeEdges);
        trees.push_back(col_tree);
    }

    // generate a tree for each row
    for(j=0; j<Ldata.height; ++j) {
        vector<node> treeNodes = vector<node>();
        vector<edge> treeEdges = vector<edge>();

        for(i=0; i<Ldata.width; ++i) {
            int node_id = getNodeIdFromCoord(j, i, nbrCol);
            node tempNode = node(node_id);
            for(label=0; label<NBR_CLASSES; ++label) {
                if(i-label>0){
                    tempNode.addUnary(unary(Ldata.data[j][i], Rdata.data[j][i-label]));
                } else {
                    tempNode.addUnary(0);
                }
            }
            if( i+1 < Ldata.width){
                edge tempEdge = edge();
                float edge_weight = weights(Ldata.data[j][i], Ldata.data[j][i+1]);
                for( label=0; label<NBR_CLASSES; ++label){
                    tempEdge.addLineWeights( weightLine(edge_weight, label));
                }
                treeEdges.push_back(tempEdge);
            }
            treeNodes.push_back(tempNode);
        }

        tree row_tree = tree(treeNodes, treeEdges);
        trees.push_back(row_tree);
    }

    for (vector<tree>::iterator tree_iter= trees.begin(), tree_end = trees.end();
         tree_iter < tree_end; ++tree_iter) {
        for(vector<node>::iterator node_iter = tree_iter->nodes.begin(), node_end = tree_iter->nodes.end();
                node_iter < node_end; ++node_iter) {
            treeLookup[node_iter->id].push_back(ref(*tree_iter));
            nodeLookup[node_iter->id].push_back(ref(*node_iter));

        }

    }

}


void divideUnaries(vector<vector<reference_wrapper<node>>> &nodeLookup) {
    for(int i=0, nb_nodes =nodeLookup.size(); i<nb_nodes; ++i) {
        vector<reference_wrapper<node>> node_instances = nodeLookup[i];
        int nb_instance_node = node_instances.size();
        for(vector<reference_wrapper<node>>::iterator node_iter= node_instances.begin(),
                 node_end = node_instances.end(); node_iter < node_end; ++node_iter) {
            node node_inst = *node_iter;
            for(vector<float>::iterator unary_value= node_inst.unaries.begin(), unary_end = node_inst.unaries.end();
                unary_value < unary_end; ++unary_value) {
                *unary_value = *unary_value / nb_instance_node;
            }
        }
    }
}


float computeDual(vector<tree> &trees) {

    float dual_value = 0;

    for (vector<tree>::iterator tree_iter= trees.begin(), tree_end = trees.end();
         tree_iter<tree_end; ++tree_iter) {
        dual_value += tree_iter->forward_backward_min_marginals();
    }

    return dual_value;
}
