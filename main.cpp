#include "ioUtils.h"
#include "imageClass.h"


int main(int argc, char * argv[]) {

    image LImageContent;
    image RImageContent;
    
    getImage(LImageContent, "imL.png");
    getImage(RImageContent, "imR.png");

    cout<<"left is: "<<LImageContent.width<<"x"<<LImageContent.height<<endl;
    cout<<"right is: "<<RImageContent.width<<"x"<<RImageContent.height<<endl;

    printImage(LImageContent, "newImage.png");
    return 0;
}