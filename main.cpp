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
#include <array>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void drawAxis(NVGcontext* vg, std::array<float, 4> window,
              std::array<float, 4> minmax) {
    const float& x = window[0];
    const float& y = window[1];
    const float& w = window[2];
    const float& h = window[3];

    const float& xmin = minmax[0];
    const float& ymin = minmax[1];
    const float& xmax = minmax[2];
    const float& ymax = minmax[3];

    nvgSave(vg);

    // axis lines
    const float lw = 5.0f;
    nvgBeginPath(vg);
    nvgMoveTo(vg, x - lw / 2, y + h);
    nvgLineTo(vg, x + w, y + h);
    nvgMoveTo(vg, x, y + h + lw / 2);
    nvgLineTo(vg, x, y);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(vg, lw);
    nvgStroke(vg);

    // drop shadow
    nvgBeginPath(vg);
    nvgMoveTo(vg, x - lw, y + h + lw / 2);
    nvgLineTo(vg, x + w - lw / 2, y + h + lw / 2);
    nvgMoveTo(vg, x - lw / 2, y + h + lw);
    nvgLineTo(vg, x - lw / 2, y + lw / 2);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 20));
    nvgStrokeWidth(vg, lw);
    nvgStroke(vg);

    // ticks
    const int ny_ticks = 5;
    const float tick_dx = h / ny_ticks;
    const float tick_dx_y = (ymax - ymin) / ny_ticks;
    const int nx_ticks = w / tick_dx;
    const float tick_len = 15.0f;
    const float tick_dx_x = (xmax - xmin) / nx_ticks;

    nvgBeginPath(vg);
    nvgFontSize(vg, 18.0f);
    nvgFontFace(vg, "sans-bold");
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 128));

    char buffer[10];
    for (int i = 0; i < nx_ticks; ++i) {
        const float tick_pos = x + (i + 0.5) * tick_dx;
        const float tick_val = xmin + (i + 0.5) * tick_dx_x;
        nvgMoveTo(vg, tick_pos, y + h + tick_len / 2);
        nvgLineTo(vg, tick_pos, y + h - tick_len / 2);
        std::sprintf(buffer, "%1.1f", tick_val);
        nvgText(vg, tick_pos, y + h + tick_len / 2 + 22, buffer, NULL);
    }
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    for (int i = 0; i < ny_ticks; ++i) {
        const float tick_pos = y + h - (i + 0.5) * tick_dx;
        const float tick_val = ymin + (i + 0.5) * tick_dx_y;
        nvgMoveTo(vg, x - tick_len / 2, tick_pos);
        nvgLineTo(vg, x + tick_len / 2, tick_pos);
        std::sprintf(buffer, "%1.1f", tick_val);
        nvgText(vg, x - tick_len / 2 - 27, tick_pos, buffer, NULL);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(vg, lw / 2);
    nvgStroke(vg);

    nvgRestore(vg);
}

template <typename F>
void drawPlot(NVGcontext* vg, std::array<float, 4> window,
              std::array<float, 4> minmax, int N, F f) {
    // TODO: use nanovg transforms so that the plots can be
    // drawn in the minmax coordinate system
    const float& x = window[0];
    const float& y = window[1];
    const float& w = window[2];
    const float& h = window[3];

    const float& xmin = minmax[0];
    const float& ymin = minmax[1];
    const float& xmax = minmax[2];
    const float& ymax = minmax[3];
    const float inv_yh = 1.0 / (ymax - ymin);
    const float xw = 1.0 / (xmax - xmin);

    nvgSave(vg);

    const float plot_y = f(xmin);
    const float win_y = y + h * (1 - (plot_y - ymin) * inv_yh);
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, win_y);
    for (int i = 1; i < N; ++i) {
        const float plot_x = xmin + xw * i / N;
        const float plot_y = f(plot_x);
        const float win_x = x + w * i / N;
        const float win_y = y + h * (1 - (plot_y - ymin) * inv_yh);
        nvgLineTo(vg, win_x, win_y);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 50, 100, 200));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    // attempt to do a drop shadow for the line
    const float offset = 1.5f;
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, win_y);
    for (int i = 1; i < N; ++i) {
        const float plot_x = xmin + xw * i / N;
        const float plot_y = f(plot_x);
        const float win_x = x + w * i / N;
        const float win_y = y + h * (1 - (plot_y - ymin) * inv_yh);
        nvgLineTo(vg, win_x - offset, win_y + offset);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 30));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgRestore(vg);
}

void frame(NVGcontext* vg, float width, float height, float mx, float my) {
    std::array<float, 4> axis_window = {50, 50, width - 100, height - 100};
    std::array<float, 4> plot_minmax = {0, -1.1, 1.1, 1};

    drawAxis(vg, axis_window, plot_minmax);

    static float freq1 = 1.0f;
    static float freq2 = 2.0f;
    drawPlot(vg, axis_window, plot_minmax, 1000, [&](auto x) {
        return std::sin(freq1 * 6.28 * x) * std::sin(freq2 * 6.28 * x);
    });

    ImGui::Begin("Slider Test");
    ImGui::SliderFloat("frequency 1", &freq1, 1.0f, 10.0f);
    ImGui::SliderFloat("frequency 2", &freq2, 1.0f, 10.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
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
        glfwCreateWindow(1280, 720, "C++ Plotting Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync
    // gl3wInit();
    // gladLoadGL();
    if (!gladLoadGL()) {
        exit(-1);
    }
    std::cout << "OpenGL Version " << GLVersion.major << '.' << GLVersion.minor
              << std::endl;

    return window;
}

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

NVGcontext* init_nanovg() {
    NVGcontext* vg =
        nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    int fontNormal = nvgCreateFont(vg, "sans", "../font/Roboto-Regular.ttf");
    if (fontNormal == -1) {
        printf("Could not add font italic.\n");
        return nullptr;
    }
    int fontBold = nvgCreateFont(vg, "sans-bold", "../font/Roboto-Bold.ttf");
    if (fontBold == -1) {
        printf("Could not add font bold.\n");
        return nullptr;
    }
    int fontEmoji = nvgCreateFont(vg, "emoji", "../font/NotoEmoji-Regular.ttf");
    if (fontEmoji == -1) {
        printf("Could not add font emoji.\n");
        return nullptr;
    }
    nvgAddFallbackFontId(vg, fontNormal, fontEmoji);
    nvgAddFallbackFontId(vg, fontBold, fontEmoji);
    return vg;
}

void begin_frame(GLFWwindow* window, NVGcontext* vg) {
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
    // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
}

void end_frame(GLFWwindow* window, NVGcontext* vg) {
    nvgEndFrame(vg);
    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    // Rendering
    glfwSwapBuffers(window);
}

void finalise(GLFWwindow* window, NVGcontext* vg) {
    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    nvgDeleteGL3(vg);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int, char**) {
    GLFWwindow* window = create_window();
    init_imgui(window);
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
