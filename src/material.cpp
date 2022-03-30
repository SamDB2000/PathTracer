#include <material.h>

namespace path_tracer {

pugi::xml_node Material::toXml(pugi::xml_node& root) {
    pugi::xml_node node = root.append_child("material");

    pugi::xml_node diffNode = node.append_child("diff");
    diffNode.append_attribute("r") = diff.r;
    diffNode.append_attribute("g") = diff.g;
    diffNode.append_attribute("b") = diff.b;

    pugi::xml_node specNode = node.append_child("spec");
    specNode.append_attribute("r") = spec.r;
    specNode.append_attribute("g") = spec.g;
    specNode.append_attribute("b") = spec.b;

    pugi::xml_node shininessNode = node.append_child("shininess");
    shininessNode.append_attribute("value") = shininess;

    pugi::xml_node ambNode = node.append_child("amb");
    ambNode.append_attribute("r") = amb.r;
    ambNode.append_attribute("g") = amb.g;
    ambNode.append_attribute("b") = amb.b;

    return node;
}

}  // namespace path_tracer
