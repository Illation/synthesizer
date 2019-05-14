#include "stdafx.h"
#include "Framework.h"

#include <glibmm/refptr.h>
#include <glibmm/miscutils.h>

#include <UI/_generated/resources.c>
#include <gio/gresource.h>
#include <glib/gerror.h>

void SetDebuggingOptions();

int main(int argc, char *argv[])
{
	SetDebuggingOptions();

	synthesizer_register_resource();

	// Since this example is running uninstalled, we have to help it find its
	// schema. This is *not* necessary in a properly installed application.
	Glib::setenv("GSETTINGS_SCHEMA_DIR", ".", false);

	GError* err;
	char** enumeratedResources;
	enumeratedResources = g_resources_enumerate_children("/com/leah-lindner/synthesizer/", G_RESOURCE_LOOKUP_FLAGS_NONE, &err);
	size_t i;
	for (i = 0; enumeratedResources[i] != nullptr; i++)
	{
		std::cout << std::string(enumeratedResources[i]) << std::endl;
	}

	Glib::RefPtr<Framework> framework = Framework::create();

	// Start the application, showing the initial window,
	// and opening extra views for any files that it is asked to open,
	// for instance as a command-line parameter.
	// run() will return when the last window has been closed.
	int32 result = framework->run(argc, argv);

	synthesizer_unregister_resource();

	return result;
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