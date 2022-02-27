#include <iostream>
#include <scene.h>

int main(int argc, char const* argv[]) {
    if (argc != 3) {
        std::cerr << "Error: Incorrect number of arguments\n"
                  << "Usage: "
                  << argv[0]
                  // << " [input file | input directory] [output file | output directory]\n";
                  << " [input file] [output file]\n";
        return 1;
    }

    dng::Scene scene(argv[1]);
    scene.render(argv[2]);

    return 0;
}
