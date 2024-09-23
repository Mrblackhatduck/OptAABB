#ifndef AABBTREE_H
#define AABBTREE_H

#include <AABB.h>
#include <iostream>

template <typename T>
using vector = std::vector<T>;


struct Node
{
    Node(AABB* AABBData, Node* parent, Node* right, Node* left) 
    :
        Data(AABBData),
        Parent(parent),
        Right(right),
        Left(left)
        {
            
        }
    Node *Parent,*Right,*Left;
    bool isLeaf = false;
    AABB* Data;
    operator AABB&()
    {
        return *Data;
    }
    bool operator==(const Node& other) const
    {
        if(other.isLeaf == this-> isLeaf && other.Data == this->Data)
            return true;
        else
            return false;

    }
    bool operator!=(const Node& other) const 
    {
        
        if(other.isLeaf == this-> isLeaf && other.Data == this->Data)
            return false;
        else
            return true;
    }
    void operator=(const Node& other)
    {
        this->Data = other.Data;
        this->Parent = other.Parent;
        this->Right = other.Right;
        this->Left = other.Left;
        this->isLeaf = other.isLeaf;
    }
};

class Tree{
    Node root;
    vector<Node> treeNodes;
    public:
        static Node nullNode;
        Tree(Node&& Root) :
        treeNodes(vector<Node>()),
        root(std::forward<Node>(Root))
        {}

    void InsertNode(Node&& newNode);
    void InsertNodeUtil(Node* insertingNode,Node* IntoNode,Node&& insertedNode);
    //bool IntersectsWith();
};

#endif