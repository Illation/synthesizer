#include "stdafx.h"
#include "SynthApp.h"

#include "linkerHelper.h"

#include <glibmm/refptr.h>
#include <glibmm/miscutils.h>

#include <UI/_generated/resources.c>
#include <EtCore/FileSystem/FileUtil.h>
#include <iostream>


// forward
void SetDebuggingOptions();

 
//---------------------------------
// main
//
// Entry point for the program
//
int main(int argc, char *argv[])
{
	// Memory leak checks
	SetDebuggingOptions();

	// working dir
	if (argc > 0)
	{
		FileUtil::SetExecutablePath(argv[0]);
	}
	else
	{
		std::cerr << "main > Couldn't get extract working directory from arguments, exiting!" << std::endl;
		return 1;
	}

	// compiled resources
	synthesizer_register_resource();

	// Since this example is running uninstalled, we have to help it find its
	// schema. This is *not* necessary in a properly installed application.
	Glib::setenv("GSETTINGS_SCHEMA_DIR", ".", false);

	Glib::RefPtr<SynthApp> synthApp = SynthApp::create();

	// Start the application, showing the initial window,
	// and opening extra views for any files that it is asked to open,
	// for instance as a command-line parameter.
	// run() will return when the last window has been closed.
	int32 result = synthApp->run(argc, argv);

	// compiled resources
	synthesizer_unregister_resource();

	return result;
}

//---------------------------------
// SetDebuggingOptions
//
// Memory checks in debug mode
//
void SetDebuggingOptions()
{
	//notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Enable run-time memory leak check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	typedef HRESULT(__stdcall *fPtr)(const IID&, void**);

	//_CrtSetBreakAlloc( 87 );
#endif
}