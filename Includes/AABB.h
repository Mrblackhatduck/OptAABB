#ifndef AABB_H
#define AABB_H

#include <Types.h>

class AABB
{


    public:
        vec3 Min;
        vec3 Max;
        float Area;
        static AABB zero;
        AABB();
        AABB(vec3&& min, vec3&& max);
        AABB(vec3 origin, float Radius);
        bool Intersects(AABB& other);
        void operator +=(AABB& other);
        static AABB Union(const AABB& first,const AABB& second);
        void Include(vec3& point);

    
    //void operator -=(AABB& other);
};


#endif
