#include <Material.h>

#include <RenderInterface.h>

using namespace pbr;

void Material::use() const {
    RHI.useProgram(_prog);
}

RRID Material::program() const {
    return _prog;
}