#ifndef TYPES_H
#define TYPES_H

#include <glm/glm.hpp>
#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/mat4x4.hpp>

#include <functional>

using vec3 = glm::vec3;
using quat = glm::quat;
using mat4 = glm::mat4;

using uint = unsigned int;

template <typename t>
using vector = std::vector<t>;

template <typename tp>
using function = std::function<tp>;
#endif