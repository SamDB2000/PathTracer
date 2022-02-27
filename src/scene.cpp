#include <scene.h>
#include <fstream>
#include <sstream>
#include <bitmap_header.h>

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
    return str;
}

std::string Scene::parseSphere(std::ifstream& ifs) {
    _mats.emplace_back();
    Material& m = _mats.back();
    Sphere s{ m };
    std::string str;
    while (ifs) {
        ifs >> str;
        if (str == "POS")
            readVec(ifs, s.pos);
        else if (str == "RADIUS")
            ifs >> s.radius;
        else if (str == "DIFF" || str == "SPEC" || str == "AMB" || str == "SHININESS")
            parseMatProp(ifs, str, m);
        else if (str.substr(0, 2) == "//")
            std::getline(ifs, str);
        else
            break;
    }
    _spheres.push_back(s);
    return str;
}

std::string Scene::parseQuad(std::ifstream& ifs) {
    _mats.emplace_back();
    Material& m = _mats.back();
    std::vector<glm::vec3> verts;
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
    if (verts.size() == 3 || verts.size() == 4) {
        int i = _verts.size();
        _verts.reserve(i + 4);
        for (auto& v : verts)
            _verts.push_back(v);
        if (verts.size() == 3)
            _verts.push_back(_verts[i + 1] + _verts[i + 2] - _verts[i]);

        _norms.push_back(
            glm::normalize(glm::cross(_verts[i] - _verts[i + 1], _verts[i] - _verts[i + 2])));
        glm::vec3& n = _norms.back();

        _tris.emplace_back(Triangle{ m, _verts[i], _verts[i + 1], _verts[i + 2], n, n, n });
        _tris.emplace_back(Triangle{ m, _verts[i + 3], _verts[i + 1], _verts[i + 2], n, n, n });
    } else {
        _mats.pop_back();
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

void Scene::render(const std::string& filename) {
    std::vector<std::vector<glm::vec3>> buffer(_width, std::vector<glm::vec3>(_height));

    // Camera Params
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 lookAt(0.0f, 0.0f, -1.0f);
    glm::vec3 eye(0.0f, 0.0f, 0.0f);

    // Gramm-Schmidt orthonormalization
    glm::vec3 l = glm::normalize(lookAt - eye);
    glm::vec3 v = glm::normalize(glm::cross(l, up));
    glm::vec3 u = glm::cross(v, l);

    float aspectRatio = (float) _width / _height;
    float focalLength = 1.0f / glm::tan(_fov / 2.0f);
    glm::vec3 ll = eye + focalLength * l - aspectRatio * v - u;
    for (int i = 0; i < _width; i++) {
        for (int j = 0; j < _height; j++) {
            glm::vec3 p = ll + 2.0f * aspectRatio * v * ((float) i / _width) + 2.0f * u * ((float) j / _height);
            glm::vec3 ray = glm::normalize(p - eye);
            buffer[i][j] = glm::vec3(std::abs(ray.x), std::abs(ray.y), std::abs(ray.z));
        }
    }
    // for (int i = 0; i < bufferSize; i++) {
    //     glm::vec3 color;
    //     for (int r = 0; r < _antialias; r++) {
    //         float y = 2 * ((i / _width) + ((float) r / _antialias)) / _height - 1;
    //         for (int c = 0; c < _antialias; c++) {
    //             float x = 2 * ((i % _width) + ((float) c / _antialias)) / _width - 1;
    //             glm::vec3 p = ll + 2.0f * aspectRatio * v * x + 2.0f * u * y;
    //             glm::vec3 ray = glm::normalize(p);
    //             color += ray;
    //         }
    //     }

    //     buffer[i] = color / ((float) _antialias * _antialias);
    // }

    // Write bmp header
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    BitmapHeader bmpHead(_width, _height);
    ofs.write(bmpHead.rawData, sizeof(bmpHead));
    for (int j = 0; j < _height; j++) {
        for (int i = 0; i < _width; i++) {
            glm::vec3 color = buffer[i][j];
            uint8_t r = (uint8_t)(255 * color.r);
            uint8_t g = (uint8_t)(255 * color.g);
            uint8_t b = (uint8_t)(255 * color.b);
            ofs.write((char*) &b, sizeof(uint8_t));
            ofs.write((char*) &g, sizeof(uint8_t));
            ofs.write((char*) &r, sizeof(uint8_t));
        }
    }
}

std::ostream& operator<<(std::ostream& os, Scene& s) {
    os << "[Scene resolution=" << s._width << "x" << s._height << " antialias=" << s._antialias
       << " maxDepth=" << s._maxDepth << " | " << s._mats.size() << " materials, "
       << s._lights.size() << " lights, " << s._spheres.size() << " spheres, " << s._tris.size()
       << " tris]";
    return os;
}

}  // namespace dng
