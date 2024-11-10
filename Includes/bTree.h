#pragma once
#include <Types.h>
#include <deque>
enum class Color
{
	BLACK,
	RED
};

struct TNode 
{
	Color Color;
	int Value;
	TNode *Left, *Right, *Parent;
    int Index = -1;
	TNode() 
	{
		Left = Right = Parent = nullptr;
		Value = 0;
		Color = Color::RED;
	}
	TNode(TNode* parent, TNode* left, TNode* right):
		Left(left),
		Parent(parent),
		Right(right),
		Value(0),
		Color(Color::RED)
	{}
	TNode(int value, TNode* parent = nullptr, TNode* left = nullptr, TNode* right = nullptr):
		Value(value),
		Left(left),
		Parent(parent),
		Right(right),
		Color(Color::RED)
	{}
};



/// Rules of black red tree's 
//  1 - new nodes are RED
//  2 - root always BLACK
//	3 - no two consecutive RED nodes, if so :
//		1 - check the aunt's node (gradfather's other child) color
//			if(color is black)
//				rotate and switch colors (if right left rotate right left, left left -> right rotate, right right -> left rotate)
//			else
//				switch colors (grandfather switch to red and children switch accordingly)
//

struct TTree
{
    std::vector<TNode> Nodes;  
    TNode* Root = nullptr;

    // pushback empty -> then access last element and forward the rvalue inside

    void PushNode(TNode&& node)
    {
        
        //int newNode = Nodes.size()-1;
        node.Index = Nodes.size();
        if (Root == nullptr)
        {
            //node.Index = Nodes.size();
            Nodes.push_back(node);
            Root = &(Nodes[Nodes.size() - 1]);
            Root->Color = Color::BLACK;
            return;
        }

        TNode* current = &(Nodes.front());
        //TNode* Added = 
        while (current != nullptr)
        {
            if (current->Value > current->Value)
            {
                if (current->Right == nullptr)
                {
                    node.Parent = current;
                    Nodes[node.Index].Parent->Right = &node;
                    break;
                }
                else
                    current = current->Right;
            }
            else
            {
                if (current->Left == nullptr)
                {
                    //current->Left = &(Nodes[Nodes.size()-1]);
                    node.Parent = current;
                    Nodes[node.Index].Parent->Left = &node;
                    break;
                }
                else
                    current = current->Left;
            }
        }
        
        Nodes.push_back(std::move(node));
        (& node)->Color = Color::BLACK;
    }
};
