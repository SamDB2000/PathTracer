#include <bounding_volume.h>
#include <algorithm>
#include <iostream>
#include <queue>

namespace path_tracer {

BoundingVolume::Pair::Pair(const SharedPtr& bv0, const SharedPtr& bv1) : bv0(bv0), bv1(bv1) {
    AABB temp((*bv0)->_aabb, (*bv1)->_aabb);
    volume = temp.volume();
}

BoundingVolume::Ptr BoundingVolume::Pair::join() {
    BoundingVolume::Ptr ptr = std::make_unique<BoundingVolume>(std::move(*bv0), std::move(*bv1));
    return ptr;
}

BoundingVolume::BoundingVolume(Triangle* tri, size_t id)
    : _isRoot(true), _isLeaf(true), _tri(tri), _id(id) {
    _aabb.bounds[0].x = std::min({ tri->v0.x, tri->v1.x, tri->v2.x });
    _aabb.bounds[0].y = std::min({ tri->v0.y, tri->v1.y, tri->v2.y });
    _aabb.bounds[0].z = std::min({ tri->v0.z, tri->v1.z, tri->v2.z });
    _aabb.bounds[1].x = std::max({ tri->v0.x, tri->v1.x, tri->v2.x });
    _aabb.bounds[1].y = std::max({ tri->v0.y, tri->v1.y, tri->v2.y });
    _aabb.bounds[1].z = std::max({ tri->v0.z, tri->v1.z, tri->v2.z });
}

BoundingVolume::BoundingVolume(Ptr bv0, Ptr bv1)
    : _isRoot(true),
      _isLeaf(false),
      _child0(std::move(bv0)),
      _child1(std::move(bv1)),
      _aabb(bv0->_aabb, bv1->_aabb) {
    _child0->_isRoot = false;
    _child1->_isRoot = false;
}

BoundingVolume::~BoundingVolume() {
    if (!_isLeaf) {
        _child0.~unique_ptr();
        _child1.~unique_ptr();
    }
}

BoundingVolume::SharedPtr BoundingVolume::makeShared(Triangle* tri, size_t id) {
    return std::make_shared<BoundingVolume::Ptr>(std::make_unique<BoundingVolume>(tri, id));
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
        float dist0 = _child0->raycast(rayPos, rayDir, hit0, norm0, mat0);

        glm::vec3 hit1;
        glm::vec3 norm1;
        Material mat1;
        float dist1 = _child1->raycast(rayPos, rayDir, hit1, norm1, mat1);

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
    pugi::xml_node node = root.append_child("bv");

    if (_isLeaf) {
        node.append_attribute("x0") = _aabb.bounds[0].x;
        node.append_attribute("y0") = _aabb.bounds[0].y;
        node.append_attribute("z0") = _aabb.bounds[0].z;
        node.append_attribute("x1") = _aabb.bounds[1].x;
        node.append_attribute("y1") = _aabb.bounds[1].y;
        node.append_attribute("z1") = _aabb.bounds[1].z;
        node.append_attribute("id") = _id;
    } else {
        _child0->toXml(node);
        _child1->toXml(node);
    }

    return node;
}

BoundingVolume::Ptr BoundingVolume::fromXml(pugi::xml_node node, std::vector<Triangle>& tris) {
    std::vector<Ptr> children;
    children.reserve(2);
    for (pugi::xml_node bv : node.children("bv")) {
        children.push_back(std::move(fromXml(bv, tris)));
    }

    if (children.size() == 0) {
        size_t id = node.attribute("id").as_ullong();
        if (id >= tris.size()) {
            std::cout << "Error Loading BVH: Triangle count mismatch\n";
            return nullptr;
        }

        Ptr bv = std::make_unique<BoundingVolume>(tris.data() + id, id);

        if (node.attribute("x0").as_float() == bv->_aabb.bounds[0].x &&
            node.attribute("y0").as_float() == bv->_aabb.bounds[0].y &&
            node.attribute("z0").as_float() == bv->_aabb.bounds[0].z &&
            node.attribute("x1").as_float() == bv->_aabb.bounds[1].x &&
            node.attribute("y1").as_float() == bv->_aabb.bounds[1].y &&
            node.attribute("z1").as_float() == bv->_aabb.bounds[1].z) {
            return std::move(bv);
        } else {
            std::cout << "Error Loading BVH: Triangle Bounds Mismatch\n";
            return nullptr;
        }
    } else if (children.size() == 2) {
        if (children[0] == nullptr || children[1] == nullptr)
            return nullptr;
        Ptr bv = std::make_unique<BoundingVolume>(std::move(children[0]), std::move(children[1]));
        return std::move(bv);
    } else {
        std::cout << "Error Loading BVH: Incorrect number of children (expected 2, got "
                  << children.size() << ")\n";
        return nullptr;
    }
}

BoundingVolume::Ptr BoundingVolume::generate(std::vector<Triangle>& tris) {
    std::cout << "Generating BVH..." << std::endl;
    clock_t timeStart = std::clock();

    std::list<SharedPtr> bvs;
    for (size_t i = 0; i < tris.size(); i++)
        bvs.push_back(makeShared(tris.data() + i, i));

    _progress = 0;
    _totalTris = tris.size();

    Ptr bvh = std::move(topDown(bvs));

    clock_t timeEnd = std::clock();
    std::cout << "\rBVH Generation completed. Beginning Render...\n"
              << "Generation time: " << (float) (timeEnd - timeStart) / CLOCKS_PER_SEC
              << " (sec)\n";

    return std::move(bvh);
}

BoundingVolume::Ptr BoundingVolume::topDown(std::list<SharedPtr>& bvs, size_t targetSize) {
    if (bvs.size() <= targetSize)
        return bottomUp(bvs);

    // Get Pair that takes up largest volume, use to split into children
    int idx0 = std::rand() % bvs.size();
    int idx1 = idx0;
    while (idx0 == idx1)
        idx1 = std::rand() % bvs.size();
    if (idx0 > idx1)
        std::swap(idx0, idx1);

    auto it = bvs.begin();
    for (int i = 0; i < idx0; i++)
        ++it;
    SharedPtr bv0 = *it;
    for (int i = idx0; i < idx1; i++)
        ++it;
    SharedPtr bv1 = *it;

    AABB bounds0 = (*(bv0))->_aabb;
    AABB bounds1 = (*(bv1))->_aabb;
    float volume0 = bounds0.volume();
    float volume1 = bounds1.volume();
    std::list<SharedPtr> bvh0;
    std::list<SharedPtr> bvh1;
    while (!bvs.empty()) {
        SharedPtr bv = bvs.front();
        bvs.pop_front();
        AABB aabb0((*bv)->_aabb, bounds0);
        AABB aabb1((*bv)->_aabb, bounds1);
        float change0 = aabb0.volume() - volume0;
        float change1 = aabb1.volume() - volume1;
        if ((change0 < change1) || (change1 == change0 && bvh0.size() < bvh1.size()))
            bvh0.push_back(bv);
        else
            bvh1.push_back(bv);
    }

    Ptr child0 = std::move(topDown(bvh0, targetSize));
    Ptr child1 = std::move(topDown(bvh1, targetSize));
    Ptr bvh = nullptr;
    if (child0 && !child1)
        bvh = std::move(child0);
    else if (child1 && !child0)
        bvh = std::move(child1);
    else if (child1 && child0)
        bvh = std::make_unique<BoundingVolume>(std::move(child0), std::move(child1));
    else
        return nullptr;
    return std::move(bvh);
}

BoundingVolume::Ptr BoundingVolume::bottomUp(std::list<SharedPtr>& bvs) {
    if (bvs.size() == 0)
        return nullptr;

    auto cmp = [](Pair bvp0, Pair bvp1) { return bvp0.volume > bvp1.volume; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);
    for (auto& t0 : bvs)
        for (auto& t1 : bvs)
            if (t0 != t1)
                pq.emplace(t0, t1);

    // iterate through all pairs of bvs

    while (!pq.empty()) {
        Pair next = pq.top();
        pq.pop();
        if (*(next.bv0) != nullptr && *(next.bv1) != nullptr) {
            auto bv = std::make_shared<Ptr>(std::move(next.join()));
            auto it = bvs.begin();
            while (it != bvs.end()) {
                if (**it == nullptr) {
                    bvs.erase(it++);
                } else {
                    pq.emplace(bv, *it);
                    ++it;
                }
            }
            bvs.push_back(bv);
            _progress++;
            int progress = 10000 * _progress / _totalTris;
            int per = progress / 100;
            int dec = progress % 100;
            std::cout << "\rBVH " << per << "." << dec << "% complete  " << std::flush;
        }
    }

    Ptr bvh = std::move(*(bvs.front()));
    bvs.pop_front();
    return std::move(bvh);
}

size_t BoundingVolume::_progress;
size_t BoundingVolume::_totalTris;

}  // namespace path_tracer
