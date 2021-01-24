#version 330 core

// for double
//#extension GL_ARB_gpu_shader_fp64 : enable

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform ivec2 u_vScreenSize;

// offset of the mandelbrot set
uniform vec2 u_vStartPos;
// addition per pixel in the complex plane
uniform vec2 u_vIncrement;

// rotation vector for rotating the set
uniform vec2 u_vRotationVec;
// max number to stop computing
uniform int u_maxIteration;

uniform bool u_bUseJuliaSet;
uniform vec2 u_vJuliaConstant;

uniform float u_exponent;

// rotate a vector by u_vRotationVec
vec2 GetRotated(vec2 vec)
{
    float oldX = vec.x;

    vec.x = vec.x * u_vRotationVec.x - vec.y * u_vRotationVec.y;
    vec.y = oldX  * u_vRotationVec.y + vec.y * u_vRotationVec.x;

    return vec;
}


uniform vec3 u_vColorPallete[4];

const int paletteMaxIndex = 2; // size - 1

uniform float u_colorPeriod;

void SetColor(const int n)
{   
    float v = (n / float(u_maxIteration)) * float(paletteMaxIndex);

    int i = int(v);
    float magic = (0.5f * sin(u_colorPeriod * n)) + 0.5f;

    int minVal = (i + 1 < paletteMaxIndex) ? i + 1 : paletteMaxIndex;

    gl_FragColor = vec4( u_vColorPallete[i] + (u_vColorPallete[minVal] - u_vColorPallete[i]) * magic, 1.0f);
}


void SetCurrentExponent(const vec2 startPos, const vec2 constant)
{
    int iteration = 0;
    
    float x = startPos.x, y = startPos.y;

    float x2, y2;

    while(
        iteration < u_maxIteration && 
        (x2 = (x * x)) + (y2 = (y * y)) <= 4.0f)
    {
        //https://en.wikipedia.org/wiki/Multibrot_set

        float atanVal = atan(y, x);
        float powVal  = pow(x2 + y2, u_exponent / 2.0f);

        x = powVal * cos(u_exponent * atanVal) + constant.x;
        y = powVal * sin(u_exponent * atanVal) + constant.y;

        ++iteration;
    }

    SetColor(iteration);
}

void SetCurrent(const vec2 startPos, const vec2 constant)
{
    int iteration = 0;
    
    float x = startPos.x, y = startPos.y;

    float x2, y2;

    while(
        iteration < u_maxIteration && 
        (x2 = (x * x)) + (y2 = (y * y)) <= 4.0f)
    {
        y = 2 * x * y + constant.y;
        x = x2 - y2   + constant.x;

        ++iteration;
    }

    SetColor(iteration);
}   



void main()
{      
    // complex plane location of the current pixel
    vec2 currentPixel = u_vStartPos + GetRotated((gl_FragCoord.xy - u_vScreenSize / 2) * u_vIncrement);

    if(u_exponent == 2.0f)
    {
        if(u_bUseJuliaSet) SetCurrent(currentPixel, u_vJuliaConstant);
        else               SetCurrent(currentPixel, currentPixel    );
    }
    else
    {
        if(u_bUseJuliaSet) SetCurrentExponent(currentPixel, u_vJuliaConstant);
        else               SetCurrentExponent(currentPixel, currentPixel    );
    }


}
