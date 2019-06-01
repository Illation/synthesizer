#include "stdafx.h"

#include "ShaderData.h"
#include "Uniform.h"

#include <rttr/registration>
#include <rttr/policy.h>
#include <rttr/detail/policies/ctor_policies.h>


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

	registration::class_<ShaderAsset>("shader asset")
		.constructor<ShaderAsset const&>()
		.constructor<>()( rttr::detail::as_object() );
	rttr::type::register_converter_func( [](ShaderAsset& shader, bool& ok) -> I_Asset*
	{
		ok = true;
		return new ShaderAsset(shader);
	});
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
