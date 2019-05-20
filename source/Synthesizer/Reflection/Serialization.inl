#pragma once

// Inline functions
//////////////////////


namespace serialization {

//---------------------------------
// SerializeToJson
//
// Create a JSON::Object from the templated type using reflection data. Returns nullptr if serialization is unsuccsesful.
//
template<typename T>
JSON::Object* SerializeToJson(T* const object)
{
	return nullptr;
}

//---------------------------------
// DeserializeFromJson
//
// Create the reflected type from JSON data. 
// Returns nullptr if deserialization is unsuccsesful. 
// Assumes parentObj is the parent of the object we are deserializing
//
template<typename T>
bool DeserializeFromJson(JSON::Object* const parentObj, T& outObject)
{
	// Get the Serialized type from our template typename
	rttr::type objectType = rttr::type::get<T>();
	if (!objectType.is_valid())
	{
		LOG("DeserializeFromJson > type is invalid!", Warning);
		return nullptr;
	}

	// get the name of our type
	std::string typeName = objectType.get_name().to_string();

	// try finding a json value in its parent by the typename
	auto foundJObjectIt = std::find_if(parentObj->value.begin(), parentObj->value.end(), [typeName](JSON::Pair const& el)
	{
		return el.first == typeName;
	});

	if (foundJObjectIt == parentObj->value.cend())
	{
		LOG("DeserializeFromJson > Couldn't find '" + typeName + std::string("' in provided json object parent!"), Warning);
		return nullptr;
	}

	// try creating an instance from the value
	FromJsonRecursive(outObject, foundJObjectIt->second);

	// copy construct our return object from the value of the variant
	return true;
}

} // namespace serialization