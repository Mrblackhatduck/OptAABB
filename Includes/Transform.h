#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <Types.h>
class Transform{
    public:
        static inline mat4 createModelMatrix(vec3 translation, vec3 rotation, vec3 scale) 
        {
            // Start with an identity matrix
            mat4 model = glm::mat4(1.0f);

            // Apply translation
            model = glm::translate(model, translation);

            // Apply rotation: Rotate around the x, y, and z axes
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X-axis rotation
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y-axis rotation
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z-axis rotation

            // Apply scaling
            model = glm::scale(model, scale);

            return model;
        }
};

#endif