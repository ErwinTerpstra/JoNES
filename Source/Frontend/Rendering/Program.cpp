#include "stdafx.h"

#include "Program.h"

#include "Shader.h"
#include "Texture.h"

#include "libnes/libnes.h"

using namespace JoNES;

Program::Program()
{
	handle = glCreateProgram();
}

Program::~Program()
{
	if (handle)
	{
		glDeleteProgram(handle);
		handle = 0;
	}

	SAFE_DELETE(vertexShader);
	SAFE_DELETE(fragmentShader);
}

void Program::Attach(Shader* shader)
{
	GL(glAttachShader(handle, shader->handle));
}

Shader* Program::CreateVertexShader()
{
	vertexShader = new Shader(GL_VERTEX_SHADER);
	return vertexShader;
}

Shader* Program::CreateFragmentShader()
{
	fragmentShader = new Shader(GL_FRAGMENT_SHADER);
	return fragmentShader;
}

void Program::Link()
{
	GL(glLinkProgram(handle));
}

void Program::Bind() const
{
	GL(glUseProgram(handle));
}

void Program::BindTexture(std::string uniformName, GLenum slot, Texture* texture)
{
	GL(glActiveTexture(slot));

	if (texture != NULL)
		texture->Bind();
	else
		Texture::ClearBoundTexture();

	GL(glUniform1i(GetUniformLocation(uniformName), slot - GL_TEXTURE0));
}

void Program::SetFloat(const std::string& uniformName, float f)
{
	GL(glUniform1f(GetUniformLocation(uniformName), f));
}

void Program::SetVector3(const std::string& uniformName, const float* v)
{
	GL(glUniform3f(GetUniformLocation(uniformName), v[0], v[1], v[2]));
}

void Program::SetVector4(const std::string& uniformName, const float* v)
{
	GL(glUniform4f(GetUniformLocation(uniformName), v[0], v[1], v[2], v[3]));
}

void Program::SetMatrix44(const std::string& uniformName, const float* m)
{
	GL(glUniformMatrix4fv(GetUniformLocation(uniformName), 1, GL_FALSE, m));
}

GLint Program::GetUniformLocation(const std::string& name)
{
	std::map<std::string, GLint>::iterator it = uniformLocations.find(name);
	
	if (it == uniformLocations.end())
	{
		GLint location = glGetUniformLocation(handle, name.c_str());
		uniformLocations[name] = location;

		return location;
	}

	return it->second;
}

GLint Program::GetAttribLocation(const std::string& name)
{
	std::map<std::string, GLint>::iterator it = attribLocations.find(name);

	if (it == attribLocations.end())
	{
		GLint location = glGetAttribLocation(handle, name.c_str());
		attribLocations[name] = location;

		return location;
	}

	return it->second;
}

void Program::SetAttribLocation(const std::string& name, GLuint location)
{
	glBindAttribLocation(handle, location, name.c_str());
	attribLocations[name] = location;
}

Program* Program::Create(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
{
	const GLchar* glslVersionString = "#version 410 core\n";
	
	Program* program = new Program();

	const GLchar* vertexShaderSources[2] = { glslVersionString, vertexShaderSource };
	const GLchar* fragmentShaderSources[2] = { glslVersionString, fragmentShaderSource };

	// Create shaders
	Shader* vertexShader = program->CreateVertexShader();
	if (!vertexShader->Compile(&vertexShaderSources[0], 2))
		return NULL;

	Shader* fragmentShader = program->CreateFragmentShader();
	if (!fragmentShader->Compile(&fragmentShaderSources[0], 2))
		return NULL;

	// Create program
	program->Attach(vertexShader);
	program->Attach(fragmentShader);
	program->Link();

	return program;
}