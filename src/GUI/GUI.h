#ifndef __PBR_GUI_H__
#define __PBR_GUI_H__

#include <OpenGLApplication.h>
#include <span>

namespace pbr {

void GuiInit(float width, float height);
void GuiBeginFrame(double mouseX, double mouseY, std::span<KeyState, 3> buttons);
void GuiResize(float width, float height);

} // namespace pbr

#endif