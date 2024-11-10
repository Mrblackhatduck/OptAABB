#ifndef TREE_H
#define TREE_H

#include <AABB.h>
#include <stack>
//#include <Types.h>
//#include <vector>

#define FRWRD(Type, DATA) std::forward<Type>(DATA)



template<typename t>
using stack = std::stack<t>; 



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
    Node(AABB* AABBdata)
    :
        Data(AABBdata),
        Parent(nullptr),
        Right(nullptr),
        Left(nullptr)
    {}
    Node()
    :
        Data(nullptr),
        Parent(nullptr),
        Right(nullptr),
        Left(nullptr)
    {}
    
    Node *Parent,*Right,*Left;
    bool isLeaf = false;
    AABB* Data;
    
    operator const AABB&()
    const {
        return *Data;
    }
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
    operator const vec3() const
    {
        return (Data->Max + Data->Min)/2.0f ;
    }
};

class Tree{
    Node root;
    public:
        vector<Node> treeNodes;
        static Node nullNode;
        Tree(Node&& Root) :
        treeNodes(vector<Node>()),
        root(std::forward<Node>(Root))
        {}
        Tree():root(nullNode),
        treeNodes(vector<Node>())
        {}
    const Node GetRoot() const{ return root;}
    void InsertNode(Node&& newNode);
    void InsertNodeUtil(Node* insertingNode,Node* IntoNode,Node&& insertedNode);
    bool RayCast(const vec3 start, const vec3 direction,float maxDistance = 500.0f);
    Node* PickBestSibling(Node* inserted );
    void BuildTree(vector<AABB>& worldObjects);
    //bool IntersectsWith();
};

#endif