#include "trw.h"
#include "problemStatic.h"
#include "tree.h"
#include <functional>
#include <limits>
#include <iostream>

// Functions declaration

void generateTrees(image &Ldata, image &Rdata, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup);
void divideUnaries(vector<vector<reference_wrapper<node>>> &nodeLookup);
float computeDual( vector<tree> &trees);
void projection(vector<vector<reference_wrapper<tree>>> &treeLookup,
                vector<vector<reference_wrapper<node>>> &nodeLookup,
                vector<vector<int>> &label);

void trw(image &Ldata, image &Rdata, vector<vector<int>> &label) {
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
    float improvement = 1001;
    while(improvement > 1000) {
        float old_dual_value = dual_value;
        improvement = 0;
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
            array<float,NBR_CLASSES> unary_buffer={};
            for(vector<reference_wrapper<node>>::iterator n_iter = concerned_nodes.begin(), n_end=concerned_nodes.end();
                n_iter < n_end; ++n_iter) {
                // could compute the min in the same pass as going
                // over the vector to do the sum
                dual_increase -= n_iter->get().get_min_unary();
                int buffer_pos = 0;
                for(array<float,NBR_CLASSES>::iterator unary_iter = n_iter->get().unaries.begin(), unary_end = n_iter->get().unaries.end();
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
                for(array<float,NBR_CLASSES>::iterator unary_iter = n_iter->get().unaries.begin(), unary_end = n_iter->get().unaries.end();
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

            improvement += dual_increase;
        }
        dual_value = computeDual(trees);
        cout << "Augmented the global dual by "<< dual_value - old_dual_value
             <<", incremental improvement report "<< improvement<< endl;
    }

    cout << "Value of the dual: " << dual_value << endl;

    cout << "Do the projection to get the labels" << endl;
    projection(treeLookup, nodeLookup, label);
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
            for(array<float, NBR_CLASSES>::iterator unary_value= node_inst.unaries.begin(), unary_end = node_inst.unaries.end();
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


void projection(vector<vector<reference_wrapper<tree>>> &treeLookup,
                vector<vector<reference_wrapper<node>>> &nodeLookup,
                vector<vector<int>> &label) {
    int i, source_label, selected_label, best_nbr;

    // Loop over all nodes starting from the end
    int nbr_node = treeLookup.size();
    for(int nodeId=0; nodeId<nbr_node; ++nodeId) {
        cout<<"\rCurrent Node: "<<(nodeId+1)<<"/"<<nbr_node<<flush;
        vector<reference_wrapper<tree>> &tree_containing_node = treeLookup[nodeId];
        vector<reference_wrapper<node>> &node_ref = nodeLookup[nodeId];
        // All the desired labels for this node
        vector<int> desired_labels = vector<int>(NBR_CLASSES, 0);
        for(i=0; i<(int)tree_containing_node.size(); ++i) {
            tree &working_tree = tree_containing_node[i];
            node &working_node = node_ref[i];
            // Do the forward/backward
            working_tree.forward(nodeId);
            working_tree.backward(nodeId);
            // We know that this node is the last one, no need for backward to get its min marginals
            // Find the best prediction
            float min_score = numeric_limits<float>::infinity();
            selected_label = -1;
            for(source_label=0; source_label<NBR_CLASSES; ++source_label) {
                if(working_node.unaries[source_label]<min_score) {
                    min_score = working_node.unaries[source_label];
                    selected_label = source_label;
                }
            }
            ++desired_labels[selected_label];
        }

        // Get the most wanted label for this node
        selected_label = 0;
        best_nbr = 0;
        for(i=0; i<NBR_CLASSES; ++i) {
            if(desired_labels[i] > best_nbr) {
                selected_label = i;
                best_nbr = desired_labels[i];
            }
        }

        // Set the selected label on the output
        label[nodeId%label.size()][nodeId/label.size()] = selected_label;

        // Update the trees to force the current node to take label selected_label in all trees
        for(i=0; i<(int)tree_containing_node.size(); ++i) {
            node &working_node = node_ref[i];
            for(source_label=0; source_label<NBR_CLASSES; ++source_label) {
                if(source_label!=selected_label) {
                    working_node.unaries[source_label] = numeric_limits<float>::infinity();
                }
            }
        }
    }
    cout<<endl;
}
