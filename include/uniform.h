#pragma once

#include "shader.h"

#include <cstring>

namespace mandel::gl
{   
    struct __uniformBase
    {
    public:

        inline void m_create(const shader& _shader, const std::string_view& u_name)
        {
            m_id = _shader.m_getUniformLocation(u_name);

            if(m_id < 0) 
            { std::cerr << "uniform::m_create cannot find the (" << u_name << ") uniform\n"; }
        }

        virtual inline void m_update() const = 0;

    protected:

        GLint m_id = 0;
    };


    template<std::size_t u_size, typename T, typename funcType>
    struct uniform : public __uniformBase
    {
    public:

        uniform(const vec4<T>& v, const funcType& func) : m_func(func), m_vec(v) {}
        uniform(const funcType& func)                   : m_func(func)           {}

        ~uniform() {}

        inline const vec4<T>& vec() const { return m_vec; }

	    inline void setVec(const vec4<T>& v) { m_vec = v; m_update();}

        // update gl uniforms
        inline void m_update() const override
        {  
            static_assert(u_size < 4, "uniform::m_update uniform size cannot be larger than 4\n");

            if      constexpr (u_size == 1) { GLCALL(m_func(m_id, m_vec[0]));                               }
            else if constexpr (u_size == 2) { GLCALL(m_func(m_id, m_vec[0], m_vec[1]));                     }
            else if constexpr (u_size == 3) { GLCALL(m_func(m_id, m_vec[0], m_vec[1], m_vec[2]));           }
            else if constexpr (u_size == 4) { GLCALL(m_func(m_id, m_vec[0], m_vec[1], m_vec[2], m_vec[3])); }        
        }

    private:

        vec4<T> m_vec;
        const funcType& m_func;
    };

    // create a uniform object
    template<std::size_t u_size, typename T, typename funcType>
    inline uniform<u_size, T, funcType> s_getUniform(const funcType& func)
    {
        return uniform<u_size, T, funcType>(func);
    }

    template<std::size_t u_size, typename T, typename funcType>
    inline uniform<u_size, T, funcType> s_getUniform(const vec4<T>& v, const funcType& func)
    {
        return uniform<u_size, T, funcType>(v, func);
    }
    
    template<std::size_t count, typename T, typename funcType>
    struct uniformArray : public __uniformBase
    {
    public:

        typedef T bufferArray[count];

        uniformArray(const bufferArray& v, const funcType& func) : m_func(func), m_buffer(v) {}
        uniformArray(const funcType& func)                       : m_func(func)           {}

        ~uniformArray() {}

        inline const bufferArray& vec() const { return m_buffer; }

	    inline void setVec(const bufferArray& v) 
        { std::memcpy(m_buffer, v, sizeof(T) * count); m_update(); }

        inline void m_update() const override
        {  
            GLCALL(m_func(m_id, count, m_buffer));
        }

    private:

        bufferArray m_buffer;
        
        const funcType& m_func;
    };

    // create a uniformArray object
    template<std::size_t count, typename T, typename funcType>
    inline uniformArray<count, T, funcType> s_getUniformArray(const funcType& func)
    {
        return uniformArray<count, T, funcType>(func);
    }

    template<std::size_t count, typename T, typename funcType>
    inline uniformArray<count, T, funcType> s_getUniformArray
    (const typename uniformArray<count, T, funcType>::bufferArray& arr, const funcType& func)
    {
        return uniformArray<count, T, funcType>(arr, func);
    }

}
