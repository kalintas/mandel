#include "shader.h"


namespace mandel::gl
{

    shader::shader(const std::string& sVertexFPath, const std::string& sFragmentFPath) 
    {
        if(!m_createShaders(sVertexFPath, sFragmentFPath))
        { throw "cannot create shaders"; }
    }

    shader::shader() {}

    shader::~shader() 
    {
        glDeleteProgram(m_id);
    }


    void shader::m_bind() const
    {   
        GLCALL(glUseProgram(m_id));
    }
    void shader::m_unbind() const
    {
        GLCALL(glUseProgram(0));
    }

    std::optional<GLuint> shader::s_compileShader(const GLenum type, const std::string& sShader)
    {

        const GLuint shaderId = glCreateShader(type);

        const GLchar* shaderSrc = sShader.c_str();

        GLCALL(glShaderSource(shaderId, 1, &shaderSrc, nullptr));
        GLCALL(glCompileShader(shaderId));

        //error checking and getting the error log
        GLint success;
        GLCALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success));

        if(success == GL_TRUE) return shaderId;

        GLint error_length;
        GLCALL(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &error_length));

        std::string sError;
        sError.resize(error_length);

        GLCALL(glGetShaderInfoLog(shaderId, error_length, &error_length, &sError[0]));
        PrintError("(CompileShader) : Cannot Create Shader.\n ErrorLog = " + sError);

        return {};
    }

    bool shader::m_createShader(const std::string_view& sFilePath, const GLenum type) const
    {   
        std::optional<std::string> sShader = sGetTextFile(sFilePath);

        if(!sShader) return false;

        std::optional<GLuint> shaderId = s_compileShader(type, sShader.value());

        if(!shaderId) return false;

        GLCALL(glAttachShader(m_id, shaderId.value()));

        GLCALL(glDeleteShader(shaderId.value()));

        return true;
    } 

    bool shader::m_createShaders(const std::string_view& sVertexFPath, const std::string_view& sFragmentFPath)
    {   

        GLCALL(m_id = glCreateProgram());
        
        const bool result = 
        m_createShader(sVertexFPath  , GL_VERTEX_SHADER  ) && 
        m_createShader(sFragmentFPath, GL_FRAGMENT_SHADER);

        GLCALL(glLinkProgram(m_id));
        GLCALL(glValidateProgram(m_id));

        return result;
    }

}