#pragma once

#include <epoxy/gl.h>

#include <map>

// forward declarations
class ShaderData;


//---------------------------------
// RenderState
//
// CPU representation of the OpenGL state - helps avoiding sending too many calls to the GPU
//
class RenderState
{
public:

	// init deinit
	//--------------
	RenderState() = default;

	void Initialize(ivec2 dimensions);

	// State changes
	//--------------
	void SetDepthEnabled(bool enabled) { EnOrDisAble(m_DepthTestEnabled, enabled, GL_DEPTH_TEST); }
	void SetBlendEnabled(bool enabled);
	void SetBlendEnabled(bool enabled, uint32 index);
	void SetBlendEnabled(const std::vector<bool> &blendBuffers);
	void SetStencilEnabled(bool enabled) { EnOrDisAble(m_StencilTestEnabled, enabled, GL_STENCIL_TEST); }
	void SetCullEnabled(bool enabled) { EnOrDisAble(m_CullFaceEnabled, enabled, GL_CULL_FACE); }

	void SetFaceCullingMode(GLenum cullMode);
	void SetBlendEquation(GLenum equation);
	void SetBlendFunction(GLenum sFactor, GLenum dFactor);

	void SetViewport(ivec2 pos, ivec2 size);
	void GetViewport(ivec2 &pos, ivec2 &size);

	void SetClearColor(vec4 col);

	void SetShader(ShaderData* pShader);

	void BindFramebuffer(GLuint handle);
	void BindReadFramebuffer(GLuint handle);
	void BindDrawFramebuffer(GLuint handle);

	void SetActiveTexture(uint32 unit);
	void BindTexture(GLenum target, GLuint handle);

	//Makes sure that a texture is bound to a units target for shading, 
	//only changes active texture unit if the texture was not bound yet
	void LazyBindTexture(uint32 unit, GLenum target, GLuint handle);

	void BindVertexArray(GLuint vertexArray);
	void BindBuffer(GLenum target, GLuint buffer);

	void SetLineWidth(float const lineWidth);

	//Draw Calls
	//--------------
	void DrawArrays(GLenum mode, uint32 first, uint32 count);
	void DrawElements(GLenum mode, uint32 count, GLenum type, const void * indices);
	void DrawElementsInstanced(GLenum mode, uint32 count, GLenum type, const void * indices, uint32 primcount);

	// other commands
	//--------------
	void Flush() const;
	void Clear(GLbitfield mask) const;

	void GenerateVertexArrays(GLsizei n, GLuint *arrays) const;
	void GenerateBuffers(GLsizei n, GLuint *buffers) const;

	void DeleteVertexArrays(GLsizei n, GLuint *arrays) const;
	void DeleteBuffers(GLsizei n, GLuint *buffers) const;

	void SetBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) const;
	void SetVertexAttributeArrayEnabled(GLuint index, bool enabled) const; // could at some point be a member on VertexArray data object

	void DefineVertexAttributePointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid const* pointer) const;

	void* MapBuffer(GLenum target, GLenum access) const;
	void UnmapBuffer(GLenum target) const;

private:

	void EnOrDisAble(bool &state, bool enabled, GLenum glState);

	//The index should be validated before calling this function, only blend and scissor test can be larger than 0
	//the index for blending must be smaller than max draw buffers too
	void EnOrDisAbleIndexed(std::vector<bool> &state, bool enabled, GLenum glState, uint32 index);

	GLuint m_ReadFramebuffer = 0;
	GLuint m_DrawFramebuffer = 0;

	int32 m_MaxDrawBuffers; //Depends on gpu and drivers

	bool m_DepthTestEnabled = false;

	bool m_CullFaceEnabled = false;
	GLenum m_CullFaceMode = GL_BACK;
	
	bool m_StencilTestEnabled = false;

	bool m_BlendEnabled = false;
	bool m_IndividualBlend = false;
	std::vector<bool> m_BlendEnabledIndexed;
	GLenum m_BlendEquationRGB = GL_FUNC_ADD;
	GLenum m_BlendEquationAlpha = GL_FUNC_ADD;
	GLenum m_BlendFuncSFactor = GL_ONE;
	GLenum m_BlendFuncDFactor = GL_ZERO;

	ivec2 m_ViewportPosition = ivec2(0);
	ivec2 m_ViewportSize; //initialize with values used during context creation

	vec4 m_ClearColor = vec4(0);

	ShaderData* m_pBoundShader = nullptr;

	uint32 m_ActiveTexture = 0;
	int32 m_NumTextureUnits; //depends on gpu and drivers
	std::vector<std::map<GLenum, GLuint> > m_pTextureUnits; // #todo: in the future, abstract texture data here to support all types of textures

	GLuint m_VertexArray = 0;
	std::map<GLenum, GLuint> m_BufferTargets;

	float m_LineWidth = 1.f;
};