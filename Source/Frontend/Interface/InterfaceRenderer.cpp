#include "stdafx.h"

#include "InterfaceRenderer.h"

#include "Window.h"

#include "Rendering/Texture.h"
#include "Rendering/Program.h"
#include "Rendering/Shader.h" 

#include "libnes/libnes.h"
#include "imgui/imgui.h"

using namespace JoNES;

InterfaceRenderer::InterfaceRenderer(Window& window) : window(window)
{

}

bool InterfaceRenderer::Init()
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	if (!InitGraphicsObjects())
		return false;

	return true;
}

void InterfaceRenderer::Shutdown()
{
	DestroyGraphicsObjects();

	ImGui::DestroyContext();
}

void InterfaceRenderer::Render()
{
	static bool showDemoWindow = true;

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	ImGui::Render();
	Draw(ImGui::GetDrawData());
}

bool InterfaceRenderer::InitGraphicsObjects()
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

void InterfaceRenderer::DestroyGraphicsObjects()
{
	SAFE_DELETE(vertexShader);
	SAFE_DELETE(fragmentShader);
	SAFE_DELETE(program);
	SAFE_DELETE(fontTexture);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;
}

bool InterfaceRenderer::CreateFontTexture()
{
	// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) 
	// because it is more likely to be compatible with user's existing shaders. 
	// If your ImTextureId represent a higher-level concept than just a GL texture id, 
	// consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	ImGuiIO& io = ImGui::GetIO();

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Delete previous font texture
	SAFE_DELETE(fontTexture);

	// Instantiate new font
	fontTexture = new Texture(width, height, true);
	fontTexture->Load(pixels);
	
	// Store our identifier
	io.Fonts->TexID = (ImTextureID)fontTexture;

	return fontTexture->IsValid();
}

void InterfaceRenderer::Draw(ImDrawData* drawData)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int frameBufferWidth = (int)(drawData->DisplaySize.x * io.DisplayFramebufferScale.x);
	int frameBufferHeight = (int)(drawData->DisplaySize.y * io.DisplayFramebufferScale.y);
	if (frameBufferWidth <= 0 || frameBufferHeight <= 0)
		return;

	drawData->ScaleClipRects(io.DisplayFramebufferScale);
	
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Setup viewport, orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
	glViewport(0, 0, (GLsizei)frameBufferWidth, (GLsizei)frameBufferHeight);
	float L = drawData->DisplayPos.x;
	float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
	float T = drawData->DisplayPos.y;
	float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
	const float orthoProjection[4][4] =
	{
		{ 2.0f / (R - L),		0.0f,			 0.0f,   0.0f },
		{ 0.0f,					2.0f / (T - B),  0.0f,   0.0f },
		{ 0.0f,					0.0f,			-1.0f,   0.0f },
		{ (R + L) / (L - R),  (T + B) / (B - T), 0.0f,   1.0f },
	};

	program->Bind();
	program->SetMatrix44("ProjMtx", &orthoProjection[0][0]);
	program->BindTexture("Texture", GL_TEXTURE0, 0);

	glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
	
	// Recreate the VAO every time
	// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
	GLuint vaoHandle = 0;
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glEnableVertexAttribArray(program->GetAttribLocation("Position"));
	glEnableVertexAttribArray(program->GetAttribLocation("UV"));
	glEnableVertexAttribArray(program->GetAttribLocation("Color"));
	glVertexAttribPointer(program->GetAttribLocation("Position"), 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(program->GetAttribLocation("UV"), 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(program->GetAttribLocation("Color"), 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Draw
	ImVec2 pos = drawData->DisplayPos;
	for (int commandListIdx = 0; commandListIdx < drawData->CmdListsCount; commandListIdx++)
	{
		const ImDrawList* commandList = drawData->CmdLists[commandListIdx];
		const ImDrawIdx* indexBufferOffset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)commandList->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)commandList->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)commandList->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)commandList->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int commandIdx = 0; commandIdx < commandList->CmdBuffer.Size; commandIdx++)
		{
			const ImDrawCmd* pcmd = &commandList->CmdBuffer[commandIdx];
			if (pcmd->UserCallback)
			{
				// User callback (registered via ImDrawList::AddCallback)
				pcmd->UserCallback(commandList, pcmd);
			}
			else
			{
				ImVec4 clipRect = ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
				if (clipRect.x < frameBufferWidth && clipRect.y < frameBufferHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
				{
					// Apply scissor/clipping rectangle
					glScissor((int)clipRect.x, (int)(frameBufferHeight - clipRect.w), (int)(clipRect.z - clipRect.x), (int)(clipRect.w - clipRect.y));

					// Bind texture, Draw
					Texture* texture = (Texture*)pcmd->TextureId;
					program->BindTexture("Texture", GL_TEXTURE0, texture);

					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, indexBufferOffset);
				}
			}
			indexBufferOffset += pcmd->ElemCount;
		}
	}

	glDeleteVertexArrays(1, &vaoHandle);
}