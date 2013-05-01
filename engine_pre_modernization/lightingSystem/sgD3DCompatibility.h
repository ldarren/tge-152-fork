//-----------------------------------------------
// Synapse Gaming - Lighting Code Pack
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#ifndef _SGD3DCOMPATIBILITY_H_
#define _SGD3DCOMPATIBILITY_H_

#define SG_MAX_CHANNELS	10

class sgD3DCompatibility
{
public:
	enum sgShaderTypes
	{
		sgstEmptyShader = 0,

		sgstChannel0ModulateRGBA,
		sgstChannel1ModulateRGBA,
		sgstChannel0ModulateRGB,
		sgstChannel1ModulateRGB,
		sgstChannel0Modulate2RGBA,
		sgstChannel1Modulate2RGBA,
		sgstChannel0Modulate2RGB,
		sgstChannel1Modulate2RGB,
		sgstChannel0Modulate4RGBA,
		sgstChannel1Modulate4RGBA,
		sgstChannel0Modulate4RGB,
		sgstChannel1Modulate4RGB,
		sgstChannel0InterpolateRGB,

		sgstCount
	};

	static int sgCombineMode[SG_MAX_CHANNELS];
	static int sgScale[SG_MAX_CHANNELS];

private:
	static unsigned long sgShaders[sgstCount];

public:
	static void sgInitShaders(void *device);
	static void sgDeleteShaders(void *device);
	static unsigned long sgGetShader(long shader);
};

#endif//_SGD3DCOMPATIBILITY_H_
