#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <glm/glm.hpp>

namespace dng {

struct Light {
    glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
    glm::vec3 diff{ 1.0f, 1.0f, 1.0f };
    glm::vec3 spec{ 0.55f, 0.55f, 0.55f };
};

}  // namespace dng

#endif  //  __LIGHT_H__
