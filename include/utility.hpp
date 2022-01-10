#pragma once

#include "pch.hpp"

#ifdef _DEBUG
    #define GLCALL(x) \
        { \
            mandel::gl::ClearErrors(); \
            x; \
            if (mandel::gl::TellError(#x, __FILE__, __LINE__)) \
                std::abort(); \
        }

    #define ASSERT(x, msg) \
        { \
            if (!(x)) { \
                std::cerr << "[ASSERT FAILED](in: " << __FILE__ \
                          << ", at: " << __LINE__ << "){" << #x \
                          << ", (info: " << msg ")}\n"; \
                std::abort(); \
            } \
        }
#else
    #define GLCALL(x) x
    #define ASSERT(x, msg) x
#endif

namespace mandel {

std::optional<std::size_t> GetFileSize(FILE* file);

std::optional<std::string> sGetTextFile(const std::string_view& sFilePath);

inline void PrintError(const std::string& error, std::ostream& o = std::cerr) {
    o << "[Error](" << error << ")\n";
}

namespace gl {
    inline void ClearErrors() {
        while (glGetError() != GL_NO_ERROR) {
        }
    }

    bool TellError(
        const std::string_view& sFuncName,
        const std::string_view& sFileName,
        const int Line);

    class __baseObject {
      public:
        virtual void m_bind() const = 0;
        virtual void m_unbind() const = 0;

      protected:
        GLuint m_id = 0;
    };

}  // namespace gl

template<typename T>
class vec4 {
  public:
    union {
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
    };

    vec4(const T& vx, const T& vy, const T& vz, const T& vw) :
        x(vx),
        y(vy),
        z(vz),
        w(vw) {}
    vec4(const T& vx, const T& vy, const T& vz) :
        vec4<T>::vec4(vx, vy, vz, 0) {}
    vec4(const T& vx, const T& vy) : vec4<T>::vec4(vx, vy, 0, 0) {}
    vec4(const T& v) : vec4<T>::vec4(v, v, v, v) {}
    vec4() : vec4<T>::vec4(0, 0, 0, 0) {}

    constexpr vec4<T> operator+(const vec4<T>& v) const noexcept {
        return {x + v.x, y + v.y, z + v.z, w + v.w};
    }
    constexpr vec4<T> operator-(const vec4<T>& v) const noexcept {
        return {x - v.x, y - v.y, z - v.z, w - v.w};
    }
    constexpr vec4<T> operator*(const vec4<T>& v) const noexcept {
        return {x * v.x, y * v.y, z * v.z, w * v.w};
    }
    constexpr vec4<T> operator/(const vec4<T>& v) const noexcept {
        return {x / v.x, y / v.y, z / v.z, w / v.w};
    }

    constexpr vec4<T>& operator+=(const vec4<T>& v) noexcept {
        return *this = *this + v;
    }
    constexpr vec4<T>& operator-=(const vec4<T>& v) noexcept {
        return *this = *this - v;
    }
    constexpr vec4<T>& operator*=(const vec4<T>& v) noexcept {
        return *this = *this * v;
    }
    constexpr vec4<T>& operator/=(const vec4<T>& v) noexcept {
        return *this = *this / v;
    }

    constexpr const T& operator[](const std::size_t i) const {
        if (i >= 4)
            throw "vec4<T>::operator[] out of range index";

        return *(&x + i);
    }
    constexpr T& operator[](const std::size_t i) {
        return const_cast<T&>(const_cast<const vec4<T>&>(*this)[i]);
    }

    template<typename newT>
    constexpr operator vec4<newT>() const {
        return {
            static_cast<newT>(x),
            static_cast<newT>(y),
            static_cast<newT>(z),
            static_cast<newT>(w)};
    }

    friend inline std::ostream& operator<<(std::ostream& o, const vec4<T>& v) {
        o << "[vec4<T>](x = " << v.x << ", y = " << v.y << ", z = " << v.z
          << ", w = " << v.w << ")";
        return o;
    }
};

}  // namespace mandel