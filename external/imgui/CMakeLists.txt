add_library(imgui STATIC
        include/imgui.cpp
        include/imgui_demo.cpp
        include/imgui_draw.cpp
        include/imgui_widgets.cpp
        include/imgui_tables.cpp
        include/backends/imgui_impl_glfw.cpp
        include/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC include)

target_link_libraries(imgui PUBLIC glfw3)