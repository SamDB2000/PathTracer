#ifndef __MESH_H__
#define __MESH_H__

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <material.h>
#include <triangle.h>

namespace dng {

struct Mesh {
    Material m;
    std::vector<Triangle> tris;

    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);
    void loadStl(std::string filename);
};

}  // namespace dng

#endif  //  __MESH_H__
