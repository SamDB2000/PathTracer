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
    std::string filename = "";

    Mesh(const std::string& filename = "");
    Mesh(Material m, const std::vector<Triangle>& tris);
    Mesh(const Mesh& mesh);
    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);
    void loadStl(const std::string& filename);
    pugi::xml_node toXml(pugi::xml_node& root);
    static Mesh fromXml(pugi::xml_node node);
};

}  // namespace path_tracer

#endif  //  __MESH_H__
