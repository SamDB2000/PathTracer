#ifndef __SCENE_H__
#define __SCENE_H__

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <light.h>
#include <material.h>
#include <sphere.h>
#include <mesh.h>
#include <bounding_volume.h>

namespace path_tracer {

class Scene {
private:
    glm::vec3 _backgroundColor;
    unsigned int _maxDepth;
    unsigned int _antialias;
    unsigned int _width;
    unsigned int _height;
    float _fov;
    glm::vec3 _up;
    glm::vec3 _lookAt;
    glm::vec3 _eye;
    std::vector<Light> _lights;
    std::vector<Sphere> _spheres;
    std::vector<Mesh> _meshes;
    BoundingVolume::Ptr _bvh;

    void readVec(std::ifstream& ifs, glm::vec3& vec);
    void parseMatProp(std::ifstream& ifs, const std::string& str, Material& m);
    std::string parseLight(std::ifstream& ifs, pugi::xml_node& root);
    std::string parseSphere(std::ifstream& ifs, pugi::xml_node& root);
    std::string parseQuad(std::ifstream& ifs, pugi::xml_node& root);
    std::string parseModel(std::ifstream& ifs, pugi::xml_node& root);
    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal,
                  Material& mat);
    float raycast(glm::vec3 rayPos, glm::vec3 rayDir);
    glm::vec3 raytrace(glm::vec3 rayPos, glm::vec3 rayDir, int iter = 0);

public:
    Scene(const std::string& filename);
    void render(const std::string& filename);
    void generateBvh();
    void exportBvh(std::ostream& os);
    friend std::ostream& operator<<(std::ostream& os, Scene& s);
};

}  // namespace path_tracer

#endif  //  __SCENE_H__
