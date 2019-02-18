#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "libgl.h"

namespace JoNES
{
	class Shader;
	class Texture;

	class Program
	{

	private:
		std::map<std::string, GLint> uniformLocations;
		std::map<std::string, GLint> attribLocations;

		GLuint handle;

		Shader* vertexShader;
		Shader* fragmentShader;

	public:
		Program();
		~Program();
		
		void Attach(Shader* shader);
		Shader* CreateVertexShader();
		Shader* CreateFragmentShader();

		void Link();
		void Bind() const;

		void BindTexture(std::string uniformName, GLenum slot, Texture* texture);
		
		void SetFloat(const std::string& uniformName, float f);
		void SetVector3(const std::string& uniformName, const float* v);
		void SetVector4(const std::string& uniformName, const float* v);
		void SetMatrix44(const std::string& uniformName, const float* m);

		GLint GetUniformLocation(const std::string& name);
		GLint GetAttribLocation(const std::string& name);

		void SetAttribLocation(const std::string& name, GLuint location);

		static Program* Create(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
	};
}

#endif