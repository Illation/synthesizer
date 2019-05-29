#include "stdafx.h"
#include "FileUtil.h"

#include <giomm/resource.h>
#include <limits>

static std::vector<std::string> newLineTokens{ "\r\n", "\n\r", "\n", "\r" };

std::string FileUtil::AsText( const std::vector<uint8> &data )
{
	return std::string( data.begin(), data.end() );
}

std::vector<uint8> FileUtil::FromText( const std::string &data )
{
	return std::vector<uint8>( data.begin(), data.end() );
}

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

bool FileUtil::ParseLine( std::string &input, std::string &extractedLine )
{
	if(input.size() == 0)return false;

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
