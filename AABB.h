#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
using vec3 = glm::vec3;
using vec4 = glm::vec4;

class AABB
{
    vec3 Min;
    vec3 Max;
    AABB(vec3&& min, vec3&& max);
    AABB(vec3 min,vec3 max);
    AABB(vec3 origin, float Radius);
    void operator +=(AABB& other);
    void operator -=(AABB& other);
};


#endif
