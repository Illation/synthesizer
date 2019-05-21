#include "stdafx.h"
#include "JsonWriter.h"


namespace JSON {


//---------------------------------
// Writer::Write
//
// Write the json root object to the internal string, which gets cleared before
//
bool Writer::Write(Object const* const rootObj)
{
	if (rootObj == nullptr)
	{
		LOG("JSON::Writer::Write > Failed to write JSON to string, root object was null", LogLevel::Warning);
		return false;
	}

	m_JsonString.clear();
	return WriteObject(rootObj);
}

//---------------------------------
// Writer::WriteObject
//
// Write the pairs of an object
//
bool Writer::WriteObject(Object const* const jObj)
{
	// sanity check
	if (jObj == nullptr)
	{
		LOG("JSON::Writer::WriteObject > Failed to write Object to string, object was null", LogLevel::Warning);
		return false;
	}

	// begin the object
	m_JsonString += s_BeginObject;

	if (!m_Compact)
	{
		m_JsonString += s_NewLine;
		++m_IndentationLevel;
	}

	// write the pairs
	bool allPairsSucceeded = true;
	size_t pairIndex = 0;
	while (true)
	{
		// Write the pair
		if (!WritePair(jObj->value[pairIndex]))
		{
			LOG("JSON::Writer::WriteObject > Failed to write pair.", LogLevel::Warning);
			allPairsSucceeded = false;
		}

		// check if there will be a successor
		++pairIndex;
		bool isNotLast = pairIndex < jObj->value.size();

		// write delimiter
		if (isNotLast)
		{
			m_JsonString += s_Delimiter;
		}

		// write newline
		if (!m_Compact)
		{
			m_JsonString += s_NewLine;
		}

		if (!isNotLast)
		{
			break;
		}
	}

	// end the object
	if (!m_Compact)
	{
		--m_IndentationLevel;
		WriteIndentations();
	}

	m_JsonString += s_EndObject;

	// return success
	return allPairsSucceeded;
}

//---------------------------------
// Writer::WritePair
//
// Write a pair
//
bool Writer::WritePair(Pair const& jPair)
{
	if (!m_Compact)
	{
		WriteIndentations();
	}

	// key
	m_JsonString += s_StringScope;
	m_JsonString += jPair.first;
	m_JsonString += s_StringScope;

	m_JsonString += s_PairSeparator;

	// value
	if (!m_Compact)
	{
		m_JsonString += s_Whitespace;
	}
	return WriteValue(jPair.second);
}

//---------------------------------
// Writer::WriteValue
//
// Write a value, whatever it may be
//
bool Writer::WriteValue(Value const* const jVal)
{
	// sanity check
	if (jVal == nullptr)
	{
		LOG("JSON::Writer::WriteObject > Failed to write value to string, value was null", LogLevel::Warning);
		return false;
	}

	// do something depending on the type
	switch (jVal->GetType())
	{
	case ValueType::JSON_Null:
		m_JsonString += s_Null;
		break;
	case ValueType::JSON_Bool:
		m_JsonString += static_cast<Bool const*>(jVal)->value ? s_True : s_False;
		break;

	case ValueType::JSON_Number:
		return WriteNumber(static_cast<Number const*>(jVal));
	case ValueType::JSON_String:
		return WriteString(static_cast<String const*>(jVal));

	case ValueType::JSON_Array:
		return WriteArray(static_cast<Array const*>(jVal));
	case ValueType::JSON_Object:
		return WriteObject(static_cast<Object const*>(jVal));
	}

	return true;
}

//---------------------------------
// Writer::WriteNumber
//
// Write a number
//
bool Writer::WriteNumber(Number const* const jNum)
{
	return false;
}

//---------------------------------
// Writer::WriteString
//
// Write a string
//
bool Writer::WriteString(String const* const jString)
{
	return false;
}

//---------------------------------
// Writer::WriteArray
//
// Write an array
//
bool Writer::WriteArray(Array const* const jArray)
{
	return false;
}

//---------------------------------
// Writer::WriteIndentations
//
// Write indentations to the file up to the indentation level
//
void Writer::WriteIndentations()
{
	for (uint16 i = 0; i < m_IndentationLevel; ++i)
	{
		m_JsonString += s_Tab;
	}
}

} // namespace JSON