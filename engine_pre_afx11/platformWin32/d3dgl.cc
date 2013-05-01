#include "platformWin32/d3dgl.h"
#include "platformWin32/platformWin32.h"
#include "console/console.h"

#ifdef USEICECAP		  					
#include "icapexp.h"
#endif

static void APIENTRY d3dEnd (void);
static void APIENTRY d3dBegin (GLenum mode);
static void APIENTRY d3dVertex3fv (const GLfloat *v);

static Globals g;

static void QuakeUpdateViewport()
{
    D3DVIEWPORT7 vport;
    g.m_d3ddev->GetViewport(&vport);
    if(g.m_scissoring) 
    {
        // Check whether viewport is completely within scissor rect
        RECT scirect, vwprect, xrect;
        scirect.left   = g.m_scix;
        scirect.top    = g.m_sciy;
        scirect.right  = g.m_scix + g.m_sciw;
        scirect.bottom = g.m_sciy + g.m_scih;
        vwprect.left   = g.m_vwx;
        vwprect.top    = g.m_vwy;
        vwprect.right  = g.m_vwx + g.m_vww;
        vwprect.bottom = g.m_vwy + g.m_vwh;
        if(IntersectRect(&xrect, &scirect, &vwprect))
        {
            if(EqualRect(&xrect, &vwprect))
            {
                goto updvwp;
            }
        }
        else
        {
            goto updvwp;
        }
        // BUGBUG: if the viewport overlaps the scissor rect, then none of this works
        vport.dwX = g.m_scix;
        vport.dwY = g.m_winHeight - (g.m_sciy + g.m_scih);
        vport.dwWidth = g.m_sciw;
        vport.dwHeight = g.m_scih;
        D3DVALUE dvClipX = (2.f * g.m_scix) / g.m_vww - 1.0f;
        D3DVALUE dvClipY = (2.f * (g.m_sciy + g.m_scih)) / g.m_vwh - 1.0f;
        D3DVALUE dvClipWidth = (2.f * g.m_sciw) / g.m_vww;
        D3DVALUE dvClipHeight = (2.f * g.m_scih) / g.m_vwh;
        D3DMATRIX c;
        c._11 = 2.f / dvClipWidth;
        c._21 = 0.f;
        c._31 = 0.f;
        c._41 = -1.f - 2.f * (dvClipX / dvClipWidth);
        c._12 = 0.f;
        c._22 = 2.f / dvClipHeight;
        c._32 = 0.f;
        c._42 = -1.f - 2.f * (dvClipY / dvClipHeight);
        c._13 = 0.f;
        c._23 = 0.f;
        c._33 = 1.f;
        c._43 = 0.f;
        c._14 = 0.f;
        c._24 = 0.f;
        c._34 = 0.f;
        c._44 = 1.f;
        g.m_d3ddev->MultiplyTransform(D3DTRANSFORMSTATE_PROJECTION, &c);
    }
    else
    {
updvwp:
        vport.dwX = g.m_vwx;
        vport.dwY = g.m_winHeight - (g.m_vwy + g.m_vwh);
        vport.dwWidth = g.m_vww;
        vport.dwHeight = g.m_vwh;
    }
    g.m_d3ddev->SetViewport(&vport); 
    g.m_updvwp = FALSE;
}

static void QuakeSetTexturingState()
{
	if (g.m_updvwp)
		QuakeUpdateViewport();
	if (g.m_texturing == TRUE) {
		if (g.m_texHandleValid == FALSE) {
			TexInfo &ti = g.m_tex[g.m_curstagebinding[0]];

			if (ti.m_dwStage != 0)
			{
				g.m_d3ddev->DeleteStateBlock(ti.m_block);
				g.m_d3ddev->BeginStateBlock();
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_ADDRESSU,ti.m_addu);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_ADDRESSV,ti.m_addv);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_MAGFILTER,ti.m_magmode);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_MINFILTER,ti.m_minmode);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_MIPFILTER,ti.m_mipmode);
				g.m_d3ddev->SetTexture(0, ti.m_ddsurf);
				g.m_d3ddev->EndStateBlock(&ti.m_block);
				ti.m_dwStage = 0;
				ti.m_capture = FALSE;
			}
			else if (ti.m_capture)
			{
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_ADDRESSU,ti.m_addu);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_ADDRESSV,ti.m_addv);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_MAGFILTER,ti.m_magmode);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_MINFILTER,ti.m_minmode);
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_MIPFILTER,ti.m_mipmode);
				g.m_d3ddev->SetTexture(0, ti.m_ddsurf);
				g.m_d3ddev->CaptureStateBlock(ti.m_block);
				ti.m_capture = FALSE;
			}
			else
				g.m_d3ddev->ApplyStateBlock(ti.m_block);

			switch (g.m_blendmode[0]) {
				case GL_REPLACE:
					switch (ti.m_internalformat) {
						case GL_RGB5_A1:
						case GL_RGB:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][0]);
							break;
						case GL_RGBA:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][1]);
							break;
					}
					break;
				case GL_MODULATE:
					switch (ti.m_internalformat) {
						case GL_ALPHA:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][8]);
							break;
						case GL_LUMINANCE:
						case GL_RGB5_A1:
						case GL_RGB:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][2]);
							break;
						case GL_RGBA:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][3]);
							break;
					}
					break;
				case GL_DECAL:
					switch (ti.m_internalformat) {
						case GL_RGB5_A1:
						case GL_RGB:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][4]);
							break;
						case GL_RGBA:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][5]);
							break;
					}
					break;
				case GL_BLEND:
					switch (ti.m_internalformat) {
						case GL_RGB5_A1:
						case GL_RGB:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][6]);
							break;
						case GL_RGBA:
							g.m_d3ddev->ApplyStateBlock(g.m_shaders[0][7]);
							break;
					}
					break;
			}
            
			if (g.m_mtex != FALSE) {
				TexInfo &ti2 = g.m_tex[g.m_curstagebinding[1]];

				if (ti2.m_dwStage != 1)
				{
					g.m_d3ddev->DeleteStateBlock(ti2.m_block);
					g.m_d3ddev->BeginStateBlock();
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_ADDRESSU,ti2.m_addu);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_ADDRESSV,ti2.m_addv);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_MAGFILTER,ti2.m_magmode);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_MINFILTER,ti2.m_minmode);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_MIPFILTER,ti2.m_mipmode);
					g.m_d3ddev->SetTexture(1, ti2.m_ddsurf);
					g.m_d3ddev->EndStateBlock(&ti2.m_block);
					ti2.m_dwStage = 1;
					ti2.m_capture = FALSE;
				}
				else if (ti2.m_capture)
				{
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_ADDRESSU,ti2.m_addu);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_ADDRESSV,ti2.m_addv);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_MAGFILTER,ti2.m_magmode);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_MINFILTER,ti2.m_minmode);
					g.m_d3ddev->SetTextureStageState (1, D3DTSS_MIPFILTER,ti2.m_mipmode);
					g.m_d3ddev->SetTexture(1, ti2.m_ddsurf);
					g.m_d3ddev->CaptureStateBlock(ti2.m_block);
					ti2.m_capture = FALSE;
				}
				else
					g.m_d3ddev->ApplyStateBlock(ti2.m_block);

				switch (g.m_blendmode[1]) {
					case GL_REPLACE:
						switch (ti2.m_internalformat) {
							case GL_RGB5_A1:
							case GL_RGB:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][0]);
								break;
							case GL_RGBA:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][1]);
								break;
						}
						break;
					case GL_MODULATE:
						switch (ti2.m_internalformat) {
							case GL_RGB5_A1:
							case GL_RGB:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][2]);
								break;
							case GL_RGBA:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][3]);
								break;
						}
						break;
					case GL_DECAL:
						switch (ti2.m_internalformat) {
							case GL_RGB5_A1:
							case GL_RGB:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][4]);
								break;
							case GL_RGBA:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][5]);
								break;
						}
						break;
					case GL_BLEND:
						switch (ti2.m_internalformat) {
							case GL_RGB5_A1:
							case GL_RGB:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][6]);
								break;
							case GL_RGBA:
								g.m_d3ddev->ApplyStateBlock(g.m_shaders[1][7]);
								break;
						}
						break;
				}
			}
			g.m_texHandleValid = TRUE;
		}

		if ((g.m_texgen[0] && g.m_texgenmode[0] == GL_OBJECT_LINEAR) ||
			 (g.m_texgen[1] && g.m_texgenmode[1] == GL_OBJECT_LINEAR))
		{
			if (g.m_inversedirty)
			{
				D3DMATRIX world;
				float det;

				g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, &world);
				D3DXMatrixInverse((D3DXMATRIX *) &g.m_inverseworld,&det,(const D3DXMATRIX *) &world); 
			}
			if ((g.m_inversedirty || g.m_objectdirty[0]) &&
			 	 g.m_texgen[0] && g.m_texgenmode[0] == GL_OBJECT_LINEAR)
			{
				D3DMATRIX object;
				GLfloat *u = g.m_texgenplane[0][0];
				GLfloat *v = g.m_texgenplane[0][1];

				object._11 = u[0]; object._12 = v[0]; object._13 = 0; object._14 = 0;
				object._21 = u[1]; object._22 = v[1]; object._23 = 0; object._24 = 0;
				object._31 = u[2]; object._32 = v[2]; object._33 = 0; object._34 = 0;
				object._41 = u[3]; object._42 = v[3]; object._43 = 0; object._44 = 0;

				g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&object);
				g.m_d3ddev->MultiplyTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_curtexmatrix);
				g.m_d3ddev->MultiplyTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_inverseworld);
			}	 
			if ((g.m_inversedirty || g.m_objectdirty[1]) &&
				 g.m_texgen[1] && g.m_texgenmode[1] == GL_OBJECT_LINEAR)
			{
				D3DMATRIX object;
				GLfloat *u = g.m_texgenplane[1][0];
				GLfloat *v = g.m_texgenplane[1][1];

				object._11 = u[0]; object._12 = v[0]; object._13 = 0; object._14 = 0;
				object._21 = u[1]; object._22 = v[1]; object._23 = 0; object._24 = 0;
				object._31 = u[2]; object._32 = v[2]; object._33 = 0; object._34 = 0;
				object._41 = u[3]; object._42 = v[3]; object._43 = 0; object._44 = 0;

				g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE1,&object);
				g.m_d3ddev->MultiplyTransform(D3DTRANSFORMSTATE_TEXTURE1,&g.m_inverseworld);
			}
			if (g.m_inversedirty)
				g.m_inversedirty = FALSE;
		}
    }
}

static void RawToCanon(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight, const void *lpPixels, DWORD *lpdwCanon)
{
	switch (dwFormat)
	{
		case GL_ALPHA:
			{
				for (int i = 0, j = dwWidth*dwHeight; i < j; ++i)
					lpdwCanon[i] = ((UCHAR *) lpPixels)[i] << 24;
			}
			break;
		case GL_LUMINANCE:
			{
				for (int i = 0, j = dwWidth*dwHeight; i < j; ++i)
				{
					DWORD t = ((UCHAR *) lpPixels)[i];

					lpdwCanon[i] = (t << 24) | (t << 16) | (t << 8) | t;
				}
			}
			break;
		case GL_RGB5_A1:
			{
				USHORT *rgba = (USHORT *) lpPixels;

				for (int i = 0, j = dwWidth * dwHeight; i < j; ++i)
					lpdwCanon[i] = ((rgba[i] & 0xF800) >> 8) | 0x7 |
										((rgba[i] & 0x07C0) << 5) | 0x700 |
										((rgba[i] & 0x003E) << 18) | 0x70000;
			}
			break;
	 	case GL_RGB:
			{
				UCHAR *rgb = (UCHAR *) lpPixels;

				for (int i = 0, j = dwWidth * dwHeight, k = 0; i < j; ++i, k += 3)
					lpdwCanon[i] = rgb[k] | (rgb[k+1] << 8) | (rgb[k+2] << 16);
			}
			break;
		case GL_RGBA:
			memcpy(lpdwCanon, lpPixels, dwWidth * dwHeight * sizeof(DWORD));
			break;
		default:
		{
			char buf[64];

			dSprintf(buf,64,"Wrapper: Unsupported texture format: %x\n", dwFormat);
			OutputDebugString(buf);
		}
	}
}

static void Resize(DWORD dwWidth, DWORD dwHeight, const DWORD *lpdwCanon,
						 DWORD dwNewWidth, DWORD dwNewHeight, DWORD *lpdwNewCanon)
{
	DWORD i, j;
	double rx = (double) dwWidth / (double) dwNewWidth;
	double ry = (double) dwHeight / (double) dwNewHeight;

	for (i = 0; i < dwNewHeight; ++i)
		for (j = 0; j < dwNewWidth; ++j)
			lpdwNewCanon[i * dwNewWidth + j] = lpdwCanon[((DWORD)(i * ry)) * dwWidth + (DWORD)(j * rx)];
}

static void CanonTo565(LPRECT lprect, const DWORD *lpdwCanon, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	USHORT *lpPixels = (USHORT*)lpddsd->lpSurface;
	
	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (USHORT *) ((UCHAR *) lpPixels + lpddsd->lPitch))
        for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
            lpPixels[l] = (USHORT) (((lpdwCanon[i] & 0xF8) << 8) | ((lpdwCanon[i] & 0xFC00) >> 5) | ((lpdwCanon[i] & 0xF80000) >> 19));
}

static void RGB5To565(LPRECT lprect, const USHORT *lpushort, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	USHORT *lpPixels = (USHORT*)lpddsd->lpSurface;
	
	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (USHORT *) ((UCHAR *) lpPixels + lpddsd->lPitch))
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = (lpushort[i] & 0xFFC0) | 0x20 | ((lpushort[i] & 0x3E) >> 1);
}

static void CanonTo555(LPRECT lprect, const DWORD *lpdwCanon, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	USHORT *lpPixels = (USHORT *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (USHORT *) ((UCHAR *) lpPixels + lpddsd->lPitch))
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = (USHORT) (((lpdwCanon[i] & 0xF8) << 7) | ((lpdwCanon[i] & 0xF800) >> 6) | ((lpdwCanon[i] & 0xF80000) >> 19));
}

static void RGB5To555(LPRECT lprect, const USHORT *lpushort, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	USHORT *lpPixels = (USHORT*)lpddsd->lpSurface;
	
	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (USHORT *) ((UCHAR *) lpPixels + lpddsd->lPitch))
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = lpushort[i] >> 1;
}

static void CanonTo4444(LPRECT lprect, const DWORD *lpdwCanon, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	USHORT *lpPixels = (USHORT *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (USHORT *) ((UCHAR *) lpPixels + lpddsd->lPitch))
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = (USHORT) (((lpdwCanon[i] & 0xF0) << 4) | ((lpdwCanon[i] & 0xF000) >> 8) | ((lpdwCanon[i] & 0xF00000) >> 20) | ((lpdwCanon[i] & 0xF0000000) >> 16));
}

static void CanonTo8888(LPRECT lprect, const DWORD *lpdwCanon, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	DWORD *lpPixels = (DWORD*)lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (DWORD *) ((UCHAR *) lpPixels + lpddsd->lPitch))
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = ((lpdwCanon[i] & 0xFF00FF00) | ((lpdwCanon[i] & 0xFF) << 16) | ((lpdwCanon[i] & 0xFF0000) >> 16));
}

static void ALPHATo8888(LPRECT lprect, const UCHAR *lpuchar, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	DWORD *lpPixels = (DWORD *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (DWORD *) ((UCHAR *) lpPixels + lpddsd->lPitch)) 
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = lpuchar[i] << 24;
}

static void LUMINANCETo8888(LPRECT lprect, const UCHAR *lpuchar, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	DWORD *lpPixels = (DWORD *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (DWORD *) ((UCHAR *) lpPixels + lpddsd->lPitch)) 
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
		{
			DWORD t = lpuchar[i];
			
			lpPixels[l] = t << 24 | t << 16 | t << 8 | t;
		}
}

static void RGBTo8888(LPRECT lprect, const UCHAR *lpuchar, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	DWORD *lpPixels = (DWORD *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (DWORD *) ((UCHAR *) lpPixels + lpddsd->lPitch)) 
		for (j = lprect->left, l = 0; j < lprect->right; ++j, i += 3, ++l)
			lpPixels[l] = (lpuchar[i]) << 16 | (lpuchar[i+1] << 8) | lpuchar[i+2];
}

static void RGBATo8888(LPRECT lprect, const DWORD *lpdwCanon, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	DWORD *lpPixels = (DWORD *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (DWORD *) ((UCHAR *) lpPixels + lpddsd->lPitch)) 
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = ((lpdwCanon[i] & 0xFF00FF00) | ((lpdwCanon[i] & 0xFF) << 16) | ((lpdwCanon[i] & 0xFF0000) >> 16));
}

static void CanonTo8(LPRECT lprect, const DWORD *lpdwCanon, LPDDSURFACEDESC2 lpddsd)
{
	LONG i, j, k, l;
	UCHAR *lpPixels = (UCHAR *) lpddsd->lpSurface;

	for (k = lprect->top, i = 0; k < lprect->bottom; ++k, lpPixels = (UCHAR *) lpPixels + lpddsd->lPitch)
		for (j = lprect->left, l = 0; j < lprect->right; ++j, ++i, ++l)
			lpPixels[l] = (UCHAR)(lpdwCanon[i] & 0xFF);
}

static void LoadSurface(LPDIRECTDRAWSURFACE7 lpDDS, DWORD dwFormat,
								DWORD dwWidth, DWORD dwHeight, DWORD dwNewWidth, DWORD dwNewHeight,
								const void *pixels)
{
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	DWORD *lpdwCanon, *lpdwNewCanon;	
	RECT rect;
    
	/*
	 * Lock the surface so it can be filled with the texture
	 */
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddrval = lpDDS->Lock(NULL, &ddsd, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (ddrval != DD_OK) {
		lpDDS->Release();
		OutputDebugString("Lock failed while loading surface\n");

		return;
	}
	SetRect(&rect, 0, 0, ddsd.dwWidth, ddsd.dwHeight);

	if (dwFormat == GL_RGB5_A1 && ddsd.ddpfPixelFormat.dwRBitMask == 0xF800 && 
		 dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGB5To565(&rect,(const USHORT *) pixels,&ddsd);
	else if (dwFormat == GL_RGB5_A1 && ddsd.ddpfPixelFormat.dwRBitMask == 0x7C00 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGB5To555(&rect,(const USHORT *) pixels,&ddsd);
	else if (dwFormat == GL_ALPHA && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		ALPHATo8888(&rect,(const UCHAR *) pixels,&ddsd);
	else if (dwFormat == GL_LUMINANCE && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		LUMINANCETo8888(&rect,(const UCHAR *) pixels,&ddsd);
	else if (dwFormat == GL_RGB && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGBTo8888(&rect,(const UCHAR *) pixels,&ddsd);
	else if (dwFormat == GL_RGBA && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGBATo8888(&rect,(const DWORD *) pixels,&ddsd);
	else { 
		/*
		 * Convert the GL texture into a canonical format (8888),
		 * so that we can cleanly do image ops (such as resize) without 
		 * having to worry about the bit format.
		 */
		lpdwCanon = (DWORD *) malloc(dwWidth * dwHeight * sizeof(DWORD));
		RawToCanon(dwFormat, dwWidth, dwHeight, pixels, lpdwCanon);
		/* Now resize the canon image */
		if (dwWidth != dwNewWidth || dwHeight != dwNewHeight) {
			lpdwNewCanon = (DWORD *) malloc(dwNewWidth * dwNewHeight * sizeof(DWORD));
			Resize(dwWidth, dwHeight, lpdwCanon, dwNewWidth, dwNewHeight, lpdwNewCanon);
			free(lpdwCanon);
		}
		else
			lpdwNewCanon = lpdwCanon;
    
		/* Copy  the texture into the surface */
		if (ddsd.ddpfPixelFormat.dwLuminanceBitMask == 0xFF)
			CanonTo8(&rect, lpdwNewCanon, &ddsd);
		else if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0xF000)
			CanonTo4444(&rect, lpdwNewCanon, &ddsd);
		else if (ddsd.ddpfPixelFormat.dwRBitMask == 0xF800)
			CanonTo565(&rect, lpdwNewCanon, &ddsd);
		else if (ddsd.ddpfPixelFormat.dwRBitMask == 0x7C00)
			CanonTo555(&rect, lpdwNewCanon, &ddsd);
		else
			CanonTo8888(&rect, lpdwNewCanon, &ddsd);
		free(lpdwNewCanon);
	}

	/*
	 * unlock the surface
	 */
	lpDDS->Unlock(NULL);
}

static HRESULT LoadSubSurface(LPDIRECTDRAWSURFACE7 lpDDS, DWORD dwFormat,
										DWORD dwWidth, DWORD dwHeight, const void *pixels, 
										LPRECT lpsubimage)
{
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	DWORD *lpdwCanon, *lpdwNewCanon;	
	DWORD dwNewWidth=lpsubimage->right-lpsubimage->left;
	DWORD dwNewHeight=lpsubimage->bottom-lpsubimage->top;
	/*
	 * Lock the surface so it can be filled with the texture
	 */
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddrval = lpDDS->Lock(lpsubimage, &ddsd, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (ddrval != DD_OK) {
		lpDDS->Release();
		OutputDebugString("Lock failed while loading surface\n");

		return ddrval;
	}

	if (dwFormat == GL_RGB5_A1 && ddsd.ddpfPixelFormat.dwRBitMask == 0xF800 && 
		 dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGB5To565(lpsubimage,(const USHORT *) pixels,&ddsd);
	else if (dwFormat == GL_RGB5_A1 && ddsd.ddpfPixelFormat.dwRBitMask == 0x7C00 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGB5To555(lpsubimage,(const USHORT *) pixels,&ddsd);
	else if (dwFormat == GL_ALPHA && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		ALPHATo8888(lpsubimage,(const UCHAR *) pixels,&ddsd);
	else if (dwFormat == GL_LUMINANCE && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		LUMINANCETo8888(lpsubimage,(const UCHAR *) pixels,&ddsd);
	else if (dwFormat == GL_RGB && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGBTo8888(lpsubimage,(const UCHAR *) pixels,&ddsd);
	else if (dwFormat == GL_RGBA && ddsd.ddpfPixelFormat.dwRBitMask == 0xFF0000 && 
				dwWidth == dwNewWidth && dwHeight == dwNewHeight)
		RGBATo8888(lpsubimage,(const DWORD *) pixels,&ddsd);
	else {
		/*
		 * Convert the GL texture into a canonical format (8888),
		 * so that we can cleanly do image ops (such as resize) without 
		 * having to worry about the bit format.
		 */
		lpdwCanon = (DWORD *) malloc(dwWidth * dwHeight * sizeof(DWORD));
		RawToCanon(dwFormat, dwWidth, dwHeight, pixels, lpdwCanon);
		if (dwWidth != dwNewWidth || dwHeight != dwNewHeight)
		{
			/* Now resize the canon image */
			lpdwNewCanon = (DWORD *) malloc(dwNewWidth * dwNewHeight * sizeof(DWORD));
			Resize(dwWidth, dwHeight, lpdwCanon, dwNewWidth, dwNewHeight, lpdwNewCanon);
			free(lpdwCanon);        
		}
		else
			lpdwNewCanon=lpdwCanon;
		/* Copy the texture into the surface */
		if (ddsd.ddpfPixelFormat.dwLuminanceBitMask == 0xFF)
			CanonTo8(lpsubimage,lpdwNewCanon,&ddsd);
		else if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0xF000)
			CanonTo4444(lpsubimage,lpdwNewCanon,&ddsd);
		else if (ddsd.ddpfPixelFormat.dwRBitMask == 0xF800)
			CanonTo565(lpsubimage,lpdwNewCanon,&ddsd);
		else if (ddsd.ddpfPixelFormat.dwRBitMask == 0x7C00)
			CanonTo555(lpsubimage, lpdwNewCanon, &ddsd);
		else
			CanonTo8888(lpsubimage, lpdwNewCanon, &ddsd);
		free(lpdwNewCanon);
	}
	/*
	 * unlock the surface
	 */
	lpDDS->Unlock(NULL);

	return DD_OK;
}

///////////////////////////// BEGIN API ENTRIES ///////////////////////////////////////////////////

static void APIENTRY d3dActiveTextureARB(GLenum texture)
{
    g.m_curtgt = texture == GL_TEXTURE0_ARB ? 0 : 1;
}

static void APIENTRY d3dAlphaFunc (GLenum func, GLclampf ref)
{
    int funcvalue = -1;
    switch(func) {
    case GL_NEVER:
        funcvalue=D3DCMP_NEVER;
        break;
    case GL_LESS: 
        funcvalue=D3DCMP_LESS;
        break;
    case GL_EQUAL: 
        funcvalue=D3DCMP_EQUAL;
        break;
    case GL_LEQUAL: 
        funcvalue=D3DCMP_LESSEQUAL;
        break;
    case GL_GREATER: 
        funcvalue=D3DCMP_GREATER;
        break;
    case GL_NOTEQUAL: 
        funcvalue=D3DCMP_NOTEQUAL;
        break;
    case GL_GEQUAL: 
        funcvalue=D3DCMP_GREATEREQUAL;
        break;
    case GL_ALWAYS: 
        funcvalue=D3DCMP_ALWAYS;
        break;
    }
    if (funcvalue >= 0) {
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, funcvalue);
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHAREF, (D3DFIXED)(ref * 255.f));
    }
}

static void APIENTRY d3dArrayElement (GLint i)
{ 
	if (g.m_usetexcoordary[0])
	{
		GLfloat *start0 = (GLfloat *)	&((unsigned char *) g.m_texcoordary[0])[i*g.m_texcoordstride[0]];	

		g.m_tu = start0[0];
		g.m_tv = start0[1];
	}
	if (g.m_usetexcoordary[1])
	{
		GLfloat *start1 = (GLfloat *)	&((unsigned char *) g.m_texcoordary[1])[i*g.m_texcoordstride[1]];

		g.m_tu2 = start1[0];
		g.m_tv2 = start1[1];
	}
	if (g.m_usecolorary)
		if (g.m_colortype == GL_UNSIGNED_BYTE)
		{
#ifdef _X86_
			_asm
			{
				mov ebx, g.m_colorary;
				mov eax, i;
				mov edx, g.m_colorstride;
				mul edx;
				cld;
				mov esi, eax;
				mov edx, 0x00FF00FF;
				mov eax, [ebx + esi];
				mov ecx, eax;
				and eax, edx;
				not edx;
				rol eax, 16;
				and ecx, edx;
				or  eax, ecx;
				mov g.m_color, eax;
			}
#else
			GLubyte *start = &((GLubyte *) g.m_colorary)[i*g.m_colorstride];

			g.m_color = RGBA_MAKE(start[0],start[1],start[2],start[3]);
#endif
		}
		else
		{
			GLfloat *start = (GLfloat *) &((unsigned char *) g.m_colorary)[i*g.m_colorstride];
			static float two55 = 255.f;
			unsigned int R, G, B, A;

#ifdef _X86_
			__asm {
				mov ebx, start;
				fld [ebx];
				fld [ebx + 4];
				fld [ebx + 8];
				fld [ebx + 12];
				fld two55;
				fmul st(1), st(0);
				fmul st(2), st(0);
				fmul st(3), st(0);
				fmulp st(4), st(0);
				fistp A;
				fistp B;
				fistp G;
				fistp R;
				mov edx, A;
				cmp edx, 255;
				jle pt1;
				mov edx, 255;
		pt1:	mov eax, B;
				cmp eax, 255;
				jle pt2;
				mov eax, 255;
		pt2:	mov ebx, G;
				cmp ebx, 255;
				jle pt3;
				mov ebx, 255;
		pt3:	mov ecx, R;
				cmp ecx, 255;
				jle pt4;
				mov ecx, 255;
		pt4:	shl ebx, 8;
				shl ecx, 16;
				shl edx, 24;
				or eax, ebx;
				or ecx, edx;
				or eax, ecx;
				mov g.m_color, eax;
			}
#else
			R = (unsigned int)(start[0] * two55);
			G = (unsigned int)(start[1] * two55);
			B = (unsigned int)(start[2] * two55);
			A = (unsigned int)(start[3] * two55);
			if (R > 255)
				R = 255;
			if (G > 255)
				G = 255;
			if (B > 255)
				B = 255;
			if (A > 255)
				A = 255;
			g.m_color = RGBA_MAKE(R, G, B, A);
#endif
		}
	if (g.m_usenormalary)
	{
		unsigned char *start = &((unsigned char *) g.m_normalary)[i*g.m_normalstride];

		memcpy(&g.m_nx,start,sizeof(D3DVALUE)*3);
	}
	if (g.m_nfv[g.m_comp] + g.m_vcnt[g.m_comp] >= (VBUFSIZE - MAXVERTSPERPRIM))
	{
		if (g.m_prim == GL_TRIANGLES)
		{
			if (g.m_vcnt[g.m_comp] % 3 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
		else if (g.m_prim == GL_QUADS)
		{
			if (g.m_vcnt[g.m_comp] % 4 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
		else if(g.m_prim == GL_LINES)
		{
			if (g.m_vcnt[g.m_comp] % 2 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
	}

#ifdef _X86_
	_asm
	{   
		mov     eax, i;
		mov	  edx, g.m_vertexstride;
		mul	  edx;
		cld;
		mov     esi, g.m_vertexary;
		mov     ecx, g.m_comp;
		mov     edi, g.m_verts;
		add     esi, eax;
		mov     eax, g.m_vcnt[ecx * 4];
		lea     edx, [eax + 1];
		mov     g.m_vcnt[ecx * 4], edx;
		lea     edx, [ecx * 8 + 28];
		mul     edx;
		cld;
		add     edi, eax;
		movsd;
		movsd;
		movsd;
		lea     esi, g.m_nx;
		lea     ecx, [ecx * 2 + 4];
		rep     movsd;
    }
#else
	D3DVALUE *d3dv = &(((D3DVALUE*)g.m_verts)[g.m_vcnt[g.m_comp]++ * (g.m_comp * 2 + 7)]);
	GLfloat *v = (GLfloat *) &((unsigned char *) g.m_vertexary)[i * g.m_vertexstride];
		
	*(d3dv++) = v[0];
	*(d3dv++) = v[1];
	*(d3dv++) = v[2];
	memcpy(d3dv, &g.m_nx, sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR) + g.m_comp*sizeof(D3DVALUE)*2);
#endif
}

static void APIENTRY d3dBegin (GLenum mode)
{    
    g.m_prim = mode;
    if(g.m_texturing) 
    {
        if(g.m_mtex)
        {
            g.m_comp = 2;
            g.m_vcnt[2] = 0;
            if(g.m_nfv[2] > (VBUFSIZE - MAXVERTSPERPRIM)) // check if space available
            {
                g.m_mtvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
                g.m_nfv[2] = 0;
            }
            else
            {
                g.m_mtvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
                g.m_verts = &(((QuakeMTVertex*)g.m_verts)[g.m_nfv[2]]);
            }
        }
        else
        {
            g.m_comp = 1;
            g.m_vcnt[1] = 0;
            if(g.m_nfv[1] > (VBUFSIZE - MAXVERTSPERPRIM)) // check if space available
            {
                g.m_tvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
                g.m_nfv[1] = 0;
            }
            else
            {
                g.m_tvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
                g.m_verts = &(((QuakeTVertex*)g.m_verts)[g.m_nfv[1]]);
            }
        }
    }
    else
    {
        g.m_comp = 0;
        g.m_vcnt[0] = 0;
        if(g.m_nfv[0] > (VBUFSIZE - MAXVERTSPERPRIM)) // check if space available
        {
            g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
            g.m_nfv[0] = 0;
        }
        else
        {
            g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
            g.m_verts = &(((QuakeVertex*)g.m_verts)[g.m_nfv[0]]);
        }
    }    
	QuakeSetTexturingState();
}

static void APIENTRY d3dBindTexture (GLenum target, GLuint texture)
{
	target;

	g.m_curstagebinding[g.m_curtgt] = texture;
	g.m_texHandleValid = FALSE;
}

static void APIENTRY d3dBlendFunc (GLenum sfactor, GLenum dfactor)
{
    int svalue = -1, dvalue = -1;
    
    switch(sfactor) {
    case GL_ZERO:
        svalue=D3DBLEND_ZERO;
        break;
    case GL_ONE:
        svalue=D3DBLEND_ONE;
        break;
    case GL_DST_COLOR:
        svalue=D3DBLEND_DESTCOLOR;
        break;
    case GL_ONE_MINUS_DST_COLOR:
        svalue=D3DBLEND_INVDESTCOLOR;
        break;
    case GL_SRC_ALPHA:
        svalue=D3DBLEND_SRCALPHA;
        break;
    case GL_ONE_MINUS_SRC_ALPHA:
        svalue=D3DBLEND_INVSRCALPHA;
        break;
    case GL_DST_ALPHA:
        svalue=D3DBLEND_DESTALPHA;
        break;
    case GL_ONE_MINUS_DST_ALPHA:
        svalue=D3DBLEND_INVDESTALPHA;
        break;
    case GL_SRC_ALPHA_SATURATE:
        svalue=D3DBLEND_SRCALPHASAT;
        break;
    }
    switch(dfactor) {
    case GL_ZERO:
        dvalue=D3DBLEND_ZERO;
        break;
    case GL_ONE:
        dvalue=D3DBLEND_ONE;
        break;
    case GL_SRC_COLOR:
        dvalue=D3DBLEND_SRCCOLOR;
        break;
    case GL_ONE_MINUS_SRC_COLOR:
        dvalue=D3DBLEND_INVSRCCOLOR;
        break;
    case GL_SRC_ALPHA:
        dvalue=D3DBLEND_SRCALPHA;
        break;
    case GL_ONE_MINUS_SRC_ALPHA:
        dvalue=D3DBLEND_INVSRCALPHA;
        break;
    case GL_DST_ALPHA:
        dvalue=D3DBLEND_DESTALPHA;
        break;
    case GL_ONE_MINUS_DST_ALPHA:
        dvalue=D3DBLEND_INVDESTALPHA;
        break;
    }
    
    if (svalue >= 0) g.m_d3ddev->SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)svalue);
    if (dvalue >= 0) g.m_d3ddev->SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)dvalue);
    
}

static void APIENTRY d3dClear (GLbitfield mask)
{
	DWORD flags = 0;

	if (mask & GL_COLOR_BUFFER_BIT)
		flags |= D3DCLEAR_TARGET;
	if (mask & GL_DEPTH_BUFFER_BIT)
		flags |= D3DCLEAR_ZBUFFER;
	if (mask & GL_STENCIL_BUFFER_BIT)
		flags |= D3DCLEAR_STENCIL;

	if (g.m_updvwp)
		QuakeUpdateViewport();

	g.m_d3ddev->Clear(0, NULL, flags, g.m_clearColor, (D3DVALUE) g.m_clearDepth, 0);
}

static void APIENTRY d3dClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    static float two55 = 255.f;
    unsigned int R, G, B, A;
#ifdef _X86_
    __asm {
        fld red;
        fld green;
        fld blue;
        fld alpha;
        fld two55;
        fmul st(1), st(0);
        fmul st(2), st(0);
        fmul st(3), st(0);
        fmulp st(4), st(0);
        fistp A;
        fistp B;
        fistp G;
        fistp R;
        mov edx, A;
        cmp edx, 255;
        jle pt1;
        mov edx, 255;
pt1:    mov eax, B;
        cmp eax, 255;
        jle pt2;
        mov eax, 255;
pt2:    mov ebx, G;
        cmp ebx, 255;
        jle pt3;
        mov ebx, 255;
pt3:    mov ecx, R;
        cmp ecx, 255;
        jle pt4;
        mov ecx, 255;
pt4:    shl ebx, 8;
        shl ecx, 16;
        shl edx, 24;
        or eax, ebx;
        or ecx, edx;
        or eax, ecx;
        mov g.m_clearColor, eax;
    }
#else
    R = (unsigned int)(red * two55);
    G = (unsigned int)(green * two55);
    B = (unsigned int)(blue * two55);
    A = (unsigned int)(alpha * two55);
    if(R > 255)
        R = 255;
    if(G > 255)
        G = 255;
    if(B > 255)
        B = 255;
    if(A > 255)
        A = 255;
    g.m_clearColor = RGBA_MAKE(R, G, B, A);
#endif
}

static void APIENTRY d3dClearDepth (GLclampd depth)
{ 
    g.m_clearDepth = depth;
}

static void APIENTRY d3dClientActiveTextureARB(GLenum texture)
{
    g.m_client_active_texture_arb = texture == GL_TEXTURE0_ARB ? 0 : 1;
}

static void APIENTRY d3dClipPlane (GLenum plane, const GLdouble *equation)
{
	plane;
	equation;
}

static void APIENTRY d3dColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    static float two55 = 255.f;
    unsigned int R, G, B;
#ifdef _X86_
    __asm {
        fld red;
        fld green;
        fld blue;
        fld two55;
        fmul st(1), st(0);
        fmul st(2), st(0);
        fmulp st(3), st(0);
        fistp B;
        fistp G;
        fistp R;
        mov eax, B;
        cmp eax, 255;
        jle pt1;
        mov eax, 255;
pt1:    mov ebx, G;
        cmp ebx, 255;
        jle pt2;
        mov ebx, 255;
pt2:    mov ecx, R;
        cmp ecx, 255;
        jle pt3;
        mov ecx, 255;
pt3:    shl ebx, 8;
        shl ecx, 16;
        or eax, ebx;
        or ecx, 0xFF000000;
        or eax, ecx;
        mov g.m_color, eax;
    }
#else
    R = (unsigned int)(red * two55);
    G = (unsigned int)(green * two55);
    B = (unsigned int)(blue * two55);
    if(R > 255)
        R = 255;
    if(G > 255)
        G = 255;
    if(B > 255)
        B = 255;
    g.m_color = RGBA_MAKE(R, G, B, 255);
#endif
}

static void APIENTRY d3dColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
	g.m_color = RGBA_MAKE(red, green, blue, 255);
}

static void APIENTRY d3dColor3ubv (const GLubyte *v)
{
    g.m_color = RGBA_MAKE(v[0], v[1], v[2], 255);
}

static void APIENTRY d3dColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{ 
    g.m_color = RGBA_MAKE(red, green, blue, alpha);
}

static void APIENTRY d3dColor4ubv (const GLubyte *v)
{
#ifdef _X86_
    _asm
    {
        mov ebx, v;
        mov edx, 0x00FF00FF;
        mov eax, [ebx];
        mov ecx, eax;
        and eax, edx;
        not edx;
        rol eax, 16;
        and ecx, edx;
        or  eax, ecx;
        mov g.m_color, eax;
    }
#else
    g.m_color = RGBA_MAKE(v[0], v[1], v[2], v[3]);
#endif
}

static void APIENTRY d3dColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    static float two55 = 255.f;
    unsigned int R, G, B, A;
#ifdef _X86_
    __asm {
        fld red;
        fld green;
        fld blue;
        fld alpha;
        fld two55;
        fmul st(1), st(0);
        fmul st(2), st(0);
        fmul st(3), st(0);
        fmulp st(4), st(0);
        fistp A;
        fistp B;
        fistp G;
        fistp R;
        mov edx, A;
        cmp edx, 255;
        jle pt1;
        mov edx, 255;
pt1:    mov eax, B;
        cmp eax, 255;
        jle pt2;
        mov eax, 255;
pt2:    mov ebx, G;
        cmp ebx, 255;
        jle pt3;
        mov ebx, 255;
pt3:    mov ecx, R;
        cmp ecx, 255;
        jle pt4;
        mov ecx, 255;
pt4:    shl ebx, 8;
        shl ecx, 16;
        shl edx, 24;
        or eax, ebx;
        or ecx, edx;
        or eax, ecx;
        mov g.m_color, eax;
    }
#else
    R = (unsigned int)(red * two55);
    G = (unsigned int)(green * two55);
    B = (unsigned int)(blue * two55);
    A = (unsigned int)(alpha * two55);
    if(R > 255)
        R = 255;
    if(G > 255)
        G = 255;
    if(B > 255)
        B = 255;
    if(A > 255)
        A = 255;
    g.m_color = RGBA_MAKE(R, G, B, A);
#endif
}

static void APIENTRY d3dColor4fv (const GLfloat *v)
{
    static float two55 = 255.f;
    unsigned int R, G, B, A;

#ifdef _X86_
    __asm {
        mov ebx, v;
        fld [ebx];
        fld [ebx + 4];
        fld [ebx + 8];
        fld [ebx + 12];
        fld two55;
        fmul st(1), st(0);
        fmul st(2), st(0);
        fmul st(3), st(0);
        fmulp st(4), st(0);
        fistp A;
        fistp B;
        fistp G;
        fistp R;
        mov edx, A;
        cmp edx, 255;
        jle pt1;
        mov edx, 255;
pt1:    mov eax, B;
        cmp eax, 255;
        jle pt2;
        mov eax, 255;
pt2:    mov ebx, G;
        cmp ebx, 255;
        jle pt3;
        mov ebx, 255;
pt3:    mov ecx, R;
        cmp ecx, 255;
        jle pt4;
        mov ecx, 255;
pt4:    shl ebx, 8;
        shl ecx, 16;
        shl edx, 24;
        or eax, ebx;
        or ecx, edx;
        or eax, ecx;
        mov g.m_color, eax;
    }
#else
    R = (unsigned int)(v[0] * two55);
    G = (unsigned int)(v[1] * two55);
    B = (unsigned int)(v[2] * two55);
    A = (unsigned int)(v[3] * two55);
    if(R > 255)
        R = 255;
    if(G > 255)
        G = 255;
    if(B > 255)
        B = 255;
    if(A > 255)
        A = 255;
    g.m_color = RGBA_MAKE(R, G, B, A);
#endif
}

static void APIENTRY d3dColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (size == 4 && (type == GL_UNSIGNED_BYTE || type == GL_FLOAT))
	{
		if (stride)
			g.m_colorstride = stride;
		else
			g.m_colorstride = (type == GL_UNSIGNED_BYTE) ? 4 : 16;
		g.m_colortype = type;
		g.m_colorary = pointer;
	}
	else
		OutputDebugString("Color array not supported\n");
}

static void APIENTRY d3dCullFace (GLenum mode)
{
    g.m_cullMode = mode;
    if(g.m_cullEnabled == TRUE){
        DWORD statevalue;
        if ((mode == GL_BACK && g.m_frontFace == GL_CCW) ||
				(mode == GL_FRONT && g.m_frontFace == GL_CW))
            statevalue=D3DCULL_CW;
        else
            statevalue=D3DCULL_CCW;
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_CULLMODE, statevalue);
    }
}

static void APIENTRY d3dGenTextures (GLsizei n, GLuint *textures)
{
	GLListManip<GLuint> freeTextures(&g.m_freeTextures);

	for (int i = 0; i < n; ++i)
	{
		textures[i] = freeTextures();
		freeTextures.remove();
	}
}

static void APIENTRY d3dDeleteTextures (GLsizei n, const GLuint *textures)
{
	GLListManip<GLuint> freeTextures(&g.m_freeTextures);

	for(int i = 0; i < n; ++i) {
		TexInfo &ti = g.m_tex[textures[i]];

		if(ti.m_ddsurf != 0) {
			ti.m_ddsurf->Release();
			ti.m_ddsurf = 0;
		}
		if(ti.m_block != 0)
		{
			g.m_d3ddev->DeleteStateBlock(ti.m_block);
			ti.m_block = 0;
		}
		ti.m_capture = FALSE;
		ti.m_dwStage = 0;
		ti.m_minmode = D3DTFN_POINT;
		ti.m_magmode = D3DTFG_LINEAR;
		ti.m_mipmode = D3DTFP_LINEAR;
		ti.m_addu = D3DTADDRESS_WRAP;
		ti.m_addv = D3DTADDRESS_WRAP;

		freeTextures.insert(textures[i]);
    }
}

static void APIENTRY d3dDepthFunc (GLenum func)
{
    int state = -1;
    switch(func) {
    case GL_NEVER:
        state=D3DCMP_NEVER;
        break;
    case GL_LESS: 
        state=D3DCMP_LESS;
        break;
    case GL_EQUAL: 
        state=D3DCMP_EQUAL;
        break;
    case GL_LEQUAL: 
        state=D3DCMP_LESSEQUAL;
        break;
    case GL_GREATER: 
        state=D3DCMP_GREATER;
        break;
    case GL_NOTEQUAL: 
        state=D3DCMP_NOTEQUAL;
        break;
    case GL_GEQUAL: 
        state=D3DCMP_GREATEREQUAL;
        break;
    case GL_ALWAYS: 
        state=D3DCMP_ALWAYS;
        break;
    }
    if(state >= 0)
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZFUNC, state);
}

static void APIENTRY d3dDepthMask (GLboolean flag)
{
    if(flag == 0)
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
    else
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
}

static void APIENTRY d3dDepthRange (GLclampd zNear, GLclampd zFar)
{
    if(zFar == zNear)
    {
        zFar  += .001;
        zNear -= .001;
    }
    if(zNear < 0.)
        zNear = 0.;
    if(zNear > 1.)
        zNear = 1.;
    if(zFar < 0.)
        zFar = 0.;
    if(zFar > 1.)
        zFar = 1.;
    D3DVIEWPORT7 vport;
    g.m_d3ddev->GetViewport(&vport);
    vport.dvMinZ = (D3DVALUE)zNear;
    vport.dvMaxZ = (D3DVALUE)zFar;
    g.m_d3ddev->SetViewport(&vport);
}

static void APIENTRY d3dEnd (void)
{
	if (g.m_texturing)
	{
        if (g.m_mtex)
        {
			if (g.m_vcnt[2] && g.m_prim == GL_LINE_LOOP)
			{
				D3DVALUE *d3dv = (D3DVALUE *) g.m_verts;

				memcpy(&g.m_nx,&d3dv[3],sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR) + sizeof(D3DVALUE)*4);
				d3dVertex3fv(d3dv);
			}
				g.m_mtvbuf->Unlock();
            if(g.m_vcnt[2] == 0)
                return;
            unsigned i;
            switch(g.m_prim) 
            {
            case GL_LINES:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINELIST, g.m_mtvbuf, g.m_nfv[2], g.m_vcnt[2], 0);
                g.m_nfv[2] += g.m_vcnt[2];
                break;
				case GL_LINE_LOOP:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINESTRIP, g.m_mtvbuf, g.m_nfv[2], g.m_vcnt[2], 0);
					g.m_nfv[2] += g.m_vcnt[2];
					break;
				case GL_TRIANGLES:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_mtvbuf, g.m_nfv[2], g.m_vcnt[2], 0);
                g.m_nfv[2] += g.m_vcnt[2];
                break;
            case GL_TRIANGLE_STRIP:
				case GL_QUAD_STRIP:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_mtvbuf, g.m_nfv[2], g.m_vcnt[2], 0);
                g.m_nfv[2] += g.m_vcnt[2];
                break;
            case GL_POLYGON:
            case GL_TRIANGLE_FAN:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_mtvbuf, g.m_nfv[2], g.m_vcnt[2], 0);
                g.m_nfv[2] += g.m_vcnt[2];
                break;
            case GL_QUADS:
                for(i = 0; i < g.m_vcnt[2]; i += 4) 
                {
                    g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_mtvbuf, g.m_nfv[2], 4, 0);
                    g.m_nfv[2] += 4;
                }
                break;
            default:
					{
               	char buf[64];
               	
               	dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",g.m_prim);
               	OutputDebugString(buf);
					}
            }
        }
        else
        {
			if (g.m_vcnt[1] && g.m_prim == GL_LINE_LOOP)
			{
				D3DVALUE *d3dv = (D3DVALUE *) g.m_verts;

				memcpy(&g.m_nx,&d3dv[3],sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR) + sizeof(D3DVALUE)*2);	
				d3dVertex3fv(d3dv);
			}
				g.m_tvbuf->Unlock();
            if(g.m_vcnt[1] == 0)
                return;
            unsigned i;
            switch(g.m_prim) 
            {
            case GL_LINES:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINELIST, g.m_tvbuf, g.m_nfv[1], g.m_vcnt[1], 0);
                g.m_nfv[1] += g.m_vcnt[1];
                break;
				case GL_LINE_LOOP:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINESTRIP, g.m_tvbuf, g.m_nfv[1], g.m_vcnt[1], 0);
					g.m_nfv[1] += g.m_vcnt[1];
					break;
            case GL_TRIANGLES:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_tvbuf, g.m_nfv[1], g.m_vcnt[1], 0);
                g.m_nfv[1] += g.m_vcnt[1];
                break;
            case GL_TRIANGLE_STRIP:
            case GL_QUAD_STRIP:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_tvbuf, g.m_nfv[1], g.m_vcnt[1], 0);
                g.m_nfv[1] += g.m_vcnt[1];
                break;
            case GL_POLYGON:
            case GL_TRIANGLE_FAN:
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_tvbuf, g.m_nfv[1], g.m_vcnt[1], 0);
                g.m_nfv[1] += g.m_vcnt[1];
                break;
            case GL_QUADS:
                for(i = 0; i < g.m_vcnt[1]; i += 4) 
                {
                    g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_tvbuf, g.m_nfv[1], 4, 0);
                    g.m_nfv[1] += 4;
                }
                break;
				default:
                {
               	char buf[64];
               	
               	dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",g.m_prim);
               	OutputDebugString(buf);
					}
            }
        }
    }
    else
    {
		if (g.m_vcnt[0] && g.m_prim == GL_LINE_LOOP)
		{
			D3DVALUE *d3dv = (D3DVALUE *) g.m_verts;

			memcpy(&g.m_nx,&d3dv[3],sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR));
			d3dVertex3fv(d3dv);
		}
		  g.m_vbuf->Unlock();
        if(g.m_vcnt[0] == 0)
            return;
        unsigned i;
        switch(g.m_prim) 
        {
        case GL_LINES:
            g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINELIST, g.m_vbuf, g.m_nfv[0], g.m_vcnt[0], 0);
            g.m_nfv[0] += g.m_vcnt[0];
            break;
		  case GL_LINE_LOOP:
				g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINESTRIP, g.m_vbuf, g.m_nfv[0], g.m_vcnt[0], 0);
				g.m_nfv[0] += g.m_vcnt[0];
				break;
        case GL_TRIANGLES:
            g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_vbuf, g.m_nfv[0], g.m_vcnt[0], 0);
            g.m_nfv[0] += g.m_vcnt[0];
            break;
        case GL_TRIANGLE_STRIP:
		  case GL_QUAD_STRIP:
            g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_vbuf, g.m_nfv[0], g.m_vcnt[0], 0);
            g.m_nfv[0] += g.m_vcnt[0];
            break;
        case GL_POLYGON:
        case GL_TRIANGLE_FAN:
            g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_vbuf, g.m_nfv[0], g.m_vcnt[0], 0);
            g.m_nfv[0] += g.m_vcnt[0];
            break;
        case GL_QUADS:
            for(i = 0; i < g.m_vcnt[0]; i += 4) 
            {
                g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_vbuf, g.m_nfv[0], 4, 0);
                g.m_nfv[0] += 4;
            }
            break;
        default:
            {
            	char buf[64];
               	
               dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",g.m_prim);
               OutputDebugString(buf);
				}
        }
    }
}

static void APIENTRY d3dDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	if (count == 0)
		return;

	if (count > g.m_numIndices)
	{
		g.m_numIndices = count;
		delete [] g.m_wIndices;
		g.m_wIndices = new WORD[g.m_numIndices];
	}

	unsigned min, max, vcount;
	GLsizei i;

	if (g.m_lckcount != 0)
	{
		switch (type)
		{
			case GL_UNSIGNED_BYTE:
				for (i = 0; i < count; ++i)
					g.m_wIndices[i] = (WORD) ((unsigned char*) indices)[i];
				break;
			case GL_UNSIGNED_SHORT:
				for (i = 0; i < count; ++i)
					g.m_wIndices[i] = (WORD) ((unsigned short*) indices)[i];
				break;
			case GL_UNSIGNED_INT:
				for (i = 0; i < count; ++i)
					g.m_wIndices[i] = (WORD) ((unsigned *) indices)[i];
				break;
		}
		min = g.m_lckfirst;
		max = g.m_lckfirst+g.m_lckcount - 1;
		vcount = g.m_lckcount;
	}
	else
	{
		min = 65535;
		max = 0;
		switch (type)
		{
			case GL_UNSIGNED_BYTE:
				for (i = 0; i < count; ++i)
				{
					unsigned t = ((unsigned char *) indices)[i];
					if (t < min)
						min = t;
					if (t > max)
						max = t;
				}
				
				for (i = 0; i < count; ++i)
					g.m_wIndices[i] = (WORD) (((unsigned char*) indices)[i] - min);
            break;
			case GL_UNSIGNED_SHORT:
				for (i = 0; i < count; ++i)
				{
					unsigned t = ((unsigned short *) indices)[i];

					if (t < min)
						min = t;
					if (t > max)
						max = t;
				}

				for (i = 0; i < count; ++i)
					g.m_wIndices[i] = (WORD) (((unsigned short *) indices)[i] - min);
            break;
			case GL_UNSIGNED_INT:
				for (i = 0; i < count; ++i)
				{
					unsigned t = ((unsigned int *) indices)[i];

					if (t < min)
						min = t;
					if (t > max)
						max = t;
				}

				for (i = 0; i < count; ++i)
					g.m_wIndices[i] = (WORD) (((unsigned int *) indices)[i] - min);
				break;
		}
		vcount = max-min + 1;
	}

	if (g.m_texturing) 
		if (g.m_mtex)
		{
			g.m_comp = 2;
			if (g.m_nfv[2] > (VBUFSIZE - vcount))	// check if space available
			{
				g.m_mtvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
				g.m_nfv[2] = 0;
			}
			else
			{
				g.m_mtvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
				g.m_verts = &(((QuakeMTVertex*)g.m_verts)[g.m_nfv[2]]);
			}
		}
		else
		{
			g.m_comp = 1;
			if (g.m_nfv[1] > (VBUFSIZE - vcount))	// check if space available
			{
				g.m_tvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
				g.m_nfv[1] = 0;
			}
			else
			{
				g.m_tvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
				g.m_verts = &(((QuakeTVertex*)g.m_verts)[g.m_nfv[1]]);
			}
		}
	else
	{
		g.m_comp = 0;
		if (g.m_nfv[0] > (VBUFSIZE - vcount))	// check if space available
		{
			g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
			g.m_nfv[0] = 0;
		}
		else
		{
			g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
			g.m_verts = &(((QuakeVertex*)g.m_verts)[g.m_nfv[0]]);
		}
	} 

	unsigned dstride = 7 + g.m_comp*2;

	if (g.m_texturing)
	{
		if (g.m_usetexcoordary[0])
		{
			unsigned char *tex0 = &((unsigned char *) g.m_texcoordary[0])[min*g.m_texcoordstride[0]];	
			D3DVALUE *data = (D3DVALUE *) g.m_verts+7;

			for (i = 0; i < vcount; ++i)
			{
				memcpy(data,tex0,2*sizeof(GLfloat));
				tex0 += g.m_texcoordstride[0];
				data += dstride;
			}
		}
		else if (g.m_texgen[0] && g.m_texgenmode[0] == GL_SPHERE_MAP)
		{
			unsigned char *vtx = &((unsigned char *) g.m_vertexary)[min*g.m_vertexstride];
			D3DVALUE *data = (D3DVALUE *) g.m_verts+7;

			// Get the current world-view matrix
			D3DMATRIX world;

			g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, &world);

			// Extract world-view matrix elements for speed
			FLOAT m11 = world._11, m21 = world._21, m31 = world._31;
			FLOAT m12 = world._12, m22 = world._22, m32 = world._32;
			FLOAT m13 = world._13, m23 = world._23, m33 = world._33;

			// Loop through the vertices, transforming each one and calculating
			// the correct texture coordinates.
			for (i = 0; i < vcount; ++i)
			{
				FLOAT nx = ((FLOAT *) vtx)[0];
				FLOAT ny = ((FLOAT *) vtx)[1];
				FLOAT nz = ((FLOAT *) vtx)[2];

				// Check the z-component, to skip any vertices that face backwards
				if (nx*m13 + ny*m23 + nz*m33 > 0.0f)
					continue;

				// Assign the spheremap's texture coordinates
        		data[0] = 0.5f * (1.0f + (nx*m11 + ny*m21 + nz*m31));
        		data[1] = 0.5f * (1.0f - (nx*m12 + ny*m22 + nz*m32));

				vtx += g.m_vertexstride;
				data += dstride;
			}
		}
		else
		{
			D3DVALUE *data = (D3DVALUE *) g.m_verts+7;

			for (i = 0; i < vcount; ++i)
			{
				memcpy(data,&g.m_tu,2*sizeof(GLfloat));
				data += dstride;
			}
		}

		if (g.m_mtex)
			if (g.m_usetexcoordary[1])
			{
				unsigned char *tex1 = &((unsigned char *) g.m_texcoordary[1])[min*g.m_texcoordstride[1]];	
				D3DVALUE *data = (D3DVALUE *) g.m_verts+9;

				for (i = 0; i < vcount; ++i)
				{
					memcpy(data,tex1,2*sizeof(GLfloat));
					tex1 += g.m_texcoordstride[1];
					data += dstride;
				}
			}
			else if (g.m_texgen[1] && g.m_texgenmode[1] == GL_SPHERE_MAP)
			{
				unsigned char *vtx = &((unsigned char *) g.m_vertexary)[min*g.m_vertexstride];
				D3DVALUE *data = (D3DVALUE *) g.m_verts+9;

				// Get the current world-view matrix
				D3DMATRIX world;

				g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, &world);

				// Extract world-view matrix elements for speed
				FLOAT m11 = world._11, m21 = world._21, m31 = world._31;
				FLOAT m12 = world._12, m22 = world._22, m32 = world._32;
				FLOAT m13 = world._13, m23 = world._23, m33 = world._33;

				// Loop through the vertices, transforming each one and calculating
				// the correct texture coordinates.
				for (i = 0; i < vcount; ++i)
				{
					FLOAT nx = ((FLOAT *) vtx)[0];
					FLOAT ny = ((FLOAT *) vtx)[1];
					FLOAT nz = ((FLOAT *) vtx)[2];

					// Check the z-component, to skip any vertices that face backwards
					if (nx*m13 + ny*m23 + nz*m33 > 0.0f)
						continue;

					// Assign the spheremap's texture coordinates
        			data[0] = 0.5f * (1.0f + (nx*m11 + ny*m21 + nz*m31));
        			data[1] = 0.5f * (1.0f - (nx*m12 + ny*m22 + nz*m32));

					vtx += g.m_vertexstride;
					data += dstride;
				}
			}
			else
			{
				D3DVALUE *data = (D3DVALUE *) g.m_verts+9;

				for (i = 0; i < vcount; ++i)
				{
					memcpy(data,&g.m_tu2,2*sizeof(GLfloat));
					data += dstride;
				}
			}
	}
		
	if (g.m_usenormalary)
	{
		unsigned char *nml = &((unsigned char *) g.m_normalary)[min*g.m_normalstride];
		D3DVALUE *data = (D3DVALUE *) g.m_verts+3;

		for (i = 0; i < vcount; ++i)
		{
			memcpy(data,nml,3*sizeof(GLfloat));
			nml += g.m_normalstride;
			data += dstride;
		}
	}

	if (g.m_usecolorary)
	{		
		if (g.m_colortype == GL_UNSIGNED_BYTE)
		{	
#ifdef _X86_
			DWORD ustride = (dstride-1) * 4;

			_asm
			{
				mov esi, min;
				mov ecx, max;
				mov edx, 0x00FF00FF;
				mov ebx, g.m_colorary;
				mov edi, g.m_verts;
				add edi, 24;
				sub ecx, esi;
				lea esi, [ebx + esi * 4];
				inc ecx;
				cld;            
lp1:			lodsd;
				mov ebx, eax;
				and eax, edx;
				not edx;
				rol eax, 16;
				and ebx, edx;
				not edx;
				or  eax, ebx;
				stosd;
				add edi, ustride;
				loop lp1;
			}
#else
			GLubyte *clr = &((GLubyte *) g.m_colorary)[min*g.m_colorstride];
			D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

			for (i = 0; i < vcount; i++)
			{
				data[0] = RGBA_MAKE(clr[0],clr[1],clr[2],clr[3]);
				clr += g.m_colorstride;
				data += dstride;
			}
#endif
		}
		else
		{
			GLubyte *clr = &((GLubyte *) g.m_colorary)[min*g.m_colorstride];
			D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

			for (i = 0; i < vcount; ++i)
			{
				static float two55 = 255.f;
				unsigned int R, G, B, A;

//#ifdef _X86_
#if 0
				__asm {
					mov ebx, clr;
					fld [ebx];
					fld [ebx + 4];
					fld [ebx + 8];
					fld [ebx + 12];
					fld two55;
					fmul st(1), st(0);
					fmul st(2), st(0);
					fmul st(3), st(0);
					fmulp st(4), st(0);
					fistp A;
					fistp B;
					fistp G;
					fistp R;
					mov edx, A;
					cmp edx, 255;
					jle pt1;
					mov edx, 255;
			pt1:	mov eax, B;
					cmp eax, 255;
					jle pt2;
					mov eax, 255;
			pt2:	mov ebx, G;
					cmp ebx, 255;
					jle pt3;
					mov ebx, 255;
			pt3:	mov ecx, R;
					cmp ecx, 255;
					jle pt4;
					mov ecx, 255;
			pt4:	shl ebx, 8;
        			shl ecx, 16;
        			shl edx, 24;
        			or eax, ebx;
        			or ecx, edx;
        			or eax, ecx;
        			mov data, eax;
				}
#else
				GLfloat *fclr = (GLfloat *) clr;

				R = (unsigned int)(fclr[0] * two55);
				G = (unsigned int)(fclr[1] * two55);
				B = (unsigned int)(fclr[2] * two55);
				A = (unsigned int)(fclr[3] * two55);
				if (R > 255)
					R = 255;
				if (G > 255)
					G = 255;
				if (B > 255)
					B = 255;
				if (A > 255)
					A = 255;
				data[0] = RGBA_MAKE(R, G, B, A);
#endif
				clr += g.m_colorstride;
				data += dstride;
			}
		}
	}
	else
	{
		D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

		for (i = 0; i < vcount; ++i)
		{
			memcpy(data,&g.m_color,sizeof(D3DCOLOR));
			data += dstride;
		}
	}

	AssertFatal(g.m_usevertexary, "We're always using a vertex array...right?");

	{	
		unsigned char *vtx = &((unsigned char *) g.m_vertexary)[min*g.m_vertexstride];
		D3DVALUE *data = (D3DVALUE *) g.m_verts;

		for (i = 0; i < vcount; ++i)
		{
			memcpy(data,vtx,3*sizeof(GLfloat));
			vtx += g.m_vertexstride;
			data += dstride;
		}
	}

	QuakeSetTexturingState();

	if (g.m_texturing)
		if (g.m_mtex)
		{
			g.m_mtvbuf->Unlock();
			switch (mode) 
			{
				case GL_LINES:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_LINELIST, g.m_mtvbuf, g.m_nfv[2], vcount, g.m_wIndices, count, 0);
					g.m_nfv[2] += vcount;
        			break;
				case GL_TRIANGLES:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, g.m_mtvbuf, g.m_nfv[2], vcount, g.m_wIndices, count, 0);
					g.m_nfv[2] += vcount;
					break;
				case GL_TRIANGLE_STRIP:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_mtvbuf, g.m_nfv[2], vcount, g.m_wIndices, count, 0);
					g.m_nfv[2] += vcount;
					break;
				case GL_POLYGON:
				case GL_TRIANGLE_FAN:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_mtvbuf, g.m_nfv[2], vcount, g.m_wIndices, count, 0);
					g.m_nfv[2] += vcount;
					break;
				case GL_QUADS:
					for (i = 0; i < count; i += 4) 
						g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_mtvbuf, g.m_nfv[2], vcount, &g.m_wIndices[i], 4, 0);
					g.m_nfv[2] += vcount;
        			break;
				default:
				{
        			char buf[64];
               	
					dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
					OutputDebugString(buf);
				}
    		}
		}
		else
		{
			g.m_tvbuf->Unlock();
			switch (mode) 
			{
				case GL_LINES:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_LINELIST, g.m_tvbuf, g.m_nfv[1], vcount, g.m_wIndices, count, 0);
					g.m_nfv[1] += vcount;
        			break;
				case GL_TRIANGLES:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, g.m_tvbuf, g.m_nfv[1], vcount, g.m_wIndices, count, 0);
					g.m_nfv[1] += vcount;
					break;
				case GL_TRIANGLE_STRIP:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_tvbuf, g.m_nfv[1], vcount, g.m_wIndices, count, 0);
					g.m_nfv[1] += vcount;
					break;
				case GL_POLYGON:
				case GL_TRIANGLE_FAN:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_tvbuf, g.m_nfv[1], vcount, g.m_wIndices, count, 0);
					g.m_nfv[1] += vcount;
					break;
				case GL_QUADS:
					for (i = 0; i < count; i += 4) 
						g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_tvbuf, g.m_nfv[1], vcount, &g.m_wIndices[i], 4, 0);
					g.m_nfv[1] += vcount;
        			break;
				default:
				{
        			char buf[64];
               	
					dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
					OutputDebugString(buf);
				}
    		}
		}
	else
	{
		g.m_vbuf->Unlock();
		switch (mode) 
			{
				case GL_LINES:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_LINELIST, g.m_vbuf, g.m_nfv[0], vcount, g.m_wIndices, count, 0);
					g.m_nfv[0] += vcount;
        			break;
				case GL_TRIANGLES:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, g.m_vbuf, g.m_nfv[0], vcount, g.m_wIndices, count, 0);
					g.m_nfv[0] += vcount;
					break;
				case GL_TRIANGLE_STRIP:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_vbuf, g.m_nfv[0], vcount, g.m_wIndices, count, 0);
					g.m_nfv[0] += vcount;
					break;
				case GL_POLYGON:
				case GL_TRIANGLE_FAN:
					g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_vbuf, g.m_nfv[0], vcount, g.m_wIndices, count, 0);
					g.m_nfv[0] += vcount;
					break;
				case GL_QUADS:
					for (i = 0; i < count; i += 4) 
						g.m_d3ddev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_vbuf, g.m_nfv[0], vcount, &g.m_wIndices[i], 4, 0);
					g.m_nfv[0] += vcount;
        			break;
				default:
				{
        			char buf[64];
               	
					dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
					OutputDebugString(buf);
				}
    		}
	}
}

static void APIENTRY d3dViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    g.m_vwx = x;
    g.m_vwy = y;
    g.m_vww = width;
    g.m_vwh = height;
    g.m_updvwp = TRUE;
}

static void APIENTRY d3dLineWidth (GLfloat width)
{ 
	width;
}

static void APIENTRY d3dLoadIdentity (void)
{
	D3DMATRIX unity;

	unity._11 = 1.0f; unity._12 = 0.0f; unity._13 = 0.0f; unity._14 = 0.0f;
	unity._21 = 0.0f; unity._22 = 1.0f; unity._23 = 0.0f; unity._24 = 0.0f;
	unity._31 = 0.0f; unity._32 = 0.0f; unity._33 = 1.0f; unity._34 = 0.0f;
	unity._41 = 0.0f; unity._42 = 0.0f; unity._43 = 0.0f; unity._44 = 1.0f;
	g.m_d3ddev->SetTransform(g.m_matrixMode, &unity);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
	else if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_TEXTURE0, &g.m_curtexmatrix);
}

static void APIENTRY d3dMatrixMode (GLenum mode)
{
	if (mode == GL_MODELVIEW)
		g.m_matrixMode = D3DTRANSFORMSTATE_WORLD;
	else if (mode == GL_PROJECTION)
		g.m_matrixMode = D3DTRANSFORMSTATE_PROJECTION;
	else
		g.m_matrixMode = D3DTRANSFORMSTATE_TEXTURE0;
}

static void APIENTRY d3dDisable (GLenum cap)
{
	switch(cap)
	{
		case GL_DEPTH_TEST:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
			break;
		case GL_CULL_FACE:
			g.m_cullEnabled = FALSE;
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
			break;
		case GL_FOG:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
			break;
		case GL_BLEND:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			break;
		case GL_CLIP_PLANE0:
		case GL_CLIP_PLANE1:
		case GL_CLIP_PLANE2:
		case GL_CLIP_PLANE3:
		case GL_CLIP_PLANE4:
		case GL_CLIP_PLANE5:
			break;
		case GL_POLYGON_OFFSET_FILL:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZBIAS, 0);
			break;
		case GL_STENCIL_TEST:
			break;
		case GL_SCISSOR_TEST:
			g.m_scissoring = FALSE;
			glViewport(g.m_vwx, g.m_vwy, g.m_vww, g.m_vwh);
			break;
		case GL_TEXTURE_2D:
			if (g.m_curtgt == 0)
			{
				g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_DISABLE);
            g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            g.m_texturing = FALSE;
			}
			else
			{
            g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            g.m_mtex = FALSE;
        	}
        	g.m_texHandleValid = FALSE;
        	break;
		case GL_ALPHA_TEST:
      	g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
        	break;
		case GL_LIGHTING:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
			break;
		case GL_TEXTURE_GEN_S:
		case GL_TEXTURE_GEN_T:
			g.m_texgen[g.m_curtgt] = FALSE;
			g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXCOORDINDEX,
														g.m_curtgt);
			g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			break;
		case GL_LIGHT0:
			g.m_d3ddev->LightEnable(0, FALSE);
			break;
		case GL_LIGHT1:
			g.m_d3ddev->LightEnable(1, FALSE);
			break;
    	default:
			{
				char buf[64];

				dSprintf(buf,64,"Wrapper: glDisable on this cap not supported: %x\n", cap);
				OutputDebugString(buf);
			}
    }
}

static void APIENTRY d3dDisableClientState (GLenum array)
{ 
	switch(array)
	{
		case GL_COLOR_ARRAY:
			g.m_usecolorary = FALSE;
			break;
		case GL_TEXTURE_COORD_ARRAY:
			g.m_usetexcoordary[g.m_client_active_texture_arb] = FALSE;
			break;
		case GL_VERTEX_ARRAY:
			g.m_usevertexary = FALSE;
			break;
		case GL_NORMAL_ARRAY:
			g.m_usenormalary = FALSE;
			break;
		case GL_FOG_COORDINATE_ARRAY_EXT:
			g.m_usefogary = FALSE;
			break;
		default:
			{
				char buf[64];

				dSprintf(buf,64,"Wrapper: Array not supported: %x\n", array);
				OutputDebugString(buf);
			}
	}
}

static void APIENTRY d3dDrawBuffer (GLenum mode)
{
	mode;
}

static void APIENTRY d3dEnable (GLenum cap)
{
	switch (cap) {
		case GL_DEPTH_TEST:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
			break;
		case GL_CULL_FACE:
			g.m_cullEnabled = TRUE;
			if ((g.m_cullMode == GL_BACK && g.m_frontFace == GL_CCW) ||
			 	 (g.m_cullMode == GL_FRONT && g.m_frontFace == GL_CW))
				g.m_d3ddev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
			else
				g.m_d3ddev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
			break;
		case GL_FOG:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
			break;
		case GL_BLEND:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			break;
		case GL_CLIP_PLANE0:
		case GL_CLIP_PLANE1:
		case GL_CLIP_PLANE2:
		case GL_CLIP_PLANE3:
		case GL_CLIP_PLANE4:
		case GL_CLIP_PLANE5:
			break;
		case GL_POLYGON_OFFSET_FILL:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ZBIAS, g.m_zbias);
			break;
		case GL_SCISSOR_TEST:
			g.m_scissoring = TRUE;
			g.m_updvwp = TRUE;
			break;
		case GL_TEXTURE_2D:
			if (g.m_curtgt == 0)
				g.m_texturing = TRUE;
			else
				g.m_mtex = TRUE;
			g.m_texHandleValid = FALSE;
			break;
		case GL_ALPHA_TEST:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
			break;
		case GL_LIGHTING:
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
			break;
		case GL_TEXTURE_GEN_S:
		case GL_TEXTURE_GEN_T:
			g.m_texgen[g.m_curtgt] = TRUE;
			if (g.m_texgenmode[g.m_curtgt] == GL_OBJECT_LINEAR)
			{
				g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXCOORDINDEX,
															g.m_curtgt | D3DTSS_TCI_CAMERASPACEPOSITION);
				g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			}	
			else
			{
				g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXCOORDINDEX,
															g.m_curtgt);
				g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			}
			g.m_texcoordary[g.m_curtgt] = g.m_spherecoords;
			g.m_texcoordstride[g.m_curtgt] = 8;
			break;
		case GL_LIGHT0:
			g.m_d3ddev->SetLight(0, &g.m_lights[0]);
			g.m_d3ddev->LightEnable(0, TRUE);
			break;
		case GL_LIGHT1:
		  	g.m_d3ddev->SetLight(1, &g.m_lights[1]);
			g.m_d3ddev->LightEnable(1, TRUE);
			break;
		default:
			{
				char buf[64];

				dSprintf(buf,64,"Wrapper: glEnable on this cap not supported: %x\n",cap);
				OutputDebugString(buf);
			}
	}
}

static void APIENTRY d3dEnableClientState (GLenum array)
{ 
	switch (array)
	{
		case GL_COLOR_ARRAY:
			g.m_usecolorary = TRUE;
			break;
		case GL_TEXTURE_COORD_ARRAY:
			g.m_usetexcoordary[g.m_client_active_texture_arb] = TRUE;
			break;
		case GL_VERTEX_ARRAY:
			g.m_usevertexary = TRUE;
			break;
		case GL_NORMAL_ARRAY:
			g.m_usenormalary = TRUE;
			break;
		case GL_FOG_COORDINATE_ARRAY_EXT:
			g.m_usefogary = TRUE;
			break;
		default:
			OutputDebugString("Wrapper: Array not supported\n");
	}
}

static void APIENTRY d3dFogf (GLenum pname, GLfloat param)
{ 
	FLOAT start, end;
	
	switch (pname)
	{
		case GL_FOG_START:
			start = param;
			//g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *(DWORD*)(&start));
			break;
		case GL_FOG_END:
			end = param;
			//g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *(DWORD*)(&end));
			break;
		default:
			OutputDebugString("Wrapper: Fog pname not supported\n");
	}
}

static void APIENTRY d3dFogfv (GLenum pname, const GLfloat *params)
{ 
	if (pname == GL_FOG_COLOR)
	{
		static float two55 = 255.f;
		unsigned int R, G, B, A;

#ifdef _X86_
		__asm {
			mov ebx, params;
			fld [ebx];
			fld [ebx + 4];
			fld [ebx + 8];
			fld [ebx + 12];
			fld two55;
			fmul st(1), st(0);
			fmul st(2), st(0);
			fmul st(3), st(0);
			fmulp st(4), st(0);
			fistp A;
			fistp B;
			fistp G;
			fistp R;
			mov edx, A;
			cmp edx, 255;
			jle pt1;
			mov edx, 255;
	pt1:	mov eax, B;
			cmp eax, 255;
			jle pt2;
			mov eax, 255;
	pt2:	mov ebx, G;
			cmp ebx, 255;
			jle pt3;
			mov ebx, 255;
	pt3:	mov ecx, R;
			cmp ecx, 255;
			jle pt4;
			mov ecx, 255;
	pt4:	shl ebx, 8;
			shl ecx, 16;
			shl edx, 24;
			or eax, ebx;
			or ecx, edx;
			or eax, ecx;
			mov g.m_fogcolor, eax;
		}
#else
		R = (unsigned int) (params[0] * two55);
		G = (unsigned int) (params[1] * two55);
		B = (unsigned int) (params[2] * two55);
		A = (unsigned int) (params[3] * two55);
		if (R > 255)
			R = 255;
		if (G > 255)
			G = 255;
		if (B > 255)
			B = 255;
		if (A > 255)
			A = 255;
		g.m_fogcolor = RGBA_MAKE(R, G, B, A);
#endif
		g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGCOLOR, g.m_fogcolor);
	}
	else
		OutputDebugString("Wrapper: Fog pname not supported\n");
}

static void APIENTRY d3dFogi (GLenum pname, GLint param)
{
	switch(pname)
	{
		case GL_FOG_COORDINATE_SOURCE_EXT:
			break;
		case GL_FOG_MODE:
			switch (param)
			{
				case GL_LINEAR:
					//g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
					break;
				case GL_EXP:
					//g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP);
					break;
				case GL_EXP2:
					//g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2);
					break;
			}
			break;
		default:
			OutputDebugString("Wrapper: Fog pname not supported\n");
	}
}

static void APIENTRY d3dFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	D3DMATRIX f;

	f._11 = (D3DVALUE)((2.0 * zNear) / (right - left));
	f._21 = 0.f;
	f._31 = (D3DVALUE)((right + left) / (right - left));
	f._41 = 0.f;
	f._12 = 0.f;
	f._22 = (D3DVALUE)((2.0 * zNear) / (top - bottom));
	f._32 = (D3DVALUE)((top + bottom) / (top - bottom));
	f._42 = 0.f;
	f._13 = 0.f;
	f._23 = 0.f;
	f._33 = (D3DVALUE)(-zFar / (zFar - zNear));
	//f._33 = (D3DVALUE)(-(zFar+zNear)/(zFar-zNear));
	f._43 = (D3DVALUE)(-(zFar * zNear) / (zFar - zNear));
	//f._43 = (D3DVALUE)(-(2.0 * zFar * zNear)/(zFar - zNear));
	f._14 = 0.f;
	f._24 = 0.f;
	f._34 = -1.f;
	f._44 = 0.f;
	g.m_d3ddev->MultiplyTransform(g.m_matrixMode, &f);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
}

static GLenum APIENTRY d3dGetError (void)
{
    return GL_NO_ERROR;
}

static void APIENTRY d3dGetDoublev (GLenum pname, GLdouble *params)
{
	switch (pname) {
		case GL_MODELVIEW_MATRIX:
			{
				D3DMATRIX tmp;

				g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, &tmp);
				for (U8 i = 0; i < 16; ++i)
					params[0] = ((GLfloat *) &tmp)[i];
			}
			break;
		case GL_PROJECTION_MATRIX:
			{
				D3DMATRIX tmp;

				g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &tmp);
				for (U8 i = 0; i < 16; ++i)
					params[0] = ((GLfloat *) &tmp)[i];
			}			
			break;
		default:
			OutputDebugString("Wrapper: Unimplemented GetDoublev query\n");
	}
}

static void APIENTRY d3dGetFloatv (GLenum pname, GLfloat *params)
{
    switch (pname) {
    case GL_MODELVIEW_MATRIX:
        g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)params);
        break;
    case GL_PROJECTION_MATRIX:
        g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_PROJECTION, (LPD3DMATRIX)params);
        break;
    default:
        OutputDebugString("Wrapper: Unimplemented GetFloatv query\n");
    }
}

static void APIENTRY d3dGetIntegerv (GLenum pname, GLint *params)
{ 
	switch(pname)
	{
		case GL_MAX_TEXTURE_SIZE:
			*params = (g.m_dd.dwMaxTextureWidth >= g.m_dd.dwMaxTextureHeight) ? g.m_dd.dwMaxTextureWidth : g.m_dd.dwMaxTextureHeight;
			break;
		case GL_MAX_TEXTURE_UNITS_ARB:
			*params = g.m_usemtex ? 2 : 1;
			break;
		case GL_MODELVIEW_STACK_DEPTH:
			*params = g.m_matrixStack[0].length() + 1;
			break;
		case GL_PROJECTION_STACK_DEPTH:
			*params = g.m_matrixStack[1].length() + 1;
			break;
		case GL_MAX_LIGHTS:
			*params = 8;
			break;
		default:
			OutputDebugString("Wrapper: Unimplemented GetIntegerv query\n");
	}
}

static const GLubyte* APIENTRY d3dGetString (GLenum name)
{
    switch(name) {
    case GL_VENDOR:
        return (const GLubyte*)"Microsoft Corp.";
    case GL_RENDERER:
        return (const GLubyte*)"Direct3D";
    case GL_VERSION:
        return (const GLubyte*)"1.1";
    case GL_EXTENSIONS:
        if(g.m_usemtex != FALSE)
            return (const GLubyte*)"GL_ARB_multitexture GL_EXT_compiled_vertex_array GL_SGIS_multitexture GL_EXT_fog_coord";
        else
            return (const GLubyte*)"GL_EXT_compiled_vertex_array GL_EXT_fog_coord";
    default:
        OutputDebugString("Wrapper: Unimplemented GetString query\n");
    }
    return (const GLubyte*)"";
}

static void APIENTRY d3dLoadMatrixf (const GLfloat *m)
{
	if (g.m_matrixMode == D3DTRANSFORMSTATE_PROJECTION)
	{
		D3DMATRIX f = *((LPD3DMATRIX) m);

		f._13 = (((LPD3DMATRIX) m)->_13 + ((LPD3DMATRIX) m)->_14) * 0.5f;
		f._23 = (((LPD3DMATRIX) m)->_23 + ((LPD3DMATRIX) m)->_24) * 0.5f;
		f._33 = (((LPD3DMATRIX) m)->_33 + ((LPD3DMATRIX) m)->_34) * 0.5f;
		f._43 = (((LPD3DMATRIX) m)->_43 + ((LPD3DMATRIX) m)->_44) * 0.5f;
		g.m_d3ddev->SetTransform(g.m_matrixMode, &f);
	}
	else
	{
		g.m_d3ddev->SetTransform(g.m_matrixMode, (LPD3DMATRIX) m);
		if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
			g.m_inversedirty = TRUE;
		else if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
			g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_TEXTURE0, &g.m_curtexmatrix);
	}
}

static void APIENTRY d3dLockArraysEXT(GLint first, GLsizei count)
{
    g.m_lckfirst = first;
    g.m_lckcount = count;
}

static void APIENTRY d3dMTexCoord2fSGIS(GLenum target, GLfloat s, GLfloat t)
{
    if(target == GL_TEXTURE0_SGIS) {
        g.m_tu = s;
        g.m_tv = t;
    }
    else {
        g.m_tu2 = s;
        g.m_tv2 = t;
    }
}

static void APIENTRY d3dMultiTexCoord2fARB (GLenum texture, GLfloat s, GLfloat t)
{
    if(texture == GL_TEXTURE0_ARB) 
    {
        g.m_tu = s;
        g.m_tv = t;
    }
    else 
    {
        g.m_tu2 = s;
        g.m_tv2 = t;
    }
}

static void APIENTRY d3dMultiTexCoord2fvARB (GLenum texture, GLfloat *v)
{
	texture;
	v;

	#ifdef DODPFS
	DPF("glMultiTexCoord2fvARB");
	#endif //DODPFS
}

static void APIENTRY d3dOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	D3DMATRIX f;

	left += 0.5;
	right += 0.5;
	top += 0.5;
	bottom += 0.5;

	f._11 = (D3DVALUE) (2.0 / (right - left));
	f._21 = 0.f;
	f._31 = 0.f;
	f._41 = (D3DVALUE) (-(right + left) / (right - left));
	f._12 = 0.f;
	f._22 = (D3DVALUE) (2.0 / (top - bottom));
	f._32 = 0.f;
	f._42 = (D3DVALUE) (-(top + bottom) / (top - bottom));
	f._13 = 0.f;
	f._23 = 0.f;
	f._33 = (D3DVALUE) (-1.0 / (zFar - zNear));
	f._43 = (D3DVALUE) (-zNear / (zFar - zNear));
	f._14 = 0.f;
	f._24 = 0.f;
	f._34 = 0.f;
	f._44 = 1.f;
	g.m_d3ddev->MultiplyTransform(g.m_matrixMode, &f);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
}

static void APIENTRY d3dPolygonMode (GLenum face, GLenum mode)
{
	face;

	int statevalue=-1;
	switch(mode) {
		case GL_POINT:
			statevalue=D3DFILL_POINT;
			break;
		case GL_LINE:
			statevalue=D3DFILL_WIREFRAME; 
			break;
		case GL_FILL:
			statevalue=D3DFILL_SOLID;
			break;
	}
	if(statevalue >= 0) {
		g.m_d3ddev->SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)statevalue);
	}
}

static void APIENTRY d3dPolygonOffset (GLfloat factor, GLfloat units)
{
	factor;

	g.m_zbias = (DWORD) -units;
}

static void APIENTRY d3dPopMatrix (void)
{
	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
	{
		GLListManip<D3DMATRIX> m(&g.m_matrixStack[0]);

		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_WORLD, &(m()));
		m.remove();
		g.m_inversedirty = TRUE;
	}
	else if (g.m_matrixMode == D3DTRANSFORMSTATE_PROJECTION)
	{
		GLListManip<D3DMATRIX> m(&g.m_matrixStack[1]);

		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &(m()));
		m.remove();
	}
	else
	{
		GLListManip<D3DMATRIX> m(&g.m_matrixStack[2]);

		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0, &(m()));
		g.m_curtexmatrix = m();
		m.remove();

		//if (g.m_matrixStack[2].length() == 0 && !g.m_texgen[0])
			//g.m_d3ddev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}
}

static void APIENTRY d3dPushMatrix (void)
{
	if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
	{
		//if (g.m_matrixStack[2].length() == 0 && !g.m_texgen[0])
			//g.m_d3ddev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		g.m_matrixStack[2].prepend(g.m_curtexmatrix);
	}
	else
	{
		D3DMATRIX m;

		g.m_d3ddev->GetTransform(g.m_matrixMode, &m);
		if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
			g.m_matrixStack[0].prepend(m);
		else
			g.m_matrixStack[1].prepend(m);
	}
}

static void APIENTRY d3dRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	static double PI = 4. * atan(1.);
	D3DMATRIX f;
	double ra = angle * PI / 180.f;
	float ca = (float) cos(ra);
	float sa = (float) sin(ra);

	if (x == 1.f)
	{
		f._11 = 1.f; f._21 = 0.f; f._31 = 0.f; f._41 = 0.f;
		f._12 = 0.f; f._22 = ca;  f._32 = -sa; f._42 = 0.f;
		f._13 = 0.f; f._23 = sa;  f._33 = ca;  f._43 = 0.f;
		f._14 = 0.f; f._24 = 0.f; f._34 = 0.f; f._44 = 1.f;    
	}
	else if (y == 1.f)
	{
		f._11 = ca;  f._21 = 0.f; f._31 = sa;  f._41 = 0.f;
		f._12 = 0.f; f._22 = 1.f; f._32 = 0.f; f._42 = 0.f;
		f._13 = -sa; f._23 = 0.f; f._33 = ca;  f._43 = 0.f;
		f._14 = 0.f; f._24 = 0.f; f._34 = 0.f; f._44 = 1.f;    
	}
	else if (z == 1.f)
	{
		f._11 = ca;  f._21 = -sa; f._31 = 0.f; f._41 = 0.f;
		f._12 = sa;  f._22 = ca;  f._32 = 0.f; f._42 = 0.f;
		f._13 = 0.f; f._23 = 0.f; f._33 = 1.f; f._43 = 0.f;
		f._14 = 0.f; f._24 = 0.f; f._34 = 0.f; f._44 = 1.f;    
	}
	else
		return;

	if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_curtexmatrix);

	g.m_d3ddev->MultiplyTransform(g.m_matrixMode, &f);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
	else if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_TEXTURE0, &g.m_curtexmatrix);
}

static void APIENTRY d3dScalef (GLfloat x, GLfloat y, GLfloat z)
{
	D3DMATRIX f;

	f._11 =   x; f._21 = 0.f; f._31 = 0.f; f._41 = 0.f;
	f._12 = 0.f; f._22 =   y; f._32 = 0.f; f._42 = 0.f; 
	f._13 = 0.f; f._23 = 0.f; f._33 =   z; f._43 = 0.f;
	f._14 = 0.f; f._24 = 0.f; f._34 = 0.f; f._44 = 1.f;

	if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_curtexmatrix);

	g.m_d3ddev->MultiplyTransform(g.m_matrixMode, &f);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
	else if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_TEXTURE0, &g.m_curtexmatrix);
}

static void APIENTRY d3dScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{ 
    g.m_scix = x;
    g.m_sciy = y;
    g.m_sciw = width;
    g.m_scih = height;
    g.m_updvwp = TRUE;
}

static void APIENTRY d3dSelectTextureSGIS(GLenum target)
{
    g.m_curtgt = target == GL_TEXTURE0_SGIS ? 0 : 1;
}

static void APIENTRY d3dShadeModel (GLenum mode)
{
    if(mode == GL_SMOOTH)
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    else
        g.m_d3ddev->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
}

static void APIENTRY d3dTexCoord2f (GLfloat s, GLfloat t)
{
	g.m_tu = s;
	g.m_tv = t;
}

static void APIENTRY d3dTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (size == 2 && type == GL_FLOAT)
	{
		g.m_texcoordary[g.m_client_active_texture_arb] = (GLfloat *) pointer;
		if (stride)
			g.m_texcoordstride[g.m_client_active_texture_arb] = stride;
		else
			g.m_texcoordstride[g.m_client_active_texture_arb] = 8;
	}
	else
	{
		char buf[128];

		dSprintf(buf,128,"Wrapper: TexCoord array not supported (size: %d  type: %x  stride: %d)\n",
					size, type, stride);
		OutputDebugString(buf);
	}
}

static void APIENTRY d3dTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	target;

	if(pname == GL_TEXTURE_ENV_MODE) {
		g.m_blendmode[g.m_curtgt] = (int)param;
		g.m_texHandleValid = FALSE;
	}
	else
		OutputDebugString("Wrapper: GL_TEXTURE_ENV_COLOR not implemented\n");
}

static void APIENTRY d3dTexEnvi (GLenum target, GLenum pname, GLint param)
{ 
	d3dTexEnvf(target,pname,(GLfloat) param);
}

static void APIENTRY d3dTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei glwidth, GLsizei glheight, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	target;
	border;
	format;
	type;

    DWORD width, height;
    TexInfo &ti = g.m_tex[g.m_curstagebinding[g.m_curtgt]];
    
    /* See if texture needs to be subsampled */
    if(g.m_subsample) {
        if(glwidth > 256 || glheight > 256) {
            if(glwidth > glheight) {
                width = 256;
                height = (glheight * 256) / glwidth;
            }
            else {
                height = 256;
                width = (glwidth * 256) / glheight;
            }
        }
        else {
            width = glwidth;
            height = glheight;
        }
    }
    else {
        width = glwidth;
        height = glheight;
    }
    
    /* See if texture needs to be square */
    if(g.m_makeSquare) {
        if(height > width) {
            width = height;
        }
        else {
            height = width;
        }
    }
    
    if(level == 0) {
        LPDIRECTDRAWSURFACE7 ddsurf;
        D3DX_SURFACEFORMAT fmt;
        switch(internalformat) {
        case 1:
		  case GL_LUMINANCE:
            fmt = g.m_ddLuminanceSurfFormat;
            break;
        case GL_RGB5:
		  case GL_RGB5_A1:
            fmt = g.m_ddFiveBitSurfFormat;
            break;
        case 4:
        case GL_RGBA4:
            fmt = g.m_ddFourBitAlphaSurfFormat;
            break;
		case GL_RGB:
            fmt = g.m_ddEightBitSurfFormat;
            break;
		case GL_RGBA:
			fmt = g.m_ddEightBitAlphaSurfFormat;
			break;
		case GL_ALPHA:
			fmt = g.m_ddAlphaSurfFormat;
			break;
		default:
            OutputDebugString("Wrapper: Unimplemented internalformat\n");
            break;
        }
        DWORD flags = D3DX_TEXTURE_NOMIPMAP;
        HRESULT ddrval = D3DXCreateTexture(g.m_d3ddev,
        												  &flags,
        												  (DWORD*)&width, (DWORD*)&height, &fmt,
        												  NULL, // Palette
        												  &ddsurf,
        												  NULL);
        if (ddrval != DD_OK) 
        {
            OutputDebugString("Wrapper: CreateTexture failed\n");
            return;
        }
        LoadSurface(ddsurf, internalformat, glwidth, glheight, width, height, (const DWORD*)pixels);
        if(ti.m_ddsurf != 0) {
            ti.m_ddsurf->Release();
        }
        ti.m_dwStage = g.m_curtgt;
        ti.m_fmt = fmt;
        ti.m_internalformat = internalformat;
        ti.m_width = width;
        ti.m_height = height;
        ti.m_ddsurf = ddsurf;
        ti.m_oldwidth = glwidth;
        ti.m_oldheight = glheight;

        if(ti.m_block == 0)
		  {
            g.m_d3ddev->BeginStateBlock();
            g.m_d3ddev->SetTextureStageState (g.m_curtgt, D3DTSS_ADDRESSU,ti.m_addu);
            g.m_d3ddev->SetTextureStageState (g.m_curtgt, D3DTSS_ADDRESSV,ti.m_addv);
            g.m_d3ddev->SetTextureStageState (g.m_curtgt, D3DTSS_MAGFILTER,ti.m_magmode);
            g.m_d3ddev->SetTextureStageState (g.m_curtgt, D3DTSS_MINFILTER,ti.m_minmode);
            g.m_d3ddev->SetTextureStageState (g.m_curtgt, D3DTSS_MIPFILTER,ti.m_mipmode);
            g.m_d3ddev->SetTexture(g.m_curtgt, ti.m_ddsurf);
            g.m_d3ddev->EndStateBlock(&ti.m_block);
            ti.m_capture = FALSE;
        }
        else
        {
            ti.m_capture = TRUE;
        }
    }
    else if(level == 1 && g.m_usemipmap) { // oops, a mipmap
        LPDIRECTDRAWSURFACE7 ddsurf;
        DWORD flags = 0;
        HRESULT ddrval = D3DXCreateTexture(g.m_d3ddev,
        												  &flags,
        												  (DWORD*)&ti.m_width, (DWORD*)&ti.m_height, &ti.m_fmt,
        												  NULL, // Palette
        												  &ddsurf,
        												  NULL);
        if (ddrval != DD_OK) {
            OutputDebugString("Wrapper: CreateSurface for texture failed\n");
            return;
        }
        ddsurf->Blt(NULL, ti.m_ddsurf, NULL, DDBLT_WAIT, NULL);
        ti.m_ddsurf->Release();
        ti.m_ddsurf = ddsurf;
        DDSURFACEDESC2 ddsd;
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
        LPDIRECTDRAWSURFACE7 lpDDSTmp;
        ti.m_ddsurf->GetAttachedSurface(&ddsd.ddsCaps, &lpDDSTmp);
        LoadSurface(lpDDSTmp, internalformat, glwidth, glheight, width, height, (const DWORD*)pixels);
        lpDDSTmp->Release();
        ti.m_capture = TRUE;
    }
    else if(g.m_usemipmap) {
        LPDIRECTDRAWSURFACE7 ddsurf = ti.m_ddsurf;
        ddsurf->AddRef();
        for(int i = 0; i < level; ++i) {
            DDSURFACEDESC2 ddsd;
            memset(&ddsd, 0, sizeof(ddsd));
            ddsd.dwSize = sizeof(ddsd);
            ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
            LPDIRECTDRAWSURFACE7 lpDDSTmp;
            HRESULT ddrval = ddsurf->GetAttachedSurface(&ddsd.ddsCaps, &lpDDSTmp);
            ddsurf->Release();
            if (ddrval == DDERR_NOTFOUND) {
                return;
            }
            ddsurf = lpDDSTmp;
        }
        LoadSurface(ddsurf, internalformat, glwidth, glheight, width, height, (const DWORD*)pixels);
        ddsurf->Release();
    }
    g.m_texHandleValid = FALSE;
}

static void APIENTRY d3dTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	target;
	format;
	type;
	
	TexInfo &ti = g.m_tex[g.m_curstagebinding[g.m_curtgt]];
	LPDIRECTDRAWSURFACE7 ddsurf = ti.m_ddsurf;
	RECT subimage;

	if (!ddsurf)
	{
		// We haven't got a surface -- so just do a glTexImage2D
		// NOTE:  this won't work for a mip-mapped texture
		d3dTexImage2D(target,level,format,width,height,0,format,type,pixels);

		return;
	}

	for(int i = 0; i < level; ++i) {
		DDSURFACEDESC2 ddsd;
		HRESULT ddrval;
		memset(&ddsd.ddsCaps, 0, sizeof(ddsd.ddsCaps));
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
		LPDIRECTDRAWSURFACE7 lpDDSTmp;
		ddrval = ddsurf->GetAttachedSurface(&ddsd.ddsCaps, &lpDDSTmp);
		if (ddrval == DDERR_NOTFOUND)
			return;
		ddsurf = lpDDSTmp;
		ddsurf->Release();
	}
	xoffset = (xoffset * ti.m_width) / ti.m_oldwidth;
	yoffset = (yoffset * ti.m_height) / ti.m_oldheight;
	SetRect(&subimage, xoffset, yoffset, 
			  (width * ti.m_width) / ti.m_oldwidth + xoffset, 
			  (height * ti.m_height) / ti.m_oldheight + yoffset);
	if (DD_OK != LoadSubSurface(ddsurf, ti.m_internalformat, width, height, (const DWORD*)pixels, &subimage)) {
		OutputDebugString("Wrapper: LoadSubSurface Failure.\n");
		return;
	}
}		

static void APIENTRY d3dTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	target;

    switch(pname) {
    case GL_TEXTURE_MIN_FILTER:
        switch((int)param) {
        case GL_NEAREST:
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_minmode = D3DTFN_POINT;
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_mipmode = D3DTFP_NONE;
            break;
        case GL_LINEAR:
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_minmode = D3DTFN_LINEAR;
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_mipmode = D3DTFP_NONE;
            break;
        case GL_NEAREST_MIPMAP_NEAREST:
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_minmode = D3DTFN_POINT;
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_mipmode = D3DTFP_POINT;
            break;
        case GL_NEAREST_MIPMAP_LINEAR:
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_minmode = D3DTFN_POINT;
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_mipmode = D3DTFP_LINEAR;
            break;
        case GL_LINEAR_MIPMAP_NEAREST:
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_minmode = D3DTFN_LINEAR;
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_mipmode = D3DTFP_POINT;
            break;
        case GL_LINEAR_MIPMAP_LINEAR:
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_minmode = D3DTFN_LINEAR;
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_mipmode = D3DTFP_LINEAR;
            break;
        }
        break;
    case GL_TEXTURE_MAG_FILTER:
        if((int)param == GL_NEAREST)
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_magmode = D3DTFG_POINT;
        else
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_magmode = D3DTFG_LINEAR;
        break;
    case GL_TEXTURE_WRAP_S:
        if((int)param == GL_CLAMP)
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_addu = D3DTADDRESS_CLAMP;
        else
            //GL_REPEAT falls here
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_addu = D3DTADDRESS_WRAP;
        break;
    case GL_TEXTURE_WRAP_T:
        if((int)param == GL_CLAMP)
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_addv = D3DTADDRESS_CLAMP;
        else
            //GL_REPEAT falls here
            g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_addv = D3DTADDRESS_WRAP;
        break;
    }
    g.m_tex[g.m_curstagebinding[g.m_curtgt]].m_capture = TRUE;
    g.m_texHandleValid = FALSE;
}

static void APIENTRY d3dTexParameteri (GLenum target, GLenum pname, GLint param)
{
	d3dTexParameterf(target,pname,(GLfloat) param);
}

static void APIENTRY d3dTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	D3DMATRIX f;

	f._11 = 1.f; f._21 = 0.f; f._31 = 0.f; f._41 = x;
	f._12 = 0.f; f._22 = 1.f; f._32 = 0.f; f._42 = y;
	f._13 = 0.f; f._23 = 0.f; f._33 = 1.f; f._43 = z;
	f._14 = 0.f; f._24 = 0.f; f._34 = 0.f; f._44 = 1.f;

	if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_curtexmatrix);

	g.m_d3ddev->MultiplyTransform(g.m_matrixMode, &f);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
	else if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_TEXTURE0, &g.m_curtexmatrix);
}

static void APIENTRY d3dUnlockArraysEXT()
{
    g.m_lckfirst = 0;
    g.m_lckcount = 0;
}

static void APIENTRY d3dVertex2f (GLfloat x, GLfloat y)
{
	y;

	if (g.m_nfv[g.m_comp] + g.m_vcnt[g.m_comp] >= (VBUFSIZE - MAXVERTSPERPRIM))
	{
		if (g.m_prim == GL_TRIANGLES)
		{
			if (g.m_vcnt[g.m_comp] % 3 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
		else if (g.m_prim == GL_QUADS)
		{
			if (g.m_vcnt[g.m_comp] % 4 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
		else if (g.m_prim == GL_LINES)
		{
			if (g.m_vcnt[g.m_comp] % 2 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}	
	}
#ifdef _X86_
	__asm
	{    
		mov     ecx, g.m_comp;
		mov     edi, g.m_verts;
		lea     esi, x;
		mov     eax, g.m_vcnt[ecx * 4];
		lea     edx, [eax + 1];
		mov     g.m_vcnt[ecx * 4], edx;
		lea     edx, [ecx * 8 + 28];
		mul     edx;
		cld;
		add     edi, eax;
		xor     eax, eax;
		movsd;
		movsd;
		stosd;
		lea     esi, g.m_nx;
		lea     ecx, [ecx * 2 + 4];
		rep     movsd;
	}
#else
	D3DVALUE *d3dv = &(((D3DVALUE*)g.m_verts)[g.m_vcnt[g.m_comp]++ * (g.m_comp * 2 + 7)]);

	*(d3dv++) = x;
	*(d3dv++) = y;
	*(d3dv++) = 0.f;
	memcpy(d3dv, &g.m_nx, (sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR) + g.m_comp*sizeof(D3DVALUE)*2));
#endif
}

static void APIENTRY d3dVertex2i (GLint x, GLint y)
{
	d3dVertex2f((GLfloat) x, (GLfloat) y);
}

static void APIENTRY d3dVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
	y;
	z;

	if (g.m_nfv[g.m_comp] + g.m_vcnt[g.m_comp] >= (VBUFSIZE - MAXVERTSPERPRIM))
	{
		if (g.m_prim == GL_TRIANGLES)
		{
			if (g.m_vcnt[g.m_comp] % 3 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
		else if(g.m_prim == GL_QUADS)
		{
			if (g.m_vcnt[g.m_comp] % 4 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
		else if (g.m_prim == GL_LINES)
		{
			if (g.m_vcnt[g.m_comp] % 2 == 0)
			{
				d3dEnd();
				d3dBegin(g.m_prim);
			}
		}
	}
#ifdef _X86_
	__asm
	{    
		mov     ecx, g.m_comp;
		mov     edi, g.m_verts;
		lea     esi, x;
		mov     eax, g.m_vcnt[ecx * 4];
		lea     edx, [eax + 1];
		mov     g.m_vcnt[ecx * 4], edx;
		lea     edx, [ecx * 8 + 28];
		mul     edx;
		cld;
		add     edi, eax;
		movsd;
		movsd;
		movsd;
		lea     esi, g.m_nx;
		lea     ecx, [ecx * 2 + 4];
		rep     movsd;
	}
#else
	D3DVALUE *d3dv = &(((D3DVALUE*)g.m_verts)[g.m_vcnt[g.m_comp]++ * (g.m_comp * 2 + 7)]);

	*(d3dv++) = x;
	*(d3dv++) = y;
	*(d3dv++) = z;
	memcpy(d3dv, &g.m_nx, (sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR) + g.m_comp*sizeof(D3DVALUE)*2));
#endif
}

static void APIENTRY d3dVertex3fv (const GLfloat *v)
{
    if(g.m_nfv[g.m_comp] + g.m_vcnt[g.m_comp] >= (VBUFSIZE - MAXVERTSPERPRIM))
    {
        if(g.m_prim == GL_TRIANGLES)
        {
            if(g.m_vcnt[g.m_comp] % 3 == 0)
            {
                d3dEnd();
                d3dBegin(g.m_prim);
            }
        }
        else if(g.m_prim == GL_QUADS)
        {
            if(g.m_vcnt[g.m_comp] % 4 == 0)
            {
                d3dEnd();
                d3dBegin(g.m_prim);
            }
        }
        else if(g.m_prim == GL_LINES)
        {
            if(g.m_vcnt[g.m_comp] % 2 == 0)
            {
                d3dEnd();
                d3dBegin(g.m_prim);
            }
        }
    }
#ifdef _X86_
    __asm
    {    
        mov     ecx, g.m_comp;
        mov     edi, g.m_verts;
        mov     esi, v;
        mov     eax, g.m_vcnt[ecx * 4];
        lea     edx, [eax + 1];
        mov     g.m_vcnt[ecx * 4], edx;
        lea     edx, [ecx * 8 + 28];
        mul     edx;
        cld;
        add     edi, eax;
        movsd;
        movsd;
        movsd;
        lea     esi, g.m_nx;
        lea     ecx, [ecx * 2 + 4];
        rep     movsd;
    }
#else
    D3DVALUE *d3dv = &(((D3DVALUE*)g.m_verts)[g.m_vcnt[g.m_comp]++ * (g.m_comp * 2 + 7)]);
    *(d3dv++) = *(v++);
    *(d3dv++) = *(v++);
    *(d3dv++) = *(v++);
    memcpy(d3dv, &g.m_nx, (sizeof(D3DVALUE)*3 + sizeof(D3DCOLOR) + g.m_comp*sizeof(D3DVALUE)*2));
#endif
}

static void APIENTRY d3dVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{ 
	if (size == 3 && type == GL_FLOAT)
	{
		g.m_vertexary = (GLfloat *) pointer;
		if (stride)
			g.m_vertexstride = stride;
		else
			g.m_vertexstride = 12;
	}
	else
	{
		char buf[128];

		dSprintf(buf,128,"Wrapper: Vertex array not supported (size: %d  type: %x  stride: %d)\n",
					size, type, stride);
		OutputDebugString(buf);
	}
}

static void APIENTRY d3dRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{ 
	d3dBegin(GL_POLYGON);

	d3dVertex2f((GLfloat) x1, (GLfloat) y1);
	d3dVertex2f((GLfloat) x2, (GLfloat) y1);
	d3dVertex2f((GLfloat) x2, (GLfloat) y2);
	d3dVertex2f((GLfloat) x1, (GLfloat) y2);

	d3dEnd();
}

static void APIENTRY d3dMultMatrixf (const GLfloat *m)
{ 
	if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_curtexmatrix);

	g.m_d3ddev->MultiplyTransform(g.m_matrixMode, (LPD3DMATRIX) m);

	if (g.m_matrixMode == D3DTRANSFORMSTATE_WORLD)
		g.m_inversedirty = TRUE;
	else if (g.m_matrixMode == D3DTRANSFORMSTATE_TEXTURE0)
		g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_TEXTURE0, &g.m_curtexmatrix);
}

static GLboolean APIENTRY d3dIsEnabled (GLenum cap)
{ 
	DWORD enabled;

	switch (cap)
	{
		case GL_BLEND:
			g.m_d3ddev->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &enabled);
			break;
		case GL_TEXTURE_2D:
			if (g.m_curtgt == 0)
            enabled = g.m_texturing;
			else
            enabled = g.m_mtex;
			break;
		case GL_TEXTURE_COORD_ARRAY:
			enabled = g.m_usetexcoordary[g.m_client_active_texture_arb];
			break;
		case GL_COLOR_ARRAY:
			enabled = g.m_usecolorary;
			break;
		case GL_VERTEX_ARRAY:
			enabled = g.m_usevertexary;
			break;
		case GL_NORMAL_ARRAY:
			enabled = g.m_usenormalary;
			break;
		case GL_FOG_COORDINATE_ARRAY_EXT:
			enabled = g.m_usefogary;
			break;
		case GL_LIGHTING:
			enabled = false;
			break;
		case GL_LIGHT0:
			enabled = false;
			break;
		default:
			{		
				char buf[64];

				dSprintf(buf,64,"Wrapper: unsupported glIsEnabled query: %x", cap);	
				OutputDebugString(buf);
		
				enabled = false;
			}
			break;
	}

	return enabled;
}

static void APIENTRY d3dGetTexEnviv (GLenum target, GLenum pname, GLint *params)
{ 
	target;

	if (pname == GL_TEXTURE_ENV_MODE)
		*params = g.m_blendmode[g.m_curtgt];
	else
		OutputDebugString("Wrapper: GL_TEXTURE_ENV_COLOR not implemented\n");
}

static void APIENTRY d3dFrontFace (GLenum mode)
{ 
	g.m_frontFace = mode;
	if (g.m_cullEnabled == TRUE)
	{
		DWORD statevalue;

		if ((g.m_cullMode == GL_BACK && mode == GL_CCW) ||
			 (g.m_cullMode == GL_FRONT && mode == GL_CW))
			statevalue=D3DCULL_CW;
		else
			statevalue=D3DCULL_CCW;
		g.m_d3ddev->SetRenderState(D3DRENDERSTATE_CULLMODE, statevalue);
	}
}

static void APIENTRY d3dTexGeni (GLenum coord, GLenum pname, GLint param)
{
	coord;
	pname;

	g.m_texgenmode[g.m_curtgt] = param;
	if (g.m_texgen[g.m_curtgt] && param == GL_OBJECT_LINEAR)
	{
		g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXCOORDINDEX,
													g.m_curtgt | D3DTSS_TCI_CAMERASPACEPOSITION);
		g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	}
	else
	{
		g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXCOORDINDEX,
													g.m_curtgt);
		g.m_d3ddev->SetTextureStageState(g.m_curtgt, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}
}

static void APIENTRY d3dTexGenfv (GLenum coord, GLenum pname, const GLfloat *params)
{
	pname;

	switch (coord)
	{
		case GL_S:
			dMemcpy(g.m_texgenplane[g.m_curtgt][0],params,sizeof(GLfloat)*4);
			g.m_objectdirty[g.m_curtgt] = TRUE;
			break;
		case GL_T:
			dMemcpy(g.m_texgenplane[g.m_curtgt][1],params,sizeof(GLfloat)*4);
			g.m_objectdirty[g.m_curtgt] = TRUE;
			break;
		default:
			{
				char buf[64];

				dSprintf(buf,64,"Wrapper: TexGen coordinate unsupported: %x\n", coord);
				OutputDebugString(buf);
			}
			break;
	}
}

static void APIENTRY d3dDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	if (count == 0)
		return;

	if (g.m_usefogary)
	{
		g.m_comp = 2;
		if (g.m_nfv[3] > (VBUFSIZE - count))	// check if space available
		{
			g.m_fmtvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
			g.m_nfv[3] = 0;
		}
		else
		{
			g.m_fmtvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
			g.m_verts = &(((QuakeFMTVertex*)g.m_verts)[g.m_nfv[3]]);
		}
	}
	else if (g.m_texturing) 
		if (g.m_mtex)
		{
			g.m_comp = 2;
			if (g.m_nfv[2] > (VBUFSIZE - count))	// check if space available
			{
				g.m_mtvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
				g.m_nfv[2] = 0;
			}
			else
			{
				g.m_mtvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
				g.m_verts = &(((QuakeMTVertex*)g.m_verts)[g.m_nfv[2]]);
			}
		}
		else
		{
			g.m_comp = 1;
			if (g.m_nfv[1] > (VBUFSIZE - count))	// check if space available
			{
				g.m_tvbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
				g.m_nfv[1] = 0;
			}
			else
			{
				g.m_tvbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
				g.m_verts = &(((QuakeTVertex*)g.m_verts)[g.m_nfv[1]]);
			}
		}
	else
	{
		g.m_comp = 0;
		if (g.m_nfv[0] > (VBUFSIZE - count))	// check if space available
		{
			g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
			g.m_nfv[0] = 0;
		}
		else
		{
			g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
			g.m_verts = &(((QuakeVertex*)g.m_verts)[g.m_nfv[0]]);
		}
	} 

	GLsizei i;
	unsigned last = first+count - 1;
	unsigned dstride = 7 + g.m_usefogary + g.m_comp*2;

	if (g.m_texturing)
	{
		if (g.m_usetexcoordary[0])
		{
			unsigned char *tex0 = &((unsigned char *) g.m_texcoordary[0])[first*g.m_texcoordstride[0]];	
			D3DVALUE *data = (D3DVALUE *) g.m_verts+7 + g.m_usefogary;

			for (i = 0; i < count; ++i)
			{
				memcpy(data,tex0,2*sizeof(GLfloat));
				tex0 += g.m_texcoordstride[0];
				data += dstride;
			}
		}
		else
		{
			D3DVALUE *data = (D3DVALUE *) g.m_verts+7 + g.m_usefogary;

			for (i = 0; i < count; ++i)
			{
				memcpy(data,&g.m_tu,2*sizeof(GLfloat));
				data += dstride;
			}
		}

		if (g.m_mtex)
			if (g.m_usetexcoordary[1])
			{
				unsigned char *tex1 = &((unsigned char *) g.m_texcoordary[1])[first*g.m_texcoordstride[1]];	
				D3DVALUE *data = (D3DVALUE *) g.m_verts+9 + g.m_usefogary;

				for (i = 0; i < count; ++i)
				{
					memcpy(data,tex1,2*sizeof(GLfloat));
					tex1 += g.m_texcoordstride[1];
					data += dstride;
				}
			}
			else
			{
				D3DVALUE *data = (D3DVALUE *) g.m_verts+9 + g.m_usefogary;

				for (i = 0; i < count; ++i)
				{
					memcpy(data,&g.m_tu2,2*sizeof(GLfloat));
					data += dstride;
				}
			}
	}
		
	if (g.m_usenormalary)
	{
		unsigned char *nml = &((unsigned char *) g.m_normalary)[first*g.m_normalstride];
		D3DVALUE *data = (D3DVALUE *) g.m_verts+3;

		for (i = 0; i < count; ++i)
		{
			memcpy(data,nml,3*sizeof(GLfloat));
			nml += g.m_normalstride;
			data += dstride;
		}
	}

	if (g.m_usecolorary)
	{		
		if (g.m_colortype == GL_UNSIGNED_BYTE)
		{	
#ifdef _X86_
			DWORD ustride = (dstride-1) * 4;
			
			_asm
			{
				mov esi, first;
				mov ecx, last;
				mov edx, 0x00FF00FF;
				mov ebx, g.m_colorary;
				mov edi, g.m_verts;
				add edi, 24;
				sub ecx, esi;
				lea esi, [ebx + esi * 4];
				inc ecx;
				cld;            
lp1:			lodsd;
				mov ebx, eax;
				and eax, edx;
				not edx;
				rol eax, 16;
				and ebx, edx;
				not edx;
				or  eax, ebx;
				stosd;
				add edi, ustride;
				loop lp1;
			}
#else
			GLubyte *clr = &((GLubyte *) g.m_colorary)[first*g.m_colorstride];
			D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

			for (i = 0; i < count; i++)
			{
				data[0] = RGBA_MAKE(clr[0],clr[1],clr[2],clr[3]);
				clr += g.m_colorstride;
				data += dstride;
			}
#endif
		}
		else
		{
			GLubyte *clr = &((GLubyte *) g.m_colorary)[first*g.m_colorstride];
			D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

			for (i = 0; i < count; ++i)
			{
				static float two55 = 255.f;
				unsigned int R, G, B, A;

#ifdef _X86_
				__asm {
					mov ebx, clr;
					fld [ebx];
					fld [ebx + 4];
					fld [ebx + 8];
					fld [ebx + 12];
					fld two55;
					fmul st(1), st(0);
					fmul st(2), st(0);
					fmul st(3), st(0);
					fmulp st(4), st(0);
					fistp A;
					fistp B;
					fistp G;
					fistp R;
					mov edx, A;
					cmp edx, 255;
					jle pt1;
					mov edx, 255;
			pt1:	mov eax, B;
					cmp eax, 255;
					jle pt2;
					mov eax, 255;
			pt2:	mov ebx, G;
					cmp ebx, 255;
					jle pt3;
					mov ebx, 255;
			pt3:	mov ecx, R;
					cmp ecx, 255;
					jle pt4;
					mov ecx, 255;
			pt4:	shl ebx, 8;
        			shl ecx, 16;
        			shl edx, 24;
        			or eax, ebx;
        			or ecx, edx;
        			or eax, ecx;
        			mov data, eax;
				}
#else
				GLfloat *fclr = (GLfloat *) clr;

				R = (unsigned int)(fclr[0] * two55);
				G = (unsigned int)(fclr[1] * two55);
				B = (unsigned int)(fclr[2] * two55);
				A = (unsigned int)(fclr[3] * two55);
				if (R > 255)
					R = 255;
				if (G > 255)
					G = 255;
				if (B > 255)
					B = 255;
				if (A > 255)
					A = 255;
				data[0] = RGBA_MAKE(R, G, B, A);
#endif
				clr += g.m_colorstride;
				data += dstride;
			}
		}
	}
	else
	{
		D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

		for (i = 0; i < count; ++i)
		{
			memcpy(data,&g.m_color,sizeof(D3DCOLOR));
			data += dstride;
		}
	}

	if (g.m_usefogary)
	{
		unsigned char *fog = &((unsigned char *) g.m_fogary)[first*g.m_fogstride];
		D3DCOLOR *data = (D3DCOLOR *) g.m_verts+7;

		for (i = 0; i < count; ++i)
		{
			static float two55 = 255.f;
			unsigned int A;

			A = (unsigned int) (((GLfloat *) fog)[0] * two55);
			if (A > 255)
				A = 255;
			data[0] = (255-A) << 24;

			fog += g.m_fogstride; 
			data += dstride;
		}
	}

	AssertFatal(g.m_usevertexary, "We're always using a vertex array...right?");

	{	
		unsigned char *vtx = &((unsigned char *) g.m_vertexary)[first*g.m_vertexstride];
		D3DVALUE *data = (D3DVALUE *) g.m_verts;

		for (i = 0; i < count; ++i)
		{
			memcpy(data,vtx,3*sizeof(GLfloat));
			vtx += g.m_vertexstride;
			data += dstride;
		}
	}	

	QuakeSetTexturingState();

	if (g.m_usefogary)
	{
		g.m_fmtvbuf->Unlock();
		if (mode == GL_TRIANGLES)
		{
			g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_fmtvbuf, g.m_nfv[3], count, 0);
			g.m_nfv[3] += count;
		}
		else
		{
        	char buf[64];
               	
			dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
			OutputDebugString(buf);
		}
	}
	else if (g.m_texturing)
		if (g.m_mtex)
		{
			g.m_mtvbuf->Unlock();
			switch (mode) 
			{
				case GL_LINES:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINELIST, g.m_mtvbuf, g.m_nfv[2], count, 0);
					g.m_nfv[2] += count;
        			break;
				case GL_TRIANGLES:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_mtvbuf, g.m_nfv[2], count, 0);
					g.m_nfv[2] += count;
					break;
				case GL_TRIANGLE_STRIP:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_mtvbuf, g.m_nfv[2], count, 0);
					g.m_nfv[2] += count;
					break;
				case GL_POLYGON:
				case GL_TRIANGLE_FAN:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_mtvbuf, g.m_nfv[2], count, 0);
					g.m_nfv[2] += count;
					break;
				case GL_QUADS:
					for (i = 0; i < count; i += 4) 
					{
						g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_mtvbuf, g.m_nfv[2], 4, 0);
						g.m_nfv[2] += 4;
					}
        			break;
				default:
				{
        			char buf[64];
               	
					dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
					OutputDebugString(buf);
				}
    		}
		}
		else
		{
			g.m_tvbuf->Unlock();
			switch (mode) 
			{
				case GL_LINES:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINELIST, g.m_tvbuf, g.m_nfv[1], count, 0);
					g.m_nfv[1] += count;
        			break;
				case GL_TRIANGLES:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_tvbuf, g.m_nfv[1], count, 0);
					g.m_nfv[1] += count;
					break;
				case GL_TRIANGLE_STRIP:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_tvbuf, g.m_nfv[1], count, 0);
					g.m_nfv[1] += count;
					break;
				case GL_POLYGON:
				case GL_TRIANGLE_FAN:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_tvbuf, g.m_nfv[1], count, 0);
					g.m_nfv[1] += count;
					break;
				case GL_QUADS:
					for (i = 0; i < count; i += 4) 
					{
						g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_tvbuf, g.m_nfv[1], 4, 0);
						g.m_nfv[1] += 4;
					}
        			break;
				default:
				{
        			char buf[64];
               	
					dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
					OutputDebugString(buf);
				}
    		}
		}
	else
	{
		g.m_vbuf->Unlock();
		switch (mode) 
			{
				case GL_LINES:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_LINELIST, g.m_vbuf, g.m_nfv[0], count, 0);
					g.m_nfv[0] += count;
        			break;
				case GL_TRIANGLES:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_vbuf, g.m_nfv[0], count, 0);
					g.m_nfv[0] += count;
					break;
				case GL_TRIANGLE_STRIP:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP, g.m_vbuf, g.m_nfv[0], count, 0);
					g.m_nfv[0] += count;
					break;
				case GL_POLYGON:
				case GL_TRIANGLE_FAN:
					g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_vbuf, g.m_nfv[0], count, 0);
					g.m_nfv[0] += count;
					break;
				case GL_QUADS:
					for (i = 0; i < count; i += 4) 
					{
						g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLEFAN, g.m_vbuf, g.m_nfv[0], 4, 0);
						g.m_nfv[0] += 4;
					}
        			break;
				default:
				{
        			char buf[64];
               	
					dSprintf(buf,64,"Wrapper: Unimplemented primitive type: %x\n",mode);
					OutputDebugString(buf);
				}
    		}
	}

#if 0
	if (g.m_usefogary)
	{
		if (g.m_nfv[0] > (VBUFSIZE - count))	// check if space available
		{
			g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_OKTOSWAP, &g.m_verts, 0);
			g.m_nfv[0] = 0;
		}
		else
		{
			g.m_vbuf->Lock(DDLOCK_WAIT | DDLOCK_NOOVERWRITE, &g.m_verts, 0);
			g.m_verts = &(((QuakeVertex*)g.m_verts)[g.m_nfv[0]]);
		}
		
		{
			GLubyte *fog = &((GLubyte *) g.m_fogary)[first*g.m_fogstride];
			D3DCOLOR *data = (D3DCOLOR *) g.m_verts+6;

			for (i = 0; i < count; ++i)
			{
				static float two55 = 255.f;
				unsigned int A;

//#ifdef _X86_
//#if 0
				DWORD maskedfog = g.m_fogcolor & 0xFFFFFF;

				__asm {
					mov ebx, fog;
					fld [ebx];
					fld two55;
					fmulp st(1), st(0);
					fistp A;
					mov edx, A;
					cmp edx, 255;
					jle pt5;
					mov edx, 255;
			pt5:	shl edx, 24;
        			or edx, maskedfog;
        			mov data, eax;
				}
//#else
				GLfloat *fclr = (GLfloat *) fog;

				A = (unsigned int) (fclr[0] * two55);
				if (A > 255)
					A = 255;
				data[0] = (A << 24) | (g.m_fogcolor & 0xFFFFFF);
//#endif
				fog += g.m_fogstride;
				data += 7;
			}
		}

		{
			unsigned char *vtx = &((unsigned char *) g.m_vertexary)[first*g.m_vertexstride];
			D3DVALUE *data = (D3DVALUE *) g.m_verts;

			for (i = 0; i < count; ++i)
			{
				memcpy(data,vtx,3*sizeof(GLfloat));
				vtx += g.m_vertexstride;
				data += 7;
			}
		}

		DWORD blend;
		DWORD src;
		DWORD dst;

		g.m_d3ddev->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &blend);
		g.m_d3ddev->GetRenderState(D3DRENDERSTATE_SRCBLEND, &src);
		g.m_d3ddev->GetRenderState(D3DRENDERSTATE_DESTBLEND, &dst);

		if (!blend)
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
		if (src != D3DBLEND_SRCALPHA)
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
		if (dst != D3DBLEND_INVSRCALPHA)
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); 
   
		g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		g.m_vbuf->Unlock();
		if (mode == GL_TRIANGLES)
		{
			g.m_d3ddev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, g.m_vbuf, g.m_nfv[0], count, 0);
			g.m_nfv[0] += count;
		}
		else
		{
			char buf[64];

			dSprintf(buf,64,"Wrapper: Unsupported fog coordinate primitive: %x\n", mode);
			OutputDebugString(buf);
		}

		if (!blend)
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
		if (src != D3DBLEND_SRCALPHA)
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_SRCBLEND, src);
		if (dst != D3DBLEND_INVSRCALPHA)
			g.m_d3ddev->SetRenderState(D3DRENDERSTATE_DESTBLEND, dst);
	}
#endif
}

static void APIENTRY d3dTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{ 
	target;
	pname;
	params;

	static float two55 = 255.f;
	unsigned int R, G, B, A;
#ifdef _X86_
    __asm {
        mov ebx, params;
        fld [ebx];
        fld [ebx + 4];
        fld [ebx + 8];
        fld [ebx + 12];
        fld two55;
        fmul st(1), st(0);
        fmul st(2), st(0);
        fmul st(3), st(0);
        fmulp st(4), st(0);
        fistp A;
        fistp B;
        fistp G;
        fistp R;
        mov edx, A;
        cmp edx, 255;
        jle pt1;
        mov edx, 255;
pt1:    mov eax, B;
        cmp eax, 255;
        jle pt2;
        mov eax, 255;
pt2:    mov ebx, G;
        cmp ebx, 255;
        jle pt3;
        mov ebx, 255;
pt3:    mov ecx, R;
        cmp ecx, 255;
        jle pt4;
        mov ecx, 255;
pt4:    shl ebx, 8;
        shl ecx, 16;
        shl edx, 24;
        or eax, ebx;
        or ecx, edx;
        or eax, ecx;
        mov g.m_envcolor, eax;
    }
#else
	R = (unsigned int) (params[0] * two55);
	G = (unsigned int) (params[1] * two55);
	B = (unsigned int) (params[2] * two55);
	A = (unsigned int) (params[3] * two55);
	if (R > 255)
		R = 255;
	if (G > 255)
		G = 255;
	if (B > 255)
		B = 255;
	if (A > 255)
		A = 255;
	g.m_envcolor = RGBA_MAKE(R, G, B, A);
#endif
}

static void APIENTRY d3dTexCoord2fv (const GLfloat *v)
{
	g.m_tu = v[0];
	g.m_tv = v[1];
}

static void APIENTRY d3dNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{ 
	type;

	if (stride)
		g.m_normalstride = stride;
	else
		g.m_normalstride = 12;
	g.m_normalary = (const GLfloat *) pointer;
}

static void APIENTRY d3dMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{ 
	face;
	pname;
	params;

	switch (pname)
	{
		case GL_AMBIENT:
			break;
		case GL_DIFFUSE:
		case GL_AMBIENT_AND_DIFFUSE:
			{
				static float two55 = 255.f;
				unsigned int R, G, B, A;

#ifdef _X86_
				__asm {
					mov ebx, params;
					fld [ebx];
					fld [ebx + 4];
					fld [ebx + 8];
					fld [ebx + 12];
					fld two55;
					fmul st(1), st(0);
					fmul st(2), st(0);
					fmul st(3), st(0);
					fmulp st(4), st(0);
					fistp A;
					fistp B;
					fistp G;
					fistp R;
					mov edx, A;
					cmp edx, 255;
					jle pt1;
					mov edx, 255;
		pt1:		mov eax, B;
					cmp eax, 255;
					jle pt2;
					mov eax, 255;
		pt2:		mov ebx, G;
					cmp ebx, 255;
					jle pt3;
					mov ebx, 255;
		pt3:		mov ecx, R;
					cmp ecx, 255;
					jle pt4;
					mov ecx, 255;
		pt4:		shl ebx, 8;
					shl ecx, 16;
					shl edx, 24;
					or eax, ebx;
					or ecx, edx;
					or eax, ecx;
					mov g.m_color, eax;
				}
#else
    			R = (unsigned int)(params[0] * two55);
				G = (unsigned int)(params[1] * two55);
				B = (unsigned int)(params[2] * two55);
				A = (unsigned int)(params[3] * two55);
				if (R > 255)
					R = 255;
				if (G > 255)
					G = 255;
				if (B > 255)
					B = 255;
				if (A > 255)
					A = 255;
				g.m_color = RGBA_MAKE(R, G, B, A);
#endif
			}
			break;
		default:
			{
				char buf[64];

				dSprintf(buf,64,"Wrapper: Materialfv pname not supported: %x\n", pname);
				OutputDebugString(buf);
			}
			break;
	}
}

static void APIENTRY d3dLightf (GLenum light, GLenum pname, GLfloat param)
{ 
	GLenum i = light - GL_LIGHT0;

	switch (pname)
	{
		case GL_SPOT_CUTOFF:
			if (param >= 180.0)
			{
				g.m_lights[i].dltType = (g.m_lights[i].dltType == D3DLIGHT_DIRECTIONAL) ? D3DLIGHT_DIRECTIONAL : D3DLIGHT_POINT;
				g.m_lights[i].dvTheta = g.m_lights[i].dvPhi = M_PI/2.0;
			}
			else
			{
				g.m_lights[i].dltType = (g.m_lights[i].dltType == D3DLIGHT_DIRECTIONAL) ? D3DLIGHT_DIRECTIONAL : D3DLIGHT_SPOT;
				g.m_lights[i].dvTheta = g.m_lights[i].dvPhi = mDegToRad(param*2.0);
			}
			break;
		case GL_CONSTANT_ATTENUATION:
			g.m_lights[i].dvAttenuation0 = param;
			break;
		case GL_LINEAR_ATTENUATION:
			g.m_lights[i].dvAttenuation1 = param;
			break;
		case GL_QUADRATIC_ATTENUATION:
			g.m_lights[i].dvAttenuation2 = param;
			break;
	}

	D3DLIGHT7 &peak = g.m_lights[i];

	g.m_d3ddev->SetLight(i,&g.m_lights[i]);
}

static void APIENTRY d3dLightfv (GLenum light, GLenum pname, const GLfloat *params)
{ 
	GLenum i = light - GL_LIGHT0;

	switch (pname)
	{
		case GL_POSITION:
			{
				D3DXMATRIX world;

				g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &world);
				if (params[3] == 0.0)
				{
					D3DXVECTOR4 mdir(params), wdir, tdir(0,0,1,0);

					g.m_lights[i].dltType = D3DLIGHT_DIRECTIONAL;
					D3DXVec4Transform(&wdir,&mdir,&world);
					dMemcpy(&g.m_lights[i].dvDirection,wdir,sizeof(GLfloat)*3);
				}
				else
				{
					D3DXVECTOR4 mpos(params), wpos;

					g.m_lights[i].dltType = (g.m_lights[i].dvPhi == M_2PI) ? D3DLIGHT_POINT : D3DLIGHT_SPOT;
					D3DXVec4Transform(&wpos,&mpos,&world);
					dMemcpy(&g.m_lights[i].dvPosition,wpos,sizeof(GLfloat)*3);
				}
			}
			break;
		case GL_SPOT_DIRECTION:
			{
				D3DXMATRIX world;
				D3DXVECTOR4 mdir(params), wdir;

				g.m_d3ddev->GetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *) &world);
				D3DXVec4Transform(&wdir,&mdir,&world);
				dMemcpy(&g.m_lights[i].dvDirection,wdir,sizeof(GLfloat)*3);
			}
			break;
		case GL_DIFFUSE:
			dMemcpy(&g.m_lights[i].dcvDiffuse,params,sizeof(GLfloat)*3);
			break;
	  	case GL_AMBIENT:
			dMemcpy(&g.m_lights[i].dcvAmbient,params,sizeof(GLfloat)*3);
			break;
		case GL_SPECULAR:
			dMemcpy(&g.m_lights[i].dcvSpecular,params,sizeof(GLfloat)*3);
			break;
	}

	D3DLIGHT7 &peak = g.m_lights[i];

	g.m_d3ddev->SetLight(i,&g.m_lights[i]);
}

static void APIENTRY d3dLightModelfv (GLenum pname, const GLfloat *params)
{ 
	if (pname == GL_LIGHT_MODEL_AMBIENT)
	{
		DWORD ambient;

		static float two55 = 255.f;
		unsigned int R, G, B, A;

#ifdef _X86_
		__asm {
			mov ebx, params;
			fld [ebx];
			fld [ebx + 4];
			fld [ebx + 8];
			fld [ebx + 12];
			fld two55;
			fmul st(1), st(0);
			fmul st(2), st(0);
			fmul st(3), st(0);
			fmulp st(4), st(0);
			fistp A;
			fistp B;
			fistp G;
			fistp R;
			mov edx, A;
			cmp edx, 255;
			jle pt1;
			mov edx, 255;
	pt1:	mov eax, B;
			cmp eax, 255;
			jle pt2;
			mov eax, 255;
	pt2:	mov ebx, G;
			cmp ebx, 255;
			jle pt3;
			mov ebx, 255;
	pt3:	mov ecx, R;
			cmp ecx, 255;
			jle pt4;
			mov ecx, 255;
	pt4:	shl ebx, 8;
			shl ecx, 16;
			shl edx, 24;
			or eax, ebx;
			or ecx, edx;
			or eax, ecx;
			mov ambient, eax;
		}
#else
		R = (unsigned int) (params[0] * two55);
		G = (unsigned int) (params[1] * two55);
		B = (unsigned int) (params[2] * two55);
		A = (unsigned int) (params[3] * two55);
		if (R > 255)
			R = 255;
		if (G > 255)
			G = 255;
		if (B > 255)
			B = 255;
		if (A > 255)
			A = 255;
		ambient = RGBA_MAKE(R, G, B, A);
#endif
		g.m_d3ddev->SetRenderState(D3DRENDERSTATE_AMBIENT, ambient);
	}
	else
	{
		char buf[64];

		dSprintf(buf,64,"Wrapper:  Unsupported LightModelfv pname: %x\n", pname);
		OutputDebugString(buf);
	}
}

static void APIENTRY d3dFogCoordPointerEXT(GLenum type, GLsizei stride, void *pointer)
{
	if (type == GL_FLOAT)
	{
		if (stride)
			g.m_fogstride = stride;
		else
			g.m_fogstride = 4;
		g.m_fogary = (GLfloat *) pointer;
	}
	else
		OutputDebugString("Wrapper: Unsupported FogCoordPointerEXT array type\n");
}

static void APIENTRY d3dPointSize (GLfloat size)
{ 
	size;
}

static int WINAPI wd3dChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
	hdc;
	ppfd;

    return 1;
}

static LRESULT CALLBACK MyMsgHandler(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
)
{
    if(uMsg == WM_CHAR)
    {
        if(wParam == 0x2F)
        {
            ++g.m_lod;
            if(g.m_lod == 8)
                g.m_lod = 0;
            int changedLOD = 0;
            static char str[256];
            for(int i = 0; i < MAXGLTEXHANDLES; ++i)
            {
                if(g.m_tex[i].m_ddsurf != 0) 
                {
                    DDSURFACEDESC2 surfdesc;
                    surfdesc.dwSize = sizeof(DDSURFACEDESC2);
                    g.m_tex[i].m_ddsurf->GetSurfaceDesc(&surfdesc);
                    if(surfdesc.dwMipMapCount != 0)
                    {
                        if(g.m_lod >= surfdesc.dwMipMapCount)
                            g.m_tex[i].m_ddsurf->SetLOD(surfdesc.dwMipMapCount - 1);
                        else
                        {
                            g.m_tex[i].m_ddsurf->SetLOD(g.m_lod);
                            ++changedLOD;
                        }
                    }
                }
            }
            _itoa(changedLOD, str, 10);
            OutputDebugString(str);
            OutputDebugString("\n");
            return 0;
        }
    }
    return CallWindowProc(g.m_wndproc, hwnd, uMsg, wParam, lParam);
}

static HGLRC WINAPI wd3dCreateContext(HDC hdc)
{
    g.m_hdc = hdc;
    g.m_hwnd = WindowFromDC(g.m_hdc);
    RECT rect;
    GetClientRect(g.m_hwnd, &rect);
    g.m_winWidth = (USHORT)rect.right;
    g.m_winHeight = (USHORT)rect.bottom;
    g.m_vwx = rect.left;
    g.m_vwy = rect.top;
    g.m_vww = rect.right - rect.left;
    g.m_vwh = rect.bottom - rect.top;
    
    HRESULT hr = D3DXInitialize();
    if(FAILED(hr))
    {
        return 0;
    }
    
    DWORD DeviceType = D3DX_DEFAULT;
    // Check registry key to see if we need to do software emulation	
    HKEY hKey;
    if(ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, RESPATH_QUAKE, &hKey)) {
        DWORD dwType;
        DWORD dwValue;
        DWORD dwSize = 4;
        if (ERROR_SUCCESS == RegQueryValueEx( hKey, "Emulation", NULL, &dwType, (LPBYTE) &dwValue, &dwSize) &&
            dwType == REG_DWORD &&
            dwValue != 0) {
            DeviceType = D3DX_HWLEVEL_2D;
        }
        RegCloseKey( hKey );
    }
    
    D3DX_VIDMODEDESC VidMode;
    hr = D3DXGetCurrentVideoMode(DeviceType, &VidMode);
    if(FAILED(hr))
    {
        D3DXUninitialize();
        return 0;
    }

    // See if the window is full screen
    if( (DWORD)rect.right == VidMode.width && (DWORD)rect.bottom == VidMode.height )
    {
        // We are full screen
        hr = D3DXCreateContextEx(DeviceType, D3DX_CONTEXT_FULLSCREEN, g.m_hwnd, g.m_hwnd,
        									VidMode.bpp, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
        									VidMode.width, VidMode.height, VidMode.refreshRate, &g.m_pD3DX);
    }
    else
    {
        hr = D3DXCreateContext(DeviceType, 0, g.m_hwnd, D3DX_DEFAULT, D3DX_DEFAULT, &g.m_pD3DX);
    }
    if(FAILED(hr))
    {
        D3DXUninitialize();
        return 0;
    }
    
    // Keep a copy of the device
    g.m_d3ddev = g.m_pD3DX->GetD3DDevice();
    if(g.m_d3ddev == NULL)
    {
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }

    g.m_d3ddev->GetCaps(&g.m_dd);
    if(FAILED(hr))
    {
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }

    // Check registry key to see if we need to turn off mipmapping
    if(ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, RESPATH_QUAKE, &hKey)) {
        DWORD dwType;
        DWORD dwValue;
        DWORD dwSize = 4;
        if (ERROR_SUCCESS == RegQueryValueEx( hKey, "DisableMipMap", NULL, &dwType, (LPBYTE) &dwValue, &dwSize) &&
            dwType == REG_DWORD &&
            dwValue != 0) {
            g.m_usemipmap = FALSE;
            OutputDebugString("Wrapper: Mipmapping disabled\n");
        }
        else {
            g.m_usemipmap = TRUE;
        }
        RegCloseKey( hKey );
    }
    else {
        g.m_usemipmap = TRUE;
    }

    // Enumerate texture formats and find the right ones to use
    DWORD texfmts = D3DXGetMaxSurfaceFormats(DeviceType, NULL, D3DX_SC_COLORTEXTURE);
    // Look for a four bit alpha surface
    BOOLEAN found = FALSE;
    DWORD i;
    for(i = 0; i < texfmts; ++i)
    {
        hr = D3DXGetSurfaceFormat(DeviceType, NULL, D3DX_SC_COLORTEXTURE, i, &g.m_ddFourBitAlphaSurfFormat);
        if( FAILED(hr) )
        {
            g.m_d3ddev->Release();
            g.m_pD3DX->Release();
            D3DXUninitialize();
            return 0;
        }
        if(g.m_ddFourBitAlphaSurfFormat == D3DX_SF_A4R4G4B4) 
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE )
    {
        OutputDebugString("Wrapper: Unable to find 4444 texture.\n");
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
    // Look for a eight bit alpha surface
    found = FALSE;
    for(i = 0; i < texfmts; ++i)
    {
        hr = D3DXGetSurfaceFormat(DeviceType, NULL, D3DX_SC_COLORTEXTURE, i, &g.m_ddEightBitAlphaSurfFormat);
        if( FAILED(hr) )
        {
            g.m_d3ddev->Release();
            g.m_pD3DX->Release();
            D3DXUninitialize();
            return 0;
        }
        if(g.m_ddEightBitAlphaSurfFormat == D3DX_SF_A8R8G8B8) 
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE )
    {
        OutputDebugString("Wrapper: Not using 8888 texture\n");
        g.m_ddEightBitAlphaSurfFormat = g.m_ddFourBitAlphaSurfFormat;
    }
    // Look for a surface
    found = FALSE;
    for(i = 0; i < texfmts; ++i)
    {
        hr = D3DXGetSurfaceFormat(DeviceType, NULL, D3DX_SC_COLORTEXTURE, i, &g.m_ddFiveBitSurfFormat);
        if( FAILED(hr) )
        {
            g.m_d3ddev->Release();
            g.m_pD3DX->Release();
            D3DXUninitialize();
            return 0;
        }
        if(g.m_ddFiveBitSurfFormat == D3DX_SF_R5G5B5 || g.m_ddFiveBitSurfFormat == D3DX_SF_R5G6B5) 
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE )
    {
        OutputDebugString("Wrapper: Unable to find 555 or 565 texture.\n");
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
    // Look for a 8-bit surface
    found = FALSE;
    for(i = 0; i < texfmts; ++i)
    {
        hr = D3DXGetSurfaceFormat(DeviceType, NULL, D3DX_SC_COLORTEXTURE, i, &g.m_ddEightBitSurfFormat);
        if( FAILED(hr) )
        {
            g.m_d3ddev->Release();
            g.m_pD3DX->Release();
            D3DXUninitialize();
            return 0;
        }
        if(g.m_ddEightBitSurfFormat == D3DX_SF_X8R8G8B8) 
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE )
    {
        OutputDebugString("Wrapper: Not using 888 texture\n");
		  g.m_ddEightBitSurfFormat = g.m_ddEightBitAlphaSurfFormat;
    }
    // Look for a luminance surface
    found = FALSE;
    for(i = 0; i < texfmts; ++i)
    {
        hr = D3DXGetSurfaceFormat(DeviceType, NULL, D3DX_SC_COLORTEXTURE, i, &g.m_ddLuminanceSurfFormat);
        if( FAILED(hr) )
        {
            g.m_d3ddev->Release();
            g.m_pD3DX->Release();
            D3DXUninitialize();
            return 0;
        }
        if(g.m_ddLuminanceSurfFormat == D3DX_SF_L8) 
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE )
    {
        OutputDebugString("Wrapper: Not using luminance texture\n");
        g.m_ddLuminanceSurfFormat = g.m_ddEightBitSurfFormat;
    }
	 // Look for an alpha surface
    found = FALSE;
    for(i = 0; i < texfmts; ++i)
    {
        hr = D3DXGetSurfaceFormat(DeviceType, NULL, D3DX_SC_COLORTEXTURE, i, &g.m_ddAlphaSurfFormat);
        if( FAILED(hr) )
        {
            g.m_d3ddev->Release();
            g.m_pD3DX->Release();
            D3DXUninitialize();
            return 0;
        }
        if(g.m_ddAlphaSurfFormat == D3DX_SF_A8) 
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE )
    {
        OutputDebugString("Wrapper: Not using alpha texture\n");
        g.m_ddAlphaSurfFormat = g.m_ddEightBitSurfFormat;
    }                   

    // Do misc init stuff
    if(g.m_dd.dwMaxTextureWidth < 512 || g.m_dd.dwMaxTextureHeight < 512) {
        g.m_subsample = TRUE;
        OutputDebugString("Wrapper: Subsampling textures to 256 x 256\n");
    }
    else
        g.m_subsample = FALSE;
    if(g.m_dd.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
        g.m_makeSquare = TRUE;
        OutputDebugString("Wrapper: Forcing all textures to be square\n");
    }
    else
        g.m_makeSquare = FALSE;
    if(g.m_dd.wMaxSimultaneousTextures > 1) {
        g.m_usemtex = TRUE;
        OutputDebugString("Wrapper: Multitexturing enabled\n");
    }
    else {
        g.m_usemtex = FALSE;
        OutputDebugString("Wrapper: Multitexturing not available with this driver\n");
    }
    if(!(g.m_dd.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR) &&
        !(g.m_dd.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPNEAREST) &&
        !(g.m_dd.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPLINEAR) &&
        !(g.m_dd.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPNEAREST)) {
        g.m_usemipmap = FALSE;
        OutputDebugString("Wrapper: Mipmapping disabled\n");
    }
	if (!(g.m_dd.dwVertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
	{
		g.m_usedirectional = FALSE;
		OutputDebugString("Wrapper: Directional lights disabled\n");
	}
	else
		g.m_usedirectional = TRUE;
    if(ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, RESPATH_QUAKE, &hKey)) {
        DWORD dwType;
        DWORD dwValue;
        DWORD dwSize = 4;
        if (ERROR_SUCCESS == RegQueryValueEx( hKey, "DoFlip", NULL, &dwType, (LPBYTE) &dwValue, &dwSize) &&
            dwType == REG_DWORD &&
            dwValue != 0) {
            g.m_doFlip = TRUE;
        }
        else {
            g.m_doFlip = FALSE;
        }
        RegCloseKey( hKey );
    }
    else {
        g.m_doFlip = FALSE;
    }
    
    // Create vertex buffers
    // Get a D3D ptr to create the vertex buffer.
    LPDIRECT3D7 pD3D = g.m_pD3DX->GetD3D();
    if( pD3D == NULL )
    {
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
    D3DVERTEXBUFFERDESC vbufdesc;
    vbufdesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
    vbufdesc.dwCaps = D3DVBCAPS_WRITEONLY;
    if (!(g.m_dd.dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
        vbufdesc.dwCaps = D3DVBCAPS_SYSTEMMEMORY;
    vbufdesc.dwFVF = QUAKEVFMT;
    vbufdesc.dwNumVertices = VBUFSIZE;
    hr = pD3D->CreateVertexBuffer(&vbufdesc, &g.m_vbuf, 0);
    if( FAILED(hr) )
    {
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
    vbufdesc.dwFVF = QUAKETVFMT;
    hr = pD3D->CreateVertexBuffer(&vbufdesc, &g.m_tvbuf, 0);
    if( FAILED(hr) )
    {
        g.m_vbuf->Release();
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
    vbufdesc.dwFVF = QUAKEMTVFMT;
    hr = pD3D->CreateVertexBuffer(&vbufdesc, &g.m_mtvbuf, 0);
    if( FAILED(hr) )
    {
        g.m_tvbuf->Release();
        g.m_vbuf->Release();
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
	 vbufdesc.dwFVF = QUAKEFMTVFMT;
    hr = pD3D->CreateVertexBuffer(&vbufdesc, &g.m_fmtvbuf, 0);
    if( FAILED(hr) )
    {
		  g.m_mtvbuf->Release();
        g.m_tvbuf->Release();
        g.m_vbuf->Release();
        g.m_d3ddev->Release();
        g.m_pD3DX->Release();
        D3DXUninitialize();
        return 0;
    }
    // Done with D3D
    pD3D->Release();
    
	// Some more init stuff
	g.m_cullMode = GL_BACK;
	g.m_cullEnabled = FALSE;
	 
	g.m_texHandleValid = FALSE;
	g.m_texturing = FALSE;
	g.m_updvwp = TRUE;
	g.m_blendmode[0] = GL_MODULATE;
	g.m_blendmode[1] = GL_MODULATE;
	g.m_nfv[0] = g.m_nfv[1] = g.m_nfv[2] = g.m_nfv[3] = 0;
	g.m_curtgt = 0;
	g.m_client_active_texture_arb = 0;
	g.m_nx = 0.0;
	g.m_ny = 0.0;
	g.m_nz = 1.0;
	g.m_color = 0xFFFFFFFF;
	g.m_tu = g.m_tv = g.m_tu2 = g.m_tv2 = 0.f;
	g.m_clearColor = 0;
	g.m_clearDepth = 1.f;
	g.m_usecolorary = FALSE;
	g.m_usetexcoordary[0] = FALSE;
	g.m_usetexcoordary[1] = FALSE;
	g.m_usevertexary = FALSE;
	g.m_scix = 0;
	g.m_sciy = 0;
	g.m_sciw = g.m_winWidth;
	g.m_scih = g.m_winHeight;
	g.m_lckfirst = 0;
	g.m_lckcount = 0;
	g.m_numIndices = 1024;
	g.m_wIndices = new WORD[g.m_numIndices];
	g.m_frontFace = GL_CCW;
	g.m_usenormalary = FALSE;
	g.m_normalstride = 0;
	g.m_texcoordstride[0] = 0;
	g.m_texcoordstride[1] = 0;
	g.m_texgen[0] = FALSE;
	g.m_texgen[1] = FALSE;
	g.m_texgenmode[0] = GL_EYE_LINEAR;
	g.m_texgenmode[0] = GL_EYE_LINEAR;
	g.m_texgenplane[0][0][0] = 1.0;
	g.m_texgenplane[0][0][1] = 0.0;
	g.m_texgenplane[0][0][2] = 0.0;
	g.m_texgenplane[0][0][3] = 0.0;
	g.m_texgenplane[0][1][0] = 0.0;
	g.m_texgenplane[0][1][1] = 1.0;
	g.m_texgenplane[0][1][2] = 0.0;
	g.m_texgenplane[0][1][3] = 0.0;
	g.m_texgenplane[1][0][0] = 1.0;
	g.m_texgenplane[1][0][1] = 0.0;
	g.m_texgenplane[1][0][2] = 0.0;
	g.m_texgenplane[1][0][3] = 0.0;
	g.m_texgenplane[1][1][0] = 0.0;
	g.m_texgenplane[1][1][1] = 1.0;
	g.m_texgenplane[1][1][2] = 0.0;
	g.m_texgenplane[1][1][3] = 0.0;
	g.m_inversedirty = TRUE;
	g.m_objectdirty[0] = TRUE;
	g.m_objectdirty[1] = TRUE;
	g.m_envcolor = 0;
	g.m_colorstride = 0;
	g.m_colortype = GL_FLOAT;
	g.m_spherecoords = new GLfloat[100];
	g.m_zbias = 0;
	g.m_fogcolor = 0;
	g.m_usefogary = FALSE;
	g.m_fogstride = 0;

	// Initialize the structure.
	ZeroMemory(g.m_lights, sizeof(D3DLIGHT7));
 
	// Set up for a white point light.
	g.m_lights[0].dltType = D3DLIGHT_DIRECTIONAL;
	g.m_lights[0].dcvDiffuse.r = 1.0f;
	g.m_lights[0].dcvDiffuse.g = 1.0f;
	g.m_lights[0].dcvDiffuse.b = 1.0f;
	g.m_lights[0].dcvAmbient.r = 0.0f;
	g.m_lights[0].dcvAmbient.g = 0.0f;
	g.m_lights[0].dcvAmbient.b = 0.0f;
	g.m_lights[0].dcvSpecular.r = 1.0f;
	g.m_lights[0].dcvSpecular.g = 1.0f;
	g.m_lights[0].dcvSpecular.b = 1.0f;
 
    // Position it high in the scene, and behind the viewer.
    // (Remember, these coordinates are in world space, so
    //  the "viewer" could be anywhere in world space, too. 
    //  For the purposes of this example, assume the viewer
    //  is at the origin of world space.)
	g.m_lights[0].dvPosition.x = 0.0f;
	g.m_lights[0].dvPosition.y = 0.0f;
	g.m_lights[0].dvPosition.z = 1.0f;
	g.m_lights[0].dvDirection.x = 0.0f;
	g.m_lights[0].dvDirection.y = 0.0f;
	g.m_lights[0].dvDirection.z = -1.0f;
 
	// Don't attenuate.
	g.m_lights[0].dvAttenuation0 = 1.0f; 
	g.m_lights[0].dvRange = D3DLIGHT_RANGE_MAX;

	g.m_lights[0].dvTheta = g.m_lights[0].dvPhi = M_PI/4.0;

	for (i = 1; i < 8; ++i)
	{
		D3DLIGHT7 &d3dLight = g.m_lights[i];

		// Initialize the structure.
		ZeroMemory(&d3dLight, sizeof(D3DLIGHT7));
 
		// Set up for a white point light.
		d3dLight.dltType = D3DLIGHT_DIRECTIONAL;
		d3dLight.dcvDiffuse.r = 0.0f;
		d3dLight.dcvDiffuse.g = 0.0f;
		d3dLight.dcvDiffuse.b = 0.0f;
		d3dLight.dcvAmbient.r = 0.0f;
		d3dLight.dcvAmbient.g = 0.0f;
		d3dLight.dcvAmbient.b = 0.0f;
		d3dLight.dcvSpecular.r = 0.0f;
		d3dLight.dcvSpecular.g = 0.0f;
		d3dLight.dcvSpecular.b = 0.0f;
 
		// Position it high in the scene, and behind the viewer.
		// (Remember, these coordinates are in world space, so
		//  the "viewer" could be anywhere in world space, too. 
		//  For the purposes of this example, assume the viewer
		//  is at the origin of world space.)
		d3dLight.dvPosition.x = 0.0f;
		d3dLight.dvPosition.y = 0.0f;
		d3dLight.dvPosition.z = 1.0f;
		d3dLight.dvDirection.x = 0.0f;
		d3dLight.dvDirection.y = 0.0f;
		d3dLight.dvDirection.z = -1.0f;
 
		// Don't attenuate.
		d3dLight.dvAttenuation0 = 1.0f; 
		d3dLight.dvRange = D3DLIGHT_RANGE_MAX;

		d3dLight.dvTheta = d3dLight.dvPhi = M_PI/2.0;
	}

	g.m_curtexmatrix._11 = 1.0f; g.m_curtexmatrix._12 = 0.0f; g.m_curtexmatrix._13 = 0.0f; g.m_curtexmatrix._14 = 0.0f;
	g.m_curtexmatrix._21 = 0.0f; g.m_curtexmatrix._22 = 1.0f; g.m_curtexmatrix._23 = 0.0f; g.m_curtexmatrix._24 = 0.0f;
	g.m_curtexmatrix._31 = 0.0f; g.m_curtexmatrix._32 = 0.0f; g.m_curtexmatrix._33 = 1.0f; g.m_curtexmatrix._34 = 0.0f;
	g.m_curtexmatrix._41 = 0.0f; g.m_curtexmatrix._42 = 0.0f; g.m_curtexmatrix._43 = 0.0f; g.m_curtexmatrix._44 = 1.0f;

	g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&g.m_curtexmatrix);

	 GLListManip<GLuint> freeTextures(&g.m_freeTextures);

    for(i = 0; i < MAXGLTEXHANDLES; ++i) {
        g.m_tex[i].m_ddsurf = 0;
        g.m_tex[i].m_block = 0;
        g.m_tex[i].m_capture = FALSE;
        g.m_tex[i].m_dwStage = 0;
        g.m_tex[i].m_minmode = D3DTFN_POINT;
        g.m_tex[i].m_magmode = D3DTFG_LINEAR;
        g.m_tex[i].m_mipmode = D3DTFP_LINEAR;
        g.m_tex[i].m_addu = D3DTADDRESS_WRAP;
        g.m_tex[i].m_addv = D3DTADDRESS_WRAP;
    
    	  if (i)
	         freeTextures.insert(i);
    }

    D3DMATRIX unity;
    unity._11 = 1.0f; unity._12 = 0.0f; unity._13 = 0.0f; unity._14 = 0.0f;
    unity._21 = 0.0f; unity._22 = 1.0f; unity._23 = 0.0f; unity._24 = 0.0f;
    unity._31 = 0.0f; unity._32 = 0.0f; unity._33 = 1.0f; unity._34 = 0.0f;
    unity._41 = 0.0f; unity._42 = 0.0f; unity._43 = 0.0f; unity._44 = 1.0f;
    g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_VIEW, &unity); 
    g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_WORLD, &unity); 
    g.m_d3ddev->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &unity); 
    g.m_curtex[0] = NULL;
    g.m_curtex[1] = NULL;
    g.m_d3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
    g.m_d3ddev->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
    g.m_d3ddev->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
    g.m_d3ddev->SetRenderState(D3DRENDERSTATE_CLIPPING, TRUE);
    g.m_d3ddev->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
	 g.m_d3ddev->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x32323219);
	 g.m_d3ddev->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    g.m_d3ddev->SetRenderState(D3DRENDERSTATE_EXTENTS, FALSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_TEXCOORDINDEX,0);
    if(g.m_usemtex == TRUE)
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_TEXCOORDINDEX,1);
    
    // Create shaders
    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][0]);                

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][1]);

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][2]);

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][3]);

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][4]);

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][5]);

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][6]);

    g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][7]);

	 g.m_d3ddev->BeginStateBlock();
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    // following stage state to speedup software rasterizer
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g.m_d3ddev->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g.m_d3ddev->EndStateBlock(&g.m_shaders[0][8]);

    if(g.m_usemtex)
    {
        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][0]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][1]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_MODULATE);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][2]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_MODULATE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][3]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][4]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][5]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_MODULATE);
        // following stage state to speedup software rasterizer
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][6]);

        g.m_d3ddev->BeginStateBlock();
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_MODULATE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        g.m_d3ddev->SetTextureStageState (1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g.m_d3ddev->EndStateBlock(&g.m_shaders[1][7]);
    }

    // Hook into message loop
    // g.m_wndproc = (WNDPROC)SetWindowLong(g.m_hwnd, GWL_WNDPROC, (LONG)MyMsgHandler);
    // g.m_lod = 0;
    
    // Start a scene
    g.m_d3ddev->BeginScene();
    
    return (HGLRC)1;
}

static BOOL WINAPI wd3dDeleteContext(HGLRC hglrc)
{
	hglrc;

    g.m_d3ddev->EndScene();
    SetWindowLong(g.m_hwnd, GWL_WNDPROC, (LONG)g.m_wndproc);
    int i;
    for(i = 0; i < MAXGLTEXHANDLES; ++i){
        if(g.m_tex[i].m_ddsurf != 0) {
            g.m_tex[i].m_ddsurf->Release();
            g.m_tex[i].m_ddsurf = 0;
            if(g.m_tex[i].m_block != 0)
            {
                g.m_d3ddev->DeleteStateBlock(g.m_tex[i].m_block);
                g.m_tex[i].m_block = 0;
            }
            g.m_tex[i].m_capture = FALSE;
        }
    }

	GLListManip<GLuint> freeTextures(&g.m_freeTextures);

	while (g.m_freeTextures.length())
		freeTextures.remove();

    for(i = 0; i < 8; ++i)
        g.m_d3ddev->DeleteStateBlock(g.m_shaders[0][i]);
    if(g.m_usemtex)
    {
        for(i = 0; i < 8; ++i)
            g.m_d3ddev->DeleteStateBlock(g.m_shaders[1][i]);
    }
    g.m_vbuf->Release();
    g.m_vbuf = 0;
    g.m_mtvbuf->Release();
    g.m_mtvbuf = 0;
    g.m_tvbuf->Release();
    g.m_tvbuf = 0;
    g.m_d3ddev->Release();
    g.m_d3ddev = 0;
    g.m_pD3DX->Release();
    g.m_pD3DX = 0;
    D3DXUninitialize();
    delete[] g.m_wIndices;
    g.m_wIndices = 0;

    return TRUE;
}

static int WINAPI wd3dDescribePixelFormat(HDC hdc, INT iPixelFormat, UINT nBytes, PIXELFORMATDESCRIPTOR *ppfd)
{
	hdc;
	iPixelFormat;
	nBytes;

    HRESULT hr = D3DXInitialize();
    if (FAILED(hr))
        return 0;

    D3DX_VIDMODEDESC VidMode;
    hr = D3DXGetCurrentVideoMode(D3DX_DEFAULT, &VidMode);
    if (FAILED(hr))
    {
        D3DXUninitialize();
        return 0;
    }

    D3DXUninitialize();

    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    ppfd->nVersion = 1;
    ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
    ppfd->iPixelType = PFD_TYPE_RGBA;
    ppfd->cColorBits = (unsigned char)VidMode.bpp;    
    ppfd->cAccumBits = 0;
    ppfd->cAccumRedBits = 0;
    ppfd->cAccumGreenBits = 0;
    ppfd->cAccumBlueBits = 0;
    ppfd->cAccumAlphaBits = 0;
    ppfd->cStencilBits = 0;
    ppfd->cAuxBuffers = 0;
    ppfd->iLayerType = 0;
    ppfd->bReserved = 0;
    ppfd->dwLayerMask = 0;
    ppfd->dwVisibleMask = 0;
    ppfd->dwDamageMask = 0;
    if(VidMode.bpp == 16)
    {
        ppfd->cRedBits = 5;
        ppfd->cRedShift = 11;
        ppfd->cGreenBits = 6;
        ppfd->cGreenShift = 5;
        ppfd->cBlueBits = 5;
        ppfd->cBlueShift = 0;
        ppfd->cAlphaBits = 0;
        ppfd->cAlphaShift = 0;
        ppfd->cDepthBits = 16;
    }
    else if(VidMode.bpp == 24 || VidMode.bpp == 32)
    {
        ppfd->cRedBits = 8;
        ppfd->cRedShift = 16;
        ppfd->cGreenBits = 8;
        ppfd->cGreenShift = 8;
        ppfd->cBlueBits = 8;
        ppfd->cBlueShift = 0;
        ppfd->cAlphaBits = 0;
        ppfd->cAlphaShift = 0;
        ppfd->cDepthBits = VidMode.bpp;
    }
    else
    {
        return 0;
    }

    return 1;
} 

static HGLRC WINAPI wd3dGetCurrentContext(VOID)
{
    return (HGLRC)1;
}

static HDC WINAPI wd3dGetCurrentDC(VOID)
{
    return g.m_hdc;
}

static int WINAPI wd3dGetPixelFormat(HDC hdc)
{
	hdc;

	return 1;
}

static PROC WINAPI wd3dGetProcAddress(LPCSTR str)
{
    if(strcmp(str, "glMTexCoord2fSGIS") == 0)
        return (PROC)d3dMTexCoord2fSGIS;
    else if(strcmp(str, "glSelectTextureSGIS") == 0)
        return (PROC)d3dSelectTextureSGIS;
    else if(strcmp(str, "glActiveTextureARB") == 0)
        return (PROC)d3dActiveTextureARB;
    else if(strcmp(str, "glClientActiveTextureARB") == 0)
        return (PROC)d3dClientActiveTextureARB;
    else if(strcmp(str, "glMultiTexCoord2fARB") == 0)
        return (PROC)d3dMultiTexCoord2fARB;
	 else if(strcmp(str, "glMultiTexCoord2fvARB") == 0)
		  return (PROC)d3dMultiTexCoord2fvARB;
    else if(strcmp(str, "glLockArraysEXT") == 0)
        return (PROC)d3dLockArraysEXT;
    else if(strcmp(str, "glUnlockArraysEXT") == 0)
        return (PROC)d3dUnlockArraysEXT;
    else
    {
        OutputDebugString("Wrapper: Unimplemented function ");
        OutputDebugString(str);
        OutputDebugString("\n");
    }
    return NULL;
}

static BOOL WINAPI wd3dMakeCurrent(HDC hdc, HGLRC hglrc)
{
	hdc;
	hglrc;

	return TRUE;
}

static BOOL WINAPI wd3dSetPixelFormat(HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
	hdc;
	iPixelFormat;
	ppfd;

	return TRUE;
}

static BOOL WINAPI wd3dSwapBuffers(HDC hdc)
{
	hdc;

#ifdef USEICECAP
	static unsigned nframes = 0;
	++nframes;
	if(nframes == 1000)
	{
		OutputDebugString("Started profiling\n");
		StartCAP();
	}
	if(nframes == 1500)
	{
		StopCAP();
		OutputDebugString("stopped profiling\n");
	}
#endif

	g.m_d3ddev->EndScene();
	g.m_pD3DX->UpdateFrame(g.m_doFlip ? 0 : D3DX_UPDATE_NOVSYNC);
	//g.m_pD3DX->UpdateFrame(D3DX_UPDATE_NOVSYNC);

//LPDIRECTDRAW7 pDD = g.m_pD3DX->GetDD();

	//pDD->GetSurfaceFromDC(hdc,&g.m_ddsurface);

	//g.m_ddsurface->Flip(NULL,0);
	g.m_d3ddev->BeginScene();
	return TRUE;
}


//////////////////////////////////////////// NOT USED by QuakeGL ///////////////////////////////////////////////////////

#define DODPFS

static void DPF(char *str)
{
	OutputDebugString(str);
	OutputDebugString("\n");
}

static void APIENTRY d3dAccum (GLenum op, GLfloat value)
{ 
	op;
	value;

	#ifdef DODPFS 
	DPF("glAccum"); 
	#endif //DODPFS
}

static GLboolean APIENTRY d3dAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences)
{ 
	n;
	textures;
	residences;

	GLboolean dummy = FALSE;

	#ifdef DODPFS 
	DPF("glAreTexturesResident"); 
	#endif //DODPFS

	return dummy;
}

static void APIENTRY d3dBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{ 
	width;
	height;
	xorig;
	yorig;
	xmove;
	ymove;
	bitmap;

	#ifdef DODPFS 
	DPF("glBitmap"); 
	#endif //DODPFS
}

static void APIENTRY d3dCallList (GLuint list)
{ 
	list;

	#ifdef DODPFS 
	DPF("glCallList"); 
	#endif //DODPFS
}

static void APIENTRY d3dCallLists (GLsizei n, GLenum type, const GLvoid *lists)
{ 
	n;
	type;
	lists;

	#ifdef DODPFS 
	DPF("glCallLists"); 
	#endif //DODPFS
}

static void APIENTRY d3dClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glClearAccum"); 
	#endif //DODPFS
}

static void APIENTRY d3dClearIndex (GLfloat c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glClearIndex"); 
	#endif //DODPFS
}

static void APIENTRY d3dClearStencil (GLint s)
{ 
	s;

	#ifdef DODPFS 
	DPF("glClearStencil"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3b (GLbyte red, GLbyte green, GLbyte blue)
{ 
	red;
	green;
	blue;

	#ifdef DODPFS 
	DPF("glColor3b"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3bv (const GLbyte *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3bv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3d (GLdouble red, GLdouble green, GLdouble blue)
{ 
	red;
	green;
	blue;

	#ifdef DODPFS 
	DPF("glColor3d"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3i (GLint red, GLint green, GLint blue)
{ 
	red;
	green;
	blue;

	#ifdef DODPFS 
	DPF("glColor3i"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3s (GLshort red, GLshort green, GLshort blue)
{ 
	red;
	green;
	blue;

	#ifdef DODPFS 
	DPF("glColor3s"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3ui (GLuint red, GLuint green, GLuint blue)
{ 
	red;
	green;
	blue;

	#ifdef DODPFS 
	DPF("glColor3ui"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3uiv (const GLuint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3uiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3us (GLushort red, GLushort green, GLushort blue)
{ 
	red;
	green;
	blue;

	#ifdef DODPFS 
	DPF("glColor3us"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor3usv (const GLushort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor3usv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColor4b"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4bv (const GLbyte *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor4bv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColor4d"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor4dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColor4i"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor4iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColor4s"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor4sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColor4ui"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4uiv (const GLuint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor4uiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColor4us"); 
	#endif //DODPFS
}

static void APIENTRY d3dColor4usv (const GLushort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glColor4usv"); 
	#endif //DODPFS
}

static void APIENTRY d3dColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{ 
	red;
	green;
	blue;
	alpha;

	#ifdef DODPFS 
	DPF("glColorMask"); 
	#endif //DODPFS
}

static void APIENTRY d3dColorMaterial (GLenum face, GLenum mode)
{ 
	face;
	mode;

	#ifdef DODPFS 
	DPF("glColorMaterial"); 
	#endif //DODPFS
}

static void APIENTRY d3dCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{ 
	x;
	y;
	width;
	height;
	type;

	#ifdef DODPFS 
	DPF("glCopyPixels"); 
	#endif //DODPFS
}

static void APIENTRY d3dCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{ 
	target;
	level;
	internalFormat;
	x;
	y;
	width;
	border;

	#ifdef DODPFS 
	DPF("glCopyTexImage1D"); 
	#endif //DODPFS
}

static void APIENTRY d3dCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{ 
	target;
	level;
	internalFormat;
	x;
	y;
	width;
	height;
	border;

	#ifdef DODPFS 
	DPF("glCopyTexImage2D"); 
	#endif //DODPFS
}

static void APIENTRY d3dCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{ 
	target;
	level;
	xoffset;
	x;
	y;
	width;

	#ifdef DODPFS 
	DPF("glCopyTexSubImage1D"); 
	#endif	//DODPFS
}

static void APIENTRY d3dCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{ 
	target;
	level;
	xoffset;
	yoffset;
	x;
	y;
	width;
	height;

	#ifdef DODPFS 
	DPF("glCopyTexSubImage2D"); 
	#endif	//DODPFS
}

static void APIENTRY d3dDeleteLists (GLuint list, GLsizei range)
{ 
	list;
	range;

	#ifdef DODPFS 
	DPF("glDeleteLists"); 
	#endif //DODPFS
}

static void APIENTRY d3dDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{ 
	width;
	height;
	format;
	type;
	pixels;

	#ifdef DODPFS 
	DPF("glDrawPixels"); 
	#endif //DODPFS
}

static void APIENTRY d3dEdgeFlag (GLboolean flag)
{ 
	flag;

	#ifdef DODPFS 
	DPF("glEdgeFlag"); 
	#endif //DODPFS
}

static void APIENTRY d3dEdgeFlagPointer (GLsizei stride, const GLvoid *pointer)
{ 
	stride;
	pointer;

	#ifdef DODPFS 
	DPF("glEdgeFlagPointer"); 
	#endif //DODPFS
}

static void APIENTRY d3dEdgeFlagv (const GLboolean *flag)
{ 
	flag;

	#ifdef DODPFS 
	DPF("glEdgeFlagv"); 
	#endif //DODPFS
}

static void APIENTRY d3dEndList (void)
{ 
	#ifdef DODPFS 
	DPF("d3dEndList"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord1d (GLdouble u)
{ 
	u;

	#ifdef DODPFS 
	DPF("glEvalCoord1d"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord1dv (const GLdouble *u)
{ 
	u;

	#ifdef DODPFS 
	DPF("glEvalCoord1dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord1f (GLfloat u)
{ 
	u;

	#ifdef DODPFS 
	DPF("glEvalCoord1f"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord1fv (const GLfloat *u)
{ 
	u;

	#ifdef DODPFS 
	DPF("glEvalCoord1fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord2d (GLdouble u, GLdouble v)
{ 
	u;
	v;

	#ifdef DODPFS 
	DPF("glEvalCoord2d"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord2dv (const GLdouble *u)
{ 
	u;

	#ifdef DODPFS 
	DPF("glEvalCoord2dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord2f (GLfloat u, GLfloat v)
{ 
	u;
	v;

	#ifdef DODPFS 
	DPF("glEvalCoord2f"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalCoord2fv (const GLfloat *u)
{ 
	u;

	#ifdef DODPFS 
	DPF("glEvalCoord2fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{ 
	mode;
	i1;
	i2;

	#ifdef DODPFS 
	DPF("glEvalMesh1"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{ 
	mode;
	i1;
	i2;
	j1;
	j2;

	#ifdef DODPFS 
	DPF("glEvalMesh2"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalPoint1 (GLint i)
{ 
	i;

	#ifdef DODPFS 
	DPF("glEvalPoint1"); 
	#endif //DODPFS
}

static void APIENTRY d3dEvalPoint2 (GLint i, GLint j)
{ 
	i;
	j;

	#ifdef DODPFS 
	DPF("glEvalPoint2"); 
	#endif //DODPFS
}

static void APIENTRY d3dFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer)
{ 
	size;
	type;
	buffer;

	#ifdef DODPFS 
	DPF("glFeedbackBuffer"); 
	#endif //DODPFS
}

static void APIENTRY d3dFinish (void)
{
	#ifdef DODPFS 
	//DPF("glFinish()"); 
	#endif //DODPFS
}

static void APIENTRY d3dFlush (void)
{ 
	#ifdef DODPFS 
	DPF("glFlush"); 
	#endif //DODPFS
}

static void APIENTRY d3dFogiv (GLenum pname, const GLint *params)
{ 
	pname;
	params;

	#ifdef DODPFS 
	DPF("glFogiv"); 
	#endif //DODPFS
}

static GLuint APIENTRY d3dGenLists (GLsizei range)
{ 
	range;

	GLuint dummy = 0;
	#ifdef DODPFS 
	DPF("glGenLists"); 
	#endif //DODPFS
	return dummy;
}

static void APIENTRY d3dGetBooleanv (GLenum pname, GLboolean *params)
{ 
	pname;
	params;
	
	#ifdef DODPFS 
	DPF("glGetBooleanv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetClipPlane (GLenum plane, GLdouble *equation)
{ 
	plane;
	equation;

	#ifdef DODPFS 
	DPF("glGetClipPlane"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{ 
	light;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetLightfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetLightiv (GLenum light, GLenum pname, GLint *params)
{ 
	light;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetLightiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetMapdv (GLenum target, GLenum query, GLdouble *v)
{ 
	target;
	query;
	v;

	#ifdef DODPFS 
	DPF("glGetMapdv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetMapfv (GLenum target, GLenum query, GLfloat *v)
{ 
	target;
	query;
	v;

	#ifdef DODPFS 
	DPF("glGetMapfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetMapiv (GLenum target, GLenum query, GLint *v)
{ 
	target;
	query;
	v;

	#ifdef DODPFS 
	DPF("glGetMapiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{ 
	face;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetMaterialfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetMaterialiv (GLenum face, GLenum pname, GLint *params)
{ 
	face;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetMaterialiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetPixelMapfv (GLenum map, GLfloat *values)
{ 
	map;
	values;

	#ifdef DODPFS 
	DPF("glGetPixelMapfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetPixelMapuiv (GLenum map, GLuint *values)
{ 
	map;
	values;

	#ifdef DODPFS 
	DPF("glGetPixelMapuiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetPixelMapusv (GLenum map, GLushort *values)
{ 
	map;
	values;

	#ifdef DODPFS 
	DPF("glGetPixelMapusv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetPointerv (GLenum pname, GLvoid* *params)
{ 
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetPointerv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetPolygonStipple (GLubyte *mask)
{ 
	mask;

	#ifdef DODPFS 
	DPF("glGetPolygonStipple"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params)
{ 
	target;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexEnvfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexGendv (GLenum coord, GLenum pname, GLdouble *params)
{ 
	coord;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexGendv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params)
{
	coord;
	pname;
	params;

	#ifdef DODPFS
	DPF("glGetTexGenfv");
	#endif //DODPFS
}

static void APIENTRY d3dGetTexGeniv (GLenum coord, GLenum pname, GLint *params)
{ 
	coord;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexGeniv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{ 
	target;
	level;
	format;
	type;
	pixels;

	#ifdef DODPFS 
	DPF("glGetTexImage"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params)
{ 
	target;
	level;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexLevelParameterfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params)
{ 
	target;
	level;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexLevelParameteriv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{ 
	target;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexParameterfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{ 
	target;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glGetTexParameteriv"); 
	#endif //DODPFS
}

static void APIENTRY d3dHint (GLenum target, GLenum mode)
{
	target;
	mode;

	#ifdef DODPFS 
	//DPF("glHint(%X, %X)",target,mode); 
	DPF("glHint");
	#endif //DODPFS
}

static void APIENTRY d3dIndexMask (GLuint mask)
{ 
	mask;

	#ifdef DODPFS 
	DPF("glIndexMask"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{ 
	type;
	stride;
	pointer;

	#ifdef DODPFS 
	DPF("glIndexPointer"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexd (GLdouble c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexd"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexdv (const GLdouble *c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexdv"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexf (GLfloat c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexf"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexfv (const GLfloat *c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexi (GLint c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexi"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexiv (const GLint *c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexs (GLshort c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexs"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexsv (const GLshort *c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexsv"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexub (GLubyte c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexub"); 
	#endif //DODPFS
}

static void APIENTRY d3dIndexubv (const GLubyte *c)
{ 
	c;

	#ifdef DODPFS 
	DPF("glIndexubv"); 
	#endif //DODPFS
}

static void APIENTRY d3dInitNames (void)
{ 
	#ifdef DODPFS 
	DPF("glInitNames"); 
	#endif //DODPFS
}

static void APIENTRY d3dInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer)
{ 
	format;
	stride;
	pointer;

	#ifdef DODPFS 
	DPF("glInterleavedArrays"); 
	#endif //DODPFS
}

static GLboolean APIENTRY d3dIsList (GLuint list)
{ 
	list;

	GLboolean dummy = FALSE;
	#ifdef DODPFS 
	DPF("glIsList"); 
	#endif //DODPFS
	return dummy;
}

static GLboolean APIENTRY d3dIsTexture (GLuint texture)
{ 
	texture;

	GLboolean dummy = FALSE;
	#ifdef DODPFS 
	DPF("glIsTexture"); 
	#endif //DODPFS
	return dummy;
}

static void APIENTRY d3dLightModelf (GLenum pname, GLfloat param)
{ 
	pname;
	param;

	#ifdef DODPFS 
	DPF("glLightModelf"); 
	#endif //DODPFS
}

static void APIENTRY d3dLightModeli (GLenum pname, GLint param)
{ 
	pname;
	param;

	#ifdef DODPFS 
	DPF("glLightModeli"); 
	#endif //DODPFS
}

static void APIENTRY d3dLightModeliv (GLenum pname, const GLint *params)
{ 
	pname;
	params;

	#ifdef DODPFS 
	DPF("glLightModeliv"); 
	#endif //DODPFS
}

static void APIENTRY d3dLighti (GLenum light, GLenum pname, GLint param)
{ 
	light;
	pname;
	param;

	#ifdef DODPFS 
	DPF("glLighti"); 
	#endif //DODPFS
}

static void APIENTRY d3dLightiv (GLenum light, GLenum pname, const GLint *params)
{ 
	light;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glLightiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dLineStipple (GLint factor, GLushort pattern)
{ 
	factor;
	pattern;

	#ifdef DODPFS 
	DPF("glLineStipple"); 
	#endif //DODPFS
}

static void APIENTRY d3dListBase (GLuint base)
{ 
	base;

	#ifdef DODPFS 
	DPF("glListBase"); 
	#endif //DODPFS
}

static void APIENTRY d3dLoadMatrixd (const GLdouble *m)
{ 
	m;

	#ifdef DODPFS 
	DPF("glLoadMatrixd"); 
	#endif //DODPFS
}

static void APIENTRY d3dLoadName (GLuint name)
{ 
	name;

	#ifdef DODPFS 
	DPF("glLoadName"); 
	#endif //DODPFS
}

static void APIENTRY d3dLogicOp (GLenum opcode)
{ 
	opcode;

	#ifdef DODPFS 
	DPF("glLogicOp"); 
	#endif //DODPFS
}

static void APIENTRY d3dMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
{ 
	target;
	u1;
	u2;
	stride;
	order;
	points;

	#ifdef DODPFS 
	DPF("glMap1d"); 
	#endif //DODPFS
}

static void APIENTRY d3dMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{ 
	target;
	u1;
	u2;
	stride;
	order;
	points;

	#ifdef DODPFS 
	DPF("glMap1f"); 
	#endif //DODPFS
}

static void APIENTRY d3dMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
{ 
	target;
	u1;
	u2;
	ustride;
	uorder;
	v1;
	v2;
	vstride;
	vorder;
	points;

	#ifdef DODPFS 
	DPF("glMap2d"); 
	#endif //DODPFS
}

static void APIENTRY d3dMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
{ 
	target;
	u1;
	u2;
	ustride;
	uorder;
	v1;
	v2;
	vstride;
	vorder;
	points;

	#ifdef DODPFS 
	DPF("glMap2f"); 
	#endif //DODPFS
}

static void APIENTRY d3dMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{ 
	un;
	u1;
	u2;

	#ifdef DODPFS 
	DPF("glMapGrid1d"); 
	#endif //DODPFS
}

static void APIENTRY d3dMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{ 
	un;
	u1;
	u2;

	#ifdef DODPFS 
	DPF("glMapGrid1f"); 
	#endif //DODPFS
}

static void APIENTRY d3dMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{ 
	un;
	u1;
	u2;
	vn;
	v1;
	v2;

	#ifdef DODPFS 
	DPF("glMapGrid2d"); 
	#endif //DODPFS
}

static void APIENTRY d3dMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{ 
	un;
	u1;
	u2;
	vn;
	v1;
	v2;

	#ifdef DODPFS 
	DPF("glMapGrid2f"); 
	#endif //DODPFS
}

static void APIENTRY d3dMaterialf (GLenum face, GLenum pname, GLfloat param)
{ 
	face;
	pname;
	param;

	#ifdef DODPFS 
	DPF("glMaterialf"); 
	#endif //DODPFS
}

static void APIENTRY d3dMateriali (GLenum face, GLenum pname, GLint param)
{ 
	face;
	pname;
	param;

	#ifdef DODPFS 
	DPF("glMateriali"); 
	#endif //DODPFS
}

static void APIENTRY d3dMaterialiv (GLenum face, GLenum pname, const GLint *params)
{ 
	face;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glMaterialiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dMultMatrixd (const GLdouble *m)
{ 
	m;

	#ifdef DODPFS 
	DPF("glMultMatrixd"); 
	#endif //DODPFS
}

static void APIENTRY d3dNewList (GLuint list, GLenum mode)
{ 
	list;
	mode;

	#ifdef DODPFS 
	DPF("glNewList"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{ 
	nx;
	ny;
	nz;

	#ifdef DODPFS 
	DPF("glNormal3b"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3bv (const GLbyte *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glNormal3bv"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{ 
	nx;
	ny;
	nz;

	#ifdef DODPFS 
	DPF("glNormal3d"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glNormal3dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{ 
	nx;
	ny;
	nz;

	#ifdef DODPFS 
	DPF("glNormal3f"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glNormal3fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3i (GLint nx, GLint ny, GLint nz)
{ 
	nx;
	ny;
	nz;

	#ifdef DODPFS 
	DPF("glNormal3i"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glNormal3iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3s (GLshort nx, GLshort ny, GLshort nz)
{ 
	nx;
	ny;
	nz;

	#ifdef DODPFS 
	DPF("glNormal3s"); 
	#endif //DODPFS
}

static void APIENTRY d3dNormal3sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glNormal3sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dPassThrough (GLfloat token)
{ 
	token;

	#ifdef DODPFS 
	DPF("glPassThrough"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values)
{ 
	map;
	mapsize;
	values;

	#ifdef DODPFS 
	DPF("glPixelMapfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values)
{ 
	map;
	mapsize;
	values;

	#ifdef DODPFS 
	DPF("glPixelMapuiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values)
{ 
	map;
	mapsize;
	values;

	#ifdef DODPFS 
	DPF("glPixelMapusv"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelStoref (GLenum pname, GLfloat param)
{ 
	pname;
	param;

	#ifdef DODPFS 
	DPF("glPixelStoref"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelStorei (GLenum pname, GLint param)
{ 
	pname;
	param;

	#ifdef DODPFS 
	DPF("glPixelStorei"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelTransferf (GLenum pname, GLfloat param)
{ 
	pname;
	param;

	#ifdef DODPFS 
	DPF("glPixelTransferf"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelTransferi (GLenum pname, GLint param)
{ 
	pname;
	param;

	#ifdef DODPFS 
	DPF("glPixelTransferi"); 
	#endif //DODPFS
}

static void APIENTRY d3dPixelZoom (GLfloat xfactor, GLfloat yfactor)
{ 
	xfactor;
	yfactor;

	#ifdef DODPFS 
	DPF("glPixelZoom"); 
	#endif //DODPFS
}

static void APIENTRY d3dPolygonStipple (const GLubyte *mask)
{ 
	mask;

	#ifdef DODPFS 
	DPF("glPolygonStipple"); 
	#endif //DODPFS
}

static void APIENTRY d3dPopAttrib (void)
{ 
	#ifdef DODPFS 
	DPF("glPopAttrib"); 
	#endif //DODPFS
}

static void APIENTRY d3dPopClientAttrib (void)
{ 
	#ifdef DODPFS 
	DPF("glPopClientAttrib"); 
	#endif //DODPFS
}

static void APIENTRY d3dPopName (void)
{ 
	#ifdef DODPFS 
	DPF("glPopName"); 
	#endif //DODPFS
}

static void APIENTRY d3dPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities)
{ 
	n;
	textures;
	priorities;

	#ifdef DODPFS 
	DPF("glPrioritizeTextures"); 
	#endif //DODPFS
}

static void APIENTRY d3dPushAttrib (GLbitfield mask)
{ 
	mask;

	#ifdef DODPFS 
	DPF("glPushAttrib"); 
	#endif //DODPFS
}

static void APIENTRY d3dPushClientAttrib (GLbitfield mask)
{ 
	mask;

	#ifdef DODPFS 
	DPF("glPushClientAttrib"); 
	#endif //DODPFS
}

static void APIENTRY d3dPushName (GLuint name)
{ 
	name;

	#ifdef DODPFS 
	DPF("glPushName"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2d (GLdouble x, GLdouble y)
{ 
	x;
	y;

	#ifdef DODPFS 
	DPF("glRasterPos2d"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos2dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2f (GLfloat x, GLfloat y)
{ 
	x;
	y;

	#ifdef DODPFS 
	DPF("glRasterPos2f"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos2fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2i (GLint x, GLint y)
{ 
	x;
	y;

	#ifdef DODPFS 
	DPF("glRasterPos2i"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos2iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2s (GLshort x, GLshort y)
{ 
	x;
	y;

	#ifdef DODPFS 
	DPF("glRasterPos2s"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos2sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos2sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glRasterPos3d"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos3dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glRasterPos3f"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos3fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3i (GLint x, GLint y, GLint z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glRasterPos3i"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos3iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3s (GLshort x, GLshort y, GLshort z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glRasterPos3s"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos3sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos3sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glRasterPos4d"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos4dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glRasterPos4f"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos4fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glRasterPos4i"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos4iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glRasterPos4s"); 
	#endif //DODPFS
}

static void APIENTRY d3dRasterPos4sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glRasterPos4sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dReadBuffer (GLenum mode)
{
	mode;

	#ifdef DODPFS 
	//DPF("glReadBuffer(%X)",mode); 
	DPF("glReadBuffer"); 
	#endif //DODPFS
}

static void APIENTRY d3dReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
	x;
	y;
	width;
	height;
	format;
	type;
	pixels;

	#ifdef DODPFS 
	DPF("glReadPixels"); 
	#endif //DODPFS
}

static void APIENTRY d3dRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{ 
	x1;
	y1;
	x2;
	y2;

	#ifdef DODPFS 
	DPF("glRectd"); 
	#endif //DODPFS
}

static void APIENTRY d3dRectdv (const GLdouble *v1, const GLdouble *v2)
{
	v1; 
	v2;

	#ifdef DODPFS 
	DPF("glRectdv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{ 
	x1;
	y1;
	x2;
	y2;

	#ifdef DODPFS 
	DPF("glRectf"); 
	#endif //DODPFS
}

static void APIENTRY d3dRectfv (const GLfloat *v1, const GLfloat *v2)
{ 
	v1;
	v2;

	#ifdef DODPFS 
	DPF("glRectfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRectiv (const GLint *v1, const GLint *v2)
{ 
	v1;
	v2;

	#ifdef DODPFS 
	DPF("glRectiv"); 
	#endif //DODPFS
}

static void APIENTRY d3dRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{ 
	x1;
	y1;
	x2;
	y2;

	#ifdef DODPFS 
	DPF("glRects"); 
	#endif //DODPFS
}

static void APIENTRY d3dRectsv (const GLshort *v1, const GLshort *v2)
{ 
	v1;
	v2;

	#ifdef DODPFS 
	DPF("glRectsv"); 
	#endif //DODPFS
}

static GLint APIENTRY d3dRenderMode (GLenum mode)
{ 
	mode;

	GLint dummy = 0;

	#ifdef DODPFS 
	DPF("glRenderMode"); 
	#endif //DODPFS

	return dummy;
}

static void APIENTRY d3dRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{ 
	angle;
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glRotated"); 
	#endif //DODPFS
}

static void APIENTRY d3dScaled (GLdouble x, GLdouble y, GLdouble z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glScaled"); 
	#endif //DODPFS
}

static void APIENTRY d3dSelectBuffer (GLsizei size, GLuint *buffer)
{ 
	size;
	buffer;

	#ifdef DODPFS 
	DPF("glSelectBuffer"); 
	#endif //DODPFS
}

static void APIENTRY d3dStencilFunc (GLenum func, GLint ref, GLuint mask)
{ 
	func;
	ref;
	mask;

	#ifdef DODPFS 
	DPF("glStencilFunc"); 
	#endif //DODPFS
}

static void APIENTRY d3dStencilMask (GLuint mask)
{ 
	mask;

	#ifdef DODPFS 
	DPF("glStencilMask"); 
	#endif //DODPFS
}

static void APIENTRY d3dStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{ 
	fail;
	zfail;
	zpass;

	#ifdef DODPFS 
	DPF("glStencilOp"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1d (GLdouble s)
{ 
	s;

	#ifdef DODPFS 
	DPF("glTexCoord1d"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord1dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1f (GLfloat s)
{ 
	s;

	#ifdef DODPFS 
	DPF("glTexCoord1f"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord1fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1i (GLint s)
{ 
	s;

	#ifdef DODPFS 
	DPF("glTexCoord1i"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord1iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1s (GLshort s)
{ 
	s;

	#ifdef DODPFS 
	DPF("glTexCoord1s"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord1sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord1sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord2d (GLdouble s, GLdouble t)
{ 
	s;
	t;

	#ifdef DODPFS 
	DPF("glTexCoord2d"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord2dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord2dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord2i (GLint s, GLint t)
{ 
	s;
	t;

	#ifdef DODPFS 
	DPF("glTexCoord2i"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord2iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord2iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord2s (GLshort s, GLshort t)
{ 
	s;
	t;

	#ifdef DODPFS 
	DPF("glTexCoord2s"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord2sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord2sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{ 
	s;
	t;
	r;

	#ifdef DODPFS 
	DPF("glTexCoord3d"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord3dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{ 
	s;
	t;
	r;

	#ifdef DODPFS 
	DPF("glTexCoord3f"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord3fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3i (GLint s, GLint t, GLint r)
{ 
	s;
	t;
	r;

	#ifdef DODPFS 
	DPF("glTexCoord3i"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord3iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3s (GLshort s, GLshort t, GLshort r)
{ 
	s;
	t;
	r;

	#ifdef DODPFS 
	DPF("glTexCoord3s"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord3sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord3sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{ 
	s;
	t;
	r;
	q;

	#ifdef DODPFS 
	DPF("glTexCoord4d"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord4dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{ 
	s;
	t;
	r;
	q;

	#ifdef DODPFS 
	DPF("glTexCoord4f"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord4fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{ 
	s;
	t;
	r;
	q;

	#ifdef DODPFS 
	DPF("glTexCoord4i"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord4iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{ 
	s;
	t;
	r;
	q;

	#ifdef DODPFS 
	DPF("glTexCoord4s"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexCoord4sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glTexCoord4sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexEnviv (GLenum target, GLenum pname, const GLint *params)
{ 
	target;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glTexEnviv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexGend (GLenum coord, GLenum pname, GLdouble param)
{ 
	coord;
	pname;
	param;

	#ifdef DODPFS 
	DPF("glTexGend"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexGendv (GLenum coord, GLenum pname, const GLdouble *params)
{ 
	coord;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glTexGendv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexGenf (GLenum coord, GLenum pname, GLfloat param)
{ 
	coord;
	pname;
	param;

	#ifdef DODPFS 
	DPF("glTexGenf"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexGeniv (GLenum coord, GLenum pname, const GLint *params)
{ 
	coord;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glTexGeniv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{ 
	target;
	level;
	internalformat;
	width;
	border;
	format;
	type;
	pixels;

	#ifdef DODPFS 
	DPF("glTexImage1D"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{ 
	target;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glTexParameterfv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{ 
	target;
	pname;
	params;

	#ifdef DODPFS 
	DPF("glTexParameteriv"); 
	#endif //DODPFS
}

static void APIENTRY d3dTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels)
{ 
	target;
	level;
	xoffset;
	width;
	format;
	type;
	pixels;

	#ifdef DODPFS 
	DPF("glTexSubImage1D"); 
	#endif //DODPFS
}

static void APIENTRY d3dTranslated (GLdouble x, GLdouble y, GLdouble z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glTranslated"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex2d (GLdouble x, GLdouble y)
{ 
	x;
	y;

	#ifdef DODPFS 
	DPF("glVertex2d"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex2dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex2dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex2fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex2fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex2iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex2iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex2s (GLshort x, GLshort y)
{ 
	x;
	y;

	#ifdef DODPFS 
	DPF("glVertex2s"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex2sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex2sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex3d (GLdouble x, GLdouble y, GLdouble z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glVertex3d"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex3dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex3dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex3i (GLint x, GLint y, GLint z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glVertex3i"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex3iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex3iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex3s (GLshort x, GLshort y, GLshort z)
{ 
	x;
	y;
	z;

	#ifdef DODPFS 
	DPF("glVertex3s"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex3sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex3sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glVertex4d"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4dv (const GLdouble *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex4dv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glVertex4f"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4fv (const GLfloat *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex4fv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4i (GLint x, GLint y, GLint z, GLint w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glVertex4i"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4iv (const GLint *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex4iv"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{ 
	x;
	y;
	z;
	w;

	#ifdef DODPFS 
	DPF("glVertex4s"); 
	#endif //DODPFS
}

static void APIENTRY d3dVertex4sv (const GLshort *v)
{ 
	v;

	#ifdef DODPFS 
	DPF("glVertex4sv"); 
	#endif //DODPFS
}

static void APIENTRY d3dFogCoordfEXT (GLfloat f)
{
	f;

	#ifdef DODPFS
	DPF("glFogCoordfEXT");
	#endif //DODPFS
}

static BOOL WINAPI wd3dCopyContext(HGLRC src, HGLRC dst, UINT mask)
{
	src;
	dst;
	mask;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglCopyContext"); 
	#endif //DODPFS
	return dummy;
}

static HGLRC WINAPI wd3dCreateLayerContext(HDC hdc, int iLayerPlane)
{
	hdc;
	iLayerPlane;

	HGLRC dummy = NULL;
	#ifdef DODPFS 
	DPF("wglCreateLayerContext"); 
	#endif //DODPFS
	return dummy;
}

static BOOL WINAPI wd3dDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd)
{
	hdc;
	iPixelFormat;
	iLayerPlane;
	nBytes;
	plpd;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglDescribeLayerPlane"); 
	#endif //DODPFS
	return dummy;
}

static BOOL WINAPI wd3dGetDefaultProcAddress()
{
	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglGetDefaultProcAddress"); 
	#endif //DODPFS
	return dummy;
}

static int WINAPI wd3dGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF *pcr)
{
	hdc;
	iLayerPlane;
	iStart;
	cEntries;
	pcr;

	int dummy = 0;
	#ifdef DODPFS 
	DPF("wglGetLayerPaletteEntries"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize)
{
	hdc;
	iLayerPlane;
	bRealize;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglRealizeLayerPalette"); 
	#endif //DODPFS
	return dummy;
}
static int WINAPI wd3dSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF *pcr)
{
	hdc;
	iLayerPlane;
	iStart;
	cEntries;
	pcr;

	int dummy = 0;
	#ifdef DODPFS 
	DPF("wglSetLayerPaletteEntries"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dShareLists(HGLRC hglrc1, HGLRC hglrc2)
{
	hglrc1;
	hglrc2;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglShareLists"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dSwapLayerBuffers(HDC hdc, UINT fuPlanes)
{
	hdc;
	fuPlanes;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglSwapLayerBuffers"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listbase)
{
	hdc;
	first;
	count;
	listbase;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglUseFontBitmapsA"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dUseFontBitmapsW()
{
	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglUseFontBitmapsW"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase, float deviation, float extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf)
{
	hdc;
	first;
	count;
	listBase;
	deviation;
	extrusion;
	format;
	lpgmf;

	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglUseFontOutlinesA"); 
	#endif //DODPFS
	return dummy;
}
static BOOL WINAPI wd3dUseFontOutlinesW()
{
	BOOL dummy = FALSE;
	#ifdef DODPFS 
	DPF("wglUseFontOutlinesW"); 
	#endif //DODPFS
	return dummy;
}


//////////////////////////////////////////// GLU Implementation ///////////////////////////////////////////////////////

static const GLubyte *APIENTRY
d3duErrorString(GLenum errorCode)
{
   /* GLU Errors */
   if (errorCode == GLU_NO_ERROR) {
      return (GLubyte *) "no error";
   }
   else if (errorCode == GLU_INVALID_ENUM) {
      return (GLubyte *) "invalid enum";
   }
   else if (errorCode == GLU_INVALID_VALUE) {
      return (GLubyte *) "invalid value";
   }
   else if (errorCode == GLU_OUT_OF_MEMORY) {
      return (GLubyte *) "out of memory";
   }
   else {
      return NULL;
   }
}

static const GLubyte *APIENTRY
d3duGetString(GLenum name)
{
   static char *extensions = "";
   static char *version = "1.0 Dynamix";

   switch (name) {
   case GLU_EXTENSIONS:
      return (GLubyte *) extensions;
   case GLU_VERSION:
      return (GLubyte *) version;
   default:
      return NULL;
   }
}

static void APIENTRY
d3duOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
   d3dOrtho(left, right, bottom, top, -1.0, 1.0);
}

static void APIENTRY
d3duPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * M_PI / 360.0);
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   d3dFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}
   
static void APIENTRY
d3duPickMatrix(GLdouble x, GLdouble y,
					GLdouble width, GLdouble height, GLint viewport[4])
{
   GLfloat m[16];
   GLfloat sx, sy;
   GLfloat tx, ty;

   sx = (GLfloat) (viewport[2] / width);
   sy = (GLfloat) (viewport[3] / height);
   tx = (GLfloat) ((viewport[2] + 2.0 * (viewport[0] - x)) / width);
   ty = (GLfloat) ((viewport[3] + 2.0 * (viewport[1] - y)) / height);

#define M(row,col)  m[col*4+row]
   M(0, 0) = sx;
   M(0, 1) = 0.0;
   M(0, 2) = 0.0;
   M(0, 3) = tx;
   M(1, 0) = 0.0;
   M(1, 1) = sy;
   M(1, 2) = 0.0;
   M(1, 3) = ty;
   M(2, 0) = 0.0;
   M(2, 1) = 0.0;
   M(2, 2) = 1.0;
   M(2, 3) = 0.0;
   M(3, 0) = 0.0;
   M(3, 1) = 0.0;
   M(3, 2) = 0.0;
   M(3, 3) = 1.0;
#undef M

   d3dMultMatrixf(m);
}

static void APIENTRY
d3duLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez,
			  GLdouble centerx, GLdouble centery, GLdouble centerz,
			  GLdouble upx, GLdouble upy, GLdouble upz)
{
   GLdouble m[16];
   GLdouble x[3], y[3], z[3];
   GLdouble mag;

   /* Make rotation matrix */

   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
   if (mag) {			/* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }

   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;

   /* X vector = Y cross Z */
   x[0] = y[1] * z[2] - y[2] * z[1];
   x[1] = -y[0] * z[2] + y[2] * z[0];
   x[2] = y[0] * z[1] - y[1] * z[0];

   /* Recompute Y = Z cross X */
   y[0] = z[1] * x[2] - z[2] * x[1];
   y[1] = -z[0] * x[2] + z[2] * x[0];
   y[2] = z[0] * x[1] - z[1] * x[0];

   /* mpichler, 19950515 */
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */

   mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }

   mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }

#define M(row,col)  m[col*4+row]
   M(0, 0) = x[0];
   M(0, 1) = x[1];
   M(0, 2) = x[2];
   M(0, 3) = 0.0;
   M(1, 0) = y[0];
   M(1, 1) = y[1];
   M(1, 2) = y[2];
   M(1, 3) = 0.0;
   M(2, 0) = z[0];
   M(2, 1) = z[1];
   M(2, 2) = z[2];
   M(2, 3) = 0.0;
   M(3, 0) = 0.0;
   M(3, 1) = 0.0;
   M(3, 2) = 0.0;
   M(3, 3) = 1.0;
#undef M
   d3dMultMatrixd(m);

   /* Translate Eye to Origin */
   d3dTranslated(-eyex, -eyey, -eyez);
}

/*
 * Transform a point (column vector) by a 4x4 matrix.  I.e.  out = m * in
 * Input:  m - the 4x4 matrix
 *         in - the 4x1 vector
 * Output:  out - the resulting 4x1 vector.
 */
static void
transform_point(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
{
#define M(row,col)  m[col*4+row]
   out[0] =
      M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
   out[1] =
      M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
   out[2] =
      M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
   out[3] =
      M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

/*
 * Perform a 4x4 matrix multiplication  (product = a x b).
 * Input:  a, b - matrices to multiply
 * Output:  product - product of a and b
 */
static void
matmul(GLdouble * product, const GLdouble * a, const GLdouble * b)
{
   /* This matmul was contributed by Thomas Malik */
   GLdouble temp[16];
   GLint i;

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define T(row,col)  temp[(col<<2)+row]

   /* i-te Zeile */
   for (i = 0; i < 4; i++) {
      T(i, 0) =
	 A(i, 0) * B(0, 0) + A(i, 1) * B(1, 0) + A(i, 2) * B(2, 0) + A(i,
								       3) *
	 B(3, 0);
      T(i, 1) =
	 A(i, 0) * B(0, 1) + A(i, 1) * B(1, 1) + A(i, 2) * B(2, 1) + A(i,
								       3) *
	 B(3, 1);
      T(i, 2) =
	 A(i, 0) * B(0, 2) + A(i, 1) * B(1, 2) + A(i, 2) * B(2, 2) + A(i,
								       3) *
	 B(3, 2);
      T(i, 3) =
	 A(i, 0) * B(0, 3) + A(i, 1) * B(1, 3) + A(i, 2) * B(2, 3) + A(i,
								       3) *
	 B(3, 3);
   }

#undef A
#undef B
#undef T
   memcpy(product, temp, 16 * sizeof(GLdouble));
}

/*
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy jle@star.be
 * Return GL_TRUE for success, GL_FALSE for failure (singular matrix)
 */
static GLboolean
invert_matrix(const GLdouble * m, GLdouble * out)
{
/* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { GLdouble *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

   GLdouble wtmp[4][8];
   GLdouble m0, m1, m2, m3, s;
   GLdouble *r0, *r1, *r2, *r3;

   r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

   r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
      r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
      r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
      r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
      r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
      r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
      r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
      r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
      r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
      r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
      r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
      r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

   /* choose pivot - or die */
   if (fabs(r3[0]) > fabs(r2[0]))
      SWAP_ROWS(r3, r2);
   if (fabs(r2[0]) > fabs(r1[0]))
      SWAP_ROWS(r2, r1);
   if (fabs(r1[0]) > fabs(r0[0]))
      SWAP_ROWS(r1, r0);
   if (0.0 == r0[0])
      return GL_FALSE;

   /* eliminate first variable     */
   m1 = r1[0] / r0[0];
   m2 = r2[0] / r0[0];
   m3 = r3[0] / r0[0];
   s = r0[1];
   r1[1] -= m1 * s;
   r2[1] -= m2 * s;
   r3[1] -= m3 * s;
   s = r0[2];
   r1[2] -= m1 * s;
   r2[2] -= m2 * s;
   r3[2] -= m3 * s;
   s = r0[3];
   r1[3] -= m1 * s;
   r2[3] -= m2 * s;
   r3[3] -= m3 * s;
   s = r0[4];
   if (s != 0.0) {
      r1[4] -= m1 * s;
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
   }
   s = r0[5];
   if (s != 0.0) {
      r1[5] -= m1 * s;
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
   }
   s = r0[6];
   if (s != 0.0) {
      r1[6] -= m1 * s;
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
   }
   s = r0[7];
   if (s != 0.0) {
      r1[7] -= m1 * s;
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
   }

   /* choose pivot - or die */
   if (fabs(r3[1]) > fabs(r2[1]))
      SWAP_ROWS(r3, r2);
   if (fabs(r2[1]) > fabs(r1[1]))
      SWAP_ROWS(r2, r1);
   if (0.0 == r1[1])
      return GL_FALSE;

   /* eliminate second variable */
   m2 = r2[1] / r1[1];
   m3 = r3[1] / r1[1];
   r2[2] -= m2 * r1[2];
   r3[2] -= m3 * r1[2];
   r2[3] -= m2 * r1[3];
   r3[3] -= m3 * r1[3];
   s = r1[4];
   if (0.0 != s) {
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
   }
   s = r1[5];
   if (0.0 != s) {
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
   }
   s = r1[6];
   if (0.0 != s) {
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
   }
   s = r1[7];
   if (0.0 != s) {
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
   }

   /* choose pivot - or die */
   if (fabs(r3[2]) > fabs(r2[2]))
      SWAP_ROWS(r3, r2);
   if (0.0 == r2[2])
      return GL_FALSE;

   /* eliminate third variable */
   m3 = r3[2] / r2[2];
   r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
      r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

   /* last check */
   if (0.0 == r3[3])
      return GL_FALSE;

   s = 1.0 / r3[3];		/* now back substitute row 3 */
   r3[4] *= s;
   r3[5] *= s;
   r3[6] *= s;
   r3[7] *= s;

   m2 = r2[3];			/* now back substitute row 2 */
   s = 1.0 / r2[2];
   r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
      r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
   m1 = r1[3];
   r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
      r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
   m0 = r0[3];
   r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
      r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

   m1 = r1[2];			/* now back substitute row 1 */
   s = 1.0 / r1[1];
   r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
      r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
   m0 = r0[2];
   r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
      r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

   m0 = r0[1];			/* now back substitute row 0 */
   s = 1.0 / r0[0];
   r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
      r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

   MAT(out, 0, 0) = r0[4];
   MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
   MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
   MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
   MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
   MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
   MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
   MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
   MAT(out, 3, 3) = r3[7];

   return GL_TRUE;

#undef MAT
#undef SWAP_ROWS
}

/* projection du point (objx,objy,obz) sur l'ecran (winx,winy,winz) */
static GLint APIENTRY
d3duProject(GLdouble objx, GLdouble objy, GLdouble objz,
				const GLdouble model[16], const GLdouble proj[16],
				const GLint viewport[4],
				GLdouble * winx, GLdouble * winy, GLdouble * winz)
{
   /* matrice de transformation */
   GLdouble in[4], out[4];

   /* initilise la matrice et le vecteur a transformer */
   in[0] = objx;
   in[1] = objy;
   in[2] = objz;
   in[3] = 1.0;
   transform_point(out, model, in);
   transform_point(in, proj, out);

   /* d'ou le resultat normalise entre -1 et 1 */
   if (in[3] == 0.0)
      return GL_FALSE;

   in[0] /= in[3];
   in[1] /= in[3];
   in[2] /= in[3];

   /* en coordonnees ecran */
   *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
   *winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;
   /* entre 0 et 1 suivant z */
   *winz = (1 + in[2]) / 2;
   return GL_TRUE;
}

/* transformation du point ecran (winx,winy,winz) en point objet */
static GLint APIENTRY
d3duUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
				  const GLdouble model[16], const GLdouble proj[16],
				  const GLint viewport[4],
				  GLdouble * objx, GLdouble * objy, GLdouble * objz)
{
   /* matrice de transformation */
   GLdouble m[16], A[16];
   GLdouble in[4], out[4];

   /* transformation coordonnees normalisees entre -1 et 1 */
   in[0] = (winx - viewport[0]) * 2 / viewport[2] - 1.0;
   in[1] = (winy - viewport[1]) * 2 / viewport[3] - 1.0;
   in[2] = 2 * winz - 1.0;
   in[3] = 1.0;

   /* calcul transformation inverse */
   matmul(A, proj, model);
   invert_matrix(A, m);

   /* d'ou les coordonnees objets */
   transform_point(out, m, in);
   if (out[3] == 0.0)
      return GL_FALSE;
   *objx = out[0] / out[3];
   *objy = out[1] / out[3];
   *objz = out[2] / out[3];
   return GL_TRUE;
}

static GLint APIENTRY
d3duScaleImage(GLenum format,
					GLsizei widthin, GLsizei heightin,
					GLenum typein, const void *datain,
					GLsizei widthout, GLsizei heightout,
					GLenum typeout, void *dataout)
{
   GLint components, i, j, k;
   GLfloat *tempin, *tempout;
   GLfloat sx, sy;
   GLint unpackrowlength, unpackalignment, unpackskiprows, unpackskippixels;
   GLint packrowlength, packalignment, packskiprows, packskippixels;
   GLint sizein, sizeout;
   GLint rowstride, rowlen;


   /* Determine number of components per pixel */
   switch (format) {
   case GL_COLOR_INDEX:
   case GL_STENCIL_INDEX:
   case GL_DEPTH_COMPONENT:
   case GL_RED:
   case GL_GREEN:
   case GL_BLUE:
   case GL_ALPHA:
   case GL_LUMINANCE:
      components = 1;
      break;
   case GL_LUMINANCE_ALPHA:
      components = 2;
      break;
   case GL_RGB:
   case GL_BGR:
      components = 3;
      break;
   case GL_RGBA:
   case GL_BGRA:
#ifdef GL_EXT_abgr
   case GL_ABGR_EXT:
#endif
      components = 4;
      break;
   default:
      return GLU_INVALID_ENUM;
   }

   /* Determine bytes per input datum */
   switch (typein) {
   case GL_UNSIGNED_BYTE:
      sizein = sizeof(GLubyte);
      break;
   case GL_BYTE:
      sizein = sizeof(GLbyte);
      break;
   case GL_UNSIGNED_SHORT:
      sizein = sizeof(GLushort);
      break;
   case GL_SHORT:
      sizein = sizeof(GLshort);
      break;
   case GL_UNSIGNED_INT:
      sizein = sizeof(GLuint);
      break;
   case GL_INT:
      sizein = sizeof(GLint);
      break;
   case GL_FLOAT:
      sizein = sizeof(GLfloat);
      break;
   case GL_BITMAP:
      /* not implemented yet */
   default:
      return GL_INVALID_ENUM;
   }

   /* Determine bytes per output datum */
   switch (typeout) {
   case GL_UNSIGNED_BYTE:
      sizeout = sizeof(GLubyte);
      break;
   case GL_BYTE:
      sizeout = sizeof(GLbyte);
      break;
   case GL_UNSIGNED_SHORT:
      sizeout = sizeof(GLushort);
      break;
   case GL_SHORT:
      sizeout = sizeof(GLshort);
      break;
   case GL_UNSIGNED_INT:
      sizeout = sizeof(GLuint);
      break;
   case GL_INT:
      sizeout = sizeof(GLint);
      break;
   case GL_FLOAT:
      sizeout = sizeof(GLfloat);
      break;
   case GL_BITMAP:
      /* not implemented yet */
   default:
      return GL_INVALID_ENUM;
   }

   /* Get glPixelStore state */
   d3dGetIntegerv(GL_UNPACK_ROW_LENGTH, &unpackrowlength);
   d3dGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackalignment);
   d3dGetIntegerv(GL_UNPACK_SKIP_ROWS, &unpackskiprows);
   d3dGetIntegerv(GL_UNPACK_SKIP_PIXELS, &unpackskippixels);
   d3dGetIntegerv(GL_PACK_ROW_LENGTH, &packrowlength);
   d3dGetIntegerv(GL_PACK_ALIGNMENT, &packalignment);
   d3dGetIntegerv(GL_PACK_SKIP_ROWS, &packskiprows);
   d3dGetIntegerv(GL_PACK_SKIP_PIXELS, &packskippixels);

   /* Allocate storage for intermediate images */
   tempin = (GLfloat *) malloc(widthin * heightin
			       * components * sizeof(GLfloat));
   if (!tempin) {
      return GLU_OUT_OF_MEMORY;
   }
   tempout = (GLfloat *) malloc(widthout * heightout
				* components * sizeof(GLfloat));
   if (!tempout) {
      free(tempin);
      return GLU_OUT_OF_MEMORY;
   }


   /*
    * Unpack the pixel data and convert to floating point
    */

   if (unpackrowlength > 0) {
      rowlen = unpackrowlength;
   }
   else {
      rowlen = widthin;
   }
   if (sizein >= unpackalignment) {
      rowstride = components * rowlen;
   }
   else {
      rowstride = unpackalignment / sizein
	 * CEILING(components * rowlen * sizein, unpackalignment);
   }

   switch (typein) {
   case GL_UNSIGNED_BYTE:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLubyte *ubptr = (GLubyte *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = (GLfloat) * ubptr++;
	 }
      }
      break;
   case GL_BYTE:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLbyte *bptr = (GLbyte *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = (GLfloat) * bptr++;
	 }
      }
      break;
   case GL_UNSIGNED_SHORT:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLushort *usptr = (GLushort *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = (GLfloat) * usptr++;
	 }
      }
      break;
   case GL_SHORT:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLshort *sptr = (GLshort *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = (GLfloat) * sptr++;
	 }
      }
      break;
   case GL_UNSIGNED_INT:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLuint *uiptr = (GLuint *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = (GLfloat) * uiptr++;
	 }
      }
      break;
   case GL_INT:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLint *iptr = (GLint *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = (GLfloat) * iptr++;
	 }
      }
      break;
   case GL_FLOAT:
      k = 0;
      for (i = 0; i < heightin; i++) {
	 GLfloat *fptr = (GLfloat *) datain
	    + i * rowstride
	    + unpackskiprows * rowstride + unpackskippixels * components;
	 for (j = 0; j < widthin * components; j++) {
	    dummy(j, k);
	    tempin[k++] = *fptr++;
	 }
      }
      break;
   default:
      return GLU_INVALID_ENUM;
   }


   /*
    * Scale the image!
    */

   if (widthout > 1)
      sx = (GLfloat) (widthin - 1) / (GLfloat) (widthout - 1);
   else
      sx = (GLfloat) (widthin - 1);
   if (heightout > 1)
      sy = (GLfloat) (heightin - 1) / (GLfloat) (heightout - 1);
   else
      sy = (GLfloat) (heightin - 1);

/*#define POINT_SAMPLE*/
#ifdef POINT_SAMPLE
   for (i = 0; i < heightout; i++) {
      GLint ii = i * sy;
      for (j = 0; j < widthout; j++) {
	 GLint jj = j * sx;

	 GLfloat *src = tempin + (ii * widthin + jj) * components;
	 GLfloat *dst = tempout + (i * widthout + j) * components;

	 for (k = 0; k < components; k++) {
	    *dst++ = *src++;
	 }
      }
   }
#else
   if (sx < 1.0 && sy < 1.0) {
      /* magnify both width and height:  use weighted sample of 4 pixels */
      GLint i0, i1, j0, j1;
      GLfloat alpha, beta;
      GLfloat *src00, *src01, *src10, *src11;
      GLfloat s1, s2;
      GLfloat *dst;

      for (i = 0; i < heightout; i++) {
	 i0 = (GLint) (i * sy);
	 i1 = i0 + 1;
	 if (i1 >= heightin)
	    i1 = heightin - 1;
/*	 i1 = (i+1) * sy - EPSILON;*/
	 alpha = i * sy - i0;
	 for (j = 0; j < widthout; j++) {
	    j0 = (GLint) (j * sx);
	    j1 = j0 + 1;
	    if (j1 >= widthin)
	       j1 = widthin - 1;
/*	    j1 = (j+1) * sx - EPSILON; */
	    beta = j * sx - j0;

	    /* compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
	    src00 = tempin + (i0 * widthin + j0) * components;
	    src01 = tempin + (i0 * widthin + j1) * components;
	    src10 = tempin + (i1 * widthin + j0) * components;
	    src11 = tempin + (i1 * widthin + j1) * components;

	    dst = tempout + (i * widthout + j) * components;

	    for (k = 0; k < components; k++) {
	       s1 = (GLfloat) (*src00++ * (1.0 - beta) + *src01++ * beta);
	       s2 = (GLfloat) (*src10++ * (1.0 - beta) + *src11++ * beta);
	       *dst++ = (GLfloat) (s1 * (1.0 - alpha) + s2 * alpha);
	    }
	 }
      }
   }
   else {
      /* shrink width and/or height:  use an unweighted box filter */
      GLint i0, i1;
      GLint j0, j1;
      GLint ii, jj;
      GLfloat sum, *dst;

      for (i = 0; i < heightout; i++) {
	 i0 = (GLint) (i * sy);
	 i1 = i0 + 1;
	 if (i1 >= heightin)
	    i1 = heightin - 1;
/*	 i1 = (i+1) * sy - EPSILON; */
	 for (j = 0; j < widthout; j++) {
	    j0 = (GLint) (j * sx);
	    j1 = j0 + 1;
	    if (j1 >= widthin)
	       j1 = widthin - 1;
/*	    j1 = (j+1) * sx - EPSILON; */

	    dst = tempout + (i * widthout + j) * components;

	    /* compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
	    for (k = 0; k < components; k++) {
	       sum = 0.0;
	       for (ii = i0; ii <= i1; ii++) {
		  for (jj = j0; jj <= j1; jj++) {
		     sum += *(tempin + (ii * widthin + jj) * components + k);
		  }
	       }
	       sum /= (j1 - j0 + 1) * (i1 - i0 + 1);
	       *dst++ = sum;
	    }
	 }
      }
   }
#endif


   /*
    * Return output image
    */

   if (packrowlength > 0) {
      rowlen = packrowlength;
   }
   else {
      rowlen = widthout;
   }
   if (sizeout >= packalignment) {
      rowstride = components * rowlen;
   }
   else {
      rowstride = packalignment / sizeout
	 * CEILING(components * rowlen * sizeout, packalignment);
   }

   switch (typeout) {
   case GL_UNSIGNED_BYTE:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLubyte *ubptr = (GLubyte *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *ubptr++ = (GLubyte) tempout[k++];
	 }
      }
      break;
   case GL_BYTE:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLbyte *bptr = (GLbyte *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *bptr++ = (GLbyte) tempout[k++];
	 }
      }
      break;
   case GL_UNSIGNED_SHORT:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLushort *usptr = (GLushort *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *usptr++ = (GLushort) tempout[k++];
	 }
      }
      break;
   case GL_SHORT:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLshort *sptr = (GLshort *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *sptr++ = (GLshort) tempout[k++];
	 }
      }
      break;
   case GL_UNSIGNED_INT:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLuint *uiptr = (GLuint *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *uiptr++ = (GLuint) tempout[k++];
	 }
      }
      break;
   case GL_INT:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLint *iptr = (GLint *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *iptr++ = (GLint) tempout[k++];
	 }
      }
      break;
   case GL_FLOAT:
      k = 0;
      for (i = 0; i < heightout; i++) {
	 GLfloat *fptr = (GLfloat *) dataout
	    + i * rowstride
	    + packskiprows * rowstride + packskippixels * components;
	 for (j = 0; j < widthout * components; j++) {
	    dummy(j, k + i);
	    *fptr++ = tempout[k++];
	 }
      }
      break;
   default:
      return GLU_INVALID_ENUM;
   }


   /* free temporary image storage */
   free(tempin);
   free(tempout);

   return 0;
}

/*
 * Return the largest k such that 2^k <= n.
 */
static GLint
ilog2(GLint n)
{
   GLint k;

   if (n <= 0)
      return 0;
   for (k = 0; n >>= 1; k++) {}
   return k;
}

/*
 * Find the value nearest to n which is also a power of two.
 */
static GLint
round2(GLint n)
{
   GLint m;

   for (m = 1; m < n; m *= 2) {}

   /* m>=n */
   if (m - n <= n - m / 2) {
      return m;
   }
   else {
      return m / 2;
   }
}

/*
 * WARNING: This function isn't finished and has never been tested!!!!
 */
static GLint APIENTRY
d3duBuild1DMipmaps(GLenum target, GLint components,
						 GLsizei width, GLenum format, GLenum type, const void *data)
{
	target;

   GLubyte *texture;
   GLint levels, max_levels;
   GLint new_width, max_width;
   GLint i, j, k, l;

   if (width < 1)
      return GLU_INVALID_VALUE;

   d3dGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_width);
   max_levels = ilog2(max_width) + 1;

   /* Compute how many mipmap images to make */
   levels = ilog2(width) + 1;
   if (levels > max_levels) {
      levels = max_levels;
   }

   new_width = 1 << (levels - 1);

   texture = (GLubyte *) malloc(new_width * components);
   if (!texture) {
      return GLU_OUT_OF_MEMORY;
   }

   if (width != new_width) {
      /* initial rescaling */
      switch (type) {
      case GL_UNSIGNED_BYTE:
	 {
	    GLubyte *ub_data = (GLubyte *) data;
	    for (i = 0; i < new_width; i++) {
	       j = i * width / new_width;
	       for (k = 0; k < components; k++) {
		  texture[i * components + k] = ub_data[j * components + k];
	       }
	    }
	 }
	 break;
      default:
	 /* Not implemented */
	 return GLU_ERROR;
      }
   }

   /* generate and load mipmap images */
   for (l = 0; l < levels; l++) {
      d3dTexImage1D(GL_TEXTURE_1D, l, components, new_width, 0,
		   format, GL_UNSIGNED_BYTE, texture);

      /* Scale image down to 1/2 size */
      new_width = new_width / 2;
      for (i = 0; i < new_width; i++) {
	 for (k = 0; k < components; k++) {
	    GLint sample1, sample2;
	    sample1 = (GLint) texture[i * 2 * components + k];
	    sample2 = (GLint) texture[(i * 2 + 1) * components + k];
	    texture[i * components + k] = (GLubyte) ((sample1 + sample2) / 2);
	 }
      }
   }

   free(texture);

   return 0;
}

/*
 * Given an pixel format and datatype, return the number of bytes to
 * store one pixel.
 */
static GLint
bytes_per_pixel(GLenum format, GLenum type)
{
   GLint n, m;

   switch (format) {
   case GL_COLOR_INDEX:
   case GL_STENCIL_INDEX:
   case GL_DEPTH_COMPONENT:
   case GL_RED:
   case GL_GREEN:
   case GL_BLUE:
   case GL_ALPHA:
   case GL_LUMINANCE:
      n = 1;
      break;
   case GL_LUMINANCE_ALPHA:
      n = 2;
      break;
   case GL_RGB:
   case GL_BGR:
      n = 3;
      break;
   case GL_RGBA:
   case GL_BGRA:
#ifdef GL_EXT_abgr
   case GL_ABGR_EXT:
#endif
      n = 4;
      break;
   default:
      n = 0;
   }

   switch (type) {
   case GL_UNSIGNED_BYTE:
      m = sizeof(GLubyte);
      break;
   case GL_BYTE:
      m = sizeof(GLbyte);
      break;
   case GL_BITMAP:
      m = 1;
      break;
   case GL_UNSIGNED_SHORT:
      m = sizeof(GLushort);
      break;
   case GL_SHORT:
      m = sizeof(GLshort);
      break;
   case GL_UNSIGNED_INT:
      m = sizeof(GLuint);
      break;
   case GL_INT:
      m = sizeof(GLint);
      break;
   case GL_FLOAT:
      m = sizeof(GLfloat);
      break;
   default:
      m = 0;
   }

   return n * m;
}

static GLint APIENTRY
d3duBuild2DMipmaps(GLenum target, GLint components,
						 GLsizei width, GLsizei height, GLenum format,
						 GLenum type, const void *data)
{
   GLint w, h, maxsize;
   void *image, *newimage;
   GLint neww, newh, level, bpp;
   int error;
   GLboolean done;
   GLint retval = 0;
   GLint unpackrowlength, unpackalignment, unpackskiprows, unpackskippixels;
   GLint packrowlength, packalignment, packskiprows, packskippixels;

   if (width < 1 || height < 1)
      return GLU_INVALID_VALUE;

   d3dGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxsize);

   w = round2(width);
   if (w > maxsize) {
      w = maxsize;
   }
   h = round2(height);
   if (h > maxsize) {
      h = maxsize;
   }

   bpp = bytes_per_pixel(format, type);
   if (bpp == 0) {
      /* probably a bad format or type enum */
      return GLU_INVALID_ENUM;
   }

   /* Get current glPixelStore values */
   d3dGetIntegerv(GL_UNPACK_ROW_LENGTH, &unpackrowlength);
   d3dGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackalignment);
   d3dGetIntegerv(GL_UNPACK_SKIP_ROWS, &unpackskiprows);
   d3dGetIntegerv(GL_UNPACK_SKIP_PIXELS, &unpackskippixels);
   d3dGetIntegerv(GL_PACK_ROW_LENGTH, &packrowlength);
   d3dGetIntegerv(GL_PACK_ALIGNMENT, &packalignment);
   d3dGetIntegerv(GL_PACK_SKIP_ROWS, &packskiprows);
   d3dGetIntegerv(GL_PACK_SKIP_PIXELS, &packskippixels);

   /* set pixel packing */
   d3dPixelStorei(GL_PACK_ROW_LENGTH, 0);
   d3dPixelStorei(GL_PACK_ALIGNMENT, 1);
   d3dPixelStorei(GL_PACK_SKIP_ROWS, 0);
   d3dPixelStorei(GL_PACK_SKIP_PIXELS, 0);

   done = GL_FALSE;

   if (w != width || h != height) {
      /* must rescale image to get "top" mipmap texture image */
      image = malloc((w + 4) * h * bpp);
      if (!image) {
	 return GLU_OUT_OF_MEMORY;
      }
      error = d3duScaleImage(format, width, height, type, data,
			    w, h, type, image);
      if (error) {
	 retval = error;
	 done = GL_TRUE;
      }
   }
   else {
      image = (void *) data;
   }

   level = 0;
   while (!done) {
      if (image != data) {
	 /* set pixel unpacking */
	 d3dPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	 d3dPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	 d3dPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	 d3dPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
      }

      d3dTexImage2D(target, level, components, w, h, 0, format, type, image);

      if (w == 1 && h == 1)
	 break;

      neww = (w < 2) ? 1 : w / 2;
      newh = (h < 2) ? 1 : h / 2;
      newimage = malloc((neww + 4) * newh * bpp);
      if (!newimage) {
	 return GLU_OUT_OF_MEMORY;
      }

      error = d3duScaleImage(format, w, h, type, image,
			    neww, newh, type, newimage);
      if (error) {
	 retval = error;
	 done = GL_TRUE;
      }

      if (image != data) {
	 free(image);
      }
      image = newimage;

      w = neww;
      h = newh;
      level++;
   }

   if (image != data) {
      free(image);
   }

   /* Restore original glPixelStore state */
   d3dPixelStorei(GL_UNPACK_ROW_LENGTH, unpackrowlength);
   d3dPixelStorei(GL_UNPACK_ALIGNMENT, unpackalignment);
   d3dPixelStorei(GL_UNPACK_SKIP_ROWS, unpackskiprows);
   d3dPixelStorei(GL_UNPACK_SKIP_PIXELS, unpackskippixels);
   d3dPixelStorei(GL_PACK_ROW_LENGTH, packrowlength);
   d3dPixelStorei(GL_PACK_ALIGNMENT, packalignment);
   d3dPixelStorei(GL_PACK_SKIP_ROWS, packskiprows);
   d3dPixelStorei(GL_PACK_SKIP_PIXELS, packskippixels);

   return retval;
}


/*
** D3DGL_Init
**
** This is responsible for binding our gl function pointers to 
** the appropriate D3DGL stuff.
** 
*/

#define GPA_D3D( a ) GetProcAddress( winState.hinstD3D, a )

//--------------------------------------
bool D3DGL_Init()
{
   // GLU Functions
   gluErrorString = d3duErrorString;
   gluGetString = d3duGetString;
   gluOrtho2D = d3duOrtho2D;
   gluPerspective = d3duPerspective;
   gluPickMatrix = d3duPickMatrix;
   gluLookAt = d3duLookAt;
   gluProject = d3duProject;
   gluUnProject = d3duUnProject;
   gluScaleImage = d3duScaleImage;
   gluBuild1DMipmaps = d3duBuild1DMipmaps;
   gluBuild2DMipmaps = d3duBuild2DMipmaps;

   // GL Functions
   glAccum = d3dAccum;
   glAlphaFunc = d3dAlphaFunc;
   glAreTexturesResident = d3dAreTexturesResident;
   glArrayElement = d3dArrayElement;
   glBegin = d3dBegin;
   glBindTexture = d3dBindTexture;
   glBitmap = d3dBitmap;
   glBlendFunc = d3dBlendFunc;
   glCallList = d3dCallList;
   glCallLists = d3dCallLists;
   glClear = d3dClear;
   glClearAccum = d3dClearAccum;
   glClearColor = d3dClearColor;
   glClearDepth = d3dClearDepth;
   glClearIndex = d3dClearIndex;
   glClearStencil = d3dClearStencil;
   glClipPlane = d3dClipPlane;
   glColor3b = d3dColor3b;
   glColor3bv = d3dColor3bv;
   glColor3d = d3dColor3d;
   glColor3dv = d3dColor3dv;
   glColor3f = d3dColor3f;
   glColor3fv = d3dColor3fv;
   glColor3i = d3dColor3i;
   glColor3iv = d3dColor3iv;
   glColor3s = d3dColor3s;
   glColor3sv = d3dColor3sv;
   glColor3ub = d3dColor3ub;
   glColor3ubv = d3dColor3ubv;
   glColor3ui = d3dColor3ui;
   glColor3uiv = d3dColor3uiv;
   glColor3us = d3dColor3us;
   glColor3usv = d3dColor3usv;
   glColor4b = d3dColor4b;
   glColor4bv = d3dColor4bv;
   glColor4d = d3dColor4d;
   glColor4dv = d3dColor4dv;
   glColor4f = d3dColor4f;
   glColor4fv = d3dColor4fv;
   glColor4i = d3dColor4i;
   glColor4iv = d3dColor4iv;
   glColor4s = d3dColor4s;
   glColor4sv = d3dColor4sv;
   glColor4ub = d3dColor4ub;
   glColor4ubv = d3dColor4ubv;
   glColor4ui = d3dColor4ui;
   glColor4uiv = d3dColor4uiv;
   glColor4us = d3dColor4us;
   glColor4usv = d3dColor4usv;
   glColorMask = d3dColorMask;
   glColorMaterial = d3dColorMaterial;
   glColorPointer = d3dColorPointer;
   glCopyPixels = d3dCopyPixels;
   glCopyTexImage1D = d3dCopyTexImage1D;
   glCopyTexImage2D = d3dCopyTexImage2D;
   glCopyTexSubImage1D = d3dCopyTexSubImage1D;
   glCopyTexSubImage2D = d3dCopyTexSubImage2D;
   glCullFace = d3dCullFace;
   glDeleteLists = d3dDeleteLists;
   glDeleteTextures = d3dDeleteTextures;
   glDepthFunc = d3dDepthFunc;
   glDepthMask = d3dDepthMask;
   glDepthRange = d3dDepthRange;
   glDisable = d3dDisable;
   glDisableClientState = d3dDisableClientState;
   glDrawArrays = d3dDrawArrays;
   glDrawBuffer = d3dDrawBuffer;
   glDrawElements = d3dDrawElements;
   glDrawPixels = d3dDrawPixels;
   glEdgeFlag = d3dEdgeFlag;
   glEdgeFlagPointer = d3dEdgeFlagPointer;
   glEdgeFlagv = d3dEdgeFlagv;
   glEnable = d3dEnable;
   glEnableClientState = d3dEnableClientState;
   glEnd = d3dEnd;
   glEndList = d3dEndList;
   glEvalCoord1d = d3dEvalCoord1d;
   glEvalCoord1dv = d3dEvalCoord1dv;
   glEvalCoord1f = d3dEvalCoord1f;
   glEvalCoord1fv = d3dEvalCoord1fv;
   glEvalCoord2d = d3dEvalCoord2d;
   glEvalCoord2dv = d3dEvalCoord2dv;
   glEvalCoord2f = d3dEvalCoord2f;
   glEvalCoord2fv = d3dEvalCoord2fv;
   glEvalMesh1 = d3dEvalMesh1;
   glEvalMesh2 = d3dEvalMesh2;
   glEvalPoint1 = d3dEvalPoint1;
   glEvalPoint2 = d3dEvalPoint2;
   glFeedbackBuffer = d3dFeedbackBuffer;
   glFinish = d3dFinish;
   glFlush = d3dFlush;
   glFogf = d3dFogf;
   glFogfv = d3dFogfv;
   glFogi = d3dFogi;
   glFogiv = d3dFogiv;
   glFrontFace = d3dFrontFace;
   glFrustum = d3dFrustum;
   glGenLists = d3dGenLists;
   glGenTextures = d3dGenTextures;
   glGetBooleanv = d3dGetBooleanv;
   glGetClipPlane = d3dGetClipPlane;
   glGetDoublev = d3dGetDoublev;
   glGetError = d3dGetError;
   glGetFloatv = d3dGetFloatv;
   glGetIntegerv = d3dGetIntegerv;
   glGetLightfv = d3dGetLightfv;
   glGetLightiv = d3dGetLightiv;
   glGetMapdv = d3dGetMapdv;
   glGetMapfv = d3dGetMapfv;
   glGetMapiv = d3dGetMapiv;
   glGetMaterialfv  = d3dGetMaterialfv;
   glGetMaterialiv  = d3dGetMaterialiv;
   glGetPixelMapfv  = d3dGetPixelMapfv;
   glGetPixelMapuiv = d3dGetPixelMapuiv;
   glGetPixelMapusv = d3dGetPixelMapusv;
   glGetPointerv = d3dGetPointerv;
   glGetPolygonStipple = d3dGetPolygonStipple;
   glGetString = d3dGetString;
   glGetTexEnvfv = d3dGetTexEnvfv;
   glGetTexEnviv = d3dGetTexEnviv;
   glGetTexGendv = d3dGetTexGendv;
   glGetTexGenfv = d3dGetTexGenfv;
   glGetTexGeniv = d3dGetTexGeniv;
   glGetTexImage = d3dGetTexImage;
   glGetTexLevelParameterfv = d3dGetTexLevelParameterfv;
   glGetTexLevelParameteriv = d3dGetTexLevelParameteriv;
   glGetTexParameterfv = d3dGetTexParameterfv;
   glGetTexParameteriv = d3dGetTexParameteriv;
   glHint = d3dHint;
   glIndexMask = d3dIndexMask;
   glIndexPointer = d3dIndexPointer;
   glIndexd = d3dIndexd;
   glIndexdv = d3dIndexdv;
   glIndexf = d3dIndexf;
   glIndexfv = d3dIndexfv;
   glIndexi = d3dIndexi;
   glIndexiv = d3dIndexiv;
   glIndexs = d3dIndexs;
   glIndexsv = d3dIndexsv;
   glIndexub = d3dIndexub;
   glIndexubv = d3dIndexubv;
   glInitNames = d3dInitNames;
   glInterleavedArrays = d3dInterleavedArrays;
   glIsEnabled = d3dIsEnabled;
   glIsList = d3dIsList;
   glIsTexture = d3dIsTexture;
   glLightModelf = d3dLightModelf;
   glLightModelfv = d3dLightModelfv;
   glLightModeli = d3dLightModeli;
   glLightModeliv = d3dLightModeliv;
   glLightf = d3dLightf;
   glLightfv = d3dLightfv;
   glLighti = d3dLighti;
   glLightiv = d3dLightiv;
   glLineStipple  = d3dLineStipple;
   glLineWidth = d3dLineWidth;
   glListBase = d3dListBase;
   glLoadIdentity = d3dLoadIdentity;
   glLoadMatrixd  = d3dLoadMatrixd;
   glLoadMatrixf  = d3dLoadMatrixf;
   glLoadName = d3dLoadName;
   glLogicOp = d3dLogicOp;
   glMap1d = d3dMap1d;
   glMap1f = d3dMap1f;
   glMap2d = d3dMap2d;
   glMap2f = d3dMap2f;
   glMapGrid1d = d3dMapGrid1d;
   glMapGrid1f = d3dMapGrid1f;
   glMapGrid2d = d3dMapGrid2d;
   glMapGrid2f = d3dMapGrid2f;
   glMaterialf = d3dMaterialf;
   glMaterialfv = d3dMaterialfv;
   glMateriali = d3dMateriali;
   glMaterialiv = d3dMaterialiv;
   glMatrixMode = d3dMatrixMode;
   glMultMatrixd  = d3dMultMatrixd;
   glMultMatrixf  = d3dMultMatrixf;
   glNewList = d3dNewList;
   glNormal3b = d3dNormal3b;
   glNormal3bv = d3dNormal3bv;
   glNormal3d = d3dNormal3d;
   glNormal3dv = d3dNormal3dv;
   glNormal3f = d3dNormal3f;
   glNormal3fv = d3dNormal3fv;
   glNormal3i = d3dNormal3i;
   glNormal3iv = d3dNormal3iv;
   glNormal3s = d3dNormal3s;
   glNormal3sv = d3dNormal3sv;
   glNormalPointer = d3dNormalPointer;
   glOrtho = d3dOrtho;
   glPassThrough = d3dPassThrough;
   glPixelMapfv = d3dPixelMapfv;
   glPixelMapuiv = d3dPixelMapuiv;
   glPixelMapusv = d3dPixelMapusv;
   glPixelStoref = d3dPixelStoref;
   glPixelStorei = d3dPixelStorei;
   glPixelTransferf = d3dPixelTransferf;
   glPixelTransferi = d3dPixelTransferi;
   glPixelZoom = d3dPixelZoom;
   glPointSize = d3dPointSize;
   glPolygonMode = d3dPolygonMode;
   glPolygonOffset = d3dPolygonOffset;
   glPolygonStipple = d3dPolygonStipple;
   glPopAttrib = d3dPopAttrib;
   glPopClientAttrib = d3dPopClientAttrib;
   glPopMatrix = d3dPopMatrix;
   glPopName = d3dPopName;
   glPrioritizeTextures = d3dPrioritizeTextures;
   glPushAttrib = d3dPushAttrib;
   glPushClientAttrib = d3dPushClientAttrib;
   glPushMatrix = d3dPushMatrix;
   glPushName = d3dPushName;
   glRasterPos2d = d3dRasterPos2d;
   glRasterPos2dv = d3dRasterPos2dv;
   glRasterPos2f = d3dRasterPos2f;
   glRasterPos2fv = d3dRasterPos2fv;
   glRasterPos2i = d3dRasterPos2i;
   glRasterPos2iv = d3dRasterPos2iv;
   glRasterPos2s = d3dRasterPos2s;
   glRasterPos2sv = d3dRasterPos2sv;
   glRasterPos3d = d3dRasterPos3d;
   glRasterPos3dv = d3dRasterPos3dv;
   glRasterPos3f = d3dRasterPos3f;
   glRasterPos3fv = d3dRasterPos3fv;
   glRasterPos3i = d3dRasterPos3i;
   glRasterPos3iv = d3dRasterPos3iv;
   glRasterPos3s = d3dRasterPos3s;
   glRasterPos3sv = d3dRasterPos3sv;
   glRasterPos4d = d3dRasterPos4d;
   glRasterPos4dv = d3dRasterPos4dv;
   glRasterPos4f = d3dRasterPos4f;
   glRasterPos4fv = d3dRasterPos4fv;
   glRasterPos4i = d3dRasterPos4i;
   glRasterPos4iv = d3dRasterPos4iv;
   glRasterPos4s = d3dRasterPos4s;
   glRasterPos4sv = d3dRasterPos4sv;
   glReadBuffer = d3dReadBuffer;
   glReadPixels = d3dReadPixels;
   glRectd = d3dRectd;
   glRectdv = d3dRectdv;
   glRectf = d3dRectf;
   glRectfv = d3dRectfv;
   glRecti = d3dRecti;
   glRectiv = d3dRectiv;
   glRects = d3dRects;
   glRectsv = d3dRectsv;
   glRenderMode = d3dRenderMode;
   glRotated = d3dRotated;
   glRotatef = d3dRotatef;
   glScaled = d3dScaled;
   glScalef = d3dScalef;
   glScissor = d3dScissor;
   glSelectBuffer = d3dSelectBuffer;
   glShadeModel = d3dShadeModel;
   glStencilFunc = d3dStencilFunc;
   glStencilMask = d3dStencilMask;
   glStencilOp = d3dStencilOp;
   glTexCoord1d = d3dTexCoord1d;
   glTexCoord1dv = d3dTexCoord1dv;
   glTexCoord1f = d3dTexCoord1f;
   glTexCoord1fv = d3dTexCoord1fv;
   glTexCoord1i = d3dTexCoord1i;
   glTexCoord1iv = d3dTexCoord1iv;
   glTexCoord1s = d3dTexCoord1s;
   glTexCoord1sv = d3dTexCoord1sv;
   glTexCoord2d = d3dTexCoord2d;
   glTexCoord2dv = d3dTexCoord2dv;
   glTexCoord2f = d3dTexCoord2f;
   glTexCoord2fv = d3dTexCoord2fv;
   glTexCoord2i = d3dTexCoord2i;
   glTexCoord2iv = d3dTexCoord2iv;
   glTexCoord2s = d3dTexCoord2s;
   glTexCoord2sv = d3dTexCoord2sv;
   glTexCoord3d = d3dTexCoord3d;
   glTexCoord3dv = d3dTexCoord3dv;
   glTexCoord3f = d3dTexCoord3f;
   glTexCoord3fv = d3dTexCoord3fv;
   glTexCoord3i = d3dTexCoord3i;
   glTexCoord3iv = d3dTexCoord3iv;
   glTexCoord3s = d3dTexCoord3s;
   glTexCoord3sv = d3dTexCoord3sv;
   glTexCoord4d = d3dTexCoord4d;
   glTexCoord4dv = d3dTexCoord4dv;
   glTexCoord4f = d3dTexCoord4f;
   glTexCoord4fv = d3dTexCoord4fv;
   glTexCoord4i = d3dTexCoord4i;
   glTexCoord4iv = d3dTexCoord4iv;
   glTexCoord4s = d3dTexCoord4s;
   glTexCoord4sv = d3dTexCoord4sv;
   glTexCoordPointer = d3dTexCoordPointer;
   glTexEnvf = d3dTexEnvf;
   glTexEnvfv = d3dTexEnvfv;
   glTexEnvi = d3dTexEnvi;										 
   glTexEnviv = d3dTexEnviv;
   glTexGend = d3dTexGend;
   glTexGendv = d3dTexGendv;
   glTexGenf = d3dTexGenf;
   glTexGenfv = d3dTexGenfv;
   glTexGeni = d3dTexGeni;
   glTexGeniv = d3dTexGeniv;
   glTexImage1D = d3dTexImage1D;
   glTexImage2D = d3dTexImage2D;
   glTexParameterf  = d3dTexParameterf;
   glTexParameterfv = d3dTexParameterfv;
   glTexParameteri  = d3dTexParameteri;
   glTexParameteriv = d3dTexParameteriv;
   glTexSubImage1D  = d3dTexSubImage1D;
   glTexSubImage2D  = d3dTexSubImage2D;
   glTranslated = d3dTranslated;
   glTranslatef = d3dTranslatef;
   glVertex2d = d3dVertex2d;
   glVertex2dv = d3dVertex2dv;
   glVertex2f = d3dVertex2f;
   glVertex2fv = d3dVertex2fv;
   glVertex2i = d3dVertex2i;
   glVertex2iv = d3dVertex2iv;

   glVertex2s = d3dVertex2s;
   glVertex2sv = d3dVertex2sv;
   glVertex3d = d3dVertex3d;
   glVertex3dv = d3dVertex3dv;
   glVertex3f = d3dVertex3f;
   glVertex3fv = d3dVertex3fv;
   glVertex3i = d3dVertex3i;
   glVertex3iv = d3dVertex3iv;
   glVertex3s = d3dVertex3s;
   glVertex3sv = d3dVertex3sv;
   glVertex4d = d3dVertex4d;
   glVertex4dv = d3dVertex4dv;
   glVertex4f = d3dVertex4f;
   glVertex4fv = d3dVertex4fv;
   glVertex4i = d3dVertex4i;
   glVertex4iv = d3dVertex4iv;
   glVertex4s = d3dVertex4s;
   glVertex4sv = d3dVertex4sv;
   glVertexPointer = d3dVertexPointer;
   glViewport = d3dViewport;

   qwglCopyContext = wd3dCopyContext;
   qwglCreateContext = wd3dCreateContext;
   qwglCreateLayerContext = wd3dCreateLayerContext;
   qwglDeleteContext = wd3dDeleteContext;
   qwglDescribeLayerPlane = wd3dDescribeLayerPlane;
   qwglGetCurrentContext = wd3dGetCurrentContext;
   qwglGetCurrentDC = wd3dGetCurrentDC;
   qwglGetLayerPaletteEntries = wd3dGetLayerPaletteEntries;
   qwglGetProcAddress = wd3dGetProcAddress;
   qwglMakeCurrent = wd3dMakeCurrent;
   qwglRealizeLayerPalette = wd3dRealizeLayerPalette;
   qwglSetLayerPaletteEntries = wd3dSetLayerPaletteEntries;
   qwglShareLists = wd3dShareLists;
   qwglSwapLayerBuffers = wd3dSwapLayerBuffers;
   qwglUseFontBitmaps = wd3dUseFontBitmapsA;
   qwglUseFontOutlines = wd3dUseFontOutlinesA;

   qwglChoosePixelFormat = wd3dChoosePixelFormat;
   qwglDescribePixelFormat = wd3dDescribePixelFormat;
   qwglGetPixelFormat = wd3dGetPixelFormat;
   qwglSetPixelFormat = wd3dSetPixelFormat;
   qwglSwapBuffers = wd3dSwapBuffers;

   qwglSwapIntervalEXT = 0;
   
   return true;
}

bool D3DGL_EXT_Init()
{
   // Load extensions...
   //
   const char* pExtString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

   gGLState.primMode = 0;

   // EXT_compiled_vertex_array
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_compiled_vertex_array") != NULL) {
      glLockArraysEXT   = d3dLockArraysEXT;
      glUnlockArraysEXT = d3dUnlockArraysEXT;
      gGLState.suppLockedArrays = true;
   } else {
      glLockArraysEXT   = NULL;
      glUnlockArraysEXT = NULL;
      gGLState.suppLockedArrays = false;
   }

   // ARB_multitexture
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_multitexture") != NULL) {
      glActiveTextureARB       = d3dActiveTextureARB;
      glClientActiveTextureARB = d3dClientActiveTextureARB;
      glMultiTexCoord2fARB     = d3dMultiTexCoord2fARB;
      glMultiTexCoord2fvARB    = d3dMultiTexCoord2fvARB;
      gGLState.suppARBMultitexture = true;
   } else {
      glActiveTextureARB       = NULL;
      glClientActiveTextureARB = NULL;
      glMultiTexCoord2fARB     = NULL;
      glMultiTexCoord2fvARB    = NULL;
      gGLState.suppARBMultitexture = false;
   }

   // NV_vertex_array_range
   glVertexArrayRangeNV      = NULL;
   glFlushVertexArrayRangeNV = NULL;
   wglAllocateMemoryNV       = NULL;
   wglFreeMemoryNV           = NULL;
   gGLState.suppVertexArrayRange = false;

   // EXT_fog_coord
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_fog_coord") != NULL) {
      glFogCoordfEXT       = d3dFogCoordfEXT;
      glFogCoordPointerEXT = d3dFogCoordPointerEXT;
      gGLState.suppFogCoord = true;
   } else {
      glFogCoordfEXT       = NULL;
      glFogCoordPointerEXT = NULL;
      gGLState.suppFogCoord = false;
   }

   // ARB_texture_compression
   glCompressedTexImage3DARB    = NULL;
   glCompressedTexImage2DARB    = NULL;
   glCompressedTexImage1DARB    = NULL;
   glCompressedTexSubImage3DARB = NULL;
   glCompressedTexSubImage2DARB = NULL;
   glCompressedTexSubImage1DARB = NULL;
   glGetCompressedTexImageARB   = NULL;

   gGLState.suppTextureCompression = false;

   // 3DFX_texture_compression_FXT1
   gGLState.suppFXT1 = false;

   // EXT_texture_compression_S3TC
   gGLState.suppS3TC = false;

   // WGL_3DFS_gamma_control
   qwglGetDeviceGammaRamp3DFX = NULL;
   qwglSetDeviceGammaRamp3DFX = NULL;

   // Binary states, i.e., no supporting functions
   // EXT_packed_pixels
   // EXT_texture_env_combine
   //
   gGLState.suppPackedPixels      = false;
   gGLState.suppTextureEnvCombine = false;
   gGLState.suppEdgeClamp         = false;
   gGLState.suppTexEnvAdd         = false;

   // Anisotropic filtering
   gGLState.suppTexAnisotropic    = false;

   Con::printf("OpenGL Init: Enabled Extensions");
   if (gGLState.suppARBMultitexture)    Con::printf("  ARB_multitexture");
   if (gGLState.suppLockedArrays)       Con::printf("  EXT_compiled_vertex_array");
   if (gGLState.suppVertexArrayRange)   Con::printf("  NV_vertex_array_range");
   if (gGLState.suppTextureEnvCombine)  Con::printf("  EXT_texture_env_combine");
   if (gGLState.suppPackedPixels)       Con::printf("  EXT_packed_pixels");
   if (gGLState.suppFogCoord)           Con::printf("  EXT_fog_coord");
   if (gGLState.suppTextureCompression) Con::printf("  ARB_texture_compression");
   if (gGLState.suppS3TC)               Con::printf("  EXT_texture_compression_s3tc");
   if (gGLState.suppFXT1)               Con::printf("  3DFX_texture_compression_FXT1");
   if (gGLState.suppTexEnvAdd)          Con::printf("  (ARB|EXT)_texture_env_add");
   if (gGLState.suppTexAnisotropic)     Con::printf("  EXT_texture_filter_anisotropic (Max anisotropy: %f)", gGLState.maxAnisotropy);

   Con::warnf(ConsoleLogEntry::General, "OpenGL Init: Disabled Extensions");
   if (!gGLState.suppARBMultitexture)    Con::warnf(ConsoleLogEntry::General, "  ARB_multitexture");
   if (!gGLState.suppLockedArrays)       Con::warnf(ConsoleLogEntry::General, "  EXT_compiled_vertex_array");
   if (!gGLState.suppVertexArrayRange)   Con::warnf(ConsoleLogEntry::General, "  NV_vertex_array_range");
   if (!gGLState.suppTextureEnvCombine)  Con::warnf(ConsoleLogEntry::General, "  EXT_texture_env_combine");
   if (!gGLState.suppPackedPixels)       Con::warnf(ConsoleLogEntry::General, "  EXT_packed_pixels");
   if (!gGLState.suppFogCoord)           Con::warnf(ConsoleLogEntry::General, "  EXT_fog_coord");
   if (!gGLState.suppTextureCompression) Con::warnf(ConsoleLogEntry::General, "  ARB_texture_compression");
   if (!gGLState.suppS3TC)               Con::warnf(ConsoleLogEntry::General, "  EXT_texture_compression_s3tc");
   if (!gGLState.suppFXT1)               Con::warnf(ConsoleLogEntry::General, "  3DFX_texture_compression_FXT1");
   if (!gGLState.suppTexEnvAdd)          Con::warnf(ConsoleLogEntry::General, "  (ARB|EXT)_texture_env_add");
   if (!gGLState.suppTexAnisotropic)     Con::warnf(ConsoleLogEntry::General, "  EXT_texture_filter_anisotropic");
   Con::printf("");

   // Set some console variables:
   Con::setBoolVariable( "$FogCoordSupported", gGLState.suppFogCoord );
   Con::setBoolVariable( "$TextureCompressionSupported", gGLState.suppTextureCompression );
   Con::setBoolVariable( "$AnisotropySupported", gGLState.suppTexAnisotropic );

   return true;
}

/*
** D3DGL_Shutdown
**
** This is only called during a hard shutdown of the OGL subsystem (e.g. vid_restart).
*/
void D3DGL_Shutdown()
{
   // GLU Functions
   gluErrorString              = NULL;
   gluGetString                = NULL;
   gluOrtho2D                  = NULL;
   gluPerspective              = NULL;
   gluPickMatrix               = NULL;
   gluLookAt                   = NULL;
   gluProject                  = NULL;
   gluUnProject                = NULL;
   gluScaleImage               = NULL;
   gluBuild1DMipmaps           = NULL;
   gluBuild2DMipmaps           = NULL;

   // GL Functions
   glAccum                     = NULL;
   glAlphaFunc                 = NULL;
   glAreTexturesResident       = NULL;
   glArrayElement              = NULL;
   glBegin                     = NULL;
   glBindTexture               = NULL;
   glBitmap                    = NULL;
   glBlendFunc                 = NULL;
   glCallList                  = NULL;
   glCallLists                 = NULL;
   glClear                     = NULL;
   glClearAccum                = NULL;
   glClearColor                = NULL;
   glClearDepth                = NULL;
   glClearIndex                = NULL;
   glClearStencil              = NULL;
   glClipPlane                 = NULL;
   glColor3b                   = NULL;
   glColor3bv                  = NULL;
   glColor3d                   = NULL;
   glColor3dv                  = NULL;
   glColor3f                   = NULL;
   glColor3fv                  = NULL;
   glColor3i                   = NULL;
   glColor3iv                  = NULL;
   glColor3s                   = NULL;
   glColor3sv                  = NULL;
   glColor3ub                  = NULL;
   glColor3ubv                 = NULL;
   glColor3ui                  = NULL;
   glColor3uiv                 = NULL;
   glColor3us                  = NULL;
   glColor3usv                 = NULL;
   glColor4b                   = NULL;
   glColor4bv                  = NULL;
   glColor4d                   = NULL;
   glColor4dv                  = NULL;
   glColor4f                   = NULL;
   glColor4fv                  = NULL;
   glColor4i                   = NULL;
   glColor4iv                  = NULL;
   glColor4s                   = NULL;
   glColor4sv                  = NULL;
   glColor4ub                  = NULL;
   glColor4ubv                 = NULL;
   glColor4ui                  = NULL;
   glColor4uiv                 = NULL;
   glColor4us                  = NULL;
   glColor4usv                 = NULL;
   glColorMask                 = NULL;
   glColorMaterial             = NULL;
   glColorPointer              = NULL;
   glCopyPixels                = NULL;
   glCopyTexImage1D            = NULL;
   glCopyTexImage2D            = NULL;
   glCopyTexSubImage1D         = NULL;
   glCopyTexSubImage2D         = NULL;
   glCullFace                  = NULL;
   glDeleteLists               = NULL;
   glDeleteTextures            = NULL;
   glDepthFunc                 = NULL;
   glDepthMask                 = NULL;
   glDepthRange                = NULL;
   glDisable                   = NULL;
   glDisableClientState        = NULL;
   glDrawArrays                = NULL;
   glDrawBuffer                = NULL;
   glDrawElements              = NULL;
   glDrawPixels                = NULL;
   glEdgeFlag                  = NULL;
   glEdgeFlagPointer           = NULL;
   glEdgeFlagv                 = NULL;
   glEnable                    = NULL;
   glEnableClientState         = NULL;
   glEnd                       = NULL;
   glEndList                   = NULL;
   glEvalCoord1d               = NULL;
   glEvalCoord1dv              = NULL;
   glEvalCoord1f               = NULL;
   glEvalCoord1fv              = NULL;
   glEvalCoord2d               = NULL;
   glEvalCoord2dv              = NULL;
   glEvalCoord2f               = NULL;
   glEvalCoord2fv              = NULL;
   glEvalMesh1                 = NULL;
   glEvalMesh2                 = NULL;
   glEvalPoint1                = NULL;
   glEvalPoint2                = NULL;
   glFeedbackBuffer            = NULL;
   glFinish                    = NULL;
   glFlush                     = NULL;
   glFogf                      = NULL;
   glFogfv                     = NULL;
   glFogi                      = NULL;
   glFogiv                     = NULL;
   glFrontFace                 = NULL;
   glFrustum                   = NULL;
   glGenLists                  = NULL;
   glGenTextures               = NULL;
   glGetBooleanv               = NULL;
   glGetClipPlane              = NULL;
   glGetDoublev                = NULL;
   glGetError                  = NULL;
   glGetFloatv                 = NULL;
   glGetIntegerv               = NULL;
   glGetLightfv                = NULL;
   glGetLightiv                = NULL;
   glGetMapdv                  = NULL;
   glGetMapfv                  = NULL;
   glGetMapiv                  = NULL;
   glGetMaterialfv             = NULL;
   glGetMaterialiv             = NULL;
   glGetPixelMapfv             = NULL;
   glGetPixelMapuiv            = NULL;
   glGetPixelMapusv            = NULL;
   glGetPointerv               = NULL;
   glGetPolygonStipple         = NULL;
   glGetString                 = NULL;
   glGetTexEnvfv               = NULL;
   glGetTexEnviv               = NULL;
   glGetTexGendv               = NULL;
   glGetTexGenfv               = NULL;
   glGetTexGeniv               = NULL;
   glGetTexImage               = NULL;
   glGetTexLevelParameterfv    = NULL;
   glGetTexLevelParameteriv    = NULL;
   glGetTexParameterfv         = NULL;
   glGetTexParameteriv         = NULL;
   glHint                      = NULL;
   glIndexMask                 = NULL;
   glIndexPointer              = NULL;
   glIndexd                    = NULL;
   glIndexdv                   = NULL;
   glIndexf                    = NULL;
   glIndexfv                   = NULL;
   glIndexi                    = NULL;
   glIndexiv                   = NULL;
   glIndexs                    = NULL;
   glIndexsv                   = NULL;
   glIndexub                   = NULL;
   glIndexubv                  = NULL;
   glInitNames                 = NULL;
   glInterleavedArrays         = NULL;
   glIsEnabled                 = NULL;
   glIsList                    = NULL;
   glIsTexture                 = NULL;
   glLightModelf               = NULL;
   glLightModelfv              = NULL;
   glLightModeli               = NULL;
   glLightModeliv              = NULL;
   glLightf                    = NULL;
   glLightfv                   = NULL;
   glLighti                    = NULL;
   glLightiv                   = NULL;
   glLineStipple               = NULL;
   glLineWidth                 = NULL;
   glListBase                  = NULL;
   glLoadIdentity              = NULL;
   glLoadMatrixd               = NULL;
   glLoadMatrixf               = NULL;
   glLoadName                  = NULL;
   glLogicOp                   = NULL;
   glMap1d                     = NULL;
   glMap1f                     = NULL;
   glMap2d                     = NULL;
   glMap2f                     = NULL;
   glMapGrid1d                 = NULL;
   glMapGrid1f                 = NULL;
   glMapGrid2d                 = NULL;
   glMapGrid2f                 = NULL;
   glMaterialf                 = NULL;
   glMaterialfv                = NULL;
   glMateriali                 = NULL;
   glMaterialiv                = NULL;
   glMatrixMode                = NULL;
   glMultMatrixd               = NULL;
   glMultMatrixf               = NULL;
   glNewList                   = NULL;
   glNormal3b                  = NULL;
   glNormal3bv                 = NULL;
   glNormal3d                  = NULL;
   glNormal3dv                 = NULL;
   glNormal3f                  = NULL;
   glNormal3fv                 = NULL;
   glNormal3i                  = NULL;
   glNormal3iv                 = NULL;
   glNormal3s                  = NULL;
   glNormal3sv                 = NULL;
   glNormalPointer             = NULL;
   glOrtho                     = NULL;
   glPassThrough               = NULL;
   glPixelMapfv                = NULL;
   glPixelMapuiv               = NULL;
   glPixelMapusv               = NULL;
   glPixelStoref               = NULL;
   glPixelStorei               = NULL;
   glPixelTransferf            = NULL;
   glPixelTransferi            = NULL;
   glPixelZoom                 = NULL;
   glPointSize                 = NULL;
   glPolygonMode               = NULL;
   glPolygonOffset             = NULL;
   glPolygonStipple            = NULL;
   glPopAttrib                 = NULL;
   glPopClientAttrib           = NULL;
   glPopMatrix                 = NULL;
   glPopName                   = NULL;
   glPrioritizeTextures        = NULL;
   glPushAttrib                = NULL;
   glPushClientAttrib          = NULL;
   glPushMatrix                = NULL;
   glPushName                  = NULL;
   glRasterPos2d               = NULL;
   glRasterPos2dv              = NULL;
   glRasterPos2f               = NULL;
   glRasterPos2fv              = NULL;
   glRasterPos2i               = NULL;
   glRasterPos2iv              = NULL;
   glRasterPos2s               = NULL;
   glRasterPos2sv              = NULL;
   glRasterPos3d               = NULL;
   glRasterPos3dv              = NULL;
   glRasterPos3f               = NULL;
   glRasterPos3fv              = NULL;
   glRasterPos3i               = NULL;
   glRasterPos3iv              = NULL;
   glRasterPos3s               = NULL;
   glRasterPos3sv              = NULL;
   glRasterPos4d               = NULL;
   glRasterPos4dv              = NULL;
   glRasterPos4f               = NULL;
   glRasterPos4fv              = NULL;
   glRasterPos4i               = NULL;
   glRasterPos4iv              = NULL;
   glRasterPos4s               = NULL;
   glRasterPos4sv              = NULL;
   glReadBuffer                = NULL;
   glReadPixels                = NULL;
   glRectd                     = NULL;
   glRectdv                    = NULL;
   glRectf                     = NULL;
   glRectfv                    = NULL;
   glRecti                     = NULL;
   glRectiv                    = NULL;
   glRects                     = NULL;
   glRectsv                    = NULL;
   glRenderMode                = NULL;
   glRotated                   = NULL;
   glRotatef                   = NULL;
   glScaled                    = NULL;
   glScalef                    = NULL;
   glScissor                   = NULL;
   glSelectBuffer              = NULL;
   glShadeModel                = NULL;
   glStencilFunc               = NULL;
   glStencilMask               = NULL;
   glStencilOp                 = NULL;
   glTexCoord1d                = NULL;
   glTexCoord1dv               = NULL;
   glTexCoord1f                = NULL;
   glTexCoord1fv               = NULL;
   glTexCoord1i                = NULL;
   glTexCoord1iv               = NULL;
   glTexCoord1s                = NULL;
   glTexCoord1sv               = NULL;
   glTexCoord2d                = NULL;
   glTexCoord2dv               = NULL;
   glTexCoord2f                = NULL;
   glTexCoord2fv               = NULL;
   glTexCoord2i                = NULL;
   glTexCoord2iv               = NULL;
   glTexCoord2s                = NULL;
   glTexCoord2sv               = NULL;
   glTexCoord3d                = NULL;
   glTexCoord3dv               = NULL;
   glTexCoord3f                = NULL;
   glTexCoord3fv               = NULL;
   glTexCoord3i                = NULL;
   glTexCoord3iv               = NULL;
   glTexCoord3s                = NULL;
   glTexCoord3sv               = NULL;
   glTexCoord4d                = NULL;
   glTexCoord4dv               = NULL;
   glTexCoord4f                = NULL;
   glTexCoord4fv               = NULL;
   glTexCoord4i                = NULL;
   glTexCoord4iv               = NULL;
   glTexCoord4s                = NULL;
   glTexCoord4sv               = NULL;
   glTexCoordPointer           = NULL;
   glTexEnvf                   = NULL;
   glTexEnvfv                  = NULL;
   glTexEnvi                   = NULL;
   glTexEnviv                  = NULL;
   glTexGend                   = NULL;
   glTexGendv                  = NULL;
   glTexGenf                   = NULL;
   glTexGenfv                  = NULL;
   glTexGeni                   = NULL;
   glTexGeniv                  = NULL;
   glTexImage1D                = NULL;
   glTexImage2D                = NULL;
   glTexParameterf             = NULL;
   glTexParameterfv            = NULL;
   glTexParameteri             = NULL;
   glTexParameteriv            = NULL;
   glTexSubImage1D             = NULL;
   glTexSubImage2D             = NULL;
   glTranslated                = NULL;
   glTranslatef                = NULL;
   glVertex2d                  = NULL;
   glVertex2dv                 = NULL;
   glVertex2f                  = NULL;
   glVertex2fv                 = NULL;
   glVertex2i                  = NULL;
   glVertex2iv                 = NULL;
   glVertex2s                  = NULL;
   glVertex2sv                 = NULL;
   glVertex3d                  = NULL;
   glVertex3dv                 = NULL;
   glVertex3f                  = NULL;
   glVertex3fv                 = NULL;
   glVertex3i                  = NULL;
   glVertex3iv                 = NULL;
   glVertex3s                  = NULL;
   glVertex3sv                 = NULL;
   glVertex4d                  = NULL;
   glVertex4dv                 = NULL;
   glVertex4f                  = NULL;
   glVertex4fv                 = NULL;
   glVertex4i                  = NULL;
   glVertex4iv                 = NULL;
   glVertex4s                  = NULL;
   glVertex4sv                 = NULL;
   glVertexPointer             = NULL;
   glViewport                  = NULL;

   // EXT_compiled_vertex_array
   glLockArraysEXT             = NULL;
   glUnlockArraysEXT           = NULL;

   // ARB_multitexture
   glActiveTextureARB          = NULL;
   glClientActiveTextureARB    = NULL;
   glMultiTexCoord2fARB        = NULL;
   glMultiTexCoord2fvARB       = NULL;

   // NV_vertex_array_range
   glVertexArrayRangeNV        = NULL;
   glFlushVertexArrayRangeNV   = NULL;
   wglAllocateMemoryNV         = NULL;
   wglFreeMemoryNV             = NULL;

   // EXT_fog_coord
   glFogCoordfEXT              = NULL;
   glFogCoordPointerEXT        = NULL;

   /* ARB_texture_compression */
   glCompressedTexImage3DARB    = NULL;
   glCompressedTexImage2DARB    = NULL;
   glCompressedTexImage1DARB    = NULL;
   glCompressedTexSubImage3DARB = NULL;
   glCompressedTexSubImage2DARB = NULL;
   glCompressedTexSubImage1DARB = NULL;
   glGetCompressedTexImageARB   = NULL;

   qwglCopyContext             = NULL;
   qwglCreateContext           = NULL;
   qwglCreateLayerContext      = NULL;
   qwglDeleteContext           = NULL;
   qwglDescribeLayerPlane      = NULL;
   qwglGetCurrentContext       = NULL;
   qwglGetCurrentDC            = NULL;
   qwglGetLayerPaletteEntries  = NULL;
   qwglGetProcAddress          = NULL;
   qwglMakeCurrent             = NULL;
   qwglRealizeLayerPalette     = NULL;
   qwglSetLayerPaletteEntries  = NULL;
   qwglShareLists              = NULL;
   qwglSwapLayerBuffers        = NULL;
   qwglUseFontBitmaps          = NULL;
   qwglUseFontOutlines         = NULL;

   qwglChoosePixelFormat       = NULL;
   qwglDescribePixelFormat     = NULL;
   qwglGetPixelFormat          = NULL;
   qwglSetPixelFormat          = NULL;
   qwglSwapBuffers             = NULL;
}
