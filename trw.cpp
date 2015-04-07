#include "trw.h"
#include "problemStatic.h"
#include "tree.h"
#include <functional>

// Functions declaration

void generateTrees(image &Ldata, image &Rdata, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup);

void divideUnaries(vector<vector<reference_wrapper<node>>> &nodeLookup);


void trw(image &Ldata, image &Rdata, vector<vector<int>> label) {
    // label should be initialized with the same size as data.

    vector<vector<reference_wrapper<node>>> nodeLookup;
    vector<vector<reference_wrapper<tree>>> treeLookup;
    vector<tree> trees;

    generateTrees(Ldata, Rdata, trees, treeLookup, nodeLookup);
    divideUnaries(nodeLookup);

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
            node* tempNode = new node(node_id);
            for(label=0; label<NBR_CLASSES; ++label) {
                if(i-label > 0) {
                    tempNode->addUnary(unary(Ldata.data[j][i], Rdata.data[j][i-label]));
                } else {
                    tempNode->addUnary(0);
                }
            }
            if( j+1 < Ldata.height){
                edge* tempEdge = new edge();
                float edge_weight = weights(Ldata.data[j][i], Ldata.data[j+1][i]);
                for( label=0; label<NBR_CLASSES; ++label){
                    tempEdge->addLineWeights( weightLine(edge_weight, label));
                }
                treeEdges.push_back(*tempEdge);
            }
            treeNodes.push_back(*tempNode);
            nodeLookup[node_id].push_back(ref(*tempNode));
        }

        tree* col_tree = new tree(treeNodes, treeEdges);
        for(vector<node>::iterator n_iter= treeNodes.begin(), n_end= treeNodes.end(); n_iter < n_end; ++n_iter){
            treeLookup[n_iter->id].push_back(ref(*col_tree));
        }
        trees.push_back(*col_tree);
    }

    // generate a tree for each row
    for(j=0; j<Ldata.height; ++j) {
        vector<node> treeNodes = vector<node>();
        vector<edge> treeEdges = vector<edge>();

        for(i=0; i<Ldata.width; ++i) {
            int node_id = getNodeIdFromCoord(j, i, nbrCol);
            node* tempNode = new node(node_id);
            for(label=0; label<NBR_CLASSES; ++label) {
                if(i-label>0){
                    tempNode->addUnary(unary(Ldata.data[j][i], Rdata.data[j][i-label]));
                } else {
                    tempNode->addUnary(0);
                }
            }
            if( i+1 < Ldata.width){
                edge* tempEdge = new edge();
                float edge_weight = weights(Ldata.data[j][i], Ldata.data[j][i+1]);
                for( label=0; label<NBR_CLASSES; ++label){
                    tempEdge->addLineWeights( weightLine(edge_weight, label));
                }
                treeEdges.push_back(*tempEdge);
            }
            treeNodes.push_back(*tempNode);
            nodeLookup[node_id].push_back(ref(*tempNode));
        }

        tree* row_tree = new tree(treeNodes, treeEdges);
        for(vector<node>::iterator n_iter= treeNodes.begin(), n_end= treeNodes.end(); n_iter < n_end; ++n_iter){
            treeLookup[n_iter->id].push_back(ref(*row_tree));
        }
        trees.push_back(*row_tree);
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
