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

Material Material::fromXml(pugi::xml_node node) {
    Material m;
    pugi::xml_node diffNode = node.child("diff");
    m.diff.r = diffNode.attribute("r").as_float();
    m.diff.g = diffNode.attribute("g").as_float();
    m.diff.b = diffNode.attribute("b").as_float();

    pugi::xml_node specNode = node.child("spec");
    m.spec.r = specNode.attribute("r").as_float();
    m.spec.g = specNode.attribute("g").as_float();
    m.spec.b = specNode.attribute("b").as_float();

    pugi::xml_node shininessNode = node.child("shininess");
    m.shininess = shininessNode.attribute("value").as_float();

    pugi::xml_node ambNode = node.child("amb");
    m.amb.r = ambNode.attribute("r").as_float();
    m.amb.g = ambNode.attribute("g").as_float();
    m.amb.b = ambNode.attribute("b").as_float();
    return m;
}

}  // namespace path_tracer
