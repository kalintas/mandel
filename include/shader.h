#pragma once

#include "utility.h"

namespace mandel::gl
{       

    class shader : public __baseObject
    {
    public:

        shader(const std::string& sVertexFPath, const std::string& sFragmentFPath);
        
        shader();
        ~shader();
    
        // compile and set shaders
        bool m_createShaders(const std::string_view& sVertexFPath, const std::string_view& sFragmentFPath);

        void m_bind()   const override;
        void m_unbind() const override;


        inline GLint m_getUniformLocation(const std::string_view& u_name) const 
        { return glGetUniformLocation(m_id, u_name.data()); }

    private:

        static std::optional<GLuint> s_compileShader(const GLenum type, const std::string& shader);  
        
        bool m_createShader(const std::string_view& sFilePath, const GLenum type) const;
    };

}