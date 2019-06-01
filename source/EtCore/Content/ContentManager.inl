#pragma once


//===================
// Content Manager
//===================


//---------------------------------
// ContentManager::GetAsset
//
// Get an asset by it's template type
//
template <class T>
Asset<T>* ContentManager::GetAsset(T_Hash const assetId)
{
	I_Asset* abstractAsset = GetAsset(assetId, typeid(T));
	return static_cast<Asset<T>*>(abstractAsset);
}


//---------------------------------
// ContentManager::GetAssetData
//
// Get the data of an asset. Loads the asset if it's not loaded yet
//
template <class T>
T const* ContentManager::GetAssetData(T_Hash const assetId)
{
	// Get the asset
	Asset<T>* asset = GetAsset<T>(assetId);

	// Check we actually found the asset
	if (asset == nullptr)
	{
		LOG("ContentManager::GetAssetData > Couldn't find asset with ID '" + std::to_string(assetId) + std::string("'!"), LogLevel::Warning);
		return nullptr;
	}

	// Check if it needs to be loaded
	if (!(asset->IsLoaded()))
	{
		asset->Load();
	}

	return asset->GetData();
}

