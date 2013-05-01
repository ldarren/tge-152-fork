
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "afx/ce/afxVolumeLight.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxVolumeLightData

IMPLEMENT_CO_DATABLOCK_V1(afxVolumeLightData);

afxVolumeLightData::afxVolumeLightData()
{
	mLTextureName = StringTable->insert("");
	mlpDistance = 8.0f;
	mShootDistance = 15.0f;
	mXextent = 6.0f;
	mYextent = 6.0f;
	mSubdivideU = 32;
	mSubdivideV = 32;
	mfootColour = ColorF(1.f, 1.f, 1.f, 0.2f);
	mtailColour = ColorF(0.f, 0.f, 0.f, 0.0f);
}

#define myOffset(field) Offset(field, afxVolumeLightData)

void afxVolumeLightData::initPersistFields()
{
	Parent::initPersistFields();

	// Light Field Image
	addField( "Texture", 			  TypeFilename,   myOffset(mLTextureName));

	// Dimensions
	addField( "lpDistance",			TypeF32,		    myOffset(mlpDistance));
	addField( "ShootDistance",  TypeF32,		    myOffset(mShootDistance));
	addField( "Xextent",			  TypeF32,		    myOffset(mXextent));
	addField( "Yextent",			  TypeF32,		    myOffset(mYextent));

	// Subdivisions
	addField( "SubdivideU",			TypeS32,		    myOffset(mSubdivideU));
	addField( "SubdivideV",			TypeS32,		    myOffset(mSubdivideV));

	// Colors
	addField( "FootColour",			TypeColorF,		  myOffset(mfootColour));
	addField( "TailColour",			TypeColorF,		  myOffset(mtailColour));
}

void afxVolumeLightData::packData(BitStream* stream)
{
	Parent::packData(stream);

	// Light Field Image
	stream->writeString(mLTextureName);

	// Dimensions
	stream->write(mlpDistance);
	stream->write(mShootDistance);
	stream->write(mXextent);
	stream->write(mYextent);

	// Subdivisions
	stream->write(mSubdivideU);
	stream->write(mSubdivideV);

	// Colors
	stream->write(mfootColour);
	stream->write(mtailColour);
}

void afxVolumeLightData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

	// Light Field Image
	mLTextureName = stream->readSTString();

	// Dimensions
	stream->read(&mlpDistance);
	stream->read(&mShootDistance);
	stream->read(&mXextent);
	stream->read(&mYextent);

	// Subdivisions
	stream->read(&mSubdivideU);
	stream->read(&mSubdivideV);

	// Colors
	stream->read(&mfootColour);
	stream->read(&mtailColour);  
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//