#include <scene.h>
#include <fstream>
#include <sstream>
#include <bitmap_header.h>
#include <omp.h>

namespace dng {

void Scene::readVec(std::ifstream& ifs, glm::vec3& vec) {
    ifs >> vec.x >> vec.y >> vec.z;
}

void Scene::parseMatProp(std::ifstream& ifs, const std::string& str, Material& m) {
    if (str == "DIFF")
        readVec(ifs, m.diff);
    else if (str == "SPEC")
        readVec(ifs, m.spec);
    else if (str == "AMB")
        readVec(ifs, m.amb);
    else if (str == "SHININESS")
        ifs >> m.shininess;
}

std::string Scene::parseLight(std::ifstream& ifs) {
    Light l;
    std::string str;
    while (ifs) {
        ifs >> str;
        if (str == "POS")
            readVec(ifs, l.pos);
        else if (str == "DIFF")
            readVec(ifs, l.diff);
        else if (str == "SPEC")
            readVec(ifs, l.spec);
        else if (str.substr(0, 2) == "//")
            std::getline(ifs, str);
        else
            break;
    }
    _lights.push_back(l);
    // _spheres.push_back(Sphere{
    //     Material{ glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, glm::vec3(l.diff) }, l.pos, 10.0f });
    return str;
}

std::string Scene::parseSphere(std::ifstream& ifs) {
    Sphere s;
    std::string str;
    while (ifs) {
        ifs >> str;
        if (str == "POS")
            readVec(ifs, s.pos);
        else if (str == "RADIUS")
            ifs >> s.radius;
        else if (str == "DIFF" || str == "SPEC" || str == "AMB" || str == "SHININESS")
            parseMatProp(ifs, str, s.m);
        else if (str.substr(0, 2) == "//")
            std::getline(ifs, str);
        else
            break;
    }
    _spheres.push_back(s);
    return str;
}

std::string Scene::parseQuad(std::ifstream& ifs) {
    Material m;
    std::vector<glm::vec3> verts;
    verts.reserve(4);
    std::string str;
    while (ifs) {
        ifs >> str;
        if (str == "POS") {
            glm::vec3 v;
            readVec(ifs, v);
            verts.push_back(v);
        } else if (str == "DIFF" || str == "SPEC" || str == "AMB" || str == "SHININESS") {
            parseMatProp(ifs, str, m);
        } else if (str.substr(0, 2) == "//") {
            std::getline(ifs, str);
        } else {
            break;
        }
    }
    if (verts.size() == 3)
        verts.push_back(verts[1] + verts[2] - verts[0]);
    if (verts.size() == 4) {
        _meshes.emplace_back(Mesh{ m,
                                   { Triangle{ m, verts[2], verts[1], verts[0] },
                                     Triangle{ m, verts[1], verts[2], verts[3] } } });
    }
    return str;
}

Scene::Scene(const std::string& filename)
    : _backgroundColor(0.1f, 0.1f, 0.1f),
      _maxDepth(4),
      _antialias(1),
      _width(400),
      _height(400),
      _fov(90.0f) {
    std::ifstream ifs(filename);
    std::string str;
    ifs >> str;

    while (ifs) {
        if (str == "LIGHT")
            str = parseLight(ifs);
        else if (str == "SPHERE")
            str = parseSphere(ifs);
        else if (str == "QUAD")
            str = parseQuad(ifs);
        else if (str == "BACKGROUND")
            ifs >> _backgroundColor.r >> _backgroundColor.g >> _backgroundColor.b >> str;
        else if (str == "ANTIALIAS")
            ifs >> _antialias >> str;
        else if (str == "MAXDEPTH")
            ifs >> _maxDepth >> str;
        else if (str == "RESOLUTION")
            ifs >> _width >> _height >> str;
        else if (str == "FOV")
            ifs >> _fov >> str;
        else if (str.substr(0, 2) == "//") {
            std::getline(ifs, str);
            ifs >> str;
        } else {
            ifs >> str;
        }
    }
}

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
    for (Mesh& mesh : _meshes) {
        glm::vec3 localHitPos;
        glm::vec3 localNormal;
        float dist = mesh.raycast(rayPos, rayDir, localHitPos, localNormal);
        if (dist > 0.0f && dist < minDist) {
            minDist = dist;
            hitPos = localHitPos;
            normal = localNormal;
            mat = mesh.m;
        }
    }

    return minDist < inf ? minDist : 0.0f;
}

float Scene::raycast(glm::vec3 rayPos, glm::vec3 rayDir) {
    glm::vec3 hitPos;
    glm::vec3 normal;
    Material mat;
    float dist = raycast(rayPos, rayDir, hitPos, normal, mat);
    return dist;
}

glm::vec3 Scene::raytrace(glm::vec3 rayPos, glm::vec3 rayDir, int iter) {
    if (iter >= _maxDepth)
        return glm::vec3(0.0f);
    glm::vec3 color = _backgroundColor;
    glm::vec3 hitPos;
    glm::vec3 normal;
    Material mat;
    float dist = raycast(rayPos, rayDir, hitPos, normal, mat);
    if (dist > 0.0f) {
        color = mat.amb;
        glm::vec3 shadowRayStart = hitPos + 0.001f * normal;
        for (Light& light : _lights) {
            glm::vec3 toLight = light.pos - hitPos;
            glm::vec3 toLightNorm = glm::normalize(toLight);

            float shadowRay = raycast(shadowRayStart, toLightNorm);
            if (shadowRay == 0.0f || shadowRay > glm::distance(light.pos, hitPos)) {
                // Diffuse
                float diffuseFactor = std::max(glm::dot(normal, toLightNorm), 0.0f);
                color += light.diff * mat.diff * diffuseFactor;
                // Specular
                color +=
                    light.spec * mat.spec *
                    std::pow(glm::dot(-rayDir, glm::reflect(toLightNorm, normal)), mat.shininess);
            }
            if (mat.diff != glm::vec3(0.0f)) {
                color += mat.spec * 0.5f *
                         raytrace(shadowRayStart, glm::reflect(rayDir, normal), iter + 1);
            }
        }
    }
    return color;
}

void Scene::render(const std::string& filename) {
    int width = _width * _antialias;
    int height = _height * _antialias;
    std::vector<std::vector<glm::vec3>> buffer(width, std::vector<glm::vec3>(height));

    // Camera Params
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 lookAt(0.0f, 0.0, -1.0f);
    glm::vec3 eye(0.0f, 0.0f, 150.0f);

    // Gramm-Schmidt orthonormalization
    glm::vec3 l = glm::normalize(lookAt - eye);
    glm::vec3 v = glm::normalize(glm::cross(l, up));
    glm::vec3 u = glm::cross(v, l);

    float aspectRatio = (float) width / height;
    float focalLength = 1.0f / glm::tan(_fov / 2.0f);
    glm::vec3 ll = eye + focalLength * l - aspectRatio * v - u;

    size_t completed = 0;

#pragma omp parallel for
    for (int i = 0; i < width * height; i++) {
        float x = i / width;
        float y = i % width;
        glm::vec3 p =
            ll + 2.0f * aspectRatio * v * ((float) x / width) + 2.0f * u * ((float) y / height);
        glm::vec3 ray = glm::normalize(p - eye);
        buffer[x][y] = glm::clamp(raytrace(eye, ray), glm::vec3(0.0f), glm::vec3(1.0f));

#pragma omp atomic
        completed++;
        if ((100 * completed) % (width * height) == 0) {
            int percent = 100 * completed / (width * height);
#pragma omp critical
            std::cout << "\rRendering " << percent << "% complete";
        }
    }
    std::cout << "\rRendering 100% complete. Writing to " << filename << '\n';

    // Write bmp header
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    BitmapHeader bmpHead(_width, _height);
    ofs.write(bmpHead.rawData, sizeof(bmpHead));
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            glm::vec3 color;
            for (int r = 0; r < _antialias; r++)
                for (int c = 0; c < _antialias; c++)
                    color += buffer[_antialias * j + r][_antialias * i + c];
            color /= _antialias * _antialias;
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

std::ostream& operator<<(std::ostream& os, Scene& s) {
    os << "[Scene resolution=" << s._width << "x" << s._height << " antialias=" << s._antialias
       << " maxDepth=" << s._maxDepth << " | " << s._lights.size() << " lights, "
       << s._spheres.size() << " spheres, " << s._meshes.size() << " meshes]";
    return os;
}

}  // namespace dng
