#pragma once

#include <EtRendering/Shader.h>

//---------------------------------
// ForceLinking
//
// Add classes here that the linker thinks wouldn't be used by this project but are in fact used by reflection
//
void ForceLinking()
{
	FORCE_LINKING(ShaderAsset)
}


