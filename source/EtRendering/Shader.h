#pragma once
#include <epoxy/gl.h>

#include <rttr/type>

#include <EtCore/Content/Asset.h>
#include <EtCore/Helper/LinkerUtils.h>


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

	GLuint const GetProgram() const { return m_ShaderProgram; }

	template<typename T>
	bool Upload(uint32 uniform, const T &data)const;
private:
	friend class ShaderAsset;

	GLuint m_ShaderProgram;

	std::map<uint32, I_Uniform*> m_Uniforms;
};

//---------------------------------
// ShaderAsset
//
// Loadable Shader Data
//
class ShaderAsset final : public Asset<ShaderData>
{
	DECLARE_FORCED_LINKING()
public:
	// Construct destruct
	//---------------------
	ShaderAsset() = default;
	virtual ~ShaderAsset() = default;

	// Asset overrides
	//---------------------
	bool LoadFromMemory(std::vector<uint8> const& data) override;

	// Utility
	//---------------------
private:
	GLuint CompileShader(const std::string &shaderSourceStr, GLenum type);

	bool Precompile(std::string &shaderContent, bool &useGeo, bool &useFrag, std::string &vertSource, std::string &geoSource, std::string &fragSource);

	bool GetUniformLocations(GLuint shaderProgram, std::map<uint32, I_Uniform*> &uniforms);

	RTTR_ENABLE(Asset<ShaderData>)
};


#include "Shader.inl"