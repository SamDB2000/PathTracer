#include <iostream>
#include <scene.h>
#include <fstream>

int main(int argc, char const* argv[]) {
    if (argc != 3) {
        std::cerr << "Error: Incorrect number of arguments\n"
                  << "Usage: "
                  << argv[0]
                  // << " [input file | input directory] [output file | output directory]\n";
                  << " [input file] [output file]\n";
        return 1;
    }

    path_tracer::Scene scene(argv[1]);
    std::cout << scene << '\n';
    scene.generateBvh();

    std::ofstream ofs(argv[2] + std::string(".bvh"));
    scene.exportBvh(ofs);
    ofs.close();

    scene.render(argv[2]);

    return 0;
}
