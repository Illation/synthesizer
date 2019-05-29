#pragma once
#include <vector>
#include <typeinfo>
#include "ContentLoader.h"


//---------------------------------
// ContentManager
//
// Class that manages the lifetime of assets
//
class ContentManager
{
private:
	// Construct destruct
	//---------------------
	ContentManager() = default;
	~ContentManager() = default;
	// Protect from copy construction
	ContentManager(const ContentManager& t);
	ContentManager& operator=(const ContentManager& t);

public:
	// Loader organization
	//---------------------
	static void AddLoader(I_ContentLoader* loader);

	static void ReleaseLoaders();

	template<class T, class DataType>
	static T* GetLoader();

	// Loading content
	//---------------------
	template<class T> 
	static T* Load(const std::string& assetFile);

	template<class T>
	static T* Reload(const std::string& assetFile);

private:
	// Data
	///////

	static std::vector<I_ContentLoader*> m_Loaders;
};

#include "ContentManager.inl"