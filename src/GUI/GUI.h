#ifndef __PBR_GUI_H__
#define __PBR_GUI_H__

#include <imgui.h>

namespace pbr {

void GuiInit(float width, float height);
void GuiBeginFrame(int mouseX, int mouseY, bool mouseBtns[3]);
void GuiResize(float width, float height);

} // namespace pbr

#endif