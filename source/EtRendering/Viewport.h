#pragma once

#include <EtCore/UpdateCycle/RealTimeTickTriggerer.h>
#include <EtRendering/RenderState.h>

#include <gtkmm/glarea.h>

class I_ViewportRenderer;


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

	void SetRenderer(I_ViewportRenderer* renderer); // takes ownership

	RenderState* GetState() { return m_RenderState; }

	static RenderState* GetGlobalRenderState();

protected:

	void OnRealize();
	void OnUnrealize();
	void OnResize(int32 x, int32 y);
	bool OnRender(const Glib::RefPtr<Gdk::GLContext>& context);

private:
	bool Render();

	void MakeCurrent();


	// Data
	///////

private:
	static Viewport* g_CurrentViewport;


	Gtk::GLArea* m_GLArea = nullptr;

	I_ViewportRenderer* m_Renderer = nullptr;
	RenderState* m_RenderState = nullptr; // since a viewport has it's own open gl context, each viewport has it's own render state

	ivec2 m_Dimensions;

	bool m_IsRealized = false;
};
