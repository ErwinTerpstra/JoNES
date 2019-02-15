#include "stdafx.h"

#include "Quad.h"

#include "Program.h"

using namespace JoNES;

Quad::Quad()
{
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &vboHandle);
	glGenBuffers(1, &elementsHandle);
	
	Bind();

	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_COUNT * VERTEX_STRIDE, &VERTEX_DATA, GL_STATIC_DRAW));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * TRIANGLE_COUNT * 3, &INDEX_DATA, GL_STATIC_DRAW));
}

Quad::~Quad()
{
	glDeleteBuffers(1, &elementsHandle);
	glDeleteBuffers(1, &vboHandle);
	glDeleteVertexArrays(1, &vaoHandle);
}

void Quad::Render(Program* program)
{
	Bind();

	program->Bind();

	GLint positionAttribute = program->GetAttribLocation("Position");
	GLint uvAttribute = program->GetAttribLocation("UV");

	GL(glEnableVertexAttribArray(positionAttribute));
	GL(glEnableVertexAttribArray(uvAttribute));

	GL(glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)0));
	GL(glVertexAttribPointer(uvAttribute, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)2));
	
	GL(glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_SHORT, 0));
}

void Quad::Bind()
{
	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsHandle);
}