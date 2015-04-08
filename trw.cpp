#include "trw.h"
#include "problemStatic.h"
#include "tree.h"
#include "threadQueue.h"
#include <functional>
#include <limits>
#include <iostream>
#include <tuple>
#include <thread>

// Functions declaration

void generateTreeList(image &Ldata, image &Rdata, vector<tree> &trees);
void generateLookupTables(int width, int height, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup);
void divideUnaries(vector<vector<reference_wrapper<node>>> &nodeLookup);

double computePrimal(image &Ldata, image &Rdata, vector<vector<int>> label);
double computeDual( vector<tree> &trees);
void projection(vector<vector<reference_wrapper<tree>>> &treeLookup,
                vector<vector<reference_wrapper<node>>> &nodeLookup,
                vector<vector<int>> &label);
void infiniteWorker(Queue<vector<tree>> &workQueue,
                    image &Ldata, image &Rdata,
                    vector<float> &primal_values);

void trw(image &Ldata, image &Rdata, vector<vector<int>> &label,
         vector<float> &dual_values, vector<float> &primal_values) {
    // label should be initialized with the same size as data.

    // main storage and lookup
    vector<vector<reference_wrapper<node>>> nodeLookup;
    vector<vector<reference_wrapper<tree>>> treeLookup;
    vector<tree> trees;
    // temp storage and lookup for primal value estimation
    vector<vector<reference_wrapper<node>>> tempNodeLookup;
    vector<vector<reference_wrapper<tree>>> tempTreeLookup;
    vector<tree> tempTrees;

    // Initialization
    cout<<"Generating the trees"<<endl;
    generateTreeList(Ldata, Rdata, trees);
    generateLookupTables(Ldata.width, Ldata.height, trees, treeLookup, nodeLookup);
    cout<<"Dividing the unaries"<<endl;
    divideUnaries(nodeLookup);

    // Check
    cout << "Computation of a dual" << endl;
    double dual_value = computeDual(trees);
    cout << "Initial Value of the dual: " << dual_value << endl;

    // Threading initialization
    Queue<vector<tree>> workQueue;
    thread worker(bind(infiniteWorker, ref(workQueue),
                        ref(Ldata), ref(Rdata),
                        ref(primal_values)));

    // TRW
    cout<< "Starting the TRW" <<endl;
    int max_id = Ldata.width * Ldata.height;
    double improvement = 1;
    while(improvement > 0) {
        double old_dual_value = dual_value;
        improvement = 0;
        for(int curr_id = 0; curr_id < max_id; ++curr_id) {
            vector<reference_wrapper<tree>> concerned_trees = treeLookup[curr_id];
            vector<reference_wrapper<node>> concerned_nodes = nodeLookup[curr_id];
            double dual_increase = 0;

            // Compute the min marginals at this node for all the trees that contain it.
            for(vector<reference_wrapper<tree>>::iterator t_iter = concerned_trees.begin(), t_end=concerned_trees.end();
                t_iter < t_end; ++t_iter) {
                t_iter->get().forward(curr_id);
                t_iter->get().backward(curr_id);
            }

            // Get the value of the dual for these nodes, and sum all the unaries in a buffer
            array<double,NBR_CLASSES> unary_buffer={};
            for(vector<reference_wrapper<node>>::iterator n_iter = concerned_nodes.begin(), n_end=concerned_nodes.end();
                n_iter < n_end; ++n_iter) {
                // could compute the min in the same pass as going
                // over the vector to do the sum
                dual_increase -= n_iter->get().get_min_unary();
                int buffer_pos = 0;
                for(array<double,NBR_CLASSES>::iterator unary_iter = n_iter->get().unaries.begin(), unary_end = n_iter->get().unaries.end();
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
                for(array<double,NBR_CLASSES>::iterator unary_iter = n_iter->get().unaries.begin(), unary_end = n_iter->get().unaries.end();
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
        dual_values.push_back(dual_value);
        cout << "Augmented the global dual by "<< dual_value - old_dual_value
             <<", incremental improvement report "<< improvement<< endl;
 
        // Compute the primal increase at this iteration
        workQueue.push(trees);
    }

    cout << "Value of the dual: " << dual_value << endl;

    cout << "Waiting for the worker to finish" << endl;
    // Add a signal to stop the worker
    workQueue.push(vector<tree>());
    worker.join();

    cout << "Do the projection to get the final labels" << endl;
    projection(treeLookup, nodeLookup, label);
    double primal_val = computePrimal( Ldata, Rdata, label);
    cout << "Value of the primal: "<< primal_val <<endl;
}

void infiniteWorker(Queue<vector<tree>> &workQueue,
                    image &Ldata, image &Rdata,
                    vector<float> &primal_values) {
    int height = Ldata.height;
    int width = Ldata.width;
    vector<tree> trees;
    vector<vector<reference_wrapper<tree>>> treeLookup;
    vector<vector<reference_wrapper<node>>> nodeLookup;

    cout<<"Worker ready"<<endl;
    while(true) {
        trees = workQueue.pop();
        if(trees.size() == 0) {
            cout<<"Worker exiting"<<endl;
            break;
        }
        vector<vector<int>> label = vector<vector<int>>(height, vector<int>(width, NBR_CLASSES));
        treeLookup = vector<vector<reference_wrapper<tree>>>();
        nodeLookup = vector<vector<reference_wrapper<node>>>();
        // Recreate the lookup table on the copie of the trees
        generateLookupTables(width, height, trees, treeLookup, nodeLookup);
        // Do the projection
        projection(treeLookup, nodeLookup, label);
        // compute the primal value
        primal_values.push_back(computePrimal(Ldata, Rdata, label));
        // Print the image
        image labelsImage = image(label);
        printImage(labelsImage, "labelsImage.png");
    }
}


void generateTreeList(image &Ldata, image &Rdata, vector<tree> &trees){

    int i, j, label;
    int nbrCol = Ldata.width;

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
                double edge_weight = weights(Ldata.data[j][i], Ldata.data[j+1][i]);
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
                double edge_weight = weights(Ldata.data[j][i], Ldata.data[j][i+1]);
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
}


void generateLookupTables(int width, int height, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup) {
    int i,j;

    // Initialize lookup tables
    for(i=0; i<width; ++i) {
        for(j=0; j<height; ++j) {
            treeLookup.push_back( vector<reference_wrapper<tree>>());
            nodeLookup.push_back( vector<reference_wrapper<node>>());
        }
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
            for(array<double, NBR_CLASSES>::iterator unary_value= node_inst.unaries.begin(), unary_end = node_inst.unaries.end();
                unary_value < unary_end; ++unary_value) {
                *unary_value = *unary_value / nb_instance_node;
            }
        }
    }
}


double computeDual(vector<tree> &trees) {

    double dual_value = 0;

    for (vector<tree>::iterator tree_iter= trees.begin(), tree_end = trees.end();
         tree_iter<tree_end; ++tree_iter) {
        dual_value += tree_iter->forward_backward_min_marginals();
    }

    return dual_value;
}

double computePrimal(image &Ldata, image &Rdata, vector<vector<int>> labels){
    int height = labels.size();
    int width = labels[0].size();

    double edge_weight, binary_cost;
    double primal_value = 0;

    // Unary terms
    for(int row = 0; row< height; ++row){
        for(int col = 0; col < width; ++col){
            if(col - labels[row][col] >= 0){
                primal_value += fabs( Ldata.data[row][col] - Rdata.data[row][col - labels[row][col]]);
            }
        }
    }

    // Vertical binary terms
    for(int col=0; col<width; ++col){
        for(int row=0; row<height-1; ++row){
            edge_weight = weights(Ldata.data[row][col], Ldata.data[row+1][col]);
            binary_cost = binary(labels[row][col], labels[row+1][col]);
            primal_value += LAMBDA * edge_weight * binary_cost;
        }
    }
    // Horizontal binary terms
    for(int row=0; row<height; ++row){
        for(int col=0; col<width-1; ++col){
            edge_weight = weights(Ldata.data[row][col], Ldata.data[row][col+1]);
            binary_cost = binary(labels[row][col], labels[row][col+1]);
            primal_value += LAMBDA * edge_weight * binary_cost;
        }
    }
    return primal_value;
}


void projection(vector<vector<reference_wrapper<tree>>> &treeLookup,
                vector<vector<reference_wrapper<node>>> &nodeLookup,
                vector<vector<int>> &label) {
    int i, source_label, selected_label, best_nbr;
    int width = label[0].size();

    bool do_print = false;

    // Loop over all nodes starting from the end
    int nbr_node = treeLookup.size();
    for(int nodeId=0; nodeId<nbr_node; ++nodeId) {
        if(do_print) {
            cout<<"\rCurrent Node: "<<(nodeId+1)<<"/"<<nbr_node<<flush;
        }
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
            double min_score = numeric_limits<double>::infinity();
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
        label[nodeId/width][nodeId%width] = selected_label;

        // Update the trees to force the current node to take label selected_label in all trees
        for(i=0; i<(int)tree_containing_node.size(); ++i) {
            node &working_node = node_ref[i];
            for(source_label=0; source_label<NBR_CLASSES; ++source_label) {
                if(source_label!=selected_label) {
                    working_node.unaries[source_label] = numeric_limits<double>::infinity();
                }
            }
        }
    }
    if (do_print) {
        cout<<endl;
    }
}
