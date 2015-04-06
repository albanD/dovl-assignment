#ifndef TRW_H
#define TRW_H

#include <vector>
#include <array>

#include "imageClass.h"
#include "node.h"
#include "tree.h"

using namespace std;

void trw(image &Ldata, image &Rdata, vector<vector<int>> label);
void generateTrees(image &Ldata, image &Rdata, vector<tree> &trees,
                   vector<vector<reference_wrapper<tree>>> &treeLookup,
                   vector<vector<reference_wrapper<node>>> &nodeLookup);



#endif
