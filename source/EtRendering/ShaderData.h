#pragma once
#include <epoxy/gl.h>

#include <rttr/type>

#include <EtCore/Content/Asset.h>


// forward declarations
class I_Uniform;


//---------------------------------
// ShaderData
//
// Shader that can be used to draw things on the GPU - contains information about it's uniforms
//
class ShaderData
{
public:
	ShaderData() = default;
	ShaderData(GLuint shaderProg);
	~ShaderData();

	GLuint GetProgram() { return m_ShaderProgram; }

	std::string GetName() { return m_Name; }

	template<typename T>
	bool Upload(uint32 uniform, const T &data)const;
private:
	friend class ShaderAsset;

	GLuint m_ShaderProgram;

	std::string m_Name;

	std::map<uint32, I_Uniform*> m_Uniforms;
};

//---------------------------------
// ShaderAsset
//
// Loadable Shader Data
//
class ShaderAsset final : public Asset<ShaderData>
{
public:
	// Construct destruct
	//---------------------
	ShaderAsset() = default;
	virtual ~ShaderAsset() = default;

	// Asset overrides
	//---------------------
	bool Load() override;

	RTTR_ENABLE(Asset<ShaderData>)
};


#include "ShaderData.inl"