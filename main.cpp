// ImGui - standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to ImGui, see examples/README.txt and
// documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.) (GL3W is a helper library to access OpenGL functions since
// there is no standard header to access modern OpenGL functions easily.
// Alternatives are GLEW, Glad, etc.)

// This example is using gl3w to access OpenGL functions
//(because it is small). You may use glew/glad/glLoadGen/etc. whatever already
//#include <GL/gl3w.h>
#include <glad/glad.h>
// works for you.
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
//#include "imgui.h"
//#include "imgui_impl_glfw_gl3.h"

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void drawEyes(NVGcontext* vg, float x, float y, float w, float h, float mx,
              float my, float t) {
    NVGpaint gloss, bg;
    float ex = w * 0.23f;
    float ey = h * 0.5f;
    float lx = x + ex;
    float ly = y + ey;
    float rx = x + w - ex;
    float ry = y + ey;
    float dx, dy, d;
    float br = (ex < ey ? ex : ey) * 0.5f;
    float blink = 1 - pow(sinf(t * 0.5f), 200) * 0.8f;

    bg = nvgLinearGradient(vg, x, y + h * 0.5f, x + w * 0.1f, y + h,
                           nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 16));
    nvgBeginPath(vg);
    nvgEllipse(vg, lx + 3.0f, ly + 16.0f, ex, ey);
    nvgEllipse(vg, rx + 3.0f, ry + 16.0f, ex, ey);
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    bg = nvgLinearGradient(vg, x, y + h * 0.25f, x + w * 0.1f, y + h,
                           nvgRGBA(220, 220, 220, 255),
                           nvgRGBA(128, 128, 128, 255));
    nvgBeginPath(vg);
    nvgEllipse(vg, lx, ly, ex, ey);
    nvgEllipse(vg, rx, ry, ex, ey);
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    dx = (mx - rx) / (ex * 10);
    dy = (my - ry) / (ey * 10);
    d = sqrtf(dx * dx + dy * dy);
    if (d > 1.0f) {
        dx /= d;
        dy /= d;
    }
    dx *= ex * 0.4f;
    dy *= ey * 0.5f;
    nvgBeginPath(vg);
    nvgEllipse(vg, lx + dx, ly + dy + ey * 0.25f * (1 - blink), br, br * blink);
    nvgFillColor(vg, nvgRGBA(32, 32, 32, 255));
    nvgFill(vg);

    dx = (mx - rx) / (ex * 10);
    dy = (my - ry) / (ey * 10);
    d = sqrtf(dx * dx + dy * dy);
    if (d > 1.0f) {
        dx /= d;
        dy /= d;
    }
    dx *= ex * 0.4f;
    dy *= ey * 0.5f;
    nvgBeginPath(vg);
    nvgEllipse(vg, rx + dx, ry + dy + ey * 0.25f * (1 - blink), br, br * blink);
    nvgFillColor(vg, nvgRGBA(32, 32, 32, 255));
    nvgFill(vg);

    gloss = nvgRadialGradient(vg, lx - ex * 0.25f, ly - ey * 0.5f, ex * 0.1f,
                              ex * 0.75f, nvgRGBA(255, 255, 255, 128),
                              nvgRGBA(255, 255, 255, 0));
    nvgBeginPath(vg);
    nvgEllipse(vg, lx, ly, ex, ey);
    nvgFillPaint(vg, gloss);
    nvgFill(vg);

    gloss = nvgRadialGradient(vg, rx - ex * 0.25f, ry - ey * 0.5f, ex * 0.1f,
                              ex * 0.75f, nvgRGBA(255, 255, 255, 128),
                              nvgRGBA(255, 255, 255, 0));
    nvgBeginPath(vg);
    nvgEllipse(vg, rx, ry, ex, ey);
    nvgFillPaint(vg, gloss);
    nvgFill(vg);
}

void frame(NVGcontext* vg, float width, float height, float mx, float my) {
    const int N = 100;
    const float pi = 3.14;
    nvgBeginPath(vg);
    for (int i = 0; i < N; ++i) {
        const float x = 0.9 * width * i / N + 0.05 * width;
        const float y =
            0.5 * height + 0.3 * height * std::sin(2 * pi * x / width);
        nvgLineTo(vg, x, y + 0.5 * height);
    }
    nvgStrokeColor(vg, nvgRGBA(32, 32, 32, 255));
    nvgStrokeWidth(vg, 10.0f);
    nvgStroke(vg);

    nvgFontBlur(vg, 2);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 128));
    nvgText(vg, width - 250, 50, "Thsi is a test", NULL);
    nvgText(vg, 1.0 / 2, 1.0 / 2, "Thsi is a test", NULL);

    drawEyes(vg, width - 250, 50, 150, 100, mx, my, 0);

    // 2. Show another simple window. In most cases you will use an explicit
    // Begin/End pair to name your windows.
    /*
    ImGui::Begin("Another Window");
    ImGui::Text("Hello from another window!");
    ImGui::End();
    */
}

GLFWwindow* create_window() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync
    // gl3wInit();
    // gladLoadGL();
    if (!gladLoadGL()) {
        exit(-1);
    }
    printf("OpenGL Version %d.%d loaded", GLVersion.major, GLVersion.minor);

    return window;
}

/*
void init_imgui(GLFWwindow* window) {
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
*/

NVGcontext* init_nanovg() {
    return nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
}

void begin_frame(GLFWwindow* window, NVGcontext* vg) {
    glfwPollEvents();
    // ImGui_ImplGlfwGL3_NewFrame();

    int winWidth, winHeight;
    int fbWidth, fbHeight;

    // glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    // Calculate pixel ration for hi-dpi devices.
    float pxRatio = (float)fbWidth / (float)winWidth;

    glViewport(0, 0, fbWidth, fbHeight);
    // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
}

void end_frame(GLFWwindow* window, NVGcontext* vg) {
    // ImGui::Render();
    // ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    nvgEndFrame(vg);
    // Rendering
    glfwSwapBuffers(window);
}

void finalise(GLFWwindow* window, NVGcontext* vg) {
    // Cleanup
    // ImGui_ImplGlfwGL3_Shutdown();
    // ImGui::DestroyContext();

    nvgDeleteGL3(vg);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int, char**) {
    GLFWwindow* window = create_window();
    // init_imgui(window);
    NVGcontext* vg = init_nanovg();

    if (!window) return 1;

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
