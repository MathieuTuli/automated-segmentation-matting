#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "./libs/emscripten/emscripten_mainloop_stub.h"
#endif

#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>


GLFWwindow* g_window;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;
const char* glsl_version;
int g_width;
int g_height;

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

    // 1. Show a simple window.
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&show_demo_window);
    }

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
    ImGui::StyleColorsDark();


    ImGuiIO& io = ImGui::GetIO();

    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Load Fonts
    // io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 23.0f);
    // io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 26.0f);
    // io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 32.0f);
    // io.Fonts->AddFontDefault();
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
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
    g_width = 500;
    g_height = 500;
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
