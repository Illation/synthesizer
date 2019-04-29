#include "stdafx.h"
#include "Framework.h"

void SetDebuggingOptions();

int main(int argc, char *argv[])
{
	SetDebuggingOptions();

	Framework* framework = new Framework(CommandlineArguments(argc, argv));
	framework->Run();
	delete framework;

	return 0;
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