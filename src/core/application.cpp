#include "application.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>

glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float virtualWidth, float virtualHeight);

// Executes whenever window is resized
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->onWindowResize(width, height);
    }
}

Application::Application(const char* title, int width, int height)
    : m_Width(width), m_Height(height), m_Window(nullptr)
{
    initWindow(title);

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_VirtualWidth = m_VirtualHeight * aspectRatio;

    m_Timer = std::make_unique<Time>();
    Input::init(m_Window);

    m_ProfilerPanel = std::make_unique<ProfilerPanel>(120);
    m_ImGuiLayer = std::make_unique<ImGuiLayer>(m_Window);

    m_Renderer2D = std::make_unique<Renderer2D>();
    m_Renderer3D = std::make_unique<Renderer3D>();
    m_SpatialGrid = std::make_unique<SpatialGrid>(m_VirtualWidth, m_VirtualHeight, m_CellSize);

    m_CameraController = std::make_unique<PerspectiveCameraController>(1280.0f / 720.0f);

    // Renderer init
    m_Renderer2D->init();
    m_Renderer2D->setOrthographicProjection(0.0f, m_VirtualWidth, 0.0f, m_VirtualHeight);
    m_Renderer3D->init();

    // Physics engine init
    m_PhysicsEngine = std::make_unique<PhysicsEngine>(*m_SpatialGrid, *m_ProfilerPanel, *m_Renderer2D, *m_Renderer3D);
    m_PhysicsEngine->init(m_VirtualWidth, m_VirtualHeight, m_CellSize);
}

Application::~Application() {
    close();
}

bool Application::isRunning() const {
    return !glfwWindowShouldClose(m_Window);
}

void Application::clearScreen(float r, float g, float b, float a) const {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Application::swapBuffers() const {
    glfwSwapBuffers(m_Window);
}

void Application::onWindowResize(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);

    m_Width = width;
    m_Height = height;

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_VirtualWidth = m_VirtualHeight * aspectRatio;

    m_Renderer2D->setOrthographicProjection(0.0f, m_VirtualWidth, 0.0f, m_VirtualHeight);
    m_PhysicsEngine->resize(m_VirtualWidth, m_VirtualHeight, m_CellSize);
}

void Application::initWindow(const char* title) {
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW window creation
    m_Window = glfwCreateWindow(m_Width, m_Height, title, nullptr, nullptr);
    if (m_Window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    // OpenGL global state config
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::close() {
    glfwTerminate();
}

void Application::run() {
    float lastTotalFrameTimeMs = 0.0f;

    while (isRunning()) {
        Time::TimePoint frameStart = m_Timer->getCurrentTimePoint();
        m_Timer->update();
        float dt = m_Timer->getDeltaTime();
        // To prevent massive physics jumps during lag spikes
        if (dt > 0.1f) dt = 0.1f;
        //
        glfwPollEvents();

        // Camera update
        m_CameraController->onUpdate(dt);

        // Inputs stuff here
        processInput();

        m_Renderer2D->beginFrame();
        //m_Renderer3D->beginFrame(m_CameraController->getCamera());

        update(dt);

        render();

        m_Renderer2D->submit();

        // ImGui render /////////////
        m_ImGuiLayer->beginFrame();
        m_PhysicsEngine->renderUI(m_VirtualWidth, m_VirtualHeight, m_CellSize);
        //m_PhysicsEngine->renderProfilerPanel(lastTotalFrameTimeMs);
        //m_PhysicsEngine->renderSimulationControl(m_VirtualWidth, m_VirtualHeight, m_CellSize);
        m_ImGuiLayer->endFrame();
        /////////////////////////////

        //
        lastTotalFrameTimeMs = m_Timer->getElapsedTimeMs(frameStart);

        swapBuffers();
        Input::postUpdate();
    }
}

//void Application::renderCurrentSimulation() {
//    int width, height;
//    glfwGetFramebufferSize(m_Window, &width, &height);
//    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
//
//    if (m_CurrentSim->is3D()) {
//        // 3D render setup
//        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_CULL_FACE);
//
//        // Compute 3D View Projection
//        glm::mat4 view = m_Camera->getViewMatrix();
//        glm::mat4 proj = m_Camera->getProjectionMatrix(aspectRatio);
//
//        m_CurrentSim->render(view, proj);
//    }
//    else {
//        // 2D render setup
//        glDisable(GL_DEPTH_TEST);
//        glDisable(GL_CULL_FACE);
//
//        float orthoHeight = 10.0f;
//        glm::mat4 view = glm::mat4(1.0f);
//        glm::mat4 proj = glm::ortho(0.0f, orthoHeight * aspectRatio, 0.0f, orthoHeight, -1.0f, 1.0f);
//
//        m_CurrentSim->render(view, proj);
//    }
//
//    // Render ImGui UI on top of either pipeline
//    renderUI();
//}

void Application::update(float dt) {
    // Physics engine update
    m_PhysicsEngine->update(dt);
}

void Application::render() {
    clearScreen(0.2f, 0.3f, 0.3f, 1.0f);

    //m_Renderer3D->drawQuad({0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f});
    m_PhysicsEngine->render();
}

void Application::processInput() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
        close();
    }

    glm::vec2 clickPos = screenToWorld(Input::getMousePosition(), m_Width, m_Height, m_VirtualWidth, m_VirtualHeight);
    m_PhysicsEngine->processInput(clickPos, isLeftDragging, isRightDragging);
}

// 2D screen to world
glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float virtualWidth, float virtualHeight) {
    float normX = pos.x / windowWidth;
    float normY = pos.y / windowHeight;

    // Flip Y for bottom-left origin
    float worldX = normX * virtualWidth;
    float worldY = (1.0f - normY) * virtualHeight;

    return glm::vec2(worldX, worldY);
}

// 3D screen to world
//glm::vec3 getMouseRay3D(double mouseX, double mouseY, int screenWidth, int screenHeight, const glm::mat4& proj, const glm::mat4& view) {
//    // Normalized device coordinates (NDC)
//    float x = (2.0f * static_cast<float>(mouseX)) / screenWidth - 1.0f;
//    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenHeight;
//
//    // Unproject into 3D world space
//    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
//    glm::vec4 rayEye = glm::inverse(proj) * rayClip;
//    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
//
//    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
//    return rayWorld; // Returns ray direction vector from camera position
//}