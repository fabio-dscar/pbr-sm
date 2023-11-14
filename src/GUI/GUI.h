#ifndef __PBR_GUI_H__
#define __PBR_GUI_H__

#include <imgui.h>

using namespace ImGui;

namespace pbr {

void ImGui_RenderDrawLists(ImDrawData* draw_data);
bool ImGui_CreateFontsTexture();
bool ImGui_CreateDeviceObjects();

void ImGui_Init(float width, float height);
void ImGui_NewFrame(int mouseX, int mouseY, bool mouseBtns[3]);

} // namespace pbr

#endif