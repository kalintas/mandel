#include "mandel_handler.h"
#include "uniform.h"


namespace mandel
{
    namespace
    {
        // offset of the mandelbrot set
        auto startPos     = gl::s_getUniform<2, float>(glUniform2f);
        // addition per pixel in the complex plane
        auto increment    = gl::s_getUniform<2, float>(glUniform2f);
        // rotation vector for rotating the set
        auto rotation     = gl::s_getUniform<2, float>(glUniform2f);
        // max number to stop computing
        auto maxIteration = gl::s_getUniform<1, int  >(glUniform1i);
        

        auto colorPeriod   = gl::s_getUniform<1, float>(glUniform1f);
        auto bUseJuliaSet  = gl::s_getUniform<1, int  >(glUniform1i);
        auto juliaConstant = gl::s_getUniform<2, float>(glUniform2f); 
        auto exponent      = gl::s_getUniform<1, float>(glUniform1f);

        auto colorPalette = gl::s_getUniformArray<9, float>(glUniform3fv);

        float angle;

        // rotate a vector by rotation
        vec4<float> getRotated(vec4<float> vec)
        {
            const float oldX = vec.x;

            vec.x = vec.x * rotation.vec().x - vec.y * rotation.vec().y;
            vec.y = oldX  * rotation.vec().y + vec.y * rotation.vec().x;

            return vec;
        }

        // turn screen location to complex plane location
        vec4<float> getWorldLocation(vec4<float> pos, const vec4<float> screenSize)
        {
            return startPos.vec()  + getRotated((pos - screenSize / 2) * increment.vec());
        }

        void updateIncrement(const vec4<float> worldSpace, const vec4<float> screenSize)
        {
            vec4<float> inc{ worldSpace / screenSize };

            if(inc.x > inc.y) inc.y = inc.x;
            else              inc.x = inc.y;

            increment.setVec(inc);
        }

        constexpr float PI = 3.14159265359f;

        void setRotationVec(const float newAngle)
        {
            angle = newAngle;

            rotation.setVec({ std::cos(angle), std::sin(angle) });
        }
    }

    void CreateMandelUniforms(const gl::shader& shader)
    {
        startPos.m_create     (shader, "u_vStartPos"     );
        increment.m_create    (shader, "u_vIncrement"    );
        maxIteration.m_create (shader, "u_maxIteration"  );
        rotation.m_create     (shader, "u_vRotationVec"  );
        colorPalette.m_create (shader, "u_vColorPalette" );
        juliaConstant.m_create(shader, "u_vJuliaConstant");
        bUseJuliaSet.m_create (shader, "u_bUseJuliaSet"  );
        exponent.m_create     (shader, "u_exponent"      );
        colorPeriod.m_create  (shader, "u_colorPeriod"   );
    }

    void UpdateScreenSize(const vec4<float> oldScreenSize, const vec4<float> newScreenSize)
    {   
        updateIncrement(increment.vec() * oldScreenSize, newScreenSize);
    }

    void ResetMandel(const vec4<float> screenSize)
    {
        startPos.setVec    ({ 0.0f , 0.0f });
        rotation.setVec    ({ 1.0f, 0.0f }); // (cos0, sin0)
        maxIteration.setVec(100);

        updateIncrement({ 4.0f, 4.0f }, screenSize);

        angle = 0.0f;

        colorPalette.setVec({ 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f });

        bUseJuliaSet.setVec(false); // false
        juliaConstant.setVec({ 0.0f, 0.0f });

        exponent.setVec(2.0f);
        colorPeriod.setVec(0.1f);
    }
   

    // move the set by changing offset
    void MoveMandel(const vec4<float> movement)
    {   
        startPos.setVec(startPos.vec() - getRotated(movement * increment.vec()));
    }

    void ZoomMandel(const float zoomFactor, vec4<float> mousePos, const vec4<float> screenSize)
    {
        // mouse world location before the zoom
        const vec4<float> beforeZoom = getWorldLocation(mousePos, screenSize);

        // zoom 
        increment.setVec(increment.vec() * zoomFactor);

        // after the zoom location
        const vec4<float> afterZoom =  getWorldLocation(mousePos, screenSize);

        // subtract the difference from the offset
        startPos.setVec(startPos.vec() - (afterZoom - beforeZoom));
    }

    void DrawUniforms_ImGui(const vec4<int> screenSize)
    {
        float newAngle = angle;
        
        int newMaxIteration  = maxIteration.vec().x;
        float newColorPeriod = colorPeriod.vec().x;
        float newExponent    = exponent.vec().x;
        bool useJuliaSet   = bUseJuliaSet.vec().x;

        float* ptColorPallete =  const_cast<float*>(colorPalette.vec());

        if(ImGui::Button("*###ID0")) newAngle = 0.0f;

        ImGui::SameLine();

        ImGui::SliderFloat("Angle"        , &newAngle       , -PI, PI  );
        ImGui::SliderInt  ("Max Iteration", &newMaxIteration, 0  , 3000);

        if(ImGui::Button("Reset Mandel")) ResetMandel(screenSize);
                
        ImGui::ColorEdit3("ColorPalette[0]", ptColorPallete    );
        ImGui::ColorEdit3("ColorPalette[1]", ptColorPallete + 3);
        ImGui::ColorEdit3("ColorPalette[2]", ptColorPallete + 6);

        if(ImGui::Button("*###ID1")) newColorPeriod = 0.1f;

        ImGui::SameLine();

        ImGui::SliderFloat("Period", &newColorPeriod, 0.0f, 2.0f);
        
        if(ImGui::Button("*###ID2")) newExponent = 2.0f;

        ImGui::SameLine();

        ImGui::SliderFloat("Exponent", &newExponent, -10.0f, 10.0f);

        if(ImGui::Checkbox("Julia Set", &useJuliaSet)) bUseJuliaSet.setVec(useJuliaSet);


        if(newAngle != angle) setRotationVec(newAngle);

        maxIteration.setVec(newMaxIteration);
        colorPalette.m_update();
        colorPeriod.setVec(newColorPeriod);
        exponent.setVec(newExponent);

        if(useJuliaSet)
        {
            float* ptJuliaConstant = const_cast<float*>(&juliaConstant.vec().x);

            ImGui::SliderFloat("Julia Constant x:", ptJuliaConstant, -2.0f, 2.0f);
            ImGui::SliderFloat("Julia Constant y:", ptJuliaConstant + 1, -2.0f, 2.0f);

            juliaConstant.m_update();
        }

    }
}


