#include <CliParser.h>

#include <argparse/argparse.hpp>

using namespace pbr;
using namespace argparse;

CliOptions pbr::ParseArgs(int argc, char* argv[]) {
    ArgumentParser program("pbr-sm", "1.0");
    program.add_description("PBR Shading Model Demo");

    program.add_argument("scene")
        .help("Path to XML file containing the scene description.")
        .nargs(1);

    program.add_argument("--width")
        .help("Demo window width.")
        .nargs(1)
        .default_value(1280)
        .scan<'i', int>();

    program.add_argument("--height")
        .help("Demo window height")
        .nargs(1)
        .default_value(720)
        .scan<'i', int>();

    program.add_argument("--no-ms")
        .help("Disable multiscattering computation.")
        .nargs(0)
        .implicit_value(true)
        .default_value(false);

    program.add_argument("--msaa")
        .help("Number of msaa samples per pixel.")
        .nargs(1)
        .default_value(8u)
        .scan<'u', unsigned int>();

    program.parse_args(argc, argv);

    CliOptions opts;
    opts.sceneFile = program.get("scene");
    opts.width = program.get<int>("--width");
    opts.height = program.get<int>("--height");
    opts.msaaSamples = program.get<unsigned int>("--msaa");
    opts.multiScattering = !program.get<bool>("--no-ms");

    return opts;
}