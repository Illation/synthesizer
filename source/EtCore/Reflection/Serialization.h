#pragma once

#include <EtCore/FileSystem/JSONdom.h>

#include <rttr/type>

#include <EtCore/FileSystem/Entry.h>
#include <EtCore/FileSystem/FileUtil.h>
#include <EtCore/FileSystem/JSONdom.h>
#include <EtCore/FileSystem/JSONparser.h>
#include <EtCore/FileSystem/JsonWriter.h>

// This code is heavily based on https://github.com/rttrorg/rttr/tree/master/src/examples/json_serialization

//---------------------------------
// serialization
//
// This namespace will contain all functionality to serialize and deserialize things to various data formats, for now json
//
namespace serialization
{
	// Serialization
	//----------------

	template<typename T>
	bool SerializeToFile(std::string const& filePath, T const& serialObject);

	template<typename T>
	JSON::Value* SerializeToJson(T const& serialObject);

	// Deserialization
	//-----------------

	template<typename T>
	bool DeserializeFromFile(std::string const& filePath, T& outObject);

	template<typename T>
	bool DeserializeFromJsonResource(std::string const& resourcePath, T& outObject);

	template<typename T>
	bool DeserializeFromJson(JSON::Object* const parentObj, T& outObject);






	// Utility functions
	//---------------------

	// serialization
	bool ToJsonRecursive(rttr::instance const& inst, JSON::Value*& outJObject);

	bool VariantToJsonValue(rttr::variant const& var, JSON::Value*& outVal);
	bool AtomicTypeToJsonValue(rttr::type const& valueType, rttr::variant  const& var, JSON::Value*& outVal);
	bool ArrayToJsonArray(const rttr::variant_sequential_view& view, JSON::Value*& outVal);
	bool AssociativeContainerToJsonArray(const rttr::variant_associative_view& view, JSON::Value*& outVal);

	// deserialization
	rttr::variant ExtractBasicTypes(JSON::Value const* const jVal);
	bool ArrayFromJsonRecursive(rttr::variant_sequential_view& view, JSON::Value const* const jVal);
	rttr::variant ExtractValue(JSON::Value const* const jVal, const rttr::type& valueType);
	bool AssociativeViewFromJsonRecursive(rttr::variant_associative_view& view, JSON::Value const* const jVal);

	void FromJsonRecursive(rttr::instance const inst, JSON::Value const* const jVal);

} // namespace serialization

#include "Serialization.inl"