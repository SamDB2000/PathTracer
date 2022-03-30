#ifndef __BOUNDING_VOLUME_H__
#define __BOUNDING_VOLUME_H__

#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <memory>
#include <triangle.h>

namespace path_tracer {

class BoundingVolume {
public:
    typedef std::unique_ptr<BoundingVolume> Ptr;

    BoundingVolume(Triangle* tri);
    BoundingVolume(Ptr bv0, Ptr bv1);

    float raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal);
    pugi::xml_node toXml(pugi::xml_node& root);

private:
    glm::vec3 _min;
    glm::vec3 _max;
    bool _isRoot;
    bool _isLeaf;
    Ptr _children[2];
    Triangle* _tri;
};

}  // namespace path_tracer

#endif  //  __BOUNDING_VOLUME_H__
