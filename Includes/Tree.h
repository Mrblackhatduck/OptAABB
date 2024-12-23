#ifndef TREE_H
#define TREE_H



//#include <Types.h>
//#include <vector>

#define FRWRD(Type, DATA) std::forward<Type>(DATA)

#include <iostream>
//#include <vector>
//#include <memory>
#include <limits>
#include <algorithm>
#include <queue>
#include <Types.h>

struct AABB {
    vec3 Min;
    vec3 Max;

    AABB() : Min(vec3(std::numeric_limits<float>::max())), Max(vec3(std::numeric_limits<float>::lowest())) {}
    AABB(const vec3& min, const vec3& max) : Min(min), Max(max) {}

    bool intersects(const AABB& other) const {
        return (Min.x <= other.Max.x && Max.x >= other.Min.x) &&
            (Min.y <= other.Max.y && Max.y >= other.Min.y) &&
            (Min.z <= other.Max.z && Max.z >= other.Min.z);
    }

    AABB merge(const AABB& other) const {
        return AABB(
            vec3(std::min(Min.x, other.Min.x), std::min(Min.y, other.Min.y), std::min(Min.z, other.Min.z)),
            vec3(std::max(Max.x, other.Max.x), std::max(Max.y, other.Max.y), std::max(Max.z, other.Max.z))
        );
    }

    vec3 center() const {
        return (Min + Max) * 0.5f;
    }
    float GetWidth() const
    {
        return Max.x - Min.x;
    }

    float GetHeight() const
    {
        return Max.y - Min.y;
    }

    float GetDepth() const
    {
        return Max.z - Min.z;
    }

    float SurfaceArea() const
    {
        return 2.0f * (GetWidth() * GetHeight() + GetWidth() * GetDepth() + GetHeight() * GetDepth());
    }
    AABB operator+(AABB& other)
    {
        return merge(other);
    }
};
#define NULL_NODE -1

struct Node {
    AABB bounds;
    int left, right, parent;
    bool isLeaf;
    Node() :left(NULL_NODE), right(NULL_NODE), parent(NULL_NODE), isLeaf(false)
    {
    }
};

struct CostMapping {
    float cost; int index;
    CostMapping(float Cost, int Index):cost(Cost),index(Index) {}
    bool operator >(CostMapping& rhs) const
    {
        if (rhs.cost < cost)
            return true;
        else
            return false;
    }
    bool operator <(CostMapping& rhs) const
    {
        if (rhs.cost > cost)
            return true;
        else
            return false;
    }
};
struct Tree
{
    //node traversal costs
    //std::priority_queue<CostMapping,vector<CostMapping>,std::greater<CostMapping>> costs;
    
    vector<Node> nodes;
    int root = NULL_NODE;
    void InsertNode(AABB&& bounds) 
    {
        Node node;
        node.bounds = bounds;
        InsertNode(node);
    }
    void InsertNode(Node node)
    {
        if (root == NULL_NODE)
        {
            root = 0;
            nodes.push_back(node);
            return;
        }
        int currentNode = 0;
        Node& current = nodes[0];
        CostMapping bestSibling = CostMapping(std::numeric_limits<float>::max(), 0);
        // as long we didnt hit a leaf node 
        while (currentNode != NULL_NODE)
        {
            current = nodes[currentNode];
            if (current.left == NULL_NODE && current.right == NULL_NODE)
                break;
            float directCostL = (node.bounds + nodes[current.left].bounds).SurfaceArea();
            float directCostR = (node.bounds + nodes[current.right].bounds).SurfaceArea();
            float inheritCostR = CostRefitted(current.right,node.bounds);
            float inheritCostL = CostRefitted(current.left,node.bounds);
                if (directCostL + inheritCostL < directCostR + inheritCostR)
                {
                    currentNode = current.left;
                    bestSibling.cost = directCostL + inheritCostL;
                    bestSibling.index = current.left;
                }
                else 
                {
                    currentNode = current.right;
                    bestSibling.cost = directCostR + inheritCostR;
                    bestSibling.index = current.right;
                }
        }

        std::cout << "sus" << "\n";
        /// TO BE DONE : ADD the node to the best sibling
    }
    float CostRefitted(int index,AABB& bounds) 
    {
        AABB currentBound = bounds + nodes[index].bounds;
        while (index > 0)
        {
            currentBound = currentBound + nodes[index].bounds;
            index = nodes[index].parent;
        }
        currentBound = currentBound + nodes[0].bounds;
        return currentBound.SurfaceArea();
    }
    float Cost(Tree tree)
    {
        float cost = 0;
        for (size_t i = 0; i < tree.nodes.size(); i++)
        {
            if (!tree.nodes[i].isLeaf)
                cost += tree.nodes[i].bounds.SurfaceArea();
        }
        return cost;
    }
    float Cost(int index)
    {
        if (index == NULL_NODE || nodes[index].isLeaf)
        {
            return 0;
        }
        else
        {
            return
                nodes[index].bounds.SurfaceArea() +
                Cost(nodes[index].left) +
                Cost(nodes[index].right);
        }

    }
    float CostInherited(int index)
    {
        if (index <= -1)
            return 0;
        else
        {
            return nodes[index].bounds.SurfaceArea() + CostInherited(nodes[index].parent);
        }

    }
};


#endif