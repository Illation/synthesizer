#include "stdafx.h"
#include "FileUtil.h"

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
