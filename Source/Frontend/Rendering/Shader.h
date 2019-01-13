#ifndef _SHADER_H_
#define _SHADER_H_

#include "libgl.h" 

namespace jones
{
	class Shader
	{

	public:
		GLuint handle;

	public:
		Shader(GLenum type);
		~Shader();
		
		bool Compile(const char** source, int amount);

	};
}

#endif