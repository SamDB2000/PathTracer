#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <glm/glm.hpp>
#include <material.h>

namespace path_tracer {

struct Triangle {
    Material m;
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;

    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);
    pugi::xml_node toXml(pugi::xml_node& root);
    static Triangle fromXml(pugi::xml_node node);
};

}  // namespace path_tracer

#endif  //  __TRIANGLE_H__
