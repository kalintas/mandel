#pragma once

#include "utility.h"

namespace mandel::gl
{

    class bufferObject : public __baseObject
    {
    public:

        bufferObject();
        ~bufferObject();

        template<typename T>
        void m_create(const GLenum target, const T& buffer, const GLenum usage)
        {
            m_target = target;

            GLCALL(glGenBuffers(1, &m_id));
            m_bind();
            GLCALL(glBufferData(m_target, sizeof(buffer), buffer, usage));
            m_unbind();
        }

        void m_bind() const override;
        void m_unbind() const override;

        void m_setAndEnableVertex
        (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, std::size_t startPoint);

    private:

        GLenum m_target;
    };

}