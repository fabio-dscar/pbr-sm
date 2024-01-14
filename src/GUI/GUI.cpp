#include <GUI.h>

#include <glad/glad.h>
#include <imgui.h>

#include <Utils.h>

using namespace ImGui;

namespace {

GLint g_ShaderHandle;
int g_AttribLocationTex, g_AttribLocationProjMtx, g_VertHandle, g_FragHandle;
int g_AttribLocationPosition, g_AttribLocationUV, g_AttribLocationColor;
unsigned int g_VaoHandle, g_VboHandle, g_ElementsHandle, g_FontTexture;

struct ImVec3 {
    float x, y, z;
    ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }
};

void imgui_easy_theming(ImVec3 color_for_text, ImVec3 color_for_head,
                        ImVec3 color_for_area, ImVec3 color_for_body,
                        ImVec3 color_for_pops) {
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 0.80f;

    style.Colors[ImGuiCol_Text] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg] =
        ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.95f);
    style.Colors[ImGuiCol_ChildWindowBg] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.58f);
    style.Colors[ImGuiCol_Border] =
        ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] =
        ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
    style.Colors[ImGuiCol_FrameBg] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.21f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    // style.Colors[ImGuiCol_ComboBg] = ImVec4(color_for_area.x, color_for_area.y,
    // color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_SliderGrabActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Button] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_ButtonHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Header] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Column] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.32f);
    style.Colors[ImGuiCol_ColumnHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ColumnActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.15f);
    style.Colors[ImGuiCol_ResizeGripHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_CloseButton] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.16f);
    style.Colors[ImGuiCol_CloseButtonHovered] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.39f);
    style.Colors[ImGuiCol_CloseButtonActive] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] =
        ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] =
        ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.63f);
    style.Colors[ImGuiCol_PopupBg] =
        ImVec4(color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f);
    style.Colors[ImGuiCol_ModalWindowDarkening] =
        ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.73f);
}

void SetupImGuiStyle2() {
    static ImVec3 color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
    static ImVec3 color_for_head = ImVec3(110.f / 255.f, 110.f / 255.f, 110.f / 255.f);
    static ImVec3 color_for_area = ImVec3(60.f / 255.f, 60.f / 255.f, 60.f / 255.f);
    static ImVec3 color_for_body = ImVec3(48.f / 255.f, 48.f / 255.f, 48.f / 255.f);
    static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    // static ImVec3 color_for_slider_button = ImVec3(255.f / 255.f, 144.f / 255.f, 37.f /
    // 255.f);
    imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body,
                       color_for_pops);
}

void ImGui_RenderDrawLists(ImDrawData* draw_data) {
    // Avoid rendering when minimized, scale coordinates for retina displays
    // (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLenum last_active_texture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLint last_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_sampler;
    glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
    GLint last_array_buffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_polygon_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    GLint last_viewport[4];
    glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLenum last_blend_src_rgb;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb;
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha;
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb;
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha;
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing,
    // scissor enabled, polygon fill
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    const float ortho_projection[4][4] = {
        {2.0f / io.DisplaySize.x, 0.0f,                     0.0f,  0.0f},
        {0.0f,                    2.0f / -io.DisplaySize.y, 0.0f,  0.0f},
        {0.0f,                    0.0f,                     -1.0f, 0.0f},
        {-1.0f,                   1.0f,                     0.0f,  1.0f},
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(g_VaoHandle);
    glBindSampler(0, 0); // Rely on combined texture/sampler state.

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                     (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                     (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w),
                          (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                          (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                               sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT
                                                      : GL_UNSIGNED_INT,
                               idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindSampler(0, last_sampler);
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha,
                        last_blend_dst_alpha);

    if (last_enable_blend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    if (last_enable_cull_face)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (last_enable_depth_test)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (last_enable_scissor_test)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2],
               (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2],
              (GLsizei)last_scissor_box[3]);
}

bool ImGui_CreateFontsTexture() {
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;

    io.Fonts->GetTexDataAsRGBA32(
        &pixels, &width,
        &height); // Load as RGBA 32-bits (75% of the memory is wasted, but default font
                  // is so small) because it is more likely to be compatible with user's
                  // existing shaders. If your ImTextureId represent a higher-level
                  // concept than just a GL texture id, consider calling
                  // GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 pixels);

    // Store our identifier
    io.Fonts->TexID = (void*)(intptr_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

bool ImGui_CreateDeviceObjects() {
    // Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar* vertex_shader = "#version 330\n"
                                  "uniform mat4 ProjMtx;\n"
                                  "in vec2 Position;\n"
                                  "in vec2 UV;\n"
                                  "in vec4 Color;\n"
                                  "out vec2 Frag_UV;\n"
                                  "out vec4 Frag_Color;\n"
                                  "void main()\n"
                                  "{\n"
                                  "	Frag_UV = UV;\n"
                                  "	Frag_Color = Color;\n"
                                  "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
                                  "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE*)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                          (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                          sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    ImGui_CreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

} // namespace

void pbr::GuiInit(float width, float height) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = width;
    io.DisplaySize.y = height;
    io.RenderDrawListsFn = ImGui_RenderDrawLists;

    SetupImGuiStyle2();

    ImGui_CreateDeviceObjects();
}

void pbr::GuiBeginFrame(double mouseX, double mouseY, std::span<KeyState, 3> buttons) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(mouseX, mouseY);

    // 0 = left, 1 = right, 2 = middle + extras
    io.MouseDown[0] = ToUnderlying(buttons[0]);
    io.MouseDown[1] = ToUnderlying(buttons[2]);
    io.MouseDown[2] = ToUnderlying(buttons[1]);

    ImGui::NewFrame();
}

void pbr::GuiResize(float width, float height) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = width;
    io.DisplaySize.y = height;
}