#pragma once

#include "Renderer.h"

#include <epoxy/gl.h>

struct OscillatorParameters;

//---------------------------------
// OscillatorRenderer
//
// Renders an oscillator
//
class OscillatorRenderer : public I_Renderer
{
public:
	OscillatorRenderer(OscillatorParameters const& osc);
	virtual ~OscillatorRenderer() = default;

	void OnInit() override;
	void OnDeinit() override;
	void OnResize(ivec2 const dim) override;
	void OnRender() override;

private:

	void UpdateBuffer();
	void DrawLine(vec3 start, vec4 startCol, vec3 end, vec4 endCol);
	void CheckMetaData(float thickness);
	void InitShader();
	GLuint CompileShader(std::string const& shaderSourceStr, GLenum type);

	void DrawOscillatorLines();

	struct LineVertex
	{
		LineVertex(vec3 p, vec4 c) :pos(p), col(c) {}
		vec3 pos;
		vec4 col;
	};
	struct LineMetaData
	{
		float thickness = 1;
		uint32 start = 0;
		uint32 size = 0;
	};


	// Data
	///////

	OscillatorParameters const& m_OscillatorParams;

	float m_Timer = 0.f;



	//Linebuffer
	std::vector<LineVertex> m_Lines;
	uint32 m_BufferSize = 0;
	GLuint m_Vao = 0;
	GLuint m_Vbo = 0;

	uint32 m_PixelWidth = 0;

	//Metadata
	std::vector<LineMetaData> m_MetaData;

	GLuint m_ShaderProgram;

	GLint m_uWVP = 0;
};
