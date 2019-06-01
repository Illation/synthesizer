#pragma once

#include <EtCore/Helper/Hash.h>

#include <rttr/type>


//---------------------------------
// I_Asset
//
// Base asset interface without asset data
//
class I_Asset
{
public:
	// Construct destruct
	//---------------------
	I_Asset() = default;
	I_Asset(const I_Asset&) = default;
	virtual ~I_Asset();

	// Interface
	//---------------------
	virtual std::type_info const& GetType() const = 0;
	virtual bool IsLoaded() const = 0;
	virtual bool Load() = 0;
	virtual void Unload() {}

	// Utility
	//---------------------	
	std::string const& GetName() const { return m_Name; }
	void SetName(std::string const& val);

	std::string const& GetPath() const { return m_Path; }
	void SetPath(std::string const& val) { m_Path = val; }

	T_Hash GetId() const { return m_Id; }

private:
	// Data
	///////

	// reflected
	std::string m_Name;
	std::string m_Path;

	// derived
	T_Hash m_Id;

	RTTR_ENABLE()
};

//---------------------------------
// Asset
//
// Templated asset class with data
//
template <class T>
class Asset : public I_Asset
{
public:
	// Construct destruct
	//---------------------
	Asset() = default;
	virtual ~Asset() = default;

	// Interface
	//---------------------
	virtual bool Load() { return false; }

	// Utility
	//---------------------
	std::type_info const& GetType() const override { return typeid(T); }
	bool IsLoaded() const override { return m_Data != nullptr; }
	void Unload() override;

private:
	// Data
	///////

	T* m_Data = nullptr;

	RTTR_ENABLE(I_Asset)
};

#include "Asset.inl"