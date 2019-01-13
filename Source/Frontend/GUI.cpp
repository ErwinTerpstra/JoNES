#include "stdafx.h"

#include "GUI.h"

#include "Rendering/Program.h"
#include "Rendering/Shader.h" 

#include "imgui/imgui.h"

using namespace jones;

GUI::GUI()
{

}

bool GUI::Init()
{
	const GLchar* glslVersionString = "#version 410 core\n";

	const GLchar* vertexShaderSource =
		"layout (location = 0) in vec2 Position;\n"
		"layout (location = 1) in vec2 UV;\n"
		"layout (location = 2) in vec4 Color;\n"
		"uniform mat4 ProjMtx;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    Frag_UV = UV;\n"
		"    Frag_Color = Color;\n"
		"    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragmentShaderSource =
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"uniform sampler2D Texture;\n"
		"layout (location = 0) out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
		"}\n";
	
	const GLchar* vertexShaderSources[2] = { glslVersionString, vertexShaderSource };
	const GLchar* fragmentShaderSources[2] = { glslVersionString, fragmentShaderSource };

	// Create shaders
	vertexShader = new Shader(GL_VERTEX_SHADER);
	if (!vertexShader->Compile(vertexShaderSources, 2))
		return false;

	fragmentShader = new Shader(GL_FRAGMENT_SHADER);
	if (!fragmentShader->Compile(fragmentShaderSources, 2))
		return false;

	// Create program
	program = new Program();
	program->Attach(vertexShader);
	program->Attach(fragmentShader);
	program->Link();

	// Create buffers
	glGenBuffers(1, &vboHandle);
	glGenBuffers(1, &elementsHandle);

	// Create the font texture
	if (!CreateFontTexture())
		return false;

	return true;
}

void GUI::Shutdown()
{
	if (vertexShader)
	{
		delete vertexShader;
		vertexShader = NULL;
	}

	if (fragmentShader)
	{
		delete fragmentShader;
		fragmentShader = NULL;
	}

	if (program)
	{
		delete program;
		program = NULL;
	}

	if (fontTextureHandle)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->TexID = 0;

		glDeleteTextures(1, &fontTextureHandle);
		fontTextureHandle = 0;
	}
}

bool GUI::CreateFontTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;

	// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) 
	// because it is more likely to be compatible with user's existing shaders. 
	// If your ImTextureId represent a higher-level concept than just a GL texture id, 
	// consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   

	// Upload texture to graphics system
	glGenTextures(1, &fontTextureHandle);
	glBindTexture(GL_TEXTURE_2D, fontTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (ImTextureID)(intptr_t)fontTextureHandle;

	return fontTextureHandle != 0;
}
