//-----------------------------------------------
// Synapse Gaming - Lighting Code Pack
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include <d3dx.h>
#include "sgD3DCompatibility.h"


int sgD3DCompatibility::sgCombineMode[SG_MAX_CHANNELS];
int sgD3DCompatibility::sgScale[SG_MAX_CHANNELS];
unsigned long sgD3DCompatibility::sgShaders[sgstCount];


void sgD3DCompatibility::sgInitShaders(void *device)
{
	LPDIRECT3DDEVICE7 m_d3ddev = (LPDIRECT3DDEVICE7)device;

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0ModulateRGBA]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0ModulateRGB]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel1ModulateRGBA]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel1ModulateRGB]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0Modulate2RGBA]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0Modulate2RGB]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel1Modulate2RGBA]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel1Modulate2RGB]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0Modulate4RGBA]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0Modulate4RGB]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel1Modulate4RGBA]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel1Modulate4RGB]);

	m_d3ddev->BeginStateBlock();
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_d3ddev->EndStateBlock(&sgD3DCompatibility::sgShaders[sgstChannel0InterpolateRGB]);

	for(long i=0; i<SG_MAX_CHANNELS; i++)
	{
		sgD3DCompatibility::sgCombineMode[i] = 0;
		sgD3DCompatibility::sgScale[i] = 1;
	}
}

void sgD3DCompatibility::sgDeleteShaders(void *device)
{
	LPDIRECT3DDEVICE7 m_d3ddev = (LPDIRECT3DDEVICE7)device;

	for(long i=1; i<sgstCount; i++)
	{
		m_d3ddev->DeleteStateBlock(sgD3DCompatibility::sgShaders[i]);
	}
}

unsigned long sgD3DCompatibility::sgGetShader(long shader)
{
	if(shader == sgstChannel0ModulateRGBA)
	{
		if(sgScale[0] == 2)
			shader = sgstChannel0Modulate2RGBA;
		else if(sgScale[0] == 4)
			shader = sgstChannel0Modulate4RGBA;
	}

	if(shader == sgstChannel0ModulateRGB)
	{
		if(sgScale[0] == 2)
			shader = sgstChannel0Modulate2RGB;
		else if(sgScale[0] == 4)
			shader = sgstChannel0Modulate4RGB;
	}

	if(shader == sgstChannel1ModulateRGBA)
	{
		if(sgScale[1] == 2)
			shader = sgstChannel1Modulate2RGBA;
		else if(sgScale[1] == 4)
			shader = sgstChannel1Modulate4RGBA;
	}

	if(shader == sgstChannel1ModulateRGB)
	{
		if(sgScale[1] == 2)
			shader = sgstChannel1Modulate2RGB;
		else if(sgScale[1] == 4)
			shader = sgstChannel1Modulate4RGB;
	}

	return sgD3DCompatibility::sgShaders[shader];
}