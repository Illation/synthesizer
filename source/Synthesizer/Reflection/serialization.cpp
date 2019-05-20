#include "stdafx.h"
#include "serialization.h"

namespace serialization
{
	
//---------------------------------
// ExtractBasicTypes
//
// Convert basic JSON "leaf" values to rttr::variants, assuming the variant will convert it to its actual type later
//
rttr::variant ExtractBasicTypes(JSON::Value const* const jVal)
{
	switch (jVal->GetType())
	{
	case JSON::JSON_String:
		return jVal->str()->value;
	case JSON::JSON_Null:     
		break;
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
	case JSON::JSON_Object:
	case JSON::JSON_Array:
		break;
	}

	return rttr::variant();
}

//---------------------------------
// ArrayFromJsonRecursive
//
// deserialize a JSON array into an rttr sequential view
//
void ArrayFromJsonRecursive(rttr::variant_sequential_view& view, JSON::Value const* const jVal)
{
	if (jVal->GetType() != JSON::JSON_Array)
	{
		return;
	}
	JSON::Array const* const jArr = jVal->arr();

	view.set_size(jArr->value.size());
	const rttr::type array_value_type = view.get_rank_type(1);

	for (size_t i = 0; i < jArr->value.size(); ++i)
	{
		JSON::Value const* const jIndexVal = jArr->value[i];
		if (jIndexVal->GetType() == JSON::JSON_Array)
		{
			auto subArrayView = view.get_value(i).create_sequential_view();
			ArrayFromJsonRecursive(subArrayView, jIndexVal);
		}
		else if (jIndexVal->GetType() == JSON::JSON_Object)
		{
			rttr::variant var_tmp = view.get_value(i);
			rttr::variant wrapped_var = var_tmp.extract_wrapped_value();
			FromJsonRecursive(wrapped_var, jIndexVal);
			view.set_value(i, wrapped_var);
		}
		else
		{
			rttr::variant extractedVal = ExtractBasicTypes(jIndexVal);
			if (extractedVal.convert(array_value_type))
			{
				view.set_value(i, extractedVal);
			}
		}
	}
}

//---------------------------------
// ExtractValue
//
// Extracts a json basic type or object to an rttr variant
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
//
void AssociativeViewFromJsonRecursive(rttr::variant_associative_view& view, JSON::Value const* const jVal)
{
	if (jVal->GetType() != JSON::JSON_Array)
	{
		return;
	}
	JSON::Array const* const jArr = jVal->arr();

	for (size_t i = 0; i < jArr->value.size(); ++i)
	{
		JSON::Value const* const jIndexVal = jArr->value[i];

		if (jIndexVal->GetType() == JSON::JSON_Object) // a key-value associative view
		{
			JSON::Object const* const jIndexObj = jIndexVal->obj();
			auto jKeyIt = std::find_if(jIndexObj->value.begin(), jIndexObj->value.end(), [](JSON::Pair const& el)
			{
				return el.first == std::string("key");
			});
			auto jValIt = std::find_if(jIndexObj->value.begin(), jIndexObj->value.end(), [](JSON::Pair const& el)
			{
				return el.first == std::string("value");
			});

			if (jKeyIt != jIndexObj->value.cend() && jValIt != jIndexObj->value.cend())
			{
				rttr::variant key_var = ExtractValue(jKeyIt->second, view.get_key_type());
				rttr::variant value_var = ExtractValue(jValIt->second, view.get_value_type());
				if (key_var && value_var)
				{
					view.insert(key_var, value_var);
				}
			}
		}
		else // a key-only associative view
		{
			rttr::variant extractedVal = ExtractBasicTypes(jIndexVal);
			if (extractedVal && extractedVal.convert(view.get_key_type()))
			{
				view.insert(extractedVal);
			}
		}
	}
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
					ArrayFromJsonRecursive(view, propJVal);
				}
				else if (propValType.is_associative_container())
				{
					var = prop.get_value(instObject);
					auto associativeView = var.create_associative_view();
					AssociativeViewFromJsonRecursive(associativeView, propJVal);
				}

				prop.set_value(instObject, var);
				break;
			}
			case JSON::JSON_Object:
			{
				rttr::variant var = prop.get_value(instObject);
				FromJsonRecursive(var, propJVal);
				prop.set_value(instObject, var);
				break;
			}
			default:
			{
				rttr::variant extractedVal = ExtractBasicTypes(propJVal);
				if (extractedVal.convert(propValType))
				{
					prop.set_value(instObject, extractedVal);
				}
			}
		}
	}
}

} // namespace serialization
