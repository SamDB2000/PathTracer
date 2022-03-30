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

}  // namespace path_tracer
