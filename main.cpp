// ImGui - standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to ImGui, see examples/README.txt and
// documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.) (GL3W is a helper library to access OpenGL functions since
// there is no standard header to access modern OpenGL functions easily.
// Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "nanovg.h"
#include <GL/gl3w.h> // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include <iostream>
#include <stdio.h>

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

void frame(NVGcontext* vg, float width, float height, float mx, float my)
{

    const int N = 100;
    const float pi = 3.14;
    nvgBeginPath(vg);
    for (int i = 0; i < N; ++i) {
        const float x = 0.9 * width * i / N + 0.05 * width;
        const float y = 0.5 * height + 0.3 * height * std::sin(2 * pi * x / width);
        nvgLineTo(vg, x, y + 0.5 * height);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgFontBlur(vg, 2);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 128));
    nvgText(vg, width / 2, height / 2, "Thsi is a test", NULL);

    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
    {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!"); // Display some text (you can use a
                                      // format string too)
        ImGui::SliderFloat("float", &f, 0.0f,
            1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color",
            (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) // Buttons return true when clicked (NB: most
                                     // widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window. In most cases you will use an explicit
    // Begin/End pair to name your windows.
    ImGui::Begin("Another Window");
    ImGui::Text("Hello from another window!");
    ImGui::End();
}

GLFWwindow* create_window()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    return window;
}

void init_imgui(GLFWwindow* window)
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
    // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
    // Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
}

NVGcontext* init_nanovg() { return nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG); }

void begin_frame(GLFWwindow* window, NVGcontext* vg)
{
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    int winWidth, winHeight;
    int fbWidth, fbHeight;

    // glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    // Calculate pixel ration for hi-dpi devices.
    float pxRatio = (float)fbWidth / (float)winWidth;

    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
}

void end_frame(GLFWwindow* window, NVGcontext* vg)
{
    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    nvgEndFrame(vg);
    // Rendering
    glfwSwapBuffers(window);
}

void finalise(GLFWwindow* window, NVGcontext* vg)
{
    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    nvgDeleteGL3(vg);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int, char**)
{
    GLFWwindow* window = create_window();
    init_imgui(window);
    NVGcontext* vg = init_nanovg();

    if (!window)
        return 1;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        begin_frame(window, vg);

        int winWidth, winHeight;
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        glfwGetWindowSize(window, &winWidth, &winHeight);
        frame(vg, winWidth, winHeight, mx, my);

        end_frame(window, vg);
    }

    finalise(window, vg);

    return 0;
}
