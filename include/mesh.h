#ifndef __MESH_H__
#define __MESH_H__

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <material.h>
#include <triangle.h>
#include <bounding_volume.h>

namespace path_tracer {

class Mesh {
public:
    Material m;
    std::vector<Triangle> tris;

    Mesh(const std::string& filename = "");
    Mesh(Material m, const std::vector<Triangle>& tris);
    Mesh(const Mesh& mesh);
    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);
    void loadStl(const std::string& filename);
    void generateBVH();

private:
    BoundingVolume::Ptr _bvh;
};

}  // namespace path_tracer

#endif  //  __MESH_H__
