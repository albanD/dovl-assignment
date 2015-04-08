#include "ioUtils.h"
#include "imageClass.h"
#include "problemStatic.h"
#include "trw.h"


int main(int argc, char * argv[]) {

    image LImageContent;
    image RImageContent;
    
    getImage(LImageContent, "imL.png");
    getImage(RImageContent, "imR.png");

    cout<<"left is: "<<LImageContent.width<<"x"<<LImageContent.height<<endl;
    cout<<"right is: "<<RImageContent.width<<"x"<<RImageContent.height<<endl;

    printImage(LImageContent, "newImage.png");

    vector<vector<int>> labels = vector<vector<int>>(LImageContent.height, vector<int>(LImageContent.width, NBR_CLASSES));

    trw(LImageContent, RImageContent, labels);

    image labelsImage = image(labels);
    printImage(labelsImage, "labelsImage.png");

    return 0;
}