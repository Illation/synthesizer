#pragma once
#include <typeinfo>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>


//---------------------------------
// I_ContentLoader
//
// Interface for abstract content loaders
//
class I_ContentLoader
{
public:
	I_ContentLoader(void){}
	virtual ~I_ContentLoader(void){}

	virtual const std::type_info& GetType() const = 0;
	virtual void Unload() = 0;

private:
	I_ContentLoader(I_ContentLoader const&obj);
	I_ContentLoader& operator=(I_ContentLoader const& obj);
};

//---------------------------------
// ContentLoader
//
// Content loader of a templated type
//
template <class T>
class ContentLoader : public I_ContentLoader
{
public:
	// Construct destruct
	//---------------------
	ContentLoader();
	virtual ~ContentLoader() = default;
private:
	// Protect from copy construction
	ContentLoader( const ContentLoader &obj);
	ContentLoader& operator=( const ContentLoader& obj );

public:
	// Loading
	//---------------------
	T* ReloadContent(const std::string &assetFile);
	T* GetContent(const std::string& assetFile);
	virtual void Unload();

	// Utility
	//---------------------
	virtual const std::type_info& GetType() const { return typeid(T); }

protected:
	// Abstract - to be implemented by specific content loaders
	//---------------------------------------------------------
	virtual T* LoadContent(const std::string& assetFile) = 0;
	virtual void Destroy(T* objToDestroy) = 0;

private:
	// Data
	///////
	static std::unordered_map<std::string, T*> s_ContentReferences;
	static int32 s_LoaderReferences;

};

#include "ContentLoader.inl"