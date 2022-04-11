#include <scene.h>
#include <bitmap_header.h>
#include <omp.h>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <fstream>
#include <sstream>
#include <list>
#include <iostream>
#include <random>

namespace path_tracer {

/*
 * Initialize scene based on file input.
 */
Scene::Scene(const std::string& filename)
    : _backgroundColor(0.1f, 0.1f, 0.1f),
      _maxDepth(4),
      _antialias(1),
      _width(400),
      _height(400),
      _fov(90.0f),
      _up(0.0f, 1.0f, 0.0f),
      _lookAt(0.0f, 0.0, 1.0f),
      _eye(0.0f, 0.0f, 150.0f) {
    pugi::xml_document doc;
    doc.load_file(filename.c_str());
    pugi::xml_node root = doc.child("scene");

    // Load Camera Settings
    pugi::xml_node camera = root.child("camera");
    for (auto& attr : camera.attributes()) {
        std::string name = attr.name();
        if (name == "maxDepth")
            _maxDepth = attr.as_uint();
        else if (name == "antialias")
            _antialias = attr.as_uint();
        else if (name == "width")
            _width = attr.as_uint();
        else if (name == "height")
            _height = attr.as_uint();
        else if (name == "fov")
            _fov = attr.as_float();
        else
            std::cout << "Unknown attribute in \"camera\": \"" << name << "\"" << std::endl;
    }
    for (auto& child : camera.children()) {
        std::string name = child.name();
        if (name == "background") {
            _backgroundColor.r = child.attribute("r").as_float();
            _backgroundColor.g = child.attribute("g").as_float();
            _backgroundColor.b = child.attribute("b").as_float();
        } else if (name == "up") {
            _up.x = child.attribute("x").as_float();
            _up.y = child.attribute("y").as_float();
            _up.z = child.attribute("z").as_float();
        } else if (name == "lookAt") {
            _lookAt.x = child.attribute("x").as_float();
            _lookAt.y = child.attribute("y").as_float();
            _lookAt.z = child.attribute("z").as_float();
        } else if (name == "eye") {
            _eye.x = child.attribute("x").as_float();
            _eye.y = child.attribute("y").as_float();
            _eye.z = child.attribute("z").as_float();
        } else {
            std::cout << "Unknown child in \"camera\": \"" << name << "\"" << std::endl;
        }
    }

    // Load Object Settings
    pugi::xml_node objects = root.child("objects");
    for (auto& child : objects.children()) {
        std::string name = child.name();
        if (name == "light")
            _lights.push_back(Light::fromXml(child));
        else if (name == "sphere")
            _spheres.push_back(Sphere::fromXml(child));
        else if (name == "mesh")
            _meshes.push_back(Mesh::fromXml(child));
        else
            std::cout << "Unknown child in \"objects\": \"" << name << "\"" << std::endl;
    }

    size_t numTris = 0;
    for (auto& mesh : _meshes)
        numTris += mesh.tris.size();
    _tris.reserve(numTris);
    for (auto& mesh : _meshes) {
        for (auto& tri : mesh.tris)
            _tris.push_back(tri);
        mesh.tris.clear();
    }
}

/*
 * Overall raycast function, calls individual raycast for all spheres and BVH objects.
 * Returns minimum distance ray travels to intersection with object.
 */
float Scene::raycast(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3& hitPos, glm::vec3& normal,
                     Material& mat) {
    float inf = std::numeric_limits<float>::infinity();
    float minDist = inf;

    for (Sphere& sphere : _spheres) {
        glm::vec3 localHitPos;
        glm::vec3 localNormal;
        float dist = sphere.raycast(rayPos, rayDir, localHitPos, localNormal);
        if (dist > 0.0f && dist < minDist) {
            minDist = dist;
            hitPos = localHitPos;
            normal = localNormal;
            mat = sphere.m;
        }
    }

    if (_bvh) {
        glm::vec3 localHitPos;
        glm::vec3 localNormal;
        Material localMat;
        float dist = _bvh->raycast(rayPos, rayDir, localHitPos, localNormal, localMat);
        if (dist > 0.0f && dist < minDist) {
            minDist = dist;
            hitPos = localHitPos;
            normal = localNormal;
            mat = localMat;
        }
    }

    return minDist < inf ? minDist : 0.0f;
}

/*
 * Calls raycast with default hitPos, normal, and mat perameters.
 */
float Scene::raycast(glm::vec3 rayPos, glm::vec3 rayDir) {
    glm::vec3 hitPos;
    glm::vec3 normal;
    Material mat;
    float dist = raycast(rayPos, rayDir, hitPos, normal, mat);
    return dist;
}

/*
 * Raytrace function called for every ray sample.
 * Returns the color of the first point of interesection.
 */
glm::vec3 Scene::raytrace(glm::vec3 rayPos, glm::vec3 rayDir, int iter, int numIndirect) {
    if (iter >= _maxDepth)
        return glm::vec3(0.0f);
    glm::vec3 color = _backgroundColor;
    glm::vec3 hitPos;
    glm::vec3 normal;
    Material mat;
    float dist = raycast(rayPos, rayDir, hitPos, normal, mat);
    if (dist > 0.0f) {
        color = mat.amb;
        glm::vec3 adjHitPos = hitPos + 0.0001f * normal;
        for (Light& light : _lights) {
            glm::vec3 toLight = light.pos - hitPos;
            glm::vec3 toLightNorm = glm::normalize(toLight);

            float shadowRay = raycast(adjHitPos, toLightNorm);
            // If intersection is not in shadow
            if (shadowRay == 0.0f || shadowRay > glm::distance(light.pos, hitPos)) {
                // Phong Model
                float diffFactor = std::max(glm::dot(normal, toLightNorm), 0.0f);
                float specFactor =
                    std::pow(glm::dot(-rayDir, glm::reflect(toLightNorm, normal)), mat.shininess);
                color += light.diff * mat.diff * diffFactor + light.spec * mat.spec * specFactor;
            }

            // indirect illumination
            glm::vec3 indirectColor(0.0f);
            for (int i = 0; i < numIndirect; i++) {
                glm::vec3 v = randHemi(normal);
                indirectColor +=
                    glm::dot(normal, v) * raytrace(adjHitPos, v, iter + 1, numIndirect / 2);
            }
            if (numIndirect > 0)
                indirectColor /= numIndirect;
            color += indirectColor;
            // If the material has any specular properties, cast reflectance ray
            if (mat.spec != glm::vec3(0.0f)) {
                color += mat.spec * raytrace(adjHitPos, glm::reflect(rayDir, normal), iter + 1,
                                             numIndirect / 2);
            }
        }
    }
    return color;
}

/*
 * Returns a point on the unit hemisphere, centered on the normal
 */
glm::vec3 Scene::randHemi(glm::vec3 normal) {
    // r1 = random angle b/w [0, 2pi]
    float r1 = glm::two_pi<float>() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float r2s = glm::sqrt(r2);  // random distance from center
    // Coordinate system to sample hemisphere
    glm::vec3 sw = normal;
    glm::vec3 su = glm::normalize(glm::cross(
        fabs(sw.x) > 0.1f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f), sw));
    glm::vec3 sv = glm::cross(sw, su);
    // Random ray direction in sample hemisphere
    glm::vec3 d = glm::normalize(su * cosf(r1) * r2s + sv * sin(r1) * r2s + sw * sqrt(1 - r2));
    return d;
}

/*
 * Render function for entire scene.
 * Calls raytrace for each sample (based on camera/image settings) and writes to bmp.
 */
void Scene::render(const std::string& filename) {
    // Set clock and random number seed
    clock_t timeStart = clock();
    srand(static_cast<unsigned>(time(0)));

    int width = _width * _antialias;
    int height = _height * _antialias;
    std::vector<std::vector<glm::vec3>> buffer(width, std::vector<glm::vec3>(height));

    // Gramm-Schmidt orthonormalization
    glm::vec3 l = glm::normalize(_lookAt - _eye);
    glm::vec3 v = glm::normalize(glm::cross(l, _up));
    glm::vec3 u = glm::cross(v, l);

    float aspectRatio = (float) width / height;
    float focalLength = 1.0f / glm::tan(_fov / 2.0f);
    glm::vec3 ll = _eye + focalLength * l - aspectRatio * v - u;

    size_t completed = 0;

#pragma omp parallel for
    for (int i = 0; i < width * height; i++) {
        float x = i / width;
        float y = i % width;
        glm::vec3 p =
            ll + 2.0f * aspectRatio * v * ((float) x / width) + 2.0f * u * ((float) y / height);
        glm::vec3 ray = glm::normalize(p - _eye);
        buffer[x][y] = glm::clamp(raytrace(_eye, ray), glm::vec3(0.0f), glm::vec3(1.0f));

#pragma omp atomic
        completed++;
        if (completed % 500 == 0) {
            int progress = 10000 * completed / (width * height);
            int per = progress / 100;
            int dec = progress % 100;
#pragma omp critical
            std::cout << "\rRendering " << per << "." << dec << "% complete  " << std::flush;
        }
    }
    clock_t timeEnd = clock();
    std::cout << "\rRendering 100% complete. Writing to " << filename << '\n';
    std::cout << "Render time: " << (float) (timeEnd - timeStart) / CLOCKS_PER_SEC << " (sec)\n";

    // Write bmp header
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    BitmapHeader bmpHead(_width, _height);
    ofs.write(bmpHead.rawData, sizeof(bmpHead));
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            glm::vec3 color(0.0f);
            for (int r = 0; r < _antialias; r++)
                for (int c = 0; c < _antialias; c++)
                    color += buffer[_antialias * j + r][_antialias * i + c];
            color /= _antialias * _antialias;
            color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
            uint8_t r = (uint8_t)(255 * color.r);
            uint8_t g = (uint8_t)(255 * color.g);
            uint8_t b = (uint8_t)(255 * color.b);
            ofs.write((char*) &b, sizeof(uint8_t));
            ofs.write((char*) &g, sizeof(uint8_t));
            ofs.write((char*) &r, sizeof(uint8_t));
        }
    }
    std::cout << "Done writing\n";
}

void Scene::generateBvh() {
    if (_bvh)
        return;
    _bvh = std::move(BoundingVolume::generate(_tris));
}

void Scene::exportBvh(std::ostream& os) {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("bvh");
    if (_bvh)
        _bvh->toXml(root);
    doc.save(os, "", pugi::format_raw | pugi::format_no_declaration);
}

void Scene::importBvh(const std::string& filename) {
    if (_bvh)
        return;
    std::cout << "Loading BVH from " << filename << "...\n";
    pugi::xml_document doc;
    doc.load_file(filename.c_str());
    pugi::xml_node root = doc.child("bvh").child("bv");
    _bvh = std::move(BoundingVolume::fromXml(root, _tris));
    if (_bvh)
        std::cout << "Successfully Loaded BVH\n";
    else
        std::cout << "Could not load BVH\n";
}

std::ostream& operator<<(std::ostream& os, Scene& s) {
    os << "[Scene resolution=" << s._width << "x" << s._height << " antialias=" << s._antialias
       << " maxDepth=" << s._maxDepth << " | " << s._lights.size() << " lights, "
       << s._spheres.size() << " spheres, " << s._meshes.size() << " meshes, " << s._tris.size()
       << " triangles]";
    return os;
}

}  // namespace path_tracer
