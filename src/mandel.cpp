#include "mandel.h"
#include "utility.h"
#include "shader.h"
#include "vertex_array_object.h"
#include "buffer_object.h"
#include "uniform.h"
#include "mandel_handler.h"

#include <GLFW/glfw3.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

namespace mandel
{   
    static GLFWwindow* s_window = nullptr;

    static gl::shader s_shader;
    static gl::uniform s_uScreenSize = gl::s_getUniform<2>(vec4<int>{ 640, 640 }, glUniform2i);

    static inline vec4<int> s_getMousePos() 
    {
        double mouseX, mouseY;
        glfwGetCursorPos(s_window, &mouseX, &mouseY);

        return { static_cast<int>(mouseX), static_cast<int>(mouseY) };
    }

    static inline void s_glfwFrameBufferSizeCallbackFunc(GLFWwindow* window, int width, int height)
    {
        const vec4<int> newScreenSize{ width, height };

        UpdateScreenSize(s_uScreenSize.vec(), newScreenSize);

        s_uScreenSize.setVec(newScreenSize);

        GLCALL(glViewport(0, 0, width, height));
    }

    static bool s_mouseButtonPressed = false;
    static vec4<int> s_mousePos;

    static void s_glfwCursorPosCallbackFunc(GLFWwindow* window, double mouseX, double mouseY)
    {   
        if(s_mouseButtonPressed)
        {   
            const vec4<int> currMousePos(mouseX, mouseY);
            // get elapsed mouse movement
            const vec4<float> elapsedMov = currMousePos - s_mousePos;

            // moving the start point according to elapsed mouse movement
            MoveMandel(elapsedMov);

            // update the old s_mousePos buffer
            s_mousePos = vec4<int>(mouseX, mouseY);
        }
    }

    static void s_glfwMouseButtonCallbackFunc(GLFWwindow* window, int button, int action, int mods)
    {
        if(ImGui::GetIO().WantCaptureMouse) return;

        if(button == GLFW_MOUSE_BUTTON_LEFT)
        {
            switch(action)
            {
            case GLFW_PRESS:
                s_mouseButtonPressed = true;
                s_mousePos = s_getMousePos();
            break;
            case GLFW_RELEASE:
                s_mouseButtonPressed = false;
            break;
            default:
            break;
            }
            
        }
    }

    static void s_glfwKeyCallbackFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
    {   
        switch(key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
        break;
        case GLFW_KEY_UP:
            ZoomMandel(0.99f, s_getMousePos(), s_uScreenSize.vec());
        break;
        case GLFW_KEY_DOWN:
            ZoomMandel(1.01f, s_getMousePos(), s_uScreenSize.vec());
        break;
        default:
        break;
        }

    }

    bool Init() 
    { 
        ASSERT(glfwInit() == GLFW_TRUE, "cannot init glfw");

        s_window = glfwCreateWindow(s_uScreenSize.vec().x, s_uScreenSize.vec().y, "Mandel", nullptr, nullptr);

        ASSERT(s_window, "cannot create a window");
            
        glfwMakeContextCurrent(s_window);

        //open vsync
        glfwSwapInterval(1);

        // init ImGui
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(s_window, true);
        ImGui_ImplOpenGL3_Init("#version 330"); // glsl version
        
        ImGui::StyleColorsDark();

        // call back functions for getting events
        glfwSetFramebufferSizeCallback(s_window, s_glfwFrameBufferSizeCallbackFunc);
        glfwSetMouseButtonCallback    (s_window, s_glfwMouseButtonCallbackFunc    );
        glfwSetCursorPosCallback      (s_window, s_glfwCursorPosCallbackFunc      );
        glfwSetKeyCallback            (s_window, s_glfwKeyCallbackFunc            );

        ASSERT(glewInit() == GLEW_OK, "cannot init glew");

        ASSERT(s_shader.m_createShaders(
            "res/vertex.glsl" , 
            "res/fragment.glsl"
        ), "cannot create shaders");

        s_shader.m_bind();

        CreateMandelUniforms(s_shader);

        ResetMandel(s_uScreenSize.vec());
        
        s_uScreenSize.m_create(s_shader, "u_vScreenSize");

        s_uScreenSize.m_update();

        return true;
    }

    static void s_DrawImGui()
    {
        static bool s_fpsLock = true;

        ImGui::Begin("Mandel");

        DrawUniforms_ImGui(s_uScreenSize.vec());

        if(ImGui::Checkbox("FPS Lock", &s_fpsLock)) glfwSwapInterval(static_cast<int>(s_fpsLock));
        
        ImGui::Text("Frame render time: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void Run()
    {  
        {
        // draw two big triangles to cover up the whole screen
        GLfloat triangleVertices[] = 
        {
            -1.0f,  1.0f,
             1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f, -1.0f
        };

        GLuint triangleIndices[] = 
        {
            0, 1, 2,
            1, 2, 3
        };

        gl::verArrObj vao;

        gl::bufferObject vbo;
        gl::bufferObject ebo;

        vao.m_create();
        vao.m_bind();
    
        vbo.m_create(GL_ARRAY_BUFFER        , triangleVertices, GL_STATIC_DRAW);
        ebo.m_create(GL_ELEMENT_ARRAY_BUFFER, triangleIndices , GL_STATIC_DRAW);

        vbo.m_bind();
        ebo.m_bind();

        vbo.m_setAndEnableVertex(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        s_shader.m_bind();

        while(!glfwWindowShouldClose(s_window))
        {
            glfwPollEvents();            

            //clear screen
            GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            GLCALL(glClear(GL_COLOR_BUFFER_BIT));

            //imgui clear screen
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


            //rendering
            GLCALL(glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, nullptr));
            
            s_DrawImGui();

            //render present
            ImGui::Render();

            glViewport(0, 0, s_uScreenSize.vec().x, s_uScreenSize.vec().x);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(s_window);
        }
        }
        // extra brackets for cleanup functions

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(s_window);
        glfwTerminate();
    }
}