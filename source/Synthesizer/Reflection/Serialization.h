#pragma once

#include <FileSystem/JSONdom.h>

#include <rttr/type>

// This code is heavily based on https://github.com/rttrorg/rttr/tree/master/src/examples/json_serialization

//---------------------------------
// serialization
//
// This namespace will contain all functionality to serialize and deserialize things to various data formats, for now json
//
namespace serialization
{

	template<typename T>
	JSON::Object* SerializeToJson(T* const object);

	rttr::variant ExtractBasicTypes(JSON::Value const* const jVal);
	void ArrayFromJsonRecursive(rttr::variant_sequential_view& view, JSON::Value const* const jVal);
	rttr::variant ExtractValue(JSON::Value const* const jVal, const rttr::type& valueType);
	void AssociativeViewFromJsonRecursive(rttr::variant_associative_view& view, JSON::Value const* const jVal);
	void FromJsonRecursive(rttr::instance const inst, JSON::Value const* const jVal);

	template<typename T>
	bool DeserializeFromJson(JSON::Object* const parentObj, T& outObject);

} // namespace serialization

#include "Serialization.inl"