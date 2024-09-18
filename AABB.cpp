#include <AABB.h>



AABB::AABB(vec3&& min, vec3&& max)
{
    Min = std::forward<vec3>(min);
    Max = std::forward<vec3>(max);
}
AABB::AABB(vec3 min,vec3 max)
{
    Min = min;
    Max = max;
}
AABB::AABB(vec3 origin, float Radius)
{
    static float Halfspace = Radius/2; 
    static vec3 Halfspace3f = vec3(Halfspace,Halfspace,Halfspace);
    Min = origin - Halfspace3f;
    Max = origin + Halfspace3f;
}


//------------Need rewriting ----------
void AABB::operator+=(AABB& other)
{
    this->Min = glm::min(this->Min, other.Min);
    this->Max = glm::max(this->Max, other.Max);
}

void AABB::operator-=(AABB& other)
{
    this->Min = glm::min(this->Min, other.Min);
    this->Max = glm::max(this->Max, other.Max);
}
