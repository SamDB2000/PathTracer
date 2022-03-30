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

    // No use in finding place of hit since only used for BVH's
    bool intersect (glm::vec3 rayPos, glm::vec3 rayDir);
};

}  // namespace path_tracer