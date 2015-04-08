#ifndef TRW_H
#define TRW_H

#include <vector>
#include <array>

#include "imageClass.h"
#include "ioUtils.h"
#include "node.h"
#include "tree.h"

using namespace std;

void trw(image &Ldata, image &Rdata, vector<vector<int>> &label,
         vector<float> &dual_values, vector<float> &primal_values);

#endif
