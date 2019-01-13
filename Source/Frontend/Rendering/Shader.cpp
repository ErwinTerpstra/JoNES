#include "stdafx.h"

#include "shader.h"

using namespace jones;

Shader::Shader(GLenum type)
{
	handle = glCreateShader(type);
}

Shader::~Shader()
{
	if (handle)
	{
		glDeleteShader(handle);
		handle = 0;
	}
}

bool Shader::Compile(const char** source, int amount)
{
	glShaderSource(handle, amount, source, NULL);
	glCompileShader(handle);

	int status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	if (!status) 
	{
		printf("[Shader]: Error in compiling shader\n");

		GLchar log[10240];
		GLsizei length;
		glGetShaderInfoLog(handle, 10239, &length, log);
		
		printf("[Shader]: Compiler log : \n%s\n", log);

		printf("[Shader]: Source:\n");

		for (int sourceIdx = 0; sourceIdx < amount; ++sourceIdx)
			printf(source[amount]);

		return false;
	}

	return true;
}