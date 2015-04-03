#include "ioUtils.h"


int main(int argc, char * argv[]) {

    vector<vector<array<float,3>>> LImageContent;
    vector<vector<array<float,3>>> RImageContent;
    
    getImage(LImageContent, "imL.png");
    getImage(RImageContent, "imR.png");

    cout<<"left is: "<<LImageContent.size()<<"x"<<LImageContent[0].size()<<endl;
    cout<<"right is: "<<RImageContent.size()<<"x"<<RImageContent[0].size()<<endl;

    printImage(LImageContent, "newImage.png");
    return 0;
}