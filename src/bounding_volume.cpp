#include <bounding_volume.h>
#include <algorithm>

namespace path_tracer {

BoundingVolume::Pair::Pair(const SharedPtr& bv0, const SharedPtr& bv1) : bv0(bv0), bv1(bv1) {
    volume = (std::max((*bv0)->_aabb.bounds[1].x, (*bv1)->_aabb.bounds[1].x) -
              std::min((*bv0)->_aabb.bounds[0].x, (*bv1)->_aabb.bounds[0].x)) *
             (std::max((*bv0)->_aabb.bounds[1].y, (*bv1)->_aabb.bounds[1].y) -
              std::min((*bv0)->_aabb.bounds[0].y, (*bv1)->_aabb.bounds[0].y)) *
             (std::max((*bv0)->_aabb.bounds[1].z, (*bv1)->_aabb.bounds[1].z) -
              std::min((*bv0)->_aabb.bounds[0].z, (*bv1)->_aabb.bounds[0].z));
}

BoundingVolume::Ptr BoundingVolume::Pair::join() {
    BoundingVolume::Ptr ptr = std::make_unique<BoundingVolume>(std::move(*bv0), std::move(*bv1));
    return ptr;
}

BoundingVolume::BoundingVolume(Triangle* tri)
    : _isRoot(true), _isLeaf(true), _children{ nullptr, nullptr }, _tri(tri) {
    _aabb.bounds[0].x = std::min({ tri->v0.x, tri->v1.x, tri->v2.x });
    _aabb.bounds[0].y = std::min({ tri->v0.y, tri->v1.y, tri->v2.y });
    _aabb.bounds[0].z = std::min({ tri->v0.z, tri->v1.z, tri->v2.z });
    _aabb.bounds[1].x = std::max({ tri->v0.x, tri->v1.x, tri->v2.x });
    _aabb.bounds[1].y = std::max({ tri->v0.y, tri->v1.y, tri->v2.y });
    _aabb.bounds[1].z = std::max({ tri->v0.z, tri->v1.z, tri->v2.z });
}

BoundingVolume::BoundingVolume(Ptr bv0, Ptr bv1)
    : _isRoot(true), _isLeaf(false), _children{ std::move(bv0), std::move(bv1) }, _tri{ nullptr } {
    _children[0]->_isRoot = false;
    _children[1]->_isRoot = false;
    _aabb.bounds[0].x = std::min(_children[0]->_aabb.bounds[0].x, _children[1]->_aabb.bounds[0].x);
    _aabb.bounds[0].y = std::min(_children[0]->_aabb.bounds[0].y, _children[1]->_aabb.bounds[0].y);
    _aabb.bounds[0].z = std::min(_children[0]->_aabb.bounds[0].z, _children[1]->_aabb.bounds[0].z);
    _aabb.bounds[1].x = std::max(_children[0]->_aabb.bounds[1].x, _children[1]->_aabb.bounds[1].x);
    _aabb.bounds[1].y = std::max(_children[0]->_aabb.bounds[1].y, _children[1]->_aabb.bounds[1].y);
    _aabb.bounds[1].z = std::max(_children[0]->_aabb.bounds[1].z, _children[1]->_aabb.bounds[1].z);
}

BoundingVolume::SharedPtr BoundingVolume::makeShared(Triangle* tri) {
    return std::make_shared<BoundingVolume::Ptr>(std::make_unique<BoundingVolume>(tri));
}

BoundingVolume::SharedPtr BoundingVolume::makeShared(Ptr bv0, Ptr bv1) {
    return std::make_shared<BoundingVolume::Ptr>(
        std::make_unique<BoundingVolume>(std::move(bv0), std::move(bv1)));
}

float BoundingVolume::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos,
                              glm::vec3& normal, Material& outMat) {
    if (!_aabb.intersect(rayPos, rayDir))
        return 0.0f;
    if (_isLeaf) {
        glm::vec3 hit;
        glm::vec3 norm;
        float dist = _tri->raycast(rayPos, rayDir, hit, norm);
        if (dist > 0.0f) {
            hitPos = hit;
            normal = norm;
            outMat = _tri->m;
            return dist;
        }
        return 0.0f;
    } else {
        glm::vec3 hit0;
        glm::vec3 norm0;
        Material mat0;
        float dist0 = _children[0]->raycast(rayPos, rayDir, hit0, norm0, mat0);

        glm::vec3 hit1;
        glm::vec3 norm1;
        Material mat1;
        float dist1 = _children[1]->raycast(rayPos, rayDir, hit1, norm1, mat1);

        if ((dist0 > 0.0 && dist1 > 0.0 && dist0 < dist1) || (dist0 > 0.0 && dist1 <= 0.0)) {
            hitPos = hit0;
            normal = norm0;
            outMat = mat0;
            return dist0;
        } else if ((dist0 > 0.0 && dist1 > 0.0 && dist1 < dist0) ||
                   (dist1 > 0.0f && dist0 <= 0.0)) {
            hitPos = hit1;
            normal = norm1;
            outMat = mat1;
            return dist1;
        }
    }
    return 0.0f;
}

pugi::xml_node BoundingVolume::toXml(pugi::xml_node& root) {
    pugi::xml_node node = root.append_child("node");

    node.append_attribute("min_x") = _aabb.bounds[0].x;
    node.append_attribute("min_y") = _aabb.bounds[0].y;
    node.append_attribute("min_z") = _aabb.bounds[0].z;
    node.append_attribute("max_x") = _aabb.bounds[1].x;
    node.append_attribute("max_y") = _aabb.bounds[1].y;
    node.append_attribute("max_z") = _aabb.bounds[1].z;

    if (_isLeaf) {
        pugi::xml_node tri = node.append_child("tri");
    } else {
        _children[0]->toXml(node);
        _children[1]->toXml(node);
    }

    return node;
}

}  // namespace path_tracer
