#ifndef __BOUNDING_VOLUME_H__
#define __BOUNDING_VOLUME_H__

#include <glm/glm.hpp>
#include <memory>
#include <triangle.h>

namespace path_tracer {

class BoundingVolume {
public:
    BoundingVolume(Triangle* tri);
    BoundingVolume(std::unique_ptr<BoundingVolume> bv0, std::unique_ptr<BoundingVolume> bv1);

    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);

private:
    glm::vec3 _min;
    glm::vec3 _max;
    bool _isRoot;
    bool _isLeaf;
    std::unique_ptr<BoundingVolume> _children[2];
    Triangle* _tri;
};

}  // namespace path_tracer

#endif  //  __BOUNDING_VOLUME_H__
