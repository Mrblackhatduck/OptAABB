#include <Tree.h>



Node Tree::nullNode = Node(nullptr,nullptr,nullptr,nullptr);
void Tree::InsertNode(Node&& newNode)
{

    if(root == Tree::nullNode)
        {
            root = newNode;
            return;
        }
    if(root.Left == nullptr)
        {
            root.Left = &newNode;
            treeNodes.push_back(newNode);
            return; 
        }
    if(root.Right == nullptr)
        {
            root.Right = &newNode;
            treeNodes.push_back(newNode);
            return;
        }

    if(((AABB&)(root.Left)).Intersects(newNode))
        {
            InsertNodeUtil(&root,root.Left,std::forward<Node>(newNode));
        }
    if(((AABB&)(root.Right)).Intersects(newNode))
        {
            InsertNodeUtil(&root,root.Right,std::forward<Node>(newNode));
        }
}

void Tree::InsertNodeUtil(Node* insertingNode,Node* insertInto,Node&& insertedNode)
{
    if(insertInto == nullptr)
    

}