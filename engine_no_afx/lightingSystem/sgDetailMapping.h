//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#ifndef _SGDETAILMAPPING_H_
#define _SGDETAILMAPPING_H_

class MaterialList;

class sgDetailMapping
{
private:
	TextureHandle *sgWhiteTexture;
	MaterialList *sgMaterialsCache;
	Vector<TextureHandle *> sgDetailMaps;
public:
	sgDetailMapping()
	{
		sgWhiteTexture = NULL;
		sgMaterialsCache = NULL;
	}
	~sgDetailMapping() {sgClearDetailMapping();}
	void sgInitDetailMapping(MaterialList *materials);
	void sgClearDetailMapping();
	void sgBindDetailMap(U32 surfaceindex);
	void sgEnableDetailMapping(void *buffer, U32 elementsize);
	void sgDisableDetailMapping();
};

#endif//_SGDETAILMAPPING_H_
