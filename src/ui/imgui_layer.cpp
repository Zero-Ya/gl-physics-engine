#include "imgui_layer.h"

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

ImGuiLayer::ImGuiLayer(GLFWwindow* window)
{
    init(window);
}

ImGuiLayer::~ImGuiLayer()
{
    shutdown();
}

void ImGuiLayer::init(GLFWwindow* window)
{
    // Might want to add a window member for later

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiLayer::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// This will need a dedicated class later on
void ImGuiLayer::render(float& size, glm::vec4& color)
{
    ImGui::Begin("Window");
    ImGui::Text("Hello world!");
    ImGui::SliderFloat("Size", &size, 0.5f, 2.0f);
    ImGui::ColorEdit4("Color", &color.r);
    ImGui::End();
}

void ImGuiLayer::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}