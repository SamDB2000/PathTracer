#include <sphere.h>

namespace path_tracer {

float Sphere::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    glm::vec3 v = pos - rayPos;
    float t0 = glm::dot(v, rayDir);
    if (t0 < 0)
        return 0;
    float dSqr = glm::dot(v, v) - t0 * t0;
    float rSqr = radius * radius;
    if (dSqr >= rSqr)
        return 0;
    float td = std::sqrt(rSqr - dSqr);
    float t1 = t0 + td;
    float t2 = t0 - td;
    float t;
    if (t1 > 0.0001f && t2 > 0.0001f)
        t = (t1 < t2) ? t1 : t2;
    else if (t1 > 0.0001f && t2 <= 0.0001f)
        t = t1;
    else if (t2 <= 0.0001f && t2 > 0.0001f)
        t = t2;
    else
        return 0;

    hitPos = rayPos + rayDir * t;
    normal = glm::normalize(hitPos - pos);
    return t;
}

pugi::xml_node Sphere::toXml(pugi::xml_node& root) {
    pugi::xml_node node = root.append_child("sphere");

    node.append_child("radius").append_attribute("value") = radius;
    pugi::xml_node posNode = node.append_child("pos");
    posNode.append_attribute("x") = pos.x;
    posNode.append_attribute("y") = pos.y;
    posNode.append_attribute("z") = pos.z;

    m.toXml(node);
    return node;
}

Sphere Sphere::fromXml(pugi::xml_node node) {
    Sphere s;

    s.radius = node.child("radius").attribute("value").as_float();

    pugi::xml_node posNode = node.child("pos");
    s.pos.x = posNode.attribute("x").as_float();
    s.pos.y = posNode.attribute("y").as_float();
    s.pos.z = posNode.attribute("z").as_float();

    s.m = Material::fromXml(node.child("material"));
    return s;
}

}  // namespace path_tracer
