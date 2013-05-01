//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#ifndef _SGSCENEPERSIST_H_
#define _SGSCENEPERSIST_H_

#ifndef _SHADOWVOLUMEBSP_H_
#include "sceneGraph/shadowVolumeBSP.h"
#endif


struct PersistInfo
{
	struct PersistChunk
	{
		enum {
			MissionChunkType = 0,
			InteriorChunkType,
			TerrainChunkType,
			AtlasLightMapChunkType
		};

		U32            mChunkType;
		U32            mChunkCRC;

		virtual ~PersistChunk() {}

		virtual bool read(Stream &);
		virtual bool write(Stream &);
	};

	struct MissionChunk : public PersistChunk
	{
		typedef PersistChunk Parent;
		MissionChunk();
	};

	struct InteriorChunk : public PersistChunk
	{
		typedef PersistChunk Parent;

		InteriorChunk();
		~InteriorChunk();

		Vector<U32>          mDetailLightmapCount;
		Vector<U32>          mDetailLightmapIndices;
		Vector<GBitmap*>     mLightmaps;

		bool                 mHasAlarmState;
		Vector<U32>          mDetailVertexCount;
		Vector<ColorI>       mVertexColorsNormal;
		Vector<ColorI>       mVertexColorsAlarm;

		bool read(Stream &);
		bool write(Stream &);
	};

	struct TerrainChunk : public PersistChunk
	{
		typedef PersistChunk Parent;

		TerrainChunk();
		~TerrainChunk();

		U16 *mLightmap;

		bool read(Stream &);
		bool write(Stream &);
	};

	~PersistInfo();

	Vector<PersistChunk*>      mChunks;
	static U32                 smFileVersion;

	bool read(Stream &);
	bool write(Stream &);
};


#endif//_SGSCENEPERSIST_H_
