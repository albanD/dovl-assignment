#include "problemStatic.h"

double unary(double pixel_value_x, double pixel_value_y) {
    return abs(pixel_value_x - pixel_value_y);
}

double binary(int label1, int label2) {
    int labelDiff = abs(label1-label2);
    return labelDiff<K ? labelDiff : K;
}

double weights(double pixel_value_1, double pixel_value_2) {
    return abs(pixel_value_1-pixel_value_2)<=8 ? 2 : 1;
}

int getNodeIdFromCoord(int row, int col, int nbrCol) {
    return row * nbrCol + col;
}

array<double,NBR_CLASSES> weightLine(double edge_weight, int first_label){
    array<double, NBR_CLASSES> weightLine;
    for(int second_label = 0; second_label<NBR_CLASSES; ++second_label) {
        weightLine[second_label] = LAMBDA * edge_weight * binary(first_label, second_label);
    }
    return weightLine;
}
