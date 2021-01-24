#include "mandel_handler.h"
#include "uniform.h"

#include <ImGui/imgui.h>



namespace mandel
{
    // offset of the mandelbrot set
    static gl::uniform s_startPos     = gl::s_getUniform<2, float>(glUniform2f);
    // addition per pixel in the complex plane
    static gl::uniform s_increment    = gl::s_getUniform<2, float>(glUniform2f);
    // rotation vector for rotating the set
    static gl::uniform s_rotation     = gl::s_getUniform<2, float>(glUniform2f);
    // max number to stop computing
    static gl::uniform s_maxIteration = gl::s_getUniform<1, int  >(glUniform1i);
    

    static gl::uniform s_colorPeriod   = gl::s_getUniform<1, float>(glUniform1f);
    static gl::uniform s_bUseJuliaSet  = gl::s_getUniform<1, int  >(glUniform1i);
    static gl::uniform s_juliaConstant = gl::s_getUniform<2, float>(glUniform2f); 
    static gl::uniform s_exponent      = gl::s_getUniform<1, float>(glUniform1f);

    static gl::uniformArray s_colorPallete = gl::s_getUniformArray<9, float>(glUniform3fv);

    static float s_angle;

    // rotate a vector by s_rotation
    static vec4<float> s_getRotated(vec4<float> vec)
    {
        const float oldX = vec.x;

        vec.x = vec.x * s_rotation.vec().x - vec.y * s_rotation.vec().y;
        vec.y = oldX  * s_rotation.vec().y + vec.y * s_rotation.vec().x;

        return vec;
    }

    // turn screen location to complex plane location
    static inline vec4<float> s_getWorldLocation(vec4<float> pos, const vec4<float> screenSize)
    {
        return s_startPos.vec()  + s_getRotated((pos - screenSize / 2) * s_increment.vec());
    }

    void CreateMandelUniforms(const gl::shader& shader)
    {
        s_startPos.m_create     (shader, "u_vStartPos"     );
        s_increment.m_create    (shader, "u_vIncrement"    );
        s_maxIteration.m_create (shader, "u_maxIteration"  );
        s_rotation.m_create     (shader, "u_vRotationVec"  );
        s_colorPallete.m_create (shader, "u_vColorPallete" );
        s_juliaConstant.m_create(shader, "u_vJuliaConstant");
        s_bUseJuliaSet.m_create (shader, "u_bUseJuliaSet"  );
        s_exponent.m_create     (shader, "u_exponent"      );
        s_colorPeriod.m_create  (shader, "u_colorPeriod"   );
    }


    static void s_updateIncrement(const vec4<float> worldSpace, const vec4<float> screenSize)
    {
        vec4<float> inc{ worldSpace / screenSize };

        if(inc.x > inc.y) inc.y = inc.x;
        else              inc.x = inc.y;

        s_increment.setVec(inc);
    }


    void UpdateScreenSize(const vec4<float> oldScreenSize, const vec4<float> newScreenSize)
    {   
        s_updateIncrement(s_increment.vec() * oldScreenSize, newScreenSize);
    }


    void ResetMandel(const vec4<float> screenSize)
    {
        s_startPos.setVec    ({ 0.0f , 0.0f });
        s_rotation.setVec    ({ 1.0f, 0.0f }); // (cos0, sin0)
        s_maxIteration.setVec(100);

        s_updateIncrement({ 4.0f, 4.0f }, screenSize);

        s_angle = 0.0f;

        s_colorPallete.setVec({ 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f });

        s_bUseJuliaSet.setVec(false); // false
        s_juliaConstant.setVec({ 0.0f, 0.0f });

        s_exponent.setVec(2.0f);
        s_colorPeriod.setVec(0.1f);
    }

    static constexpr float PI = 3.14159265359f;

    void s_setRotationVec(const float newAngle)
    {
        s_angle = newAngle;

        s_rotation.setVec({ std::cos(s_angle), std::sin(s_angle) });
    }

    // move the set by changing offset
    void MoveMandel(const vec4<float> movement)
    {   
        s_startPos.setVec(s_startPos.vec() - s_getRotated(movement * s_increment.vec()));
    }

    void ZoomMandel(const float zoomFactor, vec4<float> mousePos, const vec4<float> screenSize)
    {
        // mouse world location before the zoom
        const vec4<float> beforeZoom = s_getWorldLocation(mousePos, screenSize);

        // zoom 
        s_increment.setVec(s_increment.vec() * zoomFactor);

        // after the zoom location
        const vec4<float> afterZoom =  s_getWorldLocation(mousePos, screenSize);

        // subtract the difference from the offset
        s_startPos.setVec(s_startPos.vec() - (afterZoom - beforeZoom));
    }

    void DrawUniforms_ImGui(const vec4<int> screenSize)
    {
        float newAngle = s_angle;
        
        int newMaxIteration  = s_maxIteration.vec().x;
        float newColorPeriod = s_colorPeriod.vec().x;
        float newExponent    = s_exponent.vec().x;
        bool bUseJuliaSet   = s_bUseJuliaSet.vec().x;

        float* ptColorPallete =  const_cast<float*>(s_colorPallete.vec());

        if(ImGui::Button("*###ID0")) newAngle = 0.0f;

        ImGui::SameLine();

        ImGui::SliderFloat("Angle"        , &newAngle       , -PI, PI  );
        ImGui::SliderInt  ("Max Iteration", &newMaxIteration, 0  , 3000);

        if(ImGui::Button("Reset Mandel")) ResetMandel(screenSize);
                
        ImGui::ColorEdit3("ColorPallete[0]", ptColorPallete    );
        ImGui::ColorEdit3("ColorPallete[1]", ptColorPallete + 3);
        ImGui::ColorEdit3("ColorPallete[2]", ptColorPallete + 6);

        if(ImGui::Button("*###ID1")) newColorPeriod = 0.1f;

        ImGui::SameLine();

        ImGui::SliderFloat("Period", &newColorPeriod, 0.0f, 2.0f);
        
        if(ImGui::Button("*###ID2")) newExponent = 2.0f;

        ImGui::SameLine();

        ImGui::SliderFloat("Exponent", &newExponent, -10.0f, 10.0f);

        if(ImGui::Checkbox("Julia Set", &bUseJuliaSet)) s_bUseJuliaSet.setVec(bUseJuliaSet);


        if(newAngle != s_angle) s_setRotationVec(newAngle);

        s_maxIteration.setVec(newMaxIteration);
        s_colorPallete.m_update();
        s_colorPeriod.setVec(newColorPeriod);
        s_exponent.setVec(newExponent);

        if(bUseJuliaSet)
        {
            float* ptJuliaConstant = const_cast<float*>(&s_juliaConstant.vec().x);

            ImGui::SliderFloat("Julia Constant x:", ptJuliaConstant, -2.0f, 2.0f);
            ImGui::SliderFloat("Julia Constant y:", ptJuliaConstant + 1, -2.0f, 2.0f);

            s_juliaConstant.m_update();
        }

    }
}


