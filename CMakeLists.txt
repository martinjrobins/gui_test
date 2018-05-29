cmake_minimum_required(VERSION 3.10)
project(gui_test)

set(imgui_dir imgui)
set(imgui_source
    ${imgui_dir}/imgui.cpp
    ${imgui_dir}/imgui.h
    ${imgui_dir}/imgui_draw.cpp
    ${imgui_dir}/imgui_demo.cpp
    ${imgui_dir}/imgui_internal.h
    ${imgui_dir}/imconfig.h
    ${imgui_dir}/stb_rect_pack.h
    ${imgui_dir}/stb_textedit.h
    ${imgui_dir}/stb_truetype.h
    ${imgui_dir}/imgui_impl_glfw_gl3.h
    ${imgui_dir}/imgui_impl_glfw_gl3.cpp
    ${imgui_dir}/gl3w/GL/gl3w.c
    ${imgui_dir}/gl3w/GL/gl3w.h
    )

set(nanovg_dir nanovg)
set(nanovg_source
    ${nanovg_dir}/fontstash.h
    ${nanovg_dir}/nanovg.c
    ${nanovg_dir}/nanovg_gl.h
    ${nanovg_dir}/nanovg_gl_utils.h
    ${nanovg_dir}/nanovg.h
    ${nanovg_dir}/stb_image.h
    ${nanovg_dir}/stb_truetype.h
    )


find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(imgui ${imgui_source})
target_include_directories(imgui PUBLIC ${imgui_dir} ${imgui_dir}/gl3w)
target_link_libraries(imgui PUBLIC ${OPENGL_gl_LIBRARY} glfw dl)

add_library(nanovg ${nanovg_source})
target_include_directories(nanovg PUBLIC ${nanovg_dir})
target_link_libraries(nanovg PUBLIC ${OPENGL_gl_LIBRARY} glfw dl)

add_executable(demo test.cpp)
target_link_libraries(demo imgui nanovg)

add_executable(example main.cpp)
target_link_libraries(example imgui nanovg)
