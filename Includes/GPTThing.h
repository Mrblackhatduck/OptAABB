#ifndef GPT_TEST
#define GPT_TEST
#include <vector>
#include <limits>
#include <iostream>
#include <string>
//#include <algorithm>
#include "AABB.h" // Assuming AABB is already defined with Min and Max (vec3) properties

using uint = unsigned int;
struct Object {
    AABB bounds;
    std::string name;
    // Other object data...
};

class BVHNode {
   public:
    static BVHNode nullnode;
    AABB bounds;
    BVHNode* left;
    BVHNode* right;
    Object* object; // Only for leaf nodes

    BVHNode() : left(nullptr), right(nullptr), object(nullptr) {}
};


// Compute the surface area of an AABB
float surfaceArea(const AABB& box) {
    vec3 size = box.Max - box.Min;
    return 2.0f * (size.x * size.y + size.y * size.z + size.z * size.x);
}

// Surface Area Heuristic to find the best split
float SAH(const AABB& left, const AABB& right, float traversalCost) {
    float surfaceLeft = surfaceArea(left);
    float surfaceRight = surfaceArea(right);
    float surfaceParent = surfaceArea(AABB::Union(left, right)); // Combine two AABBs

    return traversalCost + (surfaceLeft / surfaceParent) * surfaceLeft + (surfaceRight / surfaceParent) * surfaceRight;
}

// BVH partitioning
/*BVHNode* buildBVH(std::vector<Object>& objects, int start, int end) {
    BVHNode* node = new BVHNode();

    // Base case: single object
    if (start == end - 1) {
        node->object = &objects[start];
        node->bounds = objects[start].bounds;
        return node;
    }

    // Compute the bounding box for the current node
    AABB bounds;
    for (int i = start; i < end; i++) {
        bounds = AABB::Union(bounds, objects[i].bounds);
    }
    node->bounds = bounds;

    // Find the best split using Surface Area Heuristic
    float bestCost = std::numeric_limits<float>::max();
    int bestAxis = -1;
    int bestSplit = start;

    for (int axis = 0; axis < 3; axis++) {
        std::sort(objects.begin() + start, objects.begin() + end, [axis](const Object& a, const Object& b) {
            return a.bounds.Min[axis] < b.bounds.Min[axis];
        });

        for (int i = start + 1; i < end; i++) {
            AABB leftBox, rightBox;
            for (int j = start; j < i; j++) leftBox = AABB::Union(leftBox, objects[j].bounds);
            for (int j = i; j < end; j++) rightBox = AABB::Union(rightBox, objects[j].bounds);

            float cost = SAH(leftBox, rightBox, 1.0f); // Assuming traversal cost is 1.0f
            if (cost < bestCost) {
                bestCost = cost;
                bestSplit = i;
                bestAxis = axis;
            }
        }
    }

    // Recursively build children nodes
    node->left = buildBVH(objects, start, bestSplit);
    node->right = buildBVH(objects, bestSplit, end);

    return node;
}
*/

BVHNode RootNode;
void CreateTree()
{
    //if(RootNode )
} 
int CreateTreeUtil()
{
    return 0;
}
#endif
