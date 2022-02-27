#include <mesh.h>

namespace dng {

float Mesh::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    float minDist = std::numeric_limits<float>::infinity();
    bool hit = false;
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

}  // namespace dng
