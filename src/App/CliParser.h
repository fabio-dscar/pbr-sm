#ifndef PBR_CLIPARSER_H
#define PBR_CLIPARSER_H

#include <string>

namespace pbr {

struct CliOptions {
    int width;
    int height;
    unsigned int msaaSamples;
    std::string sceneFile;
    bool multiScattering;
};

CliOptions ParseArgs(int argc, char* argv[]);

} // namespace pbr

#endif