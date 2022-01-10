#include "mandel.hpp"

#include "buffer_object.hpp"
#include "mandel_handler.hpp"
#include "shader.hpp"
#include "uniform.hpp"
#include "utility.hpp"
#include "vertex_array_object.hpp"

namespace mandel {
namespace {
    GLFWwindow* window = nullptr;

    gl::shader shader;
    gl::uniform uScreenSize =
        gl::s_getUniform<2>(vec4<int> {640, 640}, glUniform2i);

    vec4<int> getMousePos() {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        return {static_cast<int>(mouseX), static_cast<int>(mouseY)};
    }

    void
    glfwFrameBufferSizeCallbackFunc(GLFWwindow* window, int width, int height) {
        const vec4<int> newScreenSize {width, height};

        UpdateScreenSize(uScreenSize.vec(), newScreenSize);

        uScreenSize.setVec(newScreenSize);

        GLCALL(glViewport(0, 0, width, height));
    }

    bool mouseButtonPressed = false;
    vec4<int> mousePos;

    void glfwCursorPosCallbackFunc(
        GLFWwindow* window,
        double mouseX,
        double mouseY) {
        if (mouseButtonPressed) {
            const vec4<int> currMousePos(mouseX, mouseY);
            // get elapsed mouse movement
            const vec4<float> elapsedMov = currMousePos - mousePos;

            // moving the start point according to elapsed mouse movement
            MoveMandel(elapsedMov);

            // update the old mousePos buffer
            mousePos = vec4<int>(mouseX, mouseY);
        }
    }

    void glfwMouseButtonCallbackFunc(
        GLFWwindow* window,
        int button,
        int action,
        int mods) {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            switch (action) {
                case GLFW_PRESS:
                    mouseButtonPressed = true;
                    mousePos = getMousePos();
                    break;
                case GLFW_RELEASE:
                    mouseButtonPressed = false;
                    break;
                default:
                    break;
            }
        }
    }

    void glfwKeyCallbackFunc(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_UP:
                ZoomMandel(0.99f, getMousePos(), uScreenSize.vec());
                break;
            case GLFW_KEY_DOWN:
                ZoomMandel(1.01f, getMousePos(), uScreenSize.vec());
                break;
            default:
                break;
        }
    }

    void DrawImGui() {
        static bool fpsLock = true;

        ImGui::Begin("Mandel");

        DrawUniforms_ImGui(uScreenSize.vec());

        if (ImGui::Checkbox("FPS Lock", &fpsLock))
            glfwSwapInterval(static_cast<int>(fpsLock));

        ImGui::Text(
            "Frame render time: %.3f ms (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::End();
    }
}  // namespace

bool Init() {
    ASSERT(glfwInit() == GLFW_TRUE, "cannot init glfw");

    window = glfwCreateWindow(
        uScreenSize.vec().x,
        uScreenSize.vec().y,
        "Mandel",
        nullptr,
        nullptr);

    ASSERT(window, "cannot create a window");

    glfwMakeContextCurrent(window);

    //open vsync
    glfwSwapInterval(1);

    // init ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");  // glsl version

    ImGui::StyleColorsDark();

    // call back functions for getting events
    glfwSetFramebufferSizeCallback(window, glfwFrameBufferSizeCallbackFunc);
    glfwSetMouseButtonCallback(window, glfwMouseButtonCallbackFunc);
    glfwSetCursorPosCallback(window, glfwCursorPosCallbackFunc);
    glfwSetKeyCallback(window, glfwKeyCallbackFunc);

    ASSERT(glewInit() == GLEW_OK, "cannot init glew");

    ASSERT(
        shader.m_createShaders("res/vertex.glsl", "res/fragment.glsl"),
        "cannot create shaders");

    shader.m_bind();

    CreateMandelUniforms(shader);

    ResetMandel(uScreenSize.vec());

    uScreenSize.m_create(shader, "u_vScreenSize");

    uScreenSize.m_update();

    return true;
}

void Run() {
    {
        // draw two big triangles to cover up the whole screen
        GLfloat triangleVertices[] =
            {-1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f};

        GLuint triangleIndices[] = {0, 1, 2, 1, 2, 3};

        gl::verArrObj vao;

        gl::bufferObject vbo;
        gl::bufferObject ebo;

        vao.m_create();
        vao.m_bind();

        vbo.m_create(GL_ARRAY_BUFFER, triangleVertices, GL_STATIC_DRAW);
        ebo.m_create(GL_ELEMENT_ARRAY_BUFFER, triangleIndices, GL_STATIC_DRAW);

        vbo.m_bind();
        ebo.m_bind();

        vbo.m_setAndEnableVertex(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(float) * 2,
            0);

        shader.m_bind();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            //clear screen
            GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            GLCALL(glClear(GL_COLOR_BUFFER_BIT));

            //imgui clear screen
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //rendering
            GLCALL(
                glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr));

            DrawImGui();

            //render present
            ImGui::Render();

            glViewport(0, 0, uScreenSize.vec().x, uScreenSize.vec().x);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
    // extra brackets for cleanup functions

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
}  // namespace mandel