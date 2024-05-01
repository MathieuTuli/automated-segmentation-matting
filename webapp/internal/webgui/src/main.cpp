#include <iostream>
#include <vector>

#include "media_io.hpp"
#include "utils.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include "./libs/emscripten/emscripten_mainloop_stub.h"
#include <emscripten.h>
#include <emscripten_file_browser.h>
#else
#include "nfd.hpp"
#endif

#define IM_CLAMP(V, MN, MX) ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

int g_width;
int g_height;
GLFWwindow *g_window;
const char *glsl_version;
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);

#ifdef __EMSCRIPTEN__
// Function used by c++ to get the size of the html canvas
EM_JS(int, canvas_get_width, (), { return Module.canvas.width; });

// Function used by c++ to get the size of the html canvas
EM_JS(int, canvas_get_height, (), { return Module.canvas.height; });

// Function called by javascript
EM_JS(void, resizeCanvas, (), { js_resizeCanvas(); });

void on_size_changed() {
  glfwSetWindowSize(g_window, g_width, g_height);
  ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}

EMSCRIPTEN_KEEPALIVE void handle_file_upload(std::string const &filename,
                                             std::string const &mime_type,
                                             std::string_view buffer, void *) {
  // TODO this is not efficient:
  // It will load the file, then I clear it
  // need an alternative
  std::cout << filename << std::endl;
  load_file(filename.c_str());
}
#endif

template <class... T>
struct DeferredDisplay {
  std::tuple<T...> args;
  std::function<int(T...)> callable;
};


template <class... T>
std::vector<DeferredDisplay<T...>> DeferredRegister;

template <class... T>
void _RegisterForDeferredRender(std::function<void(T...)> callable, T &&...args)
{
  DeferredRegister<T...>.push_back(DeferredDisplay<T...>{
      std::make_tuple(std::forward<T>(args)...), callable});
}
template<typename Func, typename... Args>
void RegisterForDeferredRender(Func&& callable, Args&&... args) {
    using Signature = std::function<int(std::decay_t<Args>...)>;
    DeferredRegister<std::decay_t<Args>...>.push_back(DeferredDisplay<std::decay_t<Args>...>{
        std::make_tuple(std::forward<Args>(args)...), Signature(std::forward<Func>(callable))});
}

int ErrorModal(const char *title, const char *err) {
  // ImGui::SetNextItemOpen(true, ImGuiCond_Always);
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                          ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(200.f, 100.f), ImGuiCond_Always);
  ImGuiWindowFlags popup_flags = 0;
  popup_flags |= ImGuiWindowFlags_NoNav;
  popup_flags |= ImGuiWindowFlags_NoResize;
  int ret = 0;
  if (ImGui::BeginPopupModal(title, NULL, popup_flags)) {
    ImGui::BeginChild(
        "error message", ImVec2(ImGui::GetContentRegionAvail().x * 0.95f, 30.f),
        ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::Text("something went wrong %s", err);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::Button("o.k.")) {
      ImGui::CloseCurrentPopup();
      ret = 2;
    }
    ImGui::SameLine();
    if (ImGui::Button("report")) {
      ImGui::CloseCurrentPopup();
      ret = 1;
    }
    ImGui::EndPopup();
  }
  return ret;
}

void loop() {
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

  ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Size.x - 12.f, 12.f),
                          ImGuiCond_Once);
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

  ImGuiWindowFlags window_flags = 0;
  // window_flags |= ImGuiWindowFlags_NoCollapse;
  window_flags |= ImGuiWindowFlags_NoNav;
  ImGui::SetNextWindowPos(ImVec2(12.f, 12.f), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(300.f, 600.f), ImGuiCond_Once);
  // if (!ImGui::Begin("Menu", NULL, window_flags))
  // {
  //     ImGui::End();
  //     return;
  // }
  ImGui::Begin("menu", NULL, window_flags);
#ifdef __EMSCRIPTEN__
  if (ImGui::Button("upload video")) {
    emscripten_browser_file::upload(".mp4,.mov,.png,.jpg", handle_file_upload);
  }
#else
  nfdresult_t result = NFD_INVALID;
  NFD::UniquePath outPath;
  if (ImGui::Button("upload video")) {
    NFD::Guard nfdGuard;
    nfdfilteritem_t filterItem[1] = {{"Media", "mp4,mov,png,jpg"}};
    result = NFD::OpenDialog(outPath, filterItem, 1);
  }
  if (result == NFD_OKAY) {
    std::cout << "uploading file:" << std::endl << outPath.get() << std::endl;
    load_file(outPath.get());
  } else if (result == NFD_CANCEL) {
    std::cout << "file upload cancelled" << std::endl;
    // ErrorModal("file upload error", "oopsie");
    RegisterForDeferredRender(ErrorModal, "file upload error", "err");
    ImGui::OpenPopup("file upload error");
  } else if (result == NFD_ERROR) {
    std::cout << "file upload error" << NFD::GetError() << std::endl;
  }
#endif
  // ErrorModal("test", "oopsie");

  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  static float progress = 0.0f, progress_dir = 1.0f;
  static bool animate = true;
  if (animate) {
    progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
    if (progress >= +1.1f) {
      progress = +1.1f;
      progress_dir *= -1.0f;
    }
    if (progress <= -0.1f) {
      progress = -0.1f;
      progress_dir *= -1.0f;
    }
  }

  // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use
  // all available width, or ImVec2(width,0.0f) for a specified width.
  // ImVec2(0.0f,0.0f) uses ItemWidth.
  const char *empty = "";
  ImGui::ProgressBar(progress, ImVec2(100.0f, 0.0f), empty);
  ImGui::SameLine();
  ImGui::Text("%d%%", int(100. * IM_CLAMP(progress, 0.0f, 1.0f)));

  ImGui::Button("save session");
  ImGui::SameLine();
  ImGui::Button("lod session");

  // if(ImGui::CollapsingHeader("Effects", ImGuiTreeNodeFlags_DefaultOpen))
  if (ImGui::CollapsingHeader("effects")) {
  }
  for (const DeferredDisplay<const char*, const char*> &deferred : DeferredRegister<const char*, const char*>) {
      std::apply(deferred.callable, deferred.args);
  }
  int i = 0;
  while (i < DeferredRegister<const char*, const char*>.size())
  {
      int ret = std::apply(DeferredRegister<const char*, const char*>[i].callable,
                           DeferredRegister<const char*, const char*>[i].args);
      if (ret)
          DeferredRegister<const char*, const char*>.erase(DeferredRegister<const char*, const char*>.begin() + i);
      else
          ++i;
  }
  ImGui::End();
  ImGui::Render();
#ifdef __EMSCRIPTEN__
  glfwMakeContextCurrent(g_window);
#endif
  int display_w, display_h;
  glfwGetFramebufferSize(g_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#ifdef __EMSCRIPTEN__
  glfwMakeContextCurrent(g_window);
#else
  glfwSwapBuffers(g_window);
#endif
}

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int init_gl() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
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
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Open a window and create its OpenGL context
  int canvasWidth = g_width;
  int canvasHeight = g_height;
  g_window = glfwCreateWindow(canvasWidth, canvasHeight,
                              "Automated Segementation Matting", NULL, NULL);
  if (g_window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(g_window); // Initialize GLEW
  glfwSwapInterval(1);              // Enable vsync

  return 0;
}

int init_imgui() {
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(g_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Setup style
  PickUpAPencil();

  ImGuiIO &io = ImGui::GetIO();

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

int init() {
  init_gl();
  init_imgui();
  return 0;
}

void quit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

extern "C" int main(int argc, char **argv) {
#ifdef __EMSCRIPTEN__
  g_width = canvas_get_width();
  g_height = canvas_get_height();
#else
  g_width = 1000;
  g_height = 750;
#endif
  if (init() != 0)
    return 1;

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
