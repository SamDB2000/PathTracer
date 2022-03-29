#include <mesh.h>
#include <fstream>
#include <list>

namespace path_tracer {

Mesh::Mesh(const std::string& filename) : _bvh(nullptr) {
    loadStl(filename);
}

Mesh::Mesh(Material m, const std::vector<Triangle>& tris) : m(m), tris(tris), _bvh(nullptr) {
}

Mesh::Mesh(const Mesh& mesh)
    : m(mesh.m), tris(mesh.tris), _bvh(nullptr) {
}

float Mesh::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    float minDist = std::numeric_limits<float>::infinity();
    bool hit = false;

    // TODO: Replace with BVH walk
    for (Triangle& tri : tris) {
        glm::vec3 localHitPos;
        glm::vec3 localNormal;
        float dist = tri.raycast(rayPos, rayDir, localHitPos, localNormal);
        if (dist > 0.0f && dist < minDist) {
            minDist = dist;
            hitPos = localHitPos;
            normal = localNormal;
            hit = true;
        }
    }
    if (hit)
        return minDist;
    else
        return 0.0f;
}

void Mesh::loadStl(const std::string& filename) {
    if (filename.empty())
        return;

    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    char header[80];
    ifs.read(header, sizeof(header));
    uint32_t numTris;
    ifs.read((char*) (&numTris), sizeof(uint32_t));

    tris.clear();
    tris.reserve(numTris);
    for (int i = 0; i < numTris; i++) {
        glm::vec3 normal;
        Triangle t;
        uint16_t attrib;
        ifs.read((char*) (&normal), sizeof(glm::vec3));
        ifs.read((char*) (&t.v0), sizeof(glm::vec3));
        ifs.read((char*) (&t.v1), sizeof(glm::vec3));
        ifs.read((char*) (&t.v2), sizeof(glm::vec3));
        ifs.read((char*) (&attrib), sizeof(attrib));
        tris.push_back(t);
    }
}

void Mesh::generateBVH() {
    if (_bvh)
        return;
    std::list<BoundingVolume::Ptr> bvs;
    for (auto& tri : tris)
        bvs.push_back(std::make_unique<BoundingVolume>(&tri));
    while (bvs.size() > 1) {
        BoundingVolume::Ptr bv0 = std::move(bvs.front());
        bvs.pop_front();
        BoundingVolume::Ptr bv1 = std::move(bvs.front());
        bvs.pop_front();
        bvs.push_back(std::make_unique<BoundingVolume>(std::move(bv0), std::move(bv1)));
    }
    _bvh = std::move(bvs.front());
    bvs.pop_front();
}

}  // namespace path_tracer
