#include "stdafx.h"
#include "Asset.h"

#include <rttr/registration>


//===================
// Asset
//===================


// reflection
RTTR_REGISTRATION
{
	using namespace rttr;

	registration::class_<I_Asset>("asset")
		.property("name", &I_Asset::GetName, &I_Asset::SetName)
		.property("path", &I_Asset::GetPath, &I_Asset::SetPath)
		;
}


// Construct destruct
///////////////

//---------------------------------
// I_Asset::~I_Asset
//
// I_Asset destructor
//
I_Asset::~I_Asset()
{
	Unload();
}


// Utility
///////////////

//---------------------------------
// I_Asset::SetName
//
// Sets the name of an asset and generates its ID
//
void I_Asset::SetName(std::string const& val)
{
	m_Name = val;
	m_Id = GetHash(m_Name);
}
