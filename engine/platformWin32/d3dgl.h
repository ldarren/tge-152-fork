#ifndef _D3DGL_H_
#define _D3DGL_H_

#define D3D_OVERLOADS

#ifndef __D3DX_H__
#include "d3dx.h"
#endif
#ifndef _GLLIST_H_
#include "platformWin32/gllist.h"
#endif
#ifndef _PLATFORMGL_H_
#include "platformWin32/platformGL.h"
#endif

#define M_2PI (3.1415926535897932384626433 * 2.0)
#define mDegToRad(d) ((d*M_PI) / 180.0)

/* Multitexture extensions */
#define GL_TEXTURE0_SGIS                  0x835E
#define GL_TEXTURE1_SGIS                  0x835F
#define GL_TEXTURE2_SGIS                  0x8360
#define GL_TEXTURE3_SGIS                  0x8361

#define MAXGLTEXHANDLES 4096

struct QuakeVertex {
	D3DVALUE x, y, z;
	D3DVALUE nx, ny, nz;
	D3DCOLOR color;
};

struct QuakeTVertex {
	D3DVALUE x, y, z;
	D3DVALUE nx, ny, nz;
	D3DCOLOR color;
	D3DVALUE tu, tv;
};

struct QuakeMTVertex {
	D3DVALUE x, y, z;
	D3DVALUE nx, ny, nz;
	D3DCOLOR color;
	D3DVALUE tu, tv, tu2, tv2;
};

struct QuakeFMTVertex {
	D3DVALUE x, y, z;
	D3DVALUE nx, ny, nz;
	D3DCOLOR diffuse;
	D3DCOLOR specular;
	D3DVALUE tu, tv, tu2, tv2;
};

struct TransformedQuakeVertex {
    D3DVALUE x, y, z, rhw;
    D3DCOLOR color;
    D3DVALUE tu, tv, tu2, tv2;
};

#define QUAKEVFMT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define QUAKETVFMT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define QUAKEMTVFMT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2)
#define QUAKEFMTVFMT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_SPECULAR)
#define QUAKETRVFMT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2)

#define VBUFSIZE 2048
#define MAXVERTSPERPRIM 128

#define RESPATH_QUAKE "Software\\Microsoft\\Quake"

struct TexInfo {
  DWORD m_block;
  DWORD m_dwStage;
  BOOL m_capture;
  GLint m_internalformat;
  D3DX_SURFACEFORMAT m_fmt;
  GLsizei m_width;
  GLsizei m_height;
  GLsizei m_oldwidth;
  GLsizei m_oldheight;
  LPDIRECTDRAWSURFACE7 m_ddsurf;
  D3DTEXTUREMINFILTER m_minmode;
  D3DTEXTUREMAGFILTER m_magmode;
  D3DTEXTUREMIPFILTER m_mipmode;
  D3DTEXTUREADDRESS m_addu, m_addv;
};

struct Globals {
	// Cache Line 1
	GLenum m_prim;
	unsigned m_comp;
	unsigned m_nfv[4];
	unsigned m_vcnt[4];

	// Cache Line 2
	void *m_verts;
	// Following always needs to be together
	/********************************/
	D3DVALUE m_nx, m_ny, m_nz;
	D3DCOLOR m_color;
	D3DVALUE m_tu, m_tv, m_tu2, m_tv2;
	/********************************/
	BOOL m_texturing;
	BOOL m_mtex;

	// Cache Line 3
	LPDIRECT3DVERTEXBUFFER7 m_vbuf, m_tvbuf, m_mtvbuf, m_fmtvbuf;
	LPDIRECT3DDEVICE7 m_d3ddev;

	DWORD m_shaders[2][9];
	int m_winWidth;
	int m_winHeight;
	GLint m_scix, m_sciy;
	GLsizei m_sciw, m_scih;
	GLint m_vwx, m_vwy;
	GLsizei m_vww, m_vwh;
	GLint m_lckfirst;
	GLsizei m_lckcount;
	HWND m_hwnd;
	HDC m_hdc;
	ID3DXContext *m_pD3DX;
	D3DX_SURFACEFORMAT m_ddFourBitAlphaSurfFormat;
	D3DX_SURFACEFORMAT m_ddEightBitAlphaSurfFormat;
	D3DX_SURFACEFORMAT m_ddFiveBitSurfFormat;
	D3DX_SURFACEFORMAT m_ddEightBitSurfFormat;
	D3DX_SURFACEFORMAT m_ddLuminanceSurfFormat;
	D3DX_SURFACEFORMAT m_ddAlphaSurfFormat;
	D3DDEVICEDESC7 m_dd;
	GLenum m_cullMode;
	D3DTRANSFORMSTATETYPE m_matrixMode;
	BOOL m_cullEnabled;
	BOOL m_texHandleValid;
	BOOL m_subsample;
	BOOL m_usemtex;
	BOOL m_usemipmap;
	BOOL m_doFlip;
	BOOL m_makeSquare;
	BOOL m_scissoring;
	BOOL m_updvwp;
	BOOL m_usecolorary, m_usetexcoordary[2], m_usevertexary;
	GLuint m_curstagebinding[2];
	GLenum m_curtgt, m_client_active_texture_arb;
	int m_blendmode[2];
	D3DCOLOR m_clearColor;
	GLclampd m_clearDepth;
	const GLfloat *m_vertexary;
	const void *m_colorary;
	GLsizei m_numIndices;
	WORD *m_wIndices;
	const GLfloat *m_texcoordary[2];
	LPDIRECTDRAWSURFACE7 m_curtex[2];
	GLList<D3DMATRIX> m_matrixStack[3];
	WNDPROC m_wndproc;
	DWORD m_lod;
	TexInfo m_tex[MAXGLTEXHANDLES]; // support upto MAXGLTEXHANDLES for the time being;
	GLList<GLuint> m_freeTextures;
	GLenum m_frontFace;
	BOOL m_usenormalary;
	const GLfloat *m_normalary;
	DWORD m_normalstride;
	DWORD m_texcoordstride[2];
	DWORD m_vertexstride;
	BOOL m_texgen[2];
	GLint m_texgenmode[2];
	GLfloat m_texgenplane[2][2][4];
	D3DMATRIX m_inverseworld;
	BOOL m_inversedirty;
	BOOL m_objectdirty[2];
	D3DCOLOR m_envcolor;
	DWORD m_colorstride;
	GLenum m_colortype;
	D3DLIGHT7 m_lights[8];
	GLfloat *m_spherecoords;
	DWORD m_zbias;
	BOOL m_usedirectional;
	D3DMATRIX m_curtexmatrix;
	BOOL m_usefogary;
	D3DCOLOR m_fogcolor;
	const GLfloat *m_fogary;
	DWORD m_fogstride;
};

#ifndef M_PI
#define M_PI 3.1415926536
#endif

#define CEILING( A, B )  ( (A) % (B) == 0 ? (A)/(B) : (A)/(B)+1 )

/* To work around optimizer bug in MSVC4.1 */
#if defined(__WIN32__) && !defined(OPENSTEP)
void
dummy(GLuint j, GLuint k)
{
}
#else
#define dummy(J, K)
#endif

/* EXT_bgra */
#define GL_BGR					0x80E0
#define GL_BGRA					0x80E1

/* Errors */
#define GLU_NO_ERROR			0
#define GLU_ERROR				100103
#define GLU_INVALID_ENUM		100900
#define GLU_INVALID_VALUE		100901
#define GLU_OUT_OF_MEMORY		100902

/* GLU 1.1 and later */
#define GLU_VERSION				100800
#define GLU_EXTENSIONS			100801

#endif
