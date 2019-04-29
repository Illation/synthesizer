#include "stdafx.h"
#include "Framework.h"

#include <gtk/gtk.h>

void SetDebuggingOptions();

int main(int argc, char *argv[])
{
	UNUSED( argc );
	UNUSED( argv );

	gtk_init(&argc, &argv);
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

	gtk_widget_show_all(window);
	gtk_main();

	// Enable when not relying on console
	//SetDebuggingOptions();

	Framework* framework = new Framework();
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