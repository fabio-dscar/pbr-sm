#ifndef __PBR_CLIPARSER_H__
#define __PBR_CLIPARSER_H__

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

#endif // __PBR_CLIPARSER_H__