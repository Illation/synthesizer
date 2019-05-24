#pragma once
#include <string>
#include <vector>
#include "../Helper/AtomicTypes.h"

namespace JSON
{
	enum ValueType : uint8
	{
		JSON_String,
		JSON_Number,
		JSON_Object,
		JSON_Array,
		JSON_Bool,
		JSON_Null
	};
	struct Value;
	typedef std::pair<std::string, Value*> Pair;
	
	struct String;
	struct Number;
	struct Object;
	struct Array;
	struct Bool;
	//ValueTypes
	struct Value
	{
		virtual JSON::ValueType GetType() const { return JSON_Null; }
		virtual ~Value() {}
	
		JSON::String* str();
		JSON::String const* const str() const;

		JSON::Number* num();
		JSON::Number const* const num() const;

		JSON::Object* obj();
		JSON::Object const* const obj() const;

		JSON::Array* arr();
		JSON::Array const* const arr() const;

		JSON::Bool* b();
		JSON::Bool const* const b() const;
	};
	struct String : public Value
	{
		JSON::ValueType GetType() const { return JSON_String; }
		std::string value;
	};
	struct Number : public Value
	{
		JSON::ValueType GetType() const { return JSON_Number; }
		double value;
		int64 valueInt;
		bool isInt = false;
	};
	struct Object : public Value
	{
		JSON::ValueType GetType() const { return JSON_Object; }
		~Object();
		JSON::Value* operator[] (const std::string &key)
		{
			for (const JSON::Pair &keyVal : value)
			{
				if (keyVal.first == key)return keyVal.second;
			}
			return nullptr;
		}
	
		std::vector<JSON::Pair> value;
	};
	struct Array : public Value
	{
		JSON::ValueType GetType() const { return JSON_Array; }
		~Array();
		JSON::Value* operator[] (const uint32 i)
		{
			if(i>(uint32)value.size()) return nullptr;
			return value[i];
		}
		std::vector<std::string> StrArr();
		std::vector<double> NumArr();
		std::vector<int64> IntArr();
	
		std::vector<JSON::Value*> value;
	};
	struct Bool : public Value
	{
		JSON::ValueType GetType() const { return JSON_Bool; }
		bool value;
	};
	
	template<typename T>
	bool ApplyNumValue(JSON::Object* obj, T &val, const std::string &name)
	{
		JSON::Value* jval = (*obj)[name];
		if (jval)
		{
			JSON::Number* jnum = jval->num();
			if (jnum)
			{
				val = static_cast<T>(jnum->value);
				return true;
			}
		}
		return false;
	}
	bool ApplyStrValue(JSON::Object* obj, std::string &val, const std::string &name);
	bool ApplyBoolValue(JSON::Object* obj, bool &val, const std::string &name);
}