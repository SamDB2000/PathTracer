#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <glm/glm.hpp>
#include <material.h>

namespace dng {

struct Triangle {
    Material& m;
    glm::vec3& v0;
    glm::vec3& v1;
    glm::vec3& v2;
    glm::vec3& n0;
    glm::vec3& n1;
    glm::vec3& n2;
};

}  // namespace dng

#endif  //  __TRIANGLE_H__
