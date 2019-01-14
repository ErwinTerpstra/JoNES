#include "stdafx.h"

#include "Program.h"

#include "Shader.h"

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
}

void Program::Prepare()
{
	Bind();
}

void Program::Attach(Shader* shader)
{
	GL_CHECK_ERROR(glAttachShader(handle, shader->handle));
}

void Program::Link()
{
	GL_CHECK_ERROR(glLinkProgram(handle));
}

void Program::Bind() const
{
	GL_CHECK_ERROR(glUseProgram(handle));
}

void Program::BindTexture(std::string uniformName, GLenum slot, GLuint texture)
{
	GL_CHECK_ERROR(glActiveTexture(slot));

	GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
	GL_CHECK_ERROR(glUniform1i(GetUniformLocation(uniformName), slot - GL_TEXTURE0));
}

void Program::SetVector3(const std::string& uniformName, const float* v)
{
	GL_CHECK_ERROR(glUniform3f(GetUniformLocation(uniformName), v[0], v[1], v[2]));
}

void Program::SetVector4(const std::string& uniformName, const float* v)
{
	GL_CHECK_ERROR(glUniform4f(GetUniformLocation(uniformName), v[0], v[1], v[2], v[3]));
}

void Program::SetMatrix44(const std::string& uniformName, const float* m)
{
	GL_CHECK_ERROR(glUniformMatrix4fv(GetUniformLocation(uniformName), 1, GL_FALSE, m));
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