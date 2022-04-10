#pragma once

#include <glm/glm.hpp>
#include <utility>

namespace path_tracer {

struct AABB {
    // Just for the min/max bounds
    glm::vec3 bounds[2];

    AABB() {
        bounds[0] = glm::vec3();
        bounds[1] = glm::vec3();
    }
    AABB(const glm::vec3& vmin, const glm::vec3& vmax) {
        bounds[0] = vmin;
        bounds[1] = vmax;
    }
    AABB(const AABB& aabb0, const AABB& aabb1) {
        bounds[0].x = std::min(aabb0.bounds[0].x, aabb1.bounds[0].x);
        bounds[0].y = std::min(aabb0.bounds[0].y, aabb1.bounds[0].y);
        bounds[0].z = std::min(aabb0.bounds[0].z, aabb1.bounds[0].z);
        bounds[1].x = std::max(aabb0.bounds[1].x, aabb1.bounds[1].x);
        bounds[1].y = std::max(aabb0.bounds[1].y, aabb1.bounds[1].y);
        bounds[1].z = std::max(aabb0.bounds[1].z, aabb1.bounds[1].z);
    }

    // No use in finding place of hit since only used for BVH's
    bool intersect(glm::vec3 rayPos, glm::vec3 rayDir);

    float volume() {
        return (bounds[1].x - bounds[0].x) * (bounds[1].y - bounds[0].y) *
               (bounds[1].z - bounds[0].z);
    }
};

}  // namespace path_tracer