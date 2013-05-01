//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


#ifndef WGL_GROUP_BEGIN
#define WGL_GROUP_BEGIN( flag )
#define UNDEF_BEGIN
#endif

#ifndef WGL_GROUP_END
#define WGL_GROUP_END()
#define UNDEF_END
#endif

//WGL_ARB_extensions_string
WGL_GROUP_BEGIN(WGL_ARB_extensions_string)
WGLEXT_FUNCTION( const char*, wglGetExtensionsStringARB, (HDC), return NULL; )
WGL_GROUP_END()

//WGL_EXT_swap_control
WGL_GROUP_BEGIN(WGL_EXT_swap_control)
WGLEXT_FUNCTION( BOOL, wglSwapIntervalEXT,(int), return 0; )
WGLEXT_FUNCTION( int, wglGetSwapIntervalEXT,(void), return 0; )
WGL_GROUP_END()

WGL_GROUP_BEGIN(WGL_3DFX_gamma_control)
WGLEXT_FUNCTION( BOOL, wglGetDeviceGammaRamp3DFX, (HDC, LPVOID), return false; )
WGLEXT_FUNCTION( BOOL, wglSetDeviceGammaRamp3DFX, (HDC, LPVOID), return false; )
WGL_GROUP_END()

#ifdef UNDEF_BEGIN
#undef WGL_GROUP_BEGIN
#undef UNDEF_BEGIN
#endif

#ifdef UNDEF_END
#undef WGL_GROUP_END
#undef UNDEF_END
#endif
