#ifndef __BOUNDING_VOLUME_H__
#define __BOUNDING_VOLUME_H__

#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <memory>
#include <triangle.h>
#include <AABB.h>

namespace path_tracer {

class BoundingVolume {
public:
    typedef std::unique_ptr<BoundingVolume> Ptr;
    typedef std::shared_ptr<BoundingVolume::Ptr> SharedPtr;

    struct Pair {
        Pair(const SharedPtr& bv0, const SharedPtr& bv1);
        BoundingVolume::Ptr join();

        SharedPtr bv0;
        SharedPtr bv1;
        float volume;
    };

    BoundingVolume(Triangle* tri);
    BoundingVolume(Ptr bv0, Ptr bv1);
    static SharedPtr makeShared(Triangle* tri);
    static SharedPtr makeShared(Ptr bv0, Ptr bv1);

    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal,
                  Material& outMat);
    float raycast(glm::vec3 rayPos, glm::vec3 rayDir);
    pugi::xml_node toXml(pugi::xml_node& root);

private:
    AABB _aabb;
    bool _isRoot;
    bool _isLeaf;
    Ptr _children[2];
    Triangle* _tri;
};

}  // namespace path_tracer

#endif  //  __BOUNDING_VOLUME_H__
