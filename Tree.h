#ifndef AABBTREE_H
#define AABBTREE_H

#include <AABB.h>
#include <iostream>

template <typename T>
using vector = std::vector<T>;


struct Node
{
    Node(){}
    bool isLeaf = false;
    AABB* Data;
    operator AABB&()
    {
        return *Data;
    }
};

class Tree{
    Node root;
    vector<Node> treeNodes;
    public:
        Tree(Node&& Root) :
        treeNodes(vector<Node>()),
        root(std::forward<Node>(Root))
        {}

    void InsertNode(Node&& newNode);
    void InsertNodeUtil(Node* insertingNode, Node&& insertedNode);
};

#endif