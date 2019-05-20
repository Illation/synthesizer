#include "stdafx.h"
#include "serialization.h"

namespace serialization {


// Serialization
//////////////////////

	
//---------------------------------
// ToJsonRecursive
//
// Recursively convert an rttr::instance to a json object - returns false if any properties fail to serialize
//
bool ToJsonRecursive(const rttr::instance& inst, JSON::Object* outJObject)
{
	outJObject = new JSON::Object();

	rttr::instance instObj = inst.get_type().get_raw_type().is_wrapper() ? inst.get_wrapped_instance() : inst;

	rttr::array_range<rttr::property> const propList = inst.get_derived_type().get_properties();

	bool allPropertiesSerialized = true;

	for (rttr::property const& prop : propList)
	{
		if (prop.get_metadata("NO_SERIALIZE")) // read only should not be serialized probably
		{
			continue;
		}

		rttr::variant const& propVal = prop.get_value(inst);
		if (!propVal)
		{
			continue; // cannot serialize, because we cannot retrieve the value - maybe handle nullptr here
		}

		JSON::Pair keyVal = std::make_pair(prop.get_name().to_string(), nullptr);

		if (!VariantToJsonValue(propVal, keyVal.second))
		{
			LOG("ToJsonRecursive > Failed to serialize property '" + keyVal.first + std::string("' !"), LogLevel::Warning);
			allPropertiesSerialized = false;
		}
		else
		{
			outJObject->value.push_back(keyVal);
		}
	}

	return allPropertiesSerialized;
}

//---------------------------------
// VariantToJsonValue
//
// Recursively convert an rttr::variant to a json value, figuring out it's type in the process
//
bool VariantToJsonValue(rttr::variant const& var, JSON::Value* outVal)
{
	rttr::type valueType = var.get_type();
	rttr::type wrappedType = valueType.is_wrapper() ? valueType.get_wrapped_type() : valueType;
	bool isWrapper = wrappedType != valueType;

	if (AtomicTypeToJsonValue(wrappedType, isWrapper ? var.extract_wrapped_value() : var, outVal))
	{
		// check here if this was a nullptr, if it wasn't and the JSON object is of type JSON_Null we should return false
	}
	else if (var.is_sequential_container())
	{
		if (!ArrayToJsonArray(var.create_sequential_view(), outVal))
		{
			LOG("VariantToJsonValue > Failed to convert variant to json array, typeName: '" + wrappedType.get_name().to_string()
				+ std::string("'!"), LogLevel::Warning);

			return false;
		}
	}
	else if (var.is_associative_container())
	{
		if (!AssociativeContainerToJsonArray(var.create_associative_view(), outVal))
		{
			LOG("VariantToJsonValue > Failed to convert variant to associate view, typeName: '" + wrappedType.get_name().to_string() 
				+ std::string("'!"), LogLevel::Warning);

			return false;
		}
	}
	else
	{
		if (!(wrappedType.get_properties().empty())) // try converting the variant to a JSON object
		{
			if (!ToJsonRecursive(var, static_cast<JSON::Object*>(outVal)))
			{
				LOG("VariantToJsonValue > Failed to convert variant to JSON object, typeName: '" + wrappedType.get_name().to_string() 
					+ std::string("'!"), LogLevel::Warning);

				return false;
			}
		}
		else // if that fails, try converting it to a string
		{
			bool stringConversionSuccess = false;
			std::string text = var.to_string(&stringConversionSuccess);

			if (!stringConversionSuccess)
			{
				LOG("VariantToJsonValue > Failed to convert variant to JSON string, result: '" + text + std::string("' typeName: '")
					+ wrappedType.get_name().to_string() + std::string("'!"), LogLevel::Warning);

				return false;
			}

			outVal = new JSON::String();
			outVal->str()->value = text;
		}
	}

	return true;
}

//---------------------------------
// AtomicTypeToJsonValue
//
// Convert a handful of basic supported "leaf" types to Atomic JSON types. Might have to add more in the future. 
// If value type is not atomic it returns false. If value type is atomic but not supported it returns true and outVal will be of type JSON_Null
//
bool AtomicTypeToJsonValue(rttr::type const& valueType, rttr::variant const& var, JSON::Value* outVal)
{
	if (valueType.is_arithmetic()) // basic arithmetic types get converted to numbers or bools
	{
		if (valueType == rttr::type::get<bool>())
		{
			JSON::Bool* jBool = new JSON::Bool();
			jBool->value = var.to_bool();
			outVal = jBool;
		}
		else if (valueType == rttr::type::get<char>())
		{
			// store char as number because otherwise it would get picked up as a string
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_uint8());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<int8>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_int8());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<int16>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_int16());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<int32>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_int32());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<int64>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = var.to_int64();
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<uint8>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_uint8());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<uint16>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_uint16());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<uint32>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_uint32());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<uint64>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->valueInt = static_cast<int64>(var.to_uint64());
			jNum->isInt = true;
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<float>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->value = var.to_double();
			outVal = jNum;
		}
		else if (valueType == rttr::type::get<double>())
		{
			JSON::Number* jNum = new JSON::Number();
			jNum->value = var.to_double();
			outVal = jNum;
		}
		else
		{
			LOG("AtomicTypeToJsonValue > Atomic type not supported, typeName: '" + valueType.get_name().to_string() + std::string("'!")
				, LogLevel::Warning);

			outVal = new JSON::Value(); // return true because it's definitly an arithmetic type but make it null
		}

		return true;
	}
	else if (valueType.is_enumeration()) // enumeration to JSON
	{
		// first try converting it to a string
		bool conversionSuccess = false;
		std::string stringVal = var.to_string(&conversionSuccess);

		if (conversionSuccess)
		{
			JSON::String* jString = new JSON::String();
			jString->value = stringVal;
			outVal = jString;
		}
		else
		{
			// if that doesn't work we convert it to a uint64 instead
			uint64 intVal = var.to_uint64(&conversionSuccess);

			if (conversionSuccess)
			{
				JSON::Number* jNum = new JSON::Number();
				jNum->valueInt = static_cast<int64>(intVal);
				jNum->isInt = true;
				outVal = jNum;
			}
			else
			{
				// That failed too
				// return true because it's definitly an arithmetic type but make it null

				LOG("AtomicTypeToJsonValue > Enum failed to convert, typeName: '" + valueType.get_name().to_string() + std::string("'!")
					, LogLevel::Warning);

				outVal = new JSON::Value();
			}
		}

		return true;
	}
	else if (valueType == rttr::type::get<std::string>()) // string to json
	{
		JSON::String* jString = new JSON::String();
		jString->value = var.to_string();
		outVal = jString;
		return true;
	}

	// couldn't handle type, return false to indicate that this should be an object or array
	return false;
}

//---------------------------------
// ArrayToJsonArray
//
// Converts a sequential view to a JSON array
//
bool ArrayToJsonArray(const rttr::variant_sequential_view& view, JSON::Value* outVal)
{
	JSON::Array* outArr = new JSON::Array();
	outVal = outArr;

	bool allItemsSucceeded = true;
	
	for (const auto& item : view)
	{
		JSON::Value* jItem = nullptr;

		if (item.is_sequential_container()) // inner array
		{
			if (!ArrayToJsonArray(item.create_sequential_view(), jItem))
			{
				LOG("ArrayToJsonArray > failed to convert array element to inner json array, typeName: '" + 
					item.get_type().get_name().to_string() + std::string("'!"), LogLevel::Warning);

				allItemsSucceeded = false;
			}
		}
		else // atomic type or object
		{
			rttr::variant wrappedVar = item.extract_wrapped_value();
			rttr::type valueType = wrappedVar.get_type();

			if (valueType.is_arithmetic() || valueType == rttr::type::get<std::string>() || valueType.is_enumeration())
			{
				if (!AtomicTypeToJsonValue(valueType, wrappedVar, jItem))
				{
					LOG("ArrayToJsonArray > failed to convert array element to atomic type, typeName: '" + valueType.get_name().to_string() 
						+ std::string("'!"), LogLevel::Warning);

					allItemsSucceeded = false;
				}
			}
			else // object
			{
				if (!ToJsonRecursive(wrappedVar, static_cast<JSON::Object*>(jItem)))
				{
					LOG("ArrayToJsonArray > failed to convert array element to json object, typeName: '" + valueType.get_name().to_string()
						+ std::string("'!"), LogLevel::Warning);

					allItemsSucceeded = false;
				}
			}
		}

		outArr->value.emplace_back(jItem);
	}

	return allItemsSucceeded;
}

//---------------------------------
// AssociativeContainerToJsonArray
//
// Converts a associative view to a JSON array of objects containing keys and values. If any item fails to be serialized this returns false
//
bool AssociativeContainerToJsonArray(const rttr::variant_associative_view& view, JSON::Value* outVal)
{
	static const std::string keyName("key");
	static const std::string valueName("value");

	JSON::Array* outArr = new JSON::Array();
	outVal = outArr;

	bool allItemsSucceeded = true;

	if (view.is_key_only_type()) // treat this as a normal array
	{
		for (auto& item : view)
		{
			JSON::Value* jItem = nullptr;

			if (!VariantToJsonValue(item.first, jItem))
			{
				LOG("ArrayToJsonArray > failed to convert key only variant to json value!", LogLevel::Warning);
				allItemsSucceeded = false;
			}

			outArr->value.emplace_back(jItem);
		}
	}
	else // treat this as a objects with keys and values
	{
		for (auto& item : view)
		{
			JSON::Object* jObj = new JSON::Object();
			
			// create the key element and place it in the object
			JSON::Pair keyPair = std::make_pair(keyName, nullptr);
			if (!VariantToJsonValue(item.first, keyPair.second))
			{
				LOG("ArrayToJsonArray > failed to convert key variant to json value!", LogLevel::Warning);
				allItemsSucceeded = false;
			}
			jObj->value.emplace_back(keyPair);

			// create the value element and place it in the object
			JSON::Pair valuePair = std::make_pair(valueName, nullptr);
			if (!VariantToJsonValue(item.second, valuePair.second))
			{
				LOG("ArrayToJsonArray > failed to convert value variant to json value!", LogLevel::Warning);
				allItemsSucceeded = false;
			}
			jObj->value.emplace_back(valuePair);

			// place the key value object in the array
			outArr->value.emplace_back(jObj);
		}
	}

	return allItemsSucceeded;
}


// Deserialization
//////////////////////


//---------------------------------
// ExtractBasicTypes
//
// Convert basic JSON "leaf" values to rttr::variants, assuming the variant will convert it to its actual type later
// If the JSON value is not a basic type we return an invalid variant
//
rttr::variant ExtractBasicTypes(JSON::Value const* const jVal)
{
	switch (jVal->GetType())
	{
	case JSON::JSON_String:
		return jVal->str()->value;
	case JSON::JSON_Bool:
		return jVal->b()->value;
	case JSON::JSON_Number:
	{
		if (jVal->num()->isInt)
		{
			return jVal->num()->valueInt;
		}
		else
		{
			return jVal->num()->value;
		}
	}

	// we handle only the basic types here
	case JSON::JSON_Null:
	case JSON::JSON_Object:
	case JSON::JSON_Array:
		break;
	}

	return rttr::variant(); // invalid
}

//---------------------------------
// ArrayFromJsonRecursive
//
// deserialize a JSON array into an rttr sequential view
//  - returns false if any of the elements failes deserialization, but tries to parse the entire view anyway
//
bool ArrayFromJsonRecursive(rttr::variant_sequential_view& view, JSON::Value const* const jVal)
{
	if (jVal->GetType() != JSON::JSON_Array)
	{
		LOG("ArrayFromJsonRecursive > Expected JSON::Value to be of type array!", LogLevel::Warning);
		return false;
	}
	JSON::Array const* const jArr = jVal->arr();

	view.set_size(jArr->value.size());
	const rttr::type arrayValueType = view.get_rank_type(1);


	bool success = true;

	// Loop over array elements and fill them, depending on the internal type
	for (size_t i = 0; i < jArr->value.size(); ++i)
	{
		JSON::Value const* const jIndexVal = jArr->value[i];
		if (jIndexVal->GetType() == JSON::JSON_Array) // multi dimensional array
		{
			auto subArrayView = view.get_value(i).create_sequential_view();
			if (!ArrayFromJsonRecursive(subArrayView, jIndexVal))
			{
				LOG("ArrayFromJsonRecursive > There was an issue deserializing the inner array, index: #" + std::to_string(i)
					+ std::string(" typeName: '") + arrayValueType.get_name().to_string() + std::string("'!"), LogLevel::Warning);

				success = false;
			}
		}
		else if (jIndexVal->GetType() == JSON::JSON_Object) // array of objects
		{
			rttr::variant tempVar = view.get_value(i);
			rttr::variant wrappedVar = tempVar.extract_wrapped_value();

			FromJsonRecursive(wrappedVar, jIndexVal);

			if (!wrappedVar.is_valid())
			{
				LOG("ArrayFromJsonRecursive > Failed to create a valid object from property, index: #" + std::to_string(i)
					+ std::string(" typeName: '") + arrayValueType.get_name().to_string() + std::string("'!"), LogLevel::Warning);

				success = false;
			}

			view.set_value(i, wrappedVar);
		}
		else // array of basic types
		{
			rttr::variant extractedVal = ExtractBasicTypes(jIndexVal);
			if (extractedVal.convert(arrayValueType))
			{
				view.set_value(i, extractedVal);
			}
			else
			{
				LOG("ArrayFromJsonRecursive > Failed to convert basic type extracted from JSON to property value type, index: #" + std::to_string(i)
					 + std::string(" typeName: '") + arrayValueType.get_name().to_string() + std::string("'!"), LogLevel::Warning);

				success = false;
			}
		}
	}

	return success;
}

//---------------------------------
// ExtractValue
//
// Extracts a json basic type or object to an rttr variant, if it is neither it will return an invalid variant
//
rttr::variant ExtractValue(JSON::Value const* const jVal, const rttr::type& valueType)
{
	// try converting from a basic type
	rttr::variant extractedVal = ExtractBasicTypes(jVal);

	// if that doesn't work, try an object
	if (!extractedVal.convert(valueType))
	{
		if (jVal->GetType() == JSON::JSON_Object)
		{
			// find the right constructor for our type
			rttr::constructor ctor = valueType.get_constructor();
			for (auto& item : valueType.get_constructors())
			{
				if (item.get_instantiated_type() == valueType)
				{
					ctor = item;
				}
			}

			//use it
			extractedVal = ctor.invoke();

			// fill the rest of our object
			FromJsonRecursive(extractedVal, jVal);
		}
	}

	return extractedVal;
}

//---------------------------------
// AssociativeViewFromJsonRecursive
//
// deserialize a JSON array into an rttr associative view (dictionary / map)
//  - returns false if any of the key value pairs failes deserialization, but tries to parse the entire view anyway
//
bool AssociativeViewFromJsonRecursive(rttr::variant_associative_view& view, JSON::Value const* const jVal)
{
	if (jVal->GetType() != JSON::JSON_Array)
	{
		LOG("AssociativeViewFromJsonRecursive > Expected JSON::Value to be of type array!", LogLevel::Warning);
		return false;
	}
	JSON::Array const* const jArr = jVal->arr();

	bool success = true;

	for (size_t i = 0; i < jArr->value.size(); ++i)
	{
		JSON::Value const* const jIndexVal = jArr->value[i];

		if (jIndexVal->GetType() == JSON::JSON_Object) // a key-value associative view
		{
			// get the json object
			JSON::Object const* const jIndexObj = jIndexVal->obj();

			// extract the key
			auto jKeyIt = std::find_if(jIndexObj->value.begin(), jIndexObj->value.end(), [](JSON::Pair const& el)
			{
				return el.first == std::string("key");
			});

			if (jKeyIt == jIndexObj->value.cend())
			{
				LOG("AssociativeViewFromJsonRecursive > Failed to find the value of 'key' in json object, index #" + std::to_string(i)
					, LogLevel::Warning);

				success = false;
				continue;
			}

			rttr::variant key_var = ExtractValue(jKeyIt->second, view.get_key_type());
			if (!key_var.is_valid())
			{
				LOG("AssociativeViewFromJsonRecursive > Failed to create a valid variant from key, index #" + std::to_string(i) +
					std::string(" typeName: '") + view.get_key_type().get_name().to_string() + std::string("'!"), LogLevel::Warning);

				success = false;
				continue;
			}

			// extract the value
			auto jValIt = std::find_if(jIndexObj->value.begin(), jIndexObj->value.end(), [](JSON::Pair const& el)
			{
				return el.first == std::string("value");
			});

			if (jValIt == jIndexObj->value.cend())
			{
				LOG("AssociativeViewFromJsonRecursive > Failed to find the value of 'value' in json object, index #" + std::to_string(i)
					, LogLevel::Warning);

				success = false;
				continue;
			}

			rttr::variant value_var = ExtractValue(jValIt->second, view.get_value_type());
			if (!value_var.is_valid())
			{
				LOG("AssociativeViewFromJsonRecursive > Failed to create a valid object from value, index #" + std::to_string(i) +
					std::string(" typeName: '") + view.get_value_type().get_name().to_string() + std::string("'!"), LogLevel::Warning);

				success = false;
				continue;
			}

			// insert the key value pair into associate container
			view.insert(key_var, value_var);
		}
		else // a key-only associative view
		{
			rttr::variant extractedVal = ExtractBasicTypes(jIndexVal);
			if (extractedVal && extractedVal.convert(view.get_key_type()))
			{
				view.insert(extractedVal);
			}
			else
			{
				LOG("AssociativeViewFromJsonRecursive > Failed to convert basic type extracted from JSON to property value type, index: #" 
					+ std::to_string(i) + std::string(" typeName: '") + view.get_key_type().get_name().to_string() + std::string("'!")
					, LogLevel::Warning);

				success = false;
			}
		}
	}

	return success;
}

//---------------------------------
// FromJsonRecursive
//
// Recursively deserialize JSON values into an object (the instance)
//
void FromJsonRecursive(rttr::instance const inst, JSON::Value const* const jVal) // assumes jVal is a JSON::Object
{
	if (jVal->GetType() != JSON::JSON_Object)
	{
		LOG("FromJsonRecursive > Expected JSON::Value to be of type object!", LogLevel::Warning);
		return;
	}
	JSON::Object const* const jObj = jVal->obj();

	rttr::instance instObject = inst.get_type().get_raw_type().is_wrapper() ? inst.get_wrapped_instance() : inst;
	rttr::array_range<rttr::property> const prop_list = instObject.get_derived_type().get_properties();

	for (auto prop : prop_list)
	{
		std::string propName = prop.get_name().to_string();

		// find children in json, if not just skip them
		auto jChildIt = std::find_if(jObj->value.begin(), jObj->value.end(), [propName](JSON::Pair const& el)
		{
			return el.first == propName;
		});
		if (jChildIt == jObj->value.cend())
		{
			// not specifying a property is valid and not an error, the default value should be used
			continue;
		}

		const rttr::type propValType = prop.get_type();

		JSON::Value const* const propJVal = jChildIt->second;
		switch (propJVal->GetType())
		{
			case JSON::JSON_Array:
			{
				rttr::variant var;
				if (propValType.is_sequential_container())
				{
					var = prop.get_value(instObject);
					auto view = var.create_sequential_view();

					if (!ArrayFromJsonRecursive(view, propJVal))
					{
						LOG("FromJsonRecursive > There was an issue deserializing the sequential view, name: '"
							+ propName + std::string("' typeName: '") + propValType.get_name().to_string() + std::string("'!"), LogLevel::Warning);
					}
				}
				else if (propValType.is_associative_container())
				{
					var = prop.get_value(instObject);
					auto associativeView = var.create_associative_view();

					if (!AssociativeViewFromJsonRecursive(associativeView, propJVal))
					{
						LOG("FromJsonRecursive > There was an issue deserializing the associate view, name: '"
							+ propName + std::string("' typeName: '") + propValType.get_name().to_string() + std::string("'!"), LogLevel::Warning);
					}
				}
				else
				{
					LOG("FromJsonRecursive > Found a JSON value of type array, but the property is not a sequential or associate container, name: '"
						+ propName + std::string("' typeName: '") + propValType.get_name().to_string() + std::string("'!"), LogLevel::Warning);
				}

				prop.set_value(instObject, var);
				break;
			}
			case JSON::JSON_Object:
			{
				rttr::variant var = prop.get_value(instObject);
				FromJsonRecursive(var, propJVal);

				if (!var.is_valid())
				{
					LOG("FromJsonRecursive > Failed to create a valid object from property, name: '"
						+ propName + std::string("' typeName: '") + propValType.get_name().to_string() + std::string("'!"), LogLevel::Warning);
				}

				prop.set_value(instObject, var);
				break;
			}
			default:
			{
				rttr::variant extractedVal = ExtractBasicTypes(propJVal); // extract the basic type to a variant
				if (extractedVal.convert(propValType)) // then try to convert it to the type of our property
				{
					prop.set_value(instObject, extractedVal);
				}
				else
				{
					LOG("FromJsonRecursive > Failed to convert basic type extracted from JSON to property value type, property: '"
						+ propName + std::string("' typeName: '") + propValType.get_name().to_string() + std::string("'!"), LogLevel::Warning);
				}
			}
		}
	}
}

} // namespace serialization
