#pragma once


//---------------------------------
// I_Renderer
//
// Interface for a class that can draw anything to a viewport - a scene, a UI element etc ....
//
class I_Renderer
{
public:
	I_Renderer() = default;
	virtual ~I_Renderer() = default;
	I_Renderer& operator=(const I_Renderer&) { return *this; }

	virtual void OnInit() {}
	virtual void OnDeinit() {}
	virtual void OnResize(ivec2 const dim) { UNUSED(dim); }
	virtual void OnRender() {}
};

