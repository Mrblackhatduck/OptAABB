#ifndef TYPES_H
#define TYPES_H

#include <glm/glm.hpp>
#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory.h>
#include <functional>
#include <string>

#define PRNTF(frmt,args) printf(frmt,args);

using string = std::string;
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using quat = glm::quat;
using mat4 = glm::mat4;

using uint = unsigned int;

template <typename t>
using vector = std::vector<t>;

template <typename tp>
using function = std::function<tp>;

#define VAL_PTR(x) glm::value_ptr(x)
//template <typename tp>
//using unique_ptr = std::unique_ptr<tp>;
////
//
//template <typename tp>
//using shared_ptr = std::shared_ptr<tp>;

#endif