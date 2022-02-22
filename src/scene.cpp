#include <scene.h>
#include <fstream>
#include <sstream>

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

std::ostream& operator<<(std::ostream& os, Scene& s) {
    os << "[Scene resolution=" << s._width << "x" << s._height << " antialias=" << s._antialias
       << " maxDepth=" << s._maxDepth << " | " << s._mats.size() << " materials, "
       << s._lights.size() << " lights, " << s._spheres.size() << " spheres, " << s._tris.size()
       << " tris]";
    return os;
}

}  // namespace dng
