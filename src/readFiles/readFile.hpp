#pragma once

#include <string> 

std::string parseFile(const std::string& filepath); 

struct ShaderSource {
	std::string vertexShader;
	std::string fragmentShader;
}; 

ShaderSource getShaders(
	const std::string& vertexPath,
	const std::string& fragmentPath
);