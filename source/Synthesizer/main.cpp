#include "stdafx.h"
#include "Framework.h"

#include <gtkmm/application.h>

void SetDebuggingOptions();

int main(int argc, char *argv[])
{
	SetDebuggingOptions();

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

	Framework framework(CommandlineArguments(argc, argv));

	return app->run(framework);
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