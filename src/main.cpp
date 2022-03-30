#include <iostream>
#include <ctime>
#include <scene.h>
#include <fstream>

int main(int argc, char const* argv[]) {
    std::string inFile;
    std::string outFile;
    std::string cacheFile;
    if (argc == 2) {
        inFile = argv[1];
        outFile = inFile.substr(0, inFile.find_last_of('.')) + ".bmp";
    } else if (argc == 3) {
        inFile = argv[1];
        outFile = argv[2];
    } else if (argc == 4) {
        inFile = argv[1];
        outFile = argv[2];
        cacheFile = argv[3];
    } else if (argc != 3) {
        std::cerr << "Error: Incorrect number of arguments\n"
                  << "Usage: " << argv[0] << " input-file [output-file] [cache-file]\n";
        return 1;
    }

    path_tracer::Scene scene(inFile);
    std::cout << scene << '\n';
    scene.generateBvh();

    if (!cacheFile.empty()) {
        std::ofstream ofs(cacheFile);
        scene.exportBvh(ofs);
        ofs.close();
    }

    scene.render(outFile);
    return 0;
}
