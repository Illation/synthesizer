#pragma once
#include <EtCore/Helper/Singleton.h>

#include <rttr/type>


// forward declarations
class I_Asset;


//---------------------------------
// ContentManager
//
// Class that manages the lifetime of assets
//
class ContentManager : public Singleton<ContentManager>
{
public:
	// Definitions
	//---------------------

	friend class Singleton<ContentManager>;

	static constexpr char s_DatabasePath[] = "/com/leah-lindner/et_core/asset_database.json";

	//---------------------------------
	// AssetDatabase
	//
	// Container for all assets
	//
	struct AssetDatabase final
	{
	public:
		// Definitions
		//---------------------
		typedef std::vector<I_Asset*> T_AssetList;
		struct AssetCache final
		{
			std::type_info const& GetType() const;

			T_AssetList cache;

			RTTR_ENABLE()
		};

		// Data
		////////
		std::vector<AssetCache> caches;

		RTTR_ENABLE()
	};

private:
	// Construct destruct
	//---------------------
	ContentManager() = default;
	~ContentManager();
	// Protect from copy construction
	ContentManager(const ContentManager& t);
	ContentManager& operator=(const ContentManager& t);

public:
	// Managing assets
	//---------------------
	void Init();
	void Deinit();

private:
	// Data
	///////

	AssetDatabase m_Database;
};