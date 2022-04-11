#include <triangle.h>

namespace path_tracer {

float Triangle::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    glm::vec3 p = v1 - v0;
    glm::vec3 q = v2 - v0;
    glm::vec3 s = rayPos - v0;
    glm::vec3 tmp1 = glm::cross(rayDir, q);
    float dot1 = glm::dot(tmp1, p);
    if (-0.0001f < dot1 && dot1 < 0.0001f)
        return 0;
    float f = 1.0f / dot1;
    float u = f * glm::dot(s, tmp1);
    if (u < 0.0f || u > 1.0f)
        return 0;
    glm::vec3 tmp2 = glm::cross(s, p);
    float v = f * glm::dot(rayDir, tmp2);
    if (v < 0.0f || u + v > 1.0f)
        return 0;
    float t = f * glm::dot(q, tmp2);

    hitPos = rayPos + rayDir * t;
    normal = glm::normalize(glm::cross(v0 - v1, v0 - v2));
    return t;
}

pugi::xml_node Triangle::toXml(pugi::xml_node& root) {
    pugi::xml_node triNode = root.append_child("tri");

    pugi::xml_node v;
    v = triNode.append_child("v");
    v.append_attribute("x") = v0.x;
    v.append_attribute("y") = v0.y;
    v.append_attribute("z") = v0.z;

    v = triNode.append_child("v");
    v.append_attribute("x") = v1.x;
    v.append_attribute("y") = v1.y;
    v.append_attribute("z") = v1.z;

    v = triNode.append_child("v");
    v.append_attribute("x") = v2.x;
    v.append_attribute("y") = v2.y;
    v.append_attribute("z") = v2.z;
    return triNode;
}

Triangle Triangle::fromXml(pugi::xml_node node) {
    Triangle tri;
    auto vt = node.children("v").begin();
    tri.v0.x = vt->attribute("x").as_float();
    tri.v0.y = vt->attribute("y").as_float();
    tri.v0.z = vt->attribute("z").as_float();

    vt++;
    tri.v1.x = vt->attribute("x").as_float();
    tri.v1.y = vt->attribute("y").as_float();
    tri.v1.z = vt->attribute("z").as_float();

    vt++;
    tri.v2.x = vt->attribute("x").as_float();
    tri.v2.y = vt->attribute("y").as_float();
    tri.v2.z = vt->attribute("z").as_float();
    return tri;
}

}  // namespace path_tracer
