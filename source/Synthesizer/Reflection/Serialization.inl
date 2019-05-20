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
JSON::Object* SerializeToJson(T* const serialObject)
{
	rttr::instance inst(serialObject);
	if (!inst.is_valid())
	{
		LOG("SerializeToJson > Couldn't create a valid instance from the object to serialize!", Warning);
		return nullptr;
	}

	JSON::Object* outObject = nullptr;

	ToJsonRecursive(inst, outObject);

	return outObject;
}


//---------------------------------
// DeserializeFromJson
//
// Create the reflected type from a file, the file type is determined by the extension
// Returns nullptr if deserialization is unsuccsesful. 
//
template<typename T>
bool DeserializeFromFile(std::string const& filePath, T& outObject)
{
	// Open the file
	File* file = new File(filePath, nullptr);
	if (!file->Open(FILE_ACCESS_MODE::Read))
	{
		LOG("DeserializeFromFile > unable to open file '" + filePath + std::string("'!"), Warning);
		return false;
	}

	// extract the necessary information
	std::string const ext(file->GetExtension());
	std::string const content(FileUtil::AsText(file->Read()));

	// We can now close the file again
	SafeDelete(file);

	// for now json is the only supported format
	if (ext == "json")
	{
		// Read the file into a json parser
		JSON::Parser parser = JSON::Parser(content);

		// if we don't have a root object parsing json was unsuccesful
		JSON::Object* root = parser.GetRoot();
		if (!root)
		{
			LOG("DeserializeFromFile > unable to parse '" + filePath + std::string("' to JSON!"), Warning);
			return false;
		}

		return DeserializeFromJson(root, outObject);
	}

	LOG("DeserializeFromFile > File type '" + ext + std::string("' not supported!"), Warning);
	return false;
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