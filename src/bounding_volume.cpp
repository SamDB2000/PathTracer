#include <bounding_volume.h>
#include <algorithm>

namespace path_tracer {

BoundingVolume::BoundingVolume(Triangle* tri)
    : _isRoot(true), _isLeaf(true), _children{ nullptr, nullptr }, _tri(tri) {
    _min.x = std::min({ tri->v0.x, tri->v1.x, tri->v2.x });
    _min.y = std::min({ tri->v0.y, tri->v1.y, tri->v2.y });
    _min.z = std::min({ tri->v0.z, tri->v1.z, tri->v2.z });
    _max.x = std::max({ tri->v0.x, tri->v1.x, tri->v2.x });
    _max.y = std::max({ tri->v0.y, tri->v1.y, tri->v2.y });
    _max.z = std::max({ tri->v0.z, tri->v1.z, tri->v2.z });
}

BoundingVolume::BoundingVolume(std::unique_ptr<BoundingVolume> bv0,
                               std::unique_ptr<BoundingVolume> bv1)
    : _isRoot(true), _isLeaf(false), _children{ std::move(bv0), std::move(bv1) }, _tri{ nullptr } {
    _children[0]->_isRoot = false;
    _children[1]->_isRoot = false;
}

float BoundingVolume::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos,
                              glm::vec3& normal) {
    // TODO
    return 0.0f;
}

}  // namespace path_tracer
