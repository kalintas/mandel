#pragma once

#include <GL/glew.h>


#include <string>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <optional>
#include <cstdlib>

#ifdef _DEBUG
    #define GLCALL(x)\
        {\
            mandel::gl::ClearErrors();\
            x;\
            if(mandel::gl::TellError(#x, __FILE__, __LINE__)) std::abort();\
        }

    #define ASSERT(x, msg)\
        {\
            if(!(x))\
            {\
                std::cerr << "[ASSERT FAILED](in: " << __FILE__\
                << ", at: " << __LINE__ << "){" << #x << ", (info: " << msg ")}\n";\
                std::abort();\
            }\
        }
#else
    #define GLCALL(x) x
    #define ASSERT(x, msg) x
#endif

namespace mandel
{   

    std::optional<std::size_t> GetFileSize(FILE* file);

    std::optional<std::string> sGetTextFile(const std::string_view& sFilePath);

    inline void PrintError(const std::string& error, std::ostream& o = std::cerr) 
    { o << "[Error](" << error << ")\n"; }

    namespace gl
    {
        inline void ClearErrors() { while(glGetError() != GL_NO_ERROR) {} }
    
        bool TellError(const std::string_view& sFuncName, const std::string_view& sFileName, const int Line);

        class __baseObject
        {
        public:

            __baseObject()  {}
            ~__baseObject() {}

            virtual void m_bind()   const {}
            virtual void m_unbind() const {}

        protected:

            GLuint m_id = 0;
        };

    }
    
    template<typename T>
    class vec4
    {
    public:

        union
        {
            struct{ T x, y, z, w; };
            struct{ T r, g, b, a; };
        };

        vec4(const T& vx, const T& vy, const T& vz, const T& vw) : x(vx) , y(vy), z(vz), w(vw)       {}
        vec4(const T& vx, const T& vy, const T& vz)              : vec4<T>::vec4(vx , vy , vz , 0)   {}
        vec4(const T& vx, const T& vy)                           : vec4<T>::vec4(vx , vy , 0  , 0)   {}
        vec4(const T& v)                                         : vec4<T>::vec4(v  , v  , v  , v)   {}
        vec4(const vec4<T>& v)                                   : vec4<T>::vec4(v.x, v.y, v.z, v.w) {}
        vec4()                                                   : vec4<T>::vec4(0  , 0  , 0  , 0)   {}

        ~vec4() {}

        inline vec4<T> operator+ (const vec4<T>& v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }    
        inline vec4<T> operator- (const vec4<T>& v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
        inline vec4<T> operator* (const vec4<T>& v) const { return { x * v.x, y * v.y, z * v.z, w * v.w }; }    
        inline vec4<T> operator/ (const vec4<T>& v) const { return { x / v.x, y / v.y, z / v.z, w / v.w }; }
        
        inline vec4<T>& operator+= (const vec4<T>& v) { return *this = *this + v; }
        inline vec4<T>& operator-= (const vec4<T>& v) { return *this = *this - v; }
        inline vec4<T>& operator*= (const vec4<T>& v) { return *this = *this * v; }
        inline vec4<T>& operator/= (const vec4<T>& v) { return *this = *this / v; }

        inline const T& operator[] (const std::size_t i) const { return *((T*)this + (i % 4)); }
        inline       T& operator[] (const std::size_t i)       { return *((T*)this + (i % 4)); }

        // template<std::size_t i>
        // inline const T& at() const 
        // { 
        //     static_assert(i < 4, "vec4<T>::at out of range\n"); 
        //     return *((T*)this + i); 
        // }

        // template<std::size_t i>
        // inline T& at() { return const_cast<T&>(const_cast<const vec4<T>&>(*this).at<i>()); }


        template<typename newT>
        inline operator vec4<newT>() const 
        { 
            return { static_cast<newT>(x), static_cast<newT>(y), static_cast<newT>(z), static_cast<newT>(w) }; 
        }

        friend inline std::ostream& operator<< (std::ostream& o, const vec4<T>& v)
        {
            o << "[vec4<T>](x = " << v.x << ", y = " << v.y << ", z = " << v.z << ", w = " << v.w << ")";
            return o;
        }   
    };

}