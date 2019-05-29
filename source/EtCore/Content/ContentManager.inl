#pragma once


//===================
// Content Manager
//===================


// inline functionality
//-------------------

// Loader organization
////////////////////////

//---------------------------------
// ContentManager::GetLoader
//
// Get a loader for the specified data type
//
template<class T, class DataType>
T* ContentManager::GetLoader()
{
	const std::type_info& ti = typeid(DataType);
	for (I_ContentLoader* loader : m_Loaders)
	{
		const std::type_info& loadertype = loader->GetType();
		if (loadertype == ti)
		{
			return (static_cast<T*>(loader));
		}
	}
	return nullptr;
}


// Loading content
////////////////////////

//---------------------------------
// ContentManager::Load
//
// Load a file by it's path
//
template<class T>
T* ContentManager::Load(const std::string& assetFile)
{
	const std::type_info& ti = typeid(T);
	for (I_ContentLoader* loader : m_Loaders)
	{
		const std::type_info& loadertype = loader->GetType();
		if (loadertype == ti)
		{
			return (static_cast<ContentLoader<T>*>(loader))->GetContent(assetFile);
		}
	}

	return nullptr;
}

//---------------------------------
// ContentManager::Reload
//
// Reload a file even if it is already loaded
//
template<class T>
T* ContentManager::Reload(const std::string& assetFile)
{
	const std::type_info& ti = typeid(T);
	for (I_ContentLoader* loader : m_Loaders)
	{
		const std::type_info& loadertype = loader->GetType();
		if (loadertype == ti)
		{
			return (static_cast<ContentLoader<T>*>(loader))->ReloadContent(assetFile);
		}
	}

	return nullptr;
}