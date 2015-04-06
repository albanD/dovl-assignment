#include "trw.h"
#include "problemStatic.h"
#include "tree.h"
#include <functional>

void trw(image &Ldata, image &Rdata, vector<vector<int>> label) {
    // label should be initialized with the same size as data.

    vector<vector<reference_wrapper<node>>> nodeLookup;
    vector<vector<reference_wrapper<tree>>> treeLookup;
    vector<tree> trees;

    generateTrees(Ldata, Rdata, trees, treeLookup, nodeLookup);

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
                // TODO unary repartition??
                // TODO handling negative indexes?
                tempNode.addUnary(unary(Ldata.data[j][i], Rdata.data[j][i-label]));
            }
            if( j+1 < Ldata.height){
                edge tempEdge = edge();
                float edge_weight = weights(Ldata.data[j][i], Ldata.data[j+1][i]);
                for( label=0; label<NBR_CLASSES; ++label){
                    tempEdge.addLineWeights( weightLine(edge_weight, label));
                }
            }
            treeNodes.push_back(tempNode);
            nodeLookup[node_id].push_back(ref(tempNode));
        }

        tree col_tree = tree(treeNodes, treeEdges);
        for(vector<node>::iterator n_iter= treeNodes.begin(), n_end= treeNodes.end(); n_iter < n_end; ++n_iter){
            treeLookup[n_iter->id].push_back(ref(col_tree));
        }
    }
}
