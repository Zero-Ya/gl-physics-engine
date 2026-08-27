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

    m_Renderer = std::make_unique<Renderer2D>();
    m_SpatialGrid = std::make_unique<SpatialGrid>(m_VirtualWidth, m_VirtualHeight, m_CellSize);

    // Renderer init
    m_Renderer->init();
    m_Renderer->setOrthographicProjection(0.0f, m_VirtualWidth, 0.0f, m_VirtualHeight);

    // Physics engine init
    m_PhysicsEngine = std::make_unique<PhysicsEngine>(*m_SpatialGrid, *m_Renderer, *m_ProfilerPanel);
    m_PhysicsEngine->init();
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

    m_Renderer->setOrthographicProjection(0.0f, m_VirtualWidth, 0.0f, m_VirtualHeight);
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

        // Inputs stuff here
        processInput();

        //m_Renderer->beginFrame();

        update(dt);
        render();

        m_Renderer->submit();

        // ImGui render /////////////
        m_ImGuiLayer->beginFrame();
        m_PhysicsEngine->renderProfilerPanel(lastTotalFrameTimeMs);
        m_PhysicsEngine->renderSimulationControl(m_VirtualWidth, m_VirtualHeight, m_CellSize);
        m_ImGuiLayer->endFrame();
        /////////////////////////////

        //
        lastTotalFrameTimeMs = m_Timer->getElapsedTimeMs(frameStart);

        swapBuffers();
        Input::postUpdate();
    }
}

void Application::update(float dt) {
    // Physics engine update
    m_PhysicsEngine->update(dt);
}

void Application::render() {
    clearScreen(0.2f, 0.3f, 0.3f, 1.0f);

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

// Map pixels to a normalized device coordinate system (0.0 to 1.0)
glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float virtualWidth, float virtualHeight) {
    float normX = pos.x / windowWidth;
    float normY = pos.y / windowHeight;

    // Flip Y for bottom-left origin
    float worldX = normX * virtualWidth;
    float worldY = (1.0f - normY) * virtualHeight;

    return glm::vec2(worldX, worldY);
}