#include <mesh.h>
#include <fstream>
#include <iostream>
#include <atomic>

namespace path_tracer {

extern uint64_t numRayTriangleTests;
extern uint64_t numRayTriangleIntersections;

float Mesh::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    float minDist = std::numeric_limits<float>::infinity();
    bool hit = false;

    numRayTriangleTests++;
    if (AABbox.intersect(rayPos, rayDir)) {
        numRayTriangleIntersections++;
        // This seems to infer it works since that prints very often.
        //std::cout << "intersection found in AABB\n";
        
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
    } else
        return 0.0f;
}

void Mesh::loadStl(std::string filename) {
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    char header[80];
    ifs.read(header, sizeof(header));
    uint32_t numTris;
    ifs.read((char*) (&numTris), sizeof(uint32_t));

    tris.clear();
    tris.reserve(numTris);

    glm::vec3 min_bound = glm::vec3();
    glm::vec3 max_bound = glm::vec3();

    //TODO: Find a better way of finding the min and max coordinates of the mesh
    // I don't know a better way of doing this at the moment, but this is gross
    for (int i = 0; i < numTris; i++) {
        glm::vec3 normal;
        Triangle t;
        uint16_t attrib;
        ifs.read((char*) (&normal), sizeof(glm::vec3));
        ifs.read((char*) (&t.v0), sizeof(glm::vec3));
        // Find min/max x,y,z coordinates for v0
        if (t.v0.x < min_bound.x) min_bound.x = t.v0.x;
        if (t.v0.y < min_bound.y) min_bound.y = t.v0.y;
        if (t.v0.z < min_bound.z) min_bound.z = t.v0.z;
        if (t.v0.x > max_bound.x) max_bound.x = t.v0.x;
        if (t.v0.y > max_bound.y) max_bound.y = t.v0.y;
        if (t.v0.z > max_bound.z) max_bound.z = t.v0.z;
        ifs.read((char*) (&t.v1), sizeof(glm::vec3));
        if (t.v1.x < min_bound.x) min_bound.x = t.v1.x;
        if (t.v1.y < min_bound.y) min_bound.y = t.v1.y;
        if (t.v1.z < min_bound.z) min_bound.z = t.v1.z;
        if (t.v1.x > max_bound.x) max_bound.x = t.v1.x;
        if (t.v1.y > max_bound.y) max_bound.y = t.v1.y;
        if (t.v1.z > max_bound.z) max_bound.z = t.v1.z;
        ifs.read((char*) (&t.v2), sizeof(glm::vec3));
        if (t.v2.x < min_bound.x) min_bound.x = t.v2.x;
        if (t.v2.y < min_bound.y) min_bound.y = t.v2.y;
        if (t.v2.z < min_bound.z) min_bound.z = t.v2.z;
        if (t.v2.x > max_bound.x) max_bound.x = t.v2.x;
        if (t.v2.y > max_bound.y) max_bound.y = t.v2.y;
        if (t.v2.z > max_bound.z) max_bound.z = t.v2.z;
        
        ifs.read((char*) (&attrib), sizeof(attrib));
        AABbox.bounds[0] = min_bound;
        AABbox.bounds[1] = max_bound;
        tris.push_back(t);
    }
}

}  // namespace path_tracer
