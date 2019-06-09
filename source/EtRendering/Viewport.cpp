#include "stdafx.h"
#include "Viewport.h"

#include "ViewportRenderer.h"
#include "RenderState.h"

#include <epoxy/gl.h>


//=====================
// Viewport
//=====================


// static
Viewport* Viewport::g_CurrentViewport = nullptr;


//---------------------------------
// Viewport::Viewport
//
// Construct a viewport from a glArea
//
Viewport::Viewport(Gtk::GLArea* glArea)
	: m_GLArea(glArea)
	, m_RenderState(new RenderState())
{
	RegisterAsTriggerer();

	m_GLArea->signal_realize().connect(sigc::mem_fun(*this, &Viewport::OnRealize));
	m_GLArea->signal_unrealize().connect(sigc::mem_fun(*this, &Viewport::OnUnrealize), false);
	m_GLArea->signal_resize().connect(sigc::mem_fun(*this, &Viewport::OnResize), false);
	m_GLArea->signal_render().connect(sigc::mem_fun(*this, &Viewport::OnRender), false);
}


//---------------------------------
// Viewport::Viewport
//
// Construct a viewport from a glArea
//
Viewport::~Viewport()
{
	if (m_Renderer != nullptr)
	{
		delete m_Renderer;
		m_Renderer = nullptr;
	}
}

//---------------------------------
// Viewport::Redraw
//
// Trigger redrawing the window
//
void Viewport::Redraw()
{
	m_GLArea->queue_draw();
}

//---------------------------------
// Viewport::SetRenderer
//
// Set the renderer, takes ownership of it
//
void Viewport::SetRenderer(I_ViewportRenderer* renderer)
{
	m_Renderer = renderer;

	if (m_IsRealized)
	{
		OnRealize();
	}
}

//---------------------------------
// Viewport::GetGlobalRenderState
//
// returns the render state of the current viewport
//
RenderState* Viewport::GetGlobalRenderState()
{
	if (g_CurrentViewport == nullptr)
	{
		LOG("GetGlobalRenderState > g_CurrentViewport not set -> couldn't retrieve state", LogLevel::Error);
		return nullptr;
	}

	return g_CurrentViewport->GetState();
}

//---------------------------------
// Viewport::OnRealize
//
// init open gl stuff
//
void Viewport::OnRealize()
{
	MakeCurrent();

	// init render state
	m_RenderState->Initialize(m_Dimensions);

	// init renderer
	if (m_Renderer != nullptr)
	{
		m_Renderer->OnResize(m_Dimensions);
		m_Renderer->OnInit();
	}

	m_IsRealized = true;
}

//---------------------------------
// Viewport::OnUnrealize
//
// Uninit open gl stuff
//
void Viewport::OnUnrealize()
{		
	MakeCurrent();

	if (m_Renderer != nullptr)
	{
		m_Renderer->OnDeinit();
	}

	m_IsRealized = false;
}

//---------------------------------
// Viewport::OnUnrealize
//
// When the drawable area resizes - we should recreate framebuffer textures here etc
//
void Viewport::OnResize(int32 x, int32 y)
{
	MakeCurrent();

	m_Dimensions = ivec2(x, y);

	if (m_Renderer != nullptr)
	{
		m_Renderer->OnResize(m_Dimensions);
	}
}

//---------------------------------
// Viewport::OnRender
//
// This function updates everything in a gameloops style and then calls Render, making sure to refresh itself at screen refresh rate
//
bool Viewport::OnRender(const Glib::RefPtr<Gdk::GLContext>& context)
{
	MakeCurrent();

	UNUSED(context);

	TriggerTick(); // if this is the first real time thing we will start the update process here

	bool result = Render();

	m_GLArea->queue_draw(); // request drawing again

	return result;
}

//---------------------------------
// Viewport::Render
//
// Draws the GL Area
//
bool Viewport::Render()
{
	try
	{
		m_GLArea->throw_if_error();

		if (m_Renderer != nullptr)
		{
			m_Renderer->OnRender();
		}
		else
		{
			// Draw pink to indicate that no renderer is attached
			STATE->SetClearColor(vec4(0.55f, 0.075f, 0.2f, 1.f));
			STATE->Clear(GL_COLOR_BUFFER_BIT);
		}

		STATE->Flush();
	}
	catch (const Gdk::GLError& gle)
	{
		LOG("Viewport::Render > An error occurred in the render callback of the GLArea", LogLevel::Warning);
		LOG(std::to_string(gle.domain()) + std::string("-") + std::to_string(gle.code()) + std::string("-") + gle.what().raw(), LogLevel::Warning);

		return false;
	}

	return true;
}

//---------------------------------
// Viewport::MakeCurrent
//
// Makes this the active viewport that openGl draws to
//
void Viewport::MakeCurrent()
{
	m_GLArea->make_current();
	try
	{
		m_GLArea->throw_if_error();
	}
	catch (const Gdk::GLError& gle)
	{
		LOG("Viewport::MakeCurrent > An error occured making the context current during realize:", LogLevel::Warning);
		LOG(std::to_string(gle.domain()) + std::string("-") + std::to_string(gle.code()) + std::string("-") + gle.what().raw(), LogLevel::Warning);
	}

	g_CurrentViewport = this;
}