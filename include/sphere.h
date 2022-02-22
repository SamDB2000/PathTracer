#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <glm/glm.hpp>
#include <material.h>

namespace dng {

struct Sphere {
    Material& m;
    glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
    float radius = 1.0f;
};

}  // namespace dng

#endif  //  __SPHERE_H__
