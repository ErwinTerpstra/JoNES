#include "stdafx.h"

#include "Quad.h"

#include "Program.h"

using namespace JoNES;

Quad::Quad()
{
	glGenVertexArrays(1, &vaoHandle);
	
	GL(glBindVertexArray(vaoHandle));

	GL(glGenBuffers(1, &vboHandle));
	GL(glBindBuffer(GL_ARRAY_BUFFER, vboHandle));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_COUNT * VERTEX_STRIDE, &VERTEX_DATA[0], GL_STATIC_DRAW));

	GL(glGenBuffers(1, &elementsHandle));
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsHandle));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * TRIANGLE_COUNT * 3, &INDEX_DATA[0], GL_STATIC_DRAW));

	GL(glBindVertexArray(0));
}

Quad::~Quad()
{
	glDeleteBuffers(1, &elementsHandle);
	glDeleteBuffers(1, &vboHandle);

	glDeleteVertexArrays(1, &vaoHandle);
}

void Quad::Render(Program* program)
{
	GLint positionAttribute = program->GetAttribLocation("position");

	GL(glBindVertexArray(vaoHandle));
	GL(glBindBuffer(GL_ARRAY_BUFFER, vboHandle));

	GL(glEnableVertexAttribArray(positionAttribute));

	GL(glVertexAttribPointer(positionAttribute,	3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*) 0));

	GL(glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_SHORT, (void*) 0));

	glBindVertexArray(0);
}
