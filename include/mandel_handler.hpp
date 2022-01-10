#pragma once
#include "shader.hpp"

namespace mandel {
void CreateMandelUniforms(const gl::shader& shader);

void UpdateScreenSize(
    const vec4<float> oldScreenSize,
    const vec4<float> newScreenSize);

void ResetMandel(const vec4<float> screenSize);

void ZoomMandel(
    const float zoomFactor,
    vec4<float> mousePos,
    const vec4<float> screenSize);

void MoveMandel(const vec4<float> movement);

void DrawUniforms_ImGui(const vec4<int> screenSize);
}  // namespace mandel
