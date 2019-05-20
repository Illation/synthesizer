#pragma once

#include <FileSystem/JSONdom.h>

#include <rttr/type>

#include <FileSystem/Entry.h>
#include <FileSystem/JSONparser.h>
#include <FileSystem/FileUtil.h>
#include <FileSystem/JSONdom.h>

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
	JSON::Object* SerializeToJson(T* const serialObject);

	// Deserialization
	//-----------------

	template<typename T>
	bool DeserializeFromFile(std::string const& filePath, T& outObject);

	template<typename T>
	bool DeserializeFromJson(JSON::Object* const parentObj, T& outObject);






	// Utility functions
	//---------------------

	// serialization
	bool ToJsonRecursive(const rttr::instance& inst, JSON::Object* outJObject);

	bool VariantToJsonValue(rttr::variant const& var, JSON::Value* outVal);
	bool AtomicTypeToJsonValue(rttr::type const& valueType, rttr::variant  const& var, JSON::Value* outVal);
	bool ArrayToJsonArray(const rttr::variant_sequential_view& view, JSON::Value* outVal);
	bool AssociativeContainerToJsonArray(const rttr::variant_associative_view& view, JSON::Value* outVal);

	// deserialization
	rttr::variant ExtractBasicTypes(JSON::Value const* const jVal);
	bool ArrayFromJsonRecursive(rttr::variant_sequential_view& view, JSON::Value const* const jVal);
	rttr::variant ExtractValue(JSON::Value const* const jVal, const rttr::type& valueType);
	bool AssociativeViewFromJsonRecursive(rttr::variant_associative_view& view, JSON::Value const* const jVal);

	void FromJsonRecursive(rttr::instance const inst, JSON::Value const* const jVal);

} // namespace serialization

#include "Serialization.inl"