#include <light.h>

namespace path_tracer {

pugi::xml_node Light::toXml(pugi::xml_node& root) {
    pugi::xml_node node = root.append_child("light");

    pugi::xml_node posNode = node.append_child("pos");
    posNode.append_attribute("x") = pos.x;
    posNode.append_attribute("y") = pos.y;
    posNode.append_attribute("z") = pos.z;

    pugi::xml_node diffNode = node.append_child("diff");
    diffNode.append_attribute("r") = diff.r;
    diffNode.append_attribute("g") = diff.g;
    diffNode.append_attribute("b") = diff.b;

    pugi::xml_node specNode = node.append_child("spec");
    specNode.append_attribute("r") = spec.r;
    specNode.append_attribute("g") = spec.g;
    specNode.append_attribute("b") = spec.b;

    return node;
}

Light Light::fromXml(pugi::xml_node node) {
    Light l;
    
    pugi::xml_node posNode = node.child("pos");
    l.pos.x = posNode.attribute("x").as_float();
    l.pos.y = posNode.attribute("y").as_float();
    l.pos.z = posNode.attribute("z").as_float();

    pugi::xml_node diffNode = node.child("diff");
    l.diff.r = diffNode.attribute("r").as_float();
    l.diff.g = diffNode.attribute("g").as_float();
    l.diff.b = diffNode.attribute("b").as_float();

    pugi::xml_node specNode = node.child("spec");
    l.spec.r = specNode.attribute("r").as_float();
    l.spec.g = specNode.attribute("g").as_float();
    l.spec.b = specNode.attribute("b").as_float();

    return l;
}

}  // namespace path_tracer
