#pragma once

#if defined(PLATFORM_Linux)
    typedef int32 FILE_HANDLE;
    #define FILE_HANDLE_INVALID (-1)
#elif defined(PLATFORM_Win)
    // #todo need include for this?
#include <windows.h>
    typedef HANDLE FILE_HANDLE;
    #define FILE_HANDLE_INVALID INVALID_HANDLE_VALUE
#endif
