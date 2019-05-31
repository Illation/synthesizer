#include "stdafx.h"

#include "ShaderData.h"
#include "Uniform.h"

#include <rttr/registration>


//===================
// Shader Data
//===================


// Construct destruct
///////////////

//---------------------------------
// ShaderData::ShaderData
//
// Construct shader data from an OpenGl program pointer
//
ShaderData::ShaderData(GLuint shaderProg) 
	: m_ShaderProgram(shaderProg) 
{ }

//---------------------------------
// ShaderData::~ShaderData
//
// Shader data destructor
//
ShaderData::~ShaderData()
{
	for (auto &uni : m_Uniforms)
	{
		delete uni.second;
	}
	STATE->DeleteProgram(m_ShaderProgram);
}


//===================
// Shader Asset
//===================


// reflection
RTTR_REGISTRATION
{
	using namespace rttr;

	registration::class_<ShaderAsset>("shader asset");
}


//---------------------------------
// ShaderAsset::Load
//
// Load shader data from a file
//
bool ShaderAsset::Load()
{
	// nothing yet
	return false;
}
