#pragma once


//===================
// Content Loader
//===================


// static
template<class T>
std::unordered_map<std::string, T*> ContentLoader<T>::s_ContentReferences = std::unordered_map<std::string, T*>();

template<class T>
int32 ContentLoader<T>::s_LoaderReferences = 0;


// Construct Destruct
/////////////////////


//---------------------------------
// ContentLoader::ContentLoader 
//
// Default content loader constructor
//
template <class T>
ContentLoader<T>::ContentLoader()
{
	++s_LoaderReferences;
}

// Loading
////////////

//---------------------------------
// ContentLoader::ReloadContent 
//
// Load content again even if it is already loaded
//
template <class T>
T* ContentLoader<T>::ReloadContent(const std::string &assetFile)
{
	for (auto &kvp : s_ContentReferences)
	{
		if (kvp.first.compare(assetFile) == 0)
		{
			Destroy(kvp.second);
			kvp.second = LoadContent(assetFile);
			return kvp.second;
		}
	}

	T* content = LoadContent(assetFile);
	if (content != nullptr)
	{
		s_ContentReferences.insert(std::pair<std::string, T*>(assetFile, content));
	}

	return content;
}

//---------------------------------
// ContentLoader::GetContent 
//
// Get content, load it if it isn't loaded already
//
template <class T>
T* ContentLoader<T>::GetContent(const std::string& assetFile)
{
	for (std::pair<std::string, T*> kvp : s_ContentReferences)
	{
		if (kvp.first.compare(assetFile) == 0)
		{
			return kvp.second;
		}
	}

	T* content = LoadContent(assetFile);
	if (content != nullptr)
	{
		s_ContentReferences.insert(std::pair<std::string, T*>(assetFile, content));
	}

	return content;
}

//---------------------------------
// ContentLoader::Unload 
//
// Unload loaded content
//
template <class T>
void ContentLoader<T>::Unload()
{
	--s_LoaderReferences;

	if (s_LoaderReferences <= 0)
	{
		for (std::pair<std::string, T*> kvp : s_ContentReferences)
		{
			Destroy(kvp.second);
		}

		s_ContentReferences.clear();
	}
}
