#include <Tree.h>



Node Tree::nullNode = Node(nullptr,nullptr,nullptr,nullptr);
void Tree::InsertNode(Node&& newNode)
{

    if(root == Tree::nullNode)
        {
            root = newNode;
            root.Parent = nullptr;
            return;
        }
    
    Node* CurrentNode = &root;
    while(CurrentNode != nullptr)
    {
        bool intersect_right, intersect_left;
        if(CurrentNode->Left != nullptr)
        {
            intersect_left = ((AABB&)*(CurrentNode->Left)).Intersects((AABB&)newNode);
        }
        if(CurrentNode->Right != nullptr)
        {
            intersect_right = ((AABB&)*(CurrentNode->Right)).Intersects((AABB&)newNode);
        }
        
        if(intersect_left && !intersect_right)
        {
            CurrentNode = CurrentNode->Left;
            continue;
        }
        else
            if(intersect_right && !intersect_left)
            {
                CurrentNode = CurrentNode->Right;
                continue;
            }
       if(intersect_left && intersect_right)
       {
        CurrentNode = 
            glm::distance((vec3)newNode,(vec3)*(CurrentNode->Left)) 
            <
            glm::distance((vec3)newNode,(vec3)*(CurrentNode->Right)) 
            ?
             CurrentNode->Left : CurrentNode->Right;
       }

    }
    
    
}

void Tree::InsertNodeUtil(Node* insertingNode,Node* insertInto,Node&& insertedNode)
{
    //if(insertInto == nullptr)
    

}