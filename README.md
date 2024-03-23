# Real Time PBR Shading Model

## Cloning and Submodules
Requires initialization of submodules on clone, using `--recurse-submodules`
```
git clone --recurse-submodules <repo>
```

Or after cloning
```
git submodule update --init
```

## Building

Requires a C++ compiler that is C++20 standard compliant and CMake. Example from within the repository's root:
```
mkdir build && cd build && cmake .. && cmake --build .
```

The glsl folder should have been automatically copied to the directory of the program:
```bash
...
├── build
│   ├── glsl
│   │   ├── common.fs
│   │   ├── pbr.vs
│   │   └── ...
│   ├── pbr
│   │   └── brdf.img
│   ├── pbr-sm
...
```

The brdf precomputation (brdf.img) should be moved to pbr folder as shown above. The brdf and integral precomputation into cubemaps can be done with the [iblenv-tool](https://github.com/fabio-dscar/iblenv-tool).