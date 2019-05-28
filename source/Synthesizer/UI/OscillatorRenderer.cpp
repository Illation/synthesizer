#include "stdafx.h"
#include "OscillatorRenderer.h"

#include <Synth/Oscillator.h>



//---------------------------------
// OscillatorRenderer::OscillatorRenderer
//
// Construct renderer with an oscillator
//
OscillatorRenderer::OscillatorRenderer(OscillatorParameters const& osc)
	: m_OscillatorParams(osc)
{ }

//---------------------------------
// OscillatorRenderer::OnInit
//
// Init all the open gl stuff
//
void OscillatorRenderer::OnInit()
{
	InitShader();

	//Generate buffers and arrays
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);

	//bind
	STATE->BindVertexArray(m_Vao);
	STATE->BindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	//set data and attributes
	glBufferData(GL_ARRAY_BUFFER, m_BufferSize, NULL, GL_DYNAMIC_DRAW);

	//input layout
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, (GLint)3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(LineVertex), (GLvoid*)offsetof(LineVertex, pos));
	glVertexAttribPointer(1, (GLint)4, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(LineVertex), (GLvoid*)offsetof(LineVertex, col));

	//unbind
	STATE->BindBuffer(GL_ARRAY_BUFFER, 0);
	STATE->BindVertexArray(0);
}

//---------------------------------
// OscillatorRenderer::OnDeinit 
//
// Deinit all the open gl stuff
//
void OscillatorRenderer::OnDeinit()
{
	glDeleteVertexArrays(1, &m_Vao);
	glDeleteBuffers(1, &m_Vbo);
	m_Lines.clear();
	m_MetaData.clear();
}

//---------------------------------
// OscillatorRenderer::OnResize 
//
// When the viewport resizes
//
void OscillatorRenderer::OnResize(ivec2 const dim)
{
	m_PixelWidth = static_cast<uint32>(dim.x);
}

//---------------------------------
// OscillatorRenderer::OnRender 
//
// Draw the oscillator
//
void OscillatorRenderer::OnRender()
{
	DrawOscillatorLines();

	if (m_Lines.size() <= 0)
	{
		m_Timer += TIME->DeltaTime();
		while (m_Timer > 1.f)
		{
			m_Timer -= 1.f;
		}

		glClearColor(0.05f, 0.075f, 0.2f * m_Timer, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	STATE->SetClearColor(vec4(0.01f, 0.02f, 0.1f, 1.f));
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_ShaderProgram);

	UpdateBuffer();

	STATE->SetBlendEnabled(true);
	STATE->SetBlendEquation(GL_FUNC_ADD);
	STATE->SetBlendFunction(GL_ONE, GL_ZERO);

	for (const auto& meta : m_MetaData)
	{
		glLineWidth(meta.thickness);
		STATE->DrawArrays(GL_LINES, meta.start, meta.size);
	}

	STATE->BindVertexArray(0);

	STATE->SetBlendEnabled(false);

	m_Lines.clear();
	m_MetaData.clear();
}

//---------------------------------
// OscillatorRenderer::UpdateBuffer 
//
// Update the line vertex buffer
//
void OscillatorRenderer::UpdateBuffer()
{
	//Bind Object vertex array
	STATE->BindVertexArray(m_Vao);

	//Send the vertex buffer again
	STATE->BindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	bool bufferResize = m_Lines.size() * sizeof(LineVertex) > m_BufferSize;
	if (!m_Vbo || bufferResize) //first creation or resize
	{
		if (bufferResize)
		{
			m_BufferSize = (uint32)m_Lines.size() * sizeof(LineVertex);
		}

		glBufferData(GL_ARRAY_BUFFER, m_BufferSize, m_Lines.data(), GL_DYNAMIC_DRAW);
	}
	else
	{
		GLvoid* p = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(p, m_Lines.data(), sizeof(LineVertex)*m_Lines.size());
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	STATE->BindBuffer(GL_ARRAY_BUFFER, 0);
}

//---------------------------------
// OscillatorRenderer::CheckMetaData 
//
// Check line metadata for thickness
//
void OscillatorRenderer::CheckMetaData(float thickness)
{
	// if we have no metadata yet or the thickness changed
	if (m_MetaData.size() == 0 || !etm::nearEquals(m_MetaData[m_MetaData.size() - 1].thickness, thickness))
	{
		m_MetaData.push_back(LineMetaData());
		m_MetaData[m_MetaData.size() - 1].thickness = thickness;
		if (m_MetaData.size() > 1)
		{
			m_MetaData[m_MetaData.size() - 1].start = m_MetaData[m_MetaData.size() - 2].start + m_MetaData[m_MetaData.size() - 2].size;
		}
	}

	// add to the size because we drew a new line
	m_MetaData[m_MetaData.size() - 1].size += sizeof(LineVertex) * 2;
}

//---------------------------------
// OscillatorRenderer::InitShader 
//
// Initialize the shader that draws the lines
//
void OscillatorRenderer::InitShader()
{
	std::string vertexShaderString =
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;\n"
		"layout (location = 1) in vec4 color;\n"
		"out vec4 Color;\n"
		"void main()\n"
		"{\n"
		"	Color = color;\n"
		"	gl_Position = vec4(pos, 1.0);\n"
		"}\n"
		;
	GLuint vertexShader = CompileShader(vertexShaderString, GL_VERTEX_SHADER);

	std::string fragmentShaderString =
		"#version 330 core\n"
		"in vec4 Color;\n"
		"out vec4 outColor;\n"
		"void main()\n"
		"{\n"
		"	outColor = Color;\n"
		"}\n"
		;
	GLuint fragmentShader = CompileShader(fragmentShaderString, GL_FRAGMENT_SHADER);


	//Combine Shaders
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram, vertexShader);
	glAttachShader(m_ShaderProgram, fragmentShader);
	glBindFragDataLocation(m_ShaderProgram, 0, "outColor");

	glLinkProgram(m_ShaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

//---------------------------------
// OscillatorRenderer::CompileShader 
//
// Compile a glsl shader
//
GLuint OscillatorRenderer::CompileShader(std::string const& shaderSourceStr, GLenum type)
{
	const char *shaderSource = shaderSourceStr.c_str();
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderSource, NULL);

	//error handling
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!(status == GL_TRUE))
	{
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		std::string sName;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			sName = "vertex";
			break;
		case GL_GEOMETRY_SHADER:
			sName = "geometry";
			break;
		case GL_FRAGMENT_SHADER:
			sName = "fragment";
			break;
		default:
			sName = "invalid type";
			break;
		}
		LOG(std::string("OscillatorRenderer::CompileShader > Compiling ") + sName + " shader failed", Warning);
		LOG(buffer, Warning);
	}

	return shader;
}

//---------------------------------
// OscillatorRenderer::DrawLine 
//
// Place a line in the buffer
//
void OscillatorRenderer::DrawLine(vec3 start, vec4 startCol, vec3 end, vec4 endCol)
{
	CheckMetaData(4.f);
	m_Lines.push_back(LineVertex(start, startCol));
	m_Lines.push_back(LineVertex(end, endCol));
}

//---------------------------------
// OscillatorRenderer::DrawOscillatorLines 
//
// Draw the oscillator into the linebuffer
//
void OscillatorRenderer::DrawOscillatorLines()
{
	if (m_PixelWidth == 0)
	{
		LOG("OscillatorRenderer::DrawOscillatorLines > Couldn't render lines because pixel width is 0", LogLevel::Warning);
		return;
	}

	// Get samples for one phase from oscillator
	std::vector<float> samples;
	float freq = 10000.f;

	// one sample per pixel
	float delta = 1.f / (static_cast<float>(m_PixelWidth) * freq);

	// get samples from a temporary oscillator created from it's parameters
	Oscillator osc(freq, m_OscillatorParams);
	for (uint32 i = 0; i < m_PixelWidth; ++i)
	{
		samples.emplace_back(osc.GetSample(delta));
	}

	// convert the samples to lines
	if (samples.size() == 0)
	{
		return;
	}

	float prevSample = samples[0];
	for (size_t sampleIdx = 1; sampleIdx < samples.size(); ++sampleIdx)
	{
		float prevSampleX = static_cast<float>(sampleIdx - 1) / static_cast<float>(samples.size()) * 2.f - 1.f;
		vec3 prevSampleVec = vec3(prevSampleX, prevSample, 1.f);

		float currSampleX = static_cast<float>(sampleIdx) / static_cast<float>(samples.size()) * 2.f - 1.f;
		vec3 currSampleVec = vec3(currSampleX, samples[sampleIdx], 1.f);

		prevSample = samples[sampleIdx];

		static vec4 const s_Color = vec4(1.f);

		DrawLine(prevSampleVec, s_Color, currSampleVec, s_Color);
	}
}
