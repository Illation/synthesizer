#include "stdafx.h"
#include "FileUtil.h"

#include <giomm/resource.h>
#include <limits>

//==============================
// File Util
//==============================


// static variables
static std::vector<std::string> newLineTokens{ "\r\n", "\n\r", "\n", "\r" };
static std::vector<char> pathDelimiters{ '\\', '/' };
std::string FileUtil::s_ExePath = std::string();


//---------------------------------
// FileUtil::AsText
//
// Converts a byte array to an std::string
//
std::string FileUtil::AsText( const std::vector<uint8> &data )
{
	return std::string( data.begin(), data.end() );
}

//---------------------------------
// FileUtil::FromText
//
// Converts an std::string to a byte array
//
std::vector<uint8> FileUtil::FromText( const std::string &data )
{
	return std::vector<uint8>( data.begin(), data.end() );
}

//---------------------------------
// FileUtil::GetCompiledResource
//
// Retrieves data stored within the executable through resource compilation
//
bool FileUtil::GetCompiledResource(std::string const& path, std::vector<uint8>& data)
{
	Glib::RefPtr<Glib::Bytes const> glibBytes = Gio::Resource::lookup_data_global(path, Gio::ResourceLookupFlags::RESOURCE_LOOKUP_FLAGS_NONE);

	gsize dataSize = glibBytes->get_size();
	if (dataSize == 0u)
	{
		LOG("FileUtil::GetCompiledResource > data retrieved from resource '" + path + std::string("' has size 0!"), LogLevel::Warning);
		return false;
	}

	uint8 const* dataArray = static_cast<uint8 const*>(glibBytes->get_data(dataSize));

	data.clear();
	for (size_t i = 0; i < dataSize; ++i)
	{
		data.emplace_back(dataArray[i]);
	}

	return true;
}

//---------------------------------
// FileUtil::ParseLine
//
// Removes one line from the input string and places it in the referenced extractedLine
//  - returns false if no line could be extracted
//
bool FileUtil::ParseLine( std::string &input, std::string &extractedLine )
{
	if (input.size() == 0)
	{
		return false;
	}

	int32 closestIdx = std::numeric_limits<int32>::max();
	uint32 tokenSize = 0;
	for(auto token : newLineTokens)
	{
		int32 index = (int32)input.find( token );
		if(index != std::string::npos && index < closestIdx)
		{
			closestIdx = index;
			tokenSize = (uint32)token.size();
		}
	}

	if(closestIdx != std::string::npos && static_cast<uint32>(closestIdx) < input.size())
	{
		extractedLine = input.substr( 0, closestIdx );
		input = input.substr( closestIdx + tokenSize );
		if(input.size() == 0) input = "";
		return true;
	}
	extractedLine = input;
	input = "";
	return true;
}

//---------------------------------
// FileUtil::ParseLines
//
// Converts a raw string to a list of lines
//
std::vector<std::string> FileUtil::ParseLines( std::string raw )
{
	std::vector<std::string> ret;
	std::string extractedLine;
	while(FileUtil::ParseLine(raw, extractedLine))
	{
		ret.push_back( extractedLine );
	}
	return ret;
}

//---------------------------------
// FileUtil::SetExecutablePath
//
// Sets the base path the executable lives in 
//
void FileUtil::SetExecutablePath(std::string const& path)
{
	// Get the last path delimiter
	size_t lastDelim = 0u;
	for (char const token : pathDelimiters)
	{
		size_t index = path.rfind(token);
		if (index != std::string::npos && index > lastDelim)
		{
			lastDelim = index;
		}
	}

	// set the executable path to the part without the exe name
	s_ExePath = path.substr(0, lastDelim + 1);
}

//---------------------------------
// FileUtil::RemoveExcessPathDelimiters
//
// Removes double / or \
//
void FileUtil::RemoveExcessPathDelimiters(std::string& path)
{
	// loop from the back so that our index stays valid as we remove elements
	for (size_t idx = path.size() - 1; idx > 0; --idx)
	{
		// check if the current character is a delimiter
		if (std::find(pathDelimiters.cbegin(), pathDelimiters.cend(), path[idx]) != pathDelimiters.cend())
		{
			// check if the next (reverse previous) character is also a delimiter
			if (std::find(pathDelimiters.cbegin(), pathDelimiters.cend(), path[idx - 1]) != pathDelimiters.cend())
			{
				// remove the current character before moving on to the next
				path.erase(idx);
			}
		}
	}
}

//---------------------------------
// FileUtil::GetAbsolutePath
//
// Gets the absolute path relative to the executable - returns false if none where found
//
bool FileUtil::RemoveRelativePath(std::string& path)
{
	// get the first '/.' sequence
	size_t closestIdx = std::numeric_limits<size_t>::max();
	for (char const token : pathDelimiters)
	{
		size_t index = path.find(std::string(1, token) + std::string("."));
		if (index != std::string::npos && index < closestIdx)
		{
			closestIdx = index;
		}
	}

	// if none is found our work is done
	if (closestIdx > path.size())
	{
		return false;
	}

	// otherwise we check if we are referring to this directory or the parent directory
	size_t preCut = closestIdx;
	size_t postCut = closestIdx + 1;
	if (closestIdx + 2 < path.size() && path[closestIdx + 2] == '.')
	{
		// we have a parentDir
		postCut++;

		// find the index of the previous ("parent") delim
		size_t lastDelim = 0u;
		for (char const token : pathDelimiters)
		{
			size_t index = path.rfind(token, preCut - 1);
			if (index != std::string::npos && index > lastDelim)
			{
				lastDelim = index;
			}
		}

		// Check we found a parent directory
		if (lastDelim == 0u || lastDelim >= preCut - 1)
		{
			LOG("FileUtil::RemoveRelativePath > expected to find a parent directory to cut", LogLevel::Error);
			return false;
		}

		preCut = lastDelim;
	}

	path = path.substr(0, preCut) + path.substr(postCut + 1);

	return true;
}

//---------------------------------
// FileUtil::GetAbsolutePath
//
// Gets the absolute path relative to the executable
//
std::string FileUtil::GetAbsolutePath(std::string const& path)
{
	// full path unedited
	std::string combinedPath;

	// if the path is already absolute we just trim without appending the exe dir
	if (IsAbsolutePath(path))
	{
		combinedPath = path;
	}
	else
	{
		combinedPath = GetExecutableDir() + path;
	}

	// remove excess delimiters first so that removing parent directories is reliable
	RemoveExcessPathDelimiters(combinedPath);

	// remove .. and . tokens until there are none left
	bool isRelative = true;
	while (isRelative)
	{
		isRelative = RemoveRelativePath(combinedPath);
	}

	return combinedPath;
}

//---------------------------------
// FileUtil::GetAbsolutePath
//
// Gets the absolute path relative to the executable
//
bool FileUtil::IsAbsolutePath(std::string const& path)
{
#if defined(PLATFORM_Linux)

	return path[0] == '/' || path[0] == '~';
	
#elif defined(PLATFORM_Win)

	// if the path has a colon, it's absolute
	return path.find(":") != std::string::npos;

#else

	LOG("FileUtil::IsAbsolutePath > Checking if a path is absolute is not implemented for this platform!", LogLevel::Warning);
	return false;

#endif
}
