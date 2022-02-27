#include <sphere.h>

namespace dng {

float Sphere::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    glm::vec3 v = pos - rayPos;
    float t0 = glm::dot(v, rayDir);
    if (t0 < 0)
        return 0;
    float dSqr = glm::dot(v, v) - t0 * t0;
    float rSqr = radius * radius;
    if (dSqr >= rSqr)
        return 0;
    float td = std::sqrt(rSqr - dSqr);
    float t1 = t0 + td;
    float t2 = t0 - td;
    float t;
    if (t1 > 0.0001f && t2 > 0.0001f)
        t = (t1 < t2) ? t1 : t2;
    else if (t1 > 0.0001f && t2 <= 0.0001f)
        t = t1;
    else if (t2 <= 0.0001f && t2 > 0.0001f)
        t = t2;
    else
        return 0;

    hitPos = rayPos + rayDir * t;
    normal = glm::normalize(hitPos - pos);
    return t;
}

}  // namespace dng
