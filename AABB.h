#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
using vec3 = glm::vec3;
using vec4 = glm::vec4;

class AABB
{
    vec3 Min;
    vec3 Max;
    AABB(vec3&& min, vec3&& max)
    {
        Min = std::forward<vec3>(min);
        Max = std::forward<vec3>(max);
    }
    

};


#endif /* C37E1256_9ACF_49D0_87E6_A4AE20C6FB73 */
