#include "trw.h"
#include "problemStatic.h"
#include "tree.h"
#include <functional>


void trw(image &Ldata, image &Rdata, vector<vector<int>> label) {
    // label should be initialized with the same size as data.

    vector<reference_wrapper<tree>> nodeLookup;
    vector<reference_wrapper<tree>> treeLookup;
    vector<tree> trees;

    generateTrees(Ldata, Rdata, trees, treeLookup);

}



void generateTrees(image &Ldata, image &Rdata, vector<tree> &trees, vector<reference_wrapper<tree>> treeLookup){//, vector<reference_wrapper<node>> nodeLookup) {
    int i, j, label;
    int nbrCol = Ldata.width;

    // Initialize lookup tables
    for(i=0; i<Ldata.width; ++i) {
        for(j=0; j<Ldata.height; ++j) {
        }
    }

    // generate a tree for each line and each column
    for(i=0; i<Ldata.width; ++i) {
        vector<node> treeNodes = vector<node>();
        vector<edge> treeEdges = vector<edge>();

        for(j=0; j<Ldata.height; ++j) {
            node tempNode = node(getNodeIdFromCoord(i, j, nbrCol));
            for(label=0; label<NBR_CLASSES; ++label) {
                tempNode.addUnary(unary(Ldata.data[j][i], Rdata.data[j][i-label]));
            }
            treeNodes.push_back(tempNode);
        }

    }
}
