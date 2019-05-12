#pragma once

#pragma warning( disable : 4250 ) // inherit via dominance

#define NOMINMAX

#pragma region
//C RunTime Header Files
#include <wchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#pragma endregion stl

#pragma region
#include "Math/Math.h"
#pragma endregion math

#pragma region
template<class T>
inline void SafeDelete(T &pObjectToDelete)
{
	if (pObjectToDelete != 0)
	{
		delete(pObjectToDelete);
		pObjectToDelete = 0;
	}
}
#pragma endregion Templates

//platform independant utility to suppress unused parameter warnings from https://stackoverflow.com/questions/4851075
// use expression as sub-expression,
// then make type of full expression int, discard result
#define UNUSED(x) (void)(x)

#pragma warning(disable : 4201) //nameless struct union - used in math library

#pragma region
#include "Helper/AtomicTypes.h"
#include "Helper/Time.h"
#include "Helper/Logger.h"
#include "Helper/PerformanceInfo.h"
//Working singleton Set
#define TIME Time::GetInstance()
#define LOG(fmt, ...) Logger::Log(fmt, __VA_ARGS__);
#define PERFORMANCE PerformanceInfo::GetInstance()
#pragma endregion Macros