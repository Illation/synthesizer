#include "stdafx.h"
#include "ContentManager.h"

#include "Asset.h"

#include <EtCore/Reflection/Serialization.h>

#include <rttr/registration>

#include <EtCore/FileSystem/FileUtil.h>


//===================
// Content Manager
//===================


// reflection
RTTR_REGISTRATION
{
	using namespace rttr;

	registration::class_<ContentManager::AssetDatabase::AssetCache>("asset cache")
		.property("cache", &ContentManager::AssetDatabase::AssetCache::cache);

	registration::class_<ContentManager::AssetDatabase>("asset database")
		.property("caches", &ContentManager::AssetDatabase::caches) ;
}


//---------------------------------
// ContentManager::AssetDatabase::AssetCache::GetType
//
// Get the type of an asset cache
//
std::type_info const& ContentManager::AssetDatabase::AssetCache::GetType() const
{
	if (cache.size() > 0)
	{
		return cache[0]->GetType();
	}
	return typeid(nullptr);
}


// Managing assets
////////////////////////

ContentManager::~ContentManager()
{
	Deinit();
}

//---------------------------------
// ContentManager::Init
//
// Load the asset database
//
void ContentManager::Init()
{
	if (!serialization::DeserializeFromJsonResource(s_DatabasePath, m_Database))
	{
		LOG("ContentManager::Init > unable to deserialize asset database at '" + std::string(s_DatabasePath) + std::string("'"), LogLevel::Error);
	}
}

//---------------------------------
// ContentManager::Deinit
//
// Delete all remaining open assets
//
void ContentManager::Deinit()
{
	for (AssetDatabase::AssetCache& cache : m_Database.caches)
	{
		for (I_Asset* asset : cache.cache)
		{
			delete asset;
			asset = nullptr;
		}
	}
	m_Database.caches.clear();
}