#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "libgl.h"

namespace JoNES
{
	class Shader;

	class Program
	{

	private:
		std::map<std::string, GLint> uniformLocations;
		std::map<std::string, GLint> attribLocations;

		GLuint handle;

	public:
		Program();
		~Program();

		void Prepare();

		void Attach(Shader* shader);
		void Link();
		void Bind() const;

		void BindTexture(std::string uniformName, GLenum slot, GLuint texture);
		
		void SetVector3(const std::string& uniformName, const float* v);
		void SetVector4(const std::string& uniformName, const float* v);
		void SetMatrix44(const std::string& uniformName, const float* m);

		GLint GetUniformLocation(const std::string& name);
		GLint GetAttribLocation(const std::string& name);

		void SetAttribLocation(const std::string& name, GLuint location);
	};
}

#endif