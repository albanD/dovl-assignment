#include "problemStatic.h"

float unary(float pixel_value_x, float pixel_value_y) {
    return abs(pixel_value_x - pixel_value_y);
}

float binary(int label1, int label2) {
    int labelDiff = abs(label1-label2);
    return labelDiff<K ? labelDiff : K;
}

float weights(float pixel_value_1, float pixel_value_2) {
    return abs(pixel_value_1-pixel_value_2)<=8 ? 2 : 1;
}

int getNodeIdFromCoord(int row, int col, int nbrCol) {
    return row * nbrCol + col;
}

vector<float> weightLine(float edge_weight, int first_label){
    vector<float> weightLine = vector<float>(NBR_CLASSES);
    for(int second_label = 0; second_label<NBR_CLASSES; ++second_label) {
        weightLine[second_label] = edge_weight * binary(first_label, second_label);
    }
    return weightLine;
}
