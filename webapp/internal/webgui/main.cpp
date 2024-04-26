#include <stdio.h>
#include <format>
#include <iostream>
#include <fmt/core.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#define GL_SILENCE_DEPRECATION
// #define GLFW_INCLUDE_ES3
// #include <GLES3/gl3.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten_file_browser.h>
#include "./libs/emscripten/emscripten_mainloop_stub.h"
#else
#include "nfd.hpp"
#endif
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

GLFWwindow* g_window;
ImVec4 clear_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;
const char* glsl_version;
int g_width;
int g_height;

void PickUpAPencil()
{
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.82f, 0.38f, 1.00f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding                     = ImVec2(12.0f, 8.00f);
  style.FramePadding                      = ImVec2(5.00f, 2.00f);
  style.CellPadding                       = ImVec2(6.00f, 6.00f);
  style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
  style.IndentSpacing                     = 25;
  style.ScrollbarSize                     = 10;
  style.GrabMinSize                       = 10;
  style.WindowBorderSize                  = 0;
  style.ChildBorderSize                   = 0;
  style.PopupBorderSize                   = 0;
  style.FrameBorderSize                   = 0;
  style.TabBorderSize                     = 1;
  style.WindowRounding                    = 0;
  style.ChildRounding                     = 4;
  style.FrameRounding                     = 0;
  style.PopupRounding                     = 0;
  style.ScrollbarRounding                 = 0;
  style.GrabRounding                      = 0;
  style.LogSliderDeadzone                 = 4;
  style.TabRounding                       = 0;
}

#ifdef __EMSCRIPTEN__
// Function used by c++ to get the size of the html canvas
EM_JS(int, canvas_get_width, (), {
        return Module.canvas.width;
        });

// Function used by c++ to get the size of the html canvas
EM_JS(int, canvas_get_height, (), {
        return Module.canvas.height;
        });

// Function called by javascript
EM_JS(void, resizeCanvas, (), {
        js_resizeCanvas();
        });

void on_size_changed()
{
    glfwSetWindowSize(g_window, g_width, g_height);

    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}

void handle_upload_file(std::string const &filename, std::string const &mime_type, std::string_view buffer, void*) {
    // define a handler to process the file
    // ...
}
#endif

void loop()
{
    // TODO don't think I need this with canvas resize callback
    // #ifdef __EMSCRIPTEN__
    //     int width = canvas_get_width();
    //     int height = canvas_get_height();
    // 
    //     if (width != g_width || height != g_height)
    //     {
    //         g_width = width;
    //         g_height = height;
    //         on_size_changed();
    //     }
    // #endif

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(g_width - 12.f, 12.f), ImGuiCond_Once);
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);


    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoNav;
    ImGui::SetNextWindowPos(ImVec2(12.f, 12.f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300.f, 600.f), ImGuiCond_Once);
    if (!ImGui::Begin("Automated Segmentation Matting", NULL, window_flags))
    {
        ImGui::End();
        return;
    }
    if (ImGui::Button("Upload Video")) {
#ifdef __EMSCRIPTEN__
        emscripten_browser_file::upload(".mp4", handle_upload_file);
#else
        NFD::Guard nfdGuard;
        // auto-freeing memory
        NFD::UniquePath outPath;

        // prepare filters for the dialog
        nfdfilteritem_t filterItem[2] = {{"Source code", "c,cpp,cc"}, {"Headers", "h,hpp"}};
        // show the dialog
        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 2);
        if (result == NFD_OKAY) {
            std::cout << "Success!" << std::endl << outPath.get() << std::endl;
        } else if (result == NFD_CANCEL) {
            std::cout << "User pressed cancel." << std::endl;
        } else {
            std::cout << "Error: " << NFD::GetError() << std::endl;
        }
#endif
    }

    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    static float progress = 0.0f, progress_dir = 1.0f;
    static bool animate = true;
    if (animate)
    {
        progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
        if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
        if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
    }

    // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
    // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
    const char* empty = "";
    ImGui::ProgressBar(progress, ImVec2(100.0f, 0.0f), empty);
    ImGui::SameLine();
    ImGui::Text("%d%%", int(100. * IM_CLAMP(progress, 0.0f, 1.0f)));

    ImGui::Button("Save Session");
    ImGui::SameLine();
    ImGui::Button("Load Session");

    // if(ImGui::CollapsingHeader("Effects", ImGuiTreeNodeFlags_DefaultOpen))
    if(ImGui::CollapsingHeader("Effects"))
    {

    }

    ImGui::End();
    ImGui::Render();

    int display_w, display_h;
#ifdef __EMSCRIPTEN__
    glfwMakeContextCurrent(g_window);
#endif
    glfwGetFramebufferSize(g_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClearColor(clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#ifdef __EMSCRIPTEN__
    glfwMakeContextCurrent(g_window);
#else
    glfwSwapBuffers(g_window);
#endif
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int init_gl()
{
    glfwSetErrorCallback(glfw_error_callback);
    if( !glfwInit() )
    {
        fprintf(stderr, "Failed to initialize GLFW\n" );
        return 1;
    }
    // We don't want the old OpenGL
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Open a window and create its OpenGL context
    int canvasWidth = g_width;
    int canvasHeight = g_height;
    g_window = glfwCreateWindow(
            canvasWidth, canvasHeight,
            "Automated Segementation Matting", NULL, NULL);
    if( g_window == NULL )
    {
        fprintf(stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(g_window); // Initialize GLEW
    glfwSwapInterval(1); // Enable vsync

    return 0;
}


int init_imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    PickUpAPencil();


    ImGuiIO& io = ImGui::GetIO();

    // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Load Fonts
    io.Fonts->AddFontFromFileTTF("data/poppins.ttf", 16.0f);
    io.Fonts->AddFontDefault();
#ifdef __EMSCRIPTEN__
    // io.IniFilename = nullptr;
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    // TODO
    // resizeCanvas();
    return 0;
}


int init()
{
    init_gl();
    init_imgui();
    return 0;
}


void quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(g_window);
    glfwTerminate();
}


extern "C" int main(int argc, char** argv)
{
#ifdef __EMSCRIPTEN__
    g_width = canvas_get_width();
    g_height = canvas_get_height();
#else
    g_width = 1000;
    g_height = 1000;
#endif
    if (init() != 0) return 1;

#ifdef __EMSCRIPTEN__
    // emscripten_set_main_loop(loop, 0, 1);
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
        while (!glfwWindowShouldClose(g_window))
#endif
        {
            loop();
        }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif
    quit();

    return 0;
}
