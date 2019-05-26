#pragma once

#include <EtCore/UpdateCycle/RealTimeTickTriggerer.h>

#include <gtkmm/glarea.h>

class I_Renderer;


//---------------------------------
// Viewport
//
// Contains a drawable area
//  - #todo support non realtime rendering
//
class Viewport : public I_RealTimeTickTriggerer
{
public:
	Viewport(Gtk::GLArea* glArea);
	virtual ~Viewport();

	void Redraw();

	void SetRenderer(I_Renderer* renderer); // takes ownership

protected:

	void OnRealize();
	void OnUnrealize();
	void OnResize(int32 x, int32 y);
	bool OnRender(const Glib::RefPtr<Gdk::GLContext>& context);

private:
	bool Render();

private:
	// Data
	///////

	Gtk::GLArea* m_GLArea = nullptr;

	I_Renderer* m_Renderer = nullptr;

	ivec2 m_Dimensions;

	bool m_IsRealized = false;
};


