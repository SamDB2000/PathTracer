#include <triangle.h>

namespace dng {

float Triangle::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    glm::vec3 p = v1 - v0;
    glm::vec3 q = v2 - v0;
    glm::vec3 s = rayPos - v0;
    glm::vec3 tmp1 = glm::cross(rayDir, q);
    float dot1 = glm::dot(tmp1, p);
    if (-0.0001f < dot1 && dot1 < 0.0001f)
        return 0;
    float f = 1.0f / dot1;
    float u = f * glm::dot(s, tmp1);
    if (u < 0.0f || u > 1.0f)
        return 0;
    glm::vec3 tmp2 = glm::cross(s, p);
    float v = f * glm::dot(rayDir, tmp2);
    if (v < 0.0f || u + v > 1.0f)
        return 0;
    float t = f * glm::dot(q, tmp2);

    hitPos = rayPos + rayDir * t;
    normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    return t;
}

}  // namespace dng
