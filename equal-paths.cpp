#ifndef RECCHECK
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// Need a helper function for checking the depth
bool depthCheckHelper(Node* root, int depth, int& leafDepth){
    if(root == nullptr){
        return true;
    }
    // If the current node is a leaf node
    if(root->left == nullptr && root->right == nullptr){
        if(leafDepth == -1){
            leafDepth = depth;
        }
        return (leafDepth == depth);
    }

    return depthCheckHelper(root->left, depth + 1, leafDepth) && depthCheckHelper(root->right, depth + 1, leafDepth);
}

bool equalPaths(Node * root)
{
    int leafDepth = -1;
    return depthCheckHelper(root, 0, leafDepth);
}