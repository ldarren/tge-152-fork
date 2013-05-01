//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "lightingSystem/sgLighting.h"
#include "game/fx/particleEngine.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameConnection.h"
#include "sceneGraph/sceneGraph.h"
#include "sim/netConnection.h"
#include "dgl/materialPropertyMap.h"
#include "lightingSystem/sgDetailMapping.h"


void sgDetailMapping::sgInitDetailMapping(MaterialList *materials)
{
	sgClearDetailMapping();

	// need this for DX to GL switch...
	sgMaterialsCache = materials;

	if(!LightManager::sgAllowDetailMaps())
		return;

	GBitmap *b = new GBitmap(2, 2);
	U8 *bits = b->getWritableBits();
	dMemset(bits, 127, b->byteSize);
	sgWhiteTexture = new TextureHandle(NULL, b);


	MaterialPropertyMap *materialprops = static_cast<MaterialPropertyMap *>(Sim::findObject("MaterialPropertyMap"));
	sgDetailMaps.setSize(materials->getMaterialCount());

	for(U32 i=0; i<materials->getMaterialCount(); i++)
	{
		const char* name = materials->getMaterialName(i);
		const MaterialPropertyMap::MapEntry* entry = materialprops->getMapEntry(name);

		// init this to NULL...
		sgDetailMaps[i] = NULL;

		// try to fill it out now...
		if((entry == NULL) || (entry->detailMapName == NULL))
			continue;

		// we're doing this by hand - Torque gives you no contol over mipmaps...
		// load the texture...
		GBitmap *bitmap = TextureManager::loadBitmapInstance(entry->detailMapName);
		if(!bitmap)
			continue;

		// make a deletable copy and use it to create the texture...
		// need to do this because BitmapTexture doesn't create mips (so refresh fails)...
		//GBitmap *bitmapcopy = new GBitmap(*bitmap);
		//TextureHandle *texture = new TextureHandle(NULL, bitmapcopy, DetailTexture);
		//TextureHandle *texture = new TextureHandle(NULL, bitmap);

		// manually extrude the original...
		bitmap->extrudeMipLevels();

		// fix the levels...
		F32 scale = (2.0f / F32(bitmap->getNumMipLevels()));
		F32 currentscale = 1.0f;
		for(U32 m=0; m<bitmap->getNumMipLevels(); m++)
		{
			for(U32 y=0; y<bitmap->getHeight(m); y++)
			{
				for(U32 x=0; x<bitmap->getWidth(m); x++)
				{
					// this is ugly, but it only happens on load...
					U8 *bits = bitmap->getAddress(x, y, m);

					F32 bit = bits[0];
					bits[0] = U8(((bit - 127.0f) * currentscale) + 127.0f);

					bit = bits[1];
					bits[1] = U8(((bit - 127.0f) * currentscale) + 127.0f);

					bit = bits[2];
					bits[2] = U8(((bit - 127.0f) * currentscale) + 127.0f);
				}
			}
			currentscale -= scale;
			currentscale = mClampF(currentscale, 0.0f, 1.0f);
		}

		// assign the bitmap...
		//TextureObject *obj = *texture;
		//obj->bitmap = bitmap;

		//obj->type = DetailTexture;

		// refresh...
		//texture->refresh();
		TextureHandle *texture = new TextureHandle(NULL, bitmap);

		// unlink and delete...
		//obj->bitmap = NULL;
		//delete bitmap;

		// keep link and set to keep texture (for DX/GL switch)...
		//obj->type = BitmapKeepTexture;

		// finally done...
		sgDetailMaps[i] = texture;
	}
}

void sgDetailMapping::sgBindDetailMap(U32 surfaceindex)
{
	if(!LightManager::sgAllowDetailMaps())
		return;

	// DX to GL switch?
	if((sgDetailMaps.size() == 0) && (sgMaterialsCache))
	{
		sgInitDetailMapping(sgMaterialsCache);
		// don't want to go through that again...
		sgMaterialsCache = NULL;
	}

	// bailout...
	if(sgDetailMaps.size() <= surfaceindex)
		return;

	AssertFatal((sgDetailMaps.size() > surfaceindex), "Invalid surface index while detail mapping.");
	TextureHandle *texture = sgDetailMaps[surfaceindex];

	glActiveTextureARB(GL_TEXTURE2_ARB);
	if(!texture)
		glBindTexture(GL_TEXTURE_2D, sgWhiteTexture->getGLName());
	else
		glBindTexture(GL_TEXTURE_2D, texture->getGLName());

	glActiveTextureARB(GL_TEXTURE3_ARB);
	if(!texture)
		glBindTexture(GL_TEXTURE_2D, sgWhiteTexture->getGLName());
	else
		glBindTexture(GL_TEXTURE_2D, texture->getGLName());
}

void sgDetailMapping::sgClearDetailMapping()
{
	sgMaterialsCache = NULL;

	if(sgWhiteTexture)
	{
		delete sgWhiteTexture;
		sgWhiteTexture = NULL;
	}
	for(U32 i=0; i<sgDetailMaps.size(); i++)
		delete sgDetailMaps[i];
	sgDetailMaps.clear();
}

void sgDetailMapping::sgEnableDetailMapping(void *buffer, U32 elementsize)
{
	if(!LightManager::sgAllowDetailMaps())
		return;

   const F32 scale[16] = {4.0, 0.0, 0.0, 0.0,
						  0.0, 4.0, 0.0, 0.0,
						  0.0, 0.0, 4.0, 0.0,
						  0.0, 0.0, 0.0, 1.0};

   const F32 scaleX4[16] = {8.0, 0.0, 0.0, 0.0,
						  0.0, 8.0, 0.0, 0.0,
						  0.0, 0.0, 8.0, 0.0,
						  0.0, 0.0, 0.0, 1.0};

   glActiveTextureARB(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   LightManager::sgSetupExposureRendering();
   glMatrixMode(GL_TEXTURE);
   glPushMatrix();
   glMultMatrixf(scale);

   glActiveTextureARB(GL_TEXTURE3_ARB);
   //glDisable(GL_TEXTURE_2D);
   glEnable(GL_TEXTURE_2D);
   LightManager::sgSetupExposureRendering();
   glMatrixMode(GL_TEXTURE);
   glPushMatrix();
   glMultMatrixf(scaleX4);

   glActiveTextureARB(GL_TEXTURE0_ARB);

   glClientActiveTextureARB(GL_TEXTURE2_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, elementsize, buffer);

   glClientActiveTextureARB(GL_TEXTURE3_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, elementsize, buffer);

   glClientActiveTextureARB(GL_TEXTURE0_ARB);
}

void sgDetailMapping::sgDisableDetailMapping()
{
	if(!LightManager::sgAllowDetailMaps())
		return;

   glClientActiveTextureARB(GL_TEXTURE3_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTextureARB(GL_TEXTURE2_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTextureARB(GL_TEXTURE0_ARB);

   glActiveTextureARB(GL_TEXTURE3_ARB);
   glMatrixMode(GL_TEXTURE);
   glPopMatrix();
   LightManager::sgResetExposureRendering();
   glDisable(GL_TEXTURE_2D);

   glActiveTextureARB(GL_TEXTURE2_ARB);
   glMatrixMode(GL_TEXTURE);
   glPopMatrix();
   LightManager::sgResetExposureRendering();
   glDisable(GL_TEXTURE_2D);

   glActiveTextureARB(GL_TEXTURE0_ARB);
}

