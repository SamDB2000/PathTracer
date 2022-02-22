#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <glm/glm.hpp>

namespace dng {

struct Material {
    glm::vec3 diff{ 1.0f, 1.0f, 1.0f };
    glm::vec3 spec{ 0.55f, 0.55f, 0.55f };
    float shininess{ .25f };
    glm::vec3 amb{ 0.0f, 0.0f, 0.0f };
};

}  // namespace dng

#endif  //  __MATERIAL_H__
