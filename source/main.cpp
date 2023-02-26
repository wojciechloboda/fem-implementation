// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "implot.h"
#include <iostream>
#include <math.h>
#include "solver.hpp"

#include "Eigen/Dense"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    //int n = 10;

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "FEM", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    double f = false;

    std::vector<double> xs;
    std::vector<double> ys;

    

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double insideBorderX = ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x * 0.2;

        {
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x * 0.2, ImGui::GetMainViewport()->Size.y));
            ImGuiWindowFlags flag = ImGuiWindowFlags_NoDecoration;
            ImGui::Begin("Button window", &show_demo_window, flag); 
            ImGui::Text("Number of elements:");
            static char text[10];
            ImGui::InputText("##source", text, 10);
            if(ImGui::Button("CREATE PLOT")){
                int n = atoi(text);
                if (n > 0){
                    auto fun = femAcusticVibrations(n);
                    xs = fun.first;
                    ys = fun.second;
                    f = true;
                }
            }
            ImGui::End();
        }

        {
            //setup();
            
            ImGui::SetNextWindowPos(ImVec2(insideBorderX, ImGui::GetMainViewport()->Pos.y));
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x * 0.8, ImGui::GetMainViewport()->Size.y));
            ImGuiWindowFlags flag = ImGuiWindowFlags_NoDecoration;
            ImGui::Begin("Plot", &show_demo_window, flag);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            if(f){
                if (ImPlot::BeginPlot("Acustic vibrations", ImVec2(ImGui::GetMainViewport()->Size.x * 0.8, ImGui::GetMainViewport()->Size.y))) {
                    ImPlot::SetupAxes("x","y");
                    ImPlot::PlotLine("u(x)", &xs[0], &ys[0], xs.size());
                    ImPlot::EndPlot();
                }
            }
            //Demo_LinePlots();

            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

