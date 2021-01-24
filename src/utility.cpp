#include "utility.h"


namespace mandel
{
    // why not
    std::optional<std::size_t> GetFileSize(FILE* file)
    {   
        std::size_t result;

        if(std::fseek(file, 0L, SEEK_END) != 0) return {};

        const long temp = std::ftell(file);
        if(temp < 0) return {};

        result = static_cast<std::size_t>(temp);

        if(std::fseek(file, 0L, SEEK_SET) != 0) return {};

        return result;
    }

    std::optional<std::string>  sGetTextFile(const std::string_view& sFilePath)
    {
        std::FILE* file = std::fopen(sFilePath.data(), "r");
        
        if(!file) return {};

        std::optional<std::size_t> fileSize = GetFileSize(file);
        if(!fileSize) return {};


        std::string result;
        result.reserve(fileSize.value() / sizeof(char));

        int c;
        while((c = std::fgetc(file)) != EOF) result.push_back(c);

        return result;
    }

    bool gl::TellError(const std::string_view& sFuncName, const std::string_view& sFileName, const int Line)
    {
        bool result = false;
        GLenum error;

        while((error = glGetError()) != GL_NO_ERROR)
        {   
            std::cerr << "[OpenGL Error]: (" << error << ") "
            << sFuncName.data() << "{In:" << sFileName.data() << " At:" << Line << "}\n"; 
            result = true;
        }

        return result;
    }

}