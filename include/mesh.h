#ifndef __MESH_H__
#define __MESH_H__

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <material.h>
#include <triangle.h>
#include <AABB.h>

namespace path_tracer {

struct Mesh {
    Material m;
    //glm::vec3 bounds[2];
    AABB AABbox;
    std::vector<Triangle> tris;
    
   
    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);
    void loadStl(std::string filename);
};

}  // namespace path_tracer

#endif  //  __MESH_H__
