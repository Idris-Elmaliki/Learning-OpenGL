#include "readFile.hpp"

#include <iostream>
#include <string> 
#include <sstream>
#include <fstream>

std::string parseFile(const std::string& filepath)
{
    std::ifstream stream = std::ifstream(filepath);

    if (!stream) {
        std::cerr << "File didn't open.\n";
        std::cout << filepath << std::endl;
        return std::string();
    }

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss;

    ShaderType type = ShaderType::NONE;

    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex")) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment")) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss << line << '\n';
        }
    }

    return ss.str();
}

ShaderSource getShaders(
    const std::string& vertexPath,
    const std::string& fragmentPath
)
{
    return 
    {
        parseFile(vertexPath),
        parseFile(fragmentPath)
    };
}