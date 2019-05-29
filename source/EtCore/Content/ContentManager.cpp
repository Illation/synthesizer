#include "stdafx.h"
#include "ContentManager.h"


//===================
// Content Manager
//===================


// static
std::vector<I_ContentLoader*> ContentManager::m_Loaders = std::vector<I_ContentLoader*>();

// Loader organization
////////////////////////

//---------------------------------
// ContentManager::AddLoader
//
// Add a loader to the content manager
//
void ContentManager::AddLoader(I_ContentLoader* loader)
{ 
	for(I_ContentLoader *ldr:m_Loaders)
	{	
		if(ldr->GetType()==loader->GetType())
		{
			if (!(loader == nullptr))
			{
				delete loader;
				loader = nullptr;
			}
			break;
		}
	}

	m_Loaders.push_back(loader);
}

//---------------------------------
// ContentManager::ReleaseLoaders
//
// Release all loaders from the content manager, deleting the assets in the process
//
void ContentManager::ReleaseLoaders()
{
	for (I_ContentLoader *ldr : m_Loaders)
	{
		ldr->Unload();
		if (!(ldr == nullptr))
		{
			delete ldr;
			ldr = nullptr;
		}
	}

	m_Loaders.clear();
}