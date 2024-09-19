#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
using vec3 = glm::vec3;
using vec4 = glm::vec4;

class AABB
{

    vec3 Min;
    vec3 Max;
    public:
    static AABB zero;
    AABB(vec3&& min, vec3&& max);
    //AABB(vec3 min,vec3 max);
    AABB(vec3 origin, float Radius);
    bool Intersects(AABB& other);
    void operator +=(AABB& other);
    static AABB Union(AABB& first,AABB& second);
    void Include(vec3& point);

    
    //void operator -=(AABB& other);
};


#endif
