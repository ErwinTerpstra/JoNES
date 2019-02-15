#ifndef _QUAD_H_
#define _QUAD_H_

#include "libgl.h"

namespace JoNES
{
	class Program;

	class Quad
	{
	private:
		const uint32_t VERTEX_STRIDE = 5;
		const uint32_t VERTEX_COUNT = 4;
		const uint32_t TRIANGLE_COUNT = 2;

		const float VERTEX_DATA[20] =
		{
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		const uint16_t INDEX_DATA[6] = { 0, 1, 2, 0, 2, 3 };

		GLuint vaoHandle;
		GLuint vboHandle;
		GLuint elementsHandle;

	public:
		Quad();
		~Quad();

		void Render(Program* program);

	private:
		void Bind();
	};

}

#endif