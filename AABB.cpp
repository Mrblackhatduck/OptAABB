#include <Utils.h>
#include <AABB.h>

AABB AABB::zero = AABB( vec3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX), vec3(FLOAT_MIN,FLOAT_MIN,FLOAT_MIN)); 
AABB::AABB(vec3&& min, vec3&& max)
{
    Min = std::forward<vec3>(min);
    Max = std::forward<vec3>(max);
}

AABB::AABB(vec3 origin, float Radius)
{
    static float Halfspace = Radius/2; 
    static vec3 Halfspace3f = vec3(Halfspace,Halfspace,Halfspace);
    Min = origin - Halfspace3f;
    Max = origin + Halfspace3f;
}

bool AABB::Intersects(AABB& other)
{
     return 
      this->Min.x <= other.Max.x && this->Max.x >= other.Min.x 
   && this->Min.y <= other.Max.y && this->Max.y >= other.Min.y 
   && this->Min.z <= other.Max.z && this->Max.z >= other.Min.z;
}
//------------Need rewriting ----------
void AABB::operator+=(AABB& other)
{
    this->Min = glm::min(this->Min, other.Min);
    this->Max = glm::max(this->Max, other.Max);
}

AABB AABB::Union(AABB& first,AABB& second)
{
return 
    AABB
  (
    vec3
    (
      glm::min(first.Min.x, second.Min.x), 
      glm::min(first.Min.y, second.Min.y), 
      glm::min(first.Min.z, second.Min.z)
    ), 
    vec3
    (
      glm::max(first.Max.x, second.Max.x), 
      glm::max(first.Max.y, second.Max.y), 
      glm::max(first.Max.z, second.Max.z)
    )
  );


}
// void AABB::operator-=(AABB& other)
// {
//     this->Min = glm::min(this->Min, other.Min);
//     this->Max = glm::max(this->Max, other.Max);
// }
