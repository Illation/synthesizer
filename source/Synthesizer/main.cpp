#include "stdafx.h"
#include "Framework.h"

#include <glibmm/refptr.h>

void SetDebuggingOptions();

int main(int argc, char *argv[])
{
	SetDebuggingOptions();

	Glib::RefPtr<Framework> framework = Framework::create();

	// Start the application, showing the initial window,
	// and opening extra views for any files that it is asked to open,
	// for instance as a command-line parameter.
	// run() will return when the last window has been closed.
	return framework->run(argc, argv);
}

void SetDebuggingOptions()
{
	//notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Enable run-time memory leak check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	typedef HRESULT(__stdcall *fPtr)(const IID&, void**);

	//_CrtSetBreakAlloc( 26783 );
#endif
}