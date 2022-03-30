#include <mesh.h>
#include <fstream>
#include <list>
#include <iostream>

namespace path_tracer {

Mesh::Mesh(const std::string& filename) {
    loadStl(filename);
}

Mesh::Mesh(Material m, const std::vector<Triangle>& tris) : m(m), tris(tris) {
}

Mesh::Mesh(const Mesh& mesh) : m(mesh.m), tris(mesh.tris) {
}

float Mesh::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal) {
    float minDist = std::numeric_limits<float>::infinity();
    bool hit = false;

    if (AABbox.intersect(rayPos, rayDir)) {
        // This seems to infer it works since that prints very often.
        //std::cout << "intersection found in AABB\n";
        // TODO: Replace with BVH walk
        for (Triangle& tri : tris) {
            glm::vec3 localHitPos;
            glm::vec3 localNormal;
            float dist = tri.raycast(rayPos, rayDir, localHitPos, localNormal);
            if (dist > 0.0f && dist < minDist) {
                minDist = dist;
                hitPos = localHitPos;
                normal = localNormal;
                hit = true;
            }
        }
        if (hit)
            return minDist;
        else
            return 0.0f;
    } else
        return 0.0f;
}

void Mesh::loadStl(const std::string& filename) {
    if (filename.empty())
        return;

    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    char header[80];
    ifs.read(header, sizeof(header));
    uint32_t numTris;
    ifs.read((char*) (&numTris), sizeof(uint32_t));

    tris.clear();
    tris.reserve(numTris);

    glm::vec3 min_bound = glm::vec3();
    glm::vec3 max_bound = glm::vec3();

    //TODO: Find a better way of finding the min and max coordinates of the mesh
    // I don't know a better way of doing this at the moment, but this is gross
    for (int i = 0; i < numTris; i++) {
        glm::vec3 normal;
        Triangle t;
        uint16_t attrib;
        ifs.read((char*) (&normal), sizeof(glm::vec3));
        ifs.read((char*) (&t.v0), sizeof(glm::vec3));
        // Find min/max x,y,z coordinates for v0
        if (t.v0.x < min_bound.x) min_bound.x = t.v0.x;
        if (t.v0.y < min_bound.y) min_bound.y = t.v0.y;
        if (t.v0.z < min_bound.z) min_bound.z = t.v0.z;
        if (t.v0.x > max_bound.x) max_bound.x = t.v0.x;
        if (t.v0.y > max_bound.y) max_bound.y = t.v0.y;
        if (t.v0.z > max_bound.z) max_bound.z = t.v0.z;
        ifs.read((char*) (&t.v1), sizeof(glm::vec3));
        if (t.v1.x < min_bound.x) min_bound.x = t.v1.x;
        if (t.v1.y < min_bound.y) min_bound.y = t.v1.y;
        if (t.v1.z < min_bound.z) min_bound.z = t.v1.z;
        if (t.v1.x > max_bound.x) max_bound.x = t.v1.x;
        if (t.v1.y > max_bound.y) max_bound.y = t.v1.y;
        if (t.v1.z > max_bound.z) max_bound.z = t.v1.z;
        ifs.read((char*) (&t.v2), sizeof(glm::vec3));
        if (t.v2.x < min_bound.x) min_bound.x = t.v2.x;
        if (t.v2.y < min_bound.y) min_bound.y = t.v2.y;
        if (t.v2.z < min_bound.z) min_bound.z = t.v2.z;
        if (t.v2.x > max_bound.x) max_bound.x = t.v2.x;
        if (t.v2.y > max_bound.y) max_bound.y = t.v2.y;
        if (t.v2.z > max_bound.z) max_bound.z = t.v2.z;
        
        ifs.read((char*) (&attrib), sizeof(attrib));
        AABbox.bounds[0] = min_bound;
        AABbox.bounds[1] = max_bound;
        tris.push_back(t);
    }
    this->filename = filename;
}

pugi::xml_node Mesh::toXml(pugi::xml_node& root) {
    pugi::xml_node node = root.append_child("mesh");
    if (!filename.empty()) {
        node.append_attribute("type") = "stl";
        node.append_attribute("filename") = filename.c_str();
    } else {
        pugi::xml_node trisNode = node.append_child("tris");
        for (auto& tri : tris) {
            pugi::xml_node triNode = trisNode.append_child("tri");

            pugi::xml_node v;
            v = triNode.append_child("v");
            v.append_attribute("x") = tri.v0.x;
            v.append_attribute("y") = tri.v0.y;
            v.append_attribute("z") = tri.v0.z;

            v = triNode.append_child("v");
            v.append_attribute("x") = tri.v1.x;
            v.append_attribute("y") = tri.v1.y;
            v.append_attribute("z") = tri.v1.z;

            v = triNode.append_child("v");
            v.append_attribute("x") = tri.v2.x;
            v.append_attribute("y") = tri.v2.y;
            v.append_attribute("z") = tri.v2.z;
        }
    }
    m.toXml(node);
    return node;
}

Mesh Mesh::fromXml(pugi::xml_node node) {
    Mesh mesh;
    std::string type = node.attribute("type").value();
    if (type == "stl") {
        mesh.loadStl(node.attribute("filename").value());
    } else if (type == "quad") {
        std::vector<glm::vec3> verts;
        verts.reserve(4);
        pugi::xml_node vertsNode = node.child("verts");
        for (auto& vert : vertsNode.children()) {
            if (std::string(vert.name()) != "v")
                continue;
            glm::vec3 v;
            v.x = vert.attribute("x").as_float();
            v.y = vert.attribute("y").as_float();
            v.z = vert.attribute("z").as_float();
            verts.push_back(v);
        }
        if (verts.size() == 3)
            verts.push_back(verts[1] + verts[2] - verts[0]);
        if (verts.size() == 4) {
            Triangle t;
            t.v0 = verts[2];
            t.v1 = verts[1];
            t.v2 = verts[0];
            mesh.tris.push_back(t);

            t.v0 = verts[1];
            t.v1 = verts[2];
            t.v2 = verts[3];
            mesh.tris.push_back(t);
        }
    } else {
        pugi::xml_node trisNode = node.child("tris");
        for (auto& triNode : trisNode.children()) {
            if (triNode.name() != "tri")
                continue;
            Triangle tri;
            auto vt = triNode.children("v").begin();
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
        }
    }
    Material mat = Material::fromXml(node.child("material"));
    mesh.m = mat;
    for (auto& tri : mesh.tris)
        tri.m = mat;
    return mesh;
}

}  // namespace path_tracer
