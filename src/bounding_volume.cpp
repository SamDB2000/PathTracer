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

BoundingVolume::BoundingVolume(Ptr bv0, Ptr bv1)
    : _isRoot(true), _isLeaf(false), _children{ std::move(bv0), std::move(bv1) }, _tri{ nullptr } {
    _children[0]->_isRoot = false;
    _children[1]->_isRoot = false;
    _min.x = std::min(_children[0]->_min.x, _children[1]->_min.x);
    _min.y = std::min(_children[0]->_min.y, _children[1]->_min.y);
    _min.z = std::min(_children[0]->_min.z, _children[1]->_min.z);
    _max.x = std::max(_children[0]->_max.x, _children[1]->_max.x);
    _max.y = std::max(_children[0]->_max.y, _children[1]->_max.y);
    _max.z = std::max(_children[0]->_max.z, _children[1]->_max.z);
}

float BoundingVolume::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos,
                              glm::vec3& normal) {
    // TODO
    return 0.0f;
}

void BoundingVolume::toXml(pugi::xml_node& root) {
    pugi::xml_node node = root.append_child("node");
    node.append_attribute("min_x") = _min.x;
    node.append_attribute("min_y") = _min.y;
    node.append_attribute("min_z") = _min.z;
    node.append_attribute("max_x") = _max.x;
    node.append_attribute("max_y") = _max.y;
    node.append_attribute("max_z") = _max.z;

    if (_isLeaf) {
        pugi::xml_node tri = node.append_child("tri");
    } else {
        _children[0]->toXml(node);
        _children[1]->toXml(node);
    }
}


}  // namespace path_tracer
