//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "dgl/gBitmap.h"
#include "terrain/terrData.h"
#include "lightingSystem/sgScenePersist.h"
#include "lightingSystem/sgSceneLighting.h"


//------------------------------------------------------------------------------
// Class SceneLighting::PersistInfo
//------------------------------------------------------------------------------
U32 PersistInfo::smFileVersion   = 0x11;

PersistInfo::~PersistInfo()
{
	for(U32 i = 0; i < mChunks.size(); i++)
		delete mChunks[i];
}

//------------------------------------------------------------------------------
bool PersistInfo::read(Stream & stream)
{
	U32 version;
	if(!stream.read(&version) || version != smFileVersion)
		return(false);

	U32 numChunks;
	if(!stream.read(&numChunks))
		return(false);

	if(numChunks == 0)
		return(false);

	// read in all the chunks
	for(U32 i = 0; i < numChunks; i++)
	{
		U32 chunkType;
		if(!stream.read(&chunkType))
			return(false);

		// MissionChunk must be first chunk
		if(i == 0 && chunkType != PersistChunk::MissionChunkType)
			return(false);
		if(i != 0 && chunkType == PersistChunk::MissionChunkType)
			return(false);

		// create the chunk
		switch(chunkType)
		{
		case PersistChunk::MissionChunkType:
			mChunks.push_back(new PersistInfo::MissionChunk);
			break;

		case PersistChunk::InteriorChunkType:
			mChunks.push_back(new PersistInfo::InteriorChunk);
			break;

		case PersistChunk::TerrainChunkType:
			mChunks.push_back(new PersistInfo::TerrainChunk);
			break;

		default:
			return(false);
			break;
		}

		// load the chunk info
		if(!mChunks[i]->read(stream))
			return(false);
	}

	return(true);
}

bool PersistInfo::write(Stream & stream)
{
	if(!stream.write(smFileVersion))
		return(false);

	if(!stream.write((U32)mChunks.size()))
		return(false);

	for(U32 i = 0; i < mChunks.size(); i++)
	{
		if(!stream.write(mChunks[i]->mChunkType))
			return(false);
		if(!mChunks[i]->write(stream))
			return(false);
	}

	return(true);
}

//------------------------------------------------------------------------------
// Class SceneLighting::PersistInfo::PersistChunk
//------------------------------------------------------------------------------
bool PersistInfo::PersistChunk::read(Stream & stream)
{
	if(!stream.read(&mChunkCRC))
		return(false);
	return(true);
}

bool PersistInfo::PersistChunk::write(Stream & stream)
{
	if(!stream.write(mChunkCRC))
		return(false);
	return(true);
}

//------------------------------------------------------------------------------
// Class SceneLighting::PersistInfo::MissionChunk
//------------------------------------------------------------------------------
PersistInfo::MissionChunk::MissionChunk()
{
	mChunkType = PersistChunk::MissionChunkType;
}

//------------------------------------------------------------------------------
// Class SceneLighting::PersistInfo::InteriorChunk
//------------------------------------------------------------------------------
PersistInfo::InteriorChunk::InteriorChunk()
{
	mChunkType = PersistChunk::InteriorChunkType;
}

PersistInfo::InteriorChunk::~InteriorChunk()
{
	for(U32 i = 0; i < mLightmaps.size(); i++)
		delete mLightmaps[i];
}

//------------------------------------------------------------------------------
// - always read in vertex lighting, lightmaps may not be needed
bool PersistInfo::InteriorChunk::read(Stream & stream)
{
	if(!Parent::read(stream))
		return(false);

	U32 size;
	U32 i;

   // lightmaps->vertex-info
   if(!SceneLighting::smUseVertexLighting)
   {
	// size of this minichunk
	if(!stream.read(&size))
		return(false);

	// lightmaps
	stream.read(&size);
	mDetailLightmapCount.setSize(size);
	for(i = 0; i < size; i++)
		if(!stream.read(&mDetailLightmapCount[i]))
			return(false);

	stream.read(&size);
	mDetailLightmapIndices.setSize(size);
	for(i = 0; i < size; i++)
		if(!stream.read(&mDetailLightmapIndices[i]))
			return(false);

	if(!stream.read(&size))
		return(false);
	mLightmaps.setSize(size);

	for(i = 0; i < size; i++)
	{
		mLightmaps[i] = new GBitmap;
		if(LightManager::sgAllowFullLightMaps())
		{
			if(!mLightmaps[i]->readPNG(stream))
				return(false);
		}
		else
		{
			if(!mLightmaps[i]->read(stream))
				return(false);
		}
	 }
	}
   else
   {
      // step past the lightmaps
      if(!stream.read(&size))
         return(false);
      if(!stream.setPosition(stream.getPosition() + size))
         return(false);
   }

   // size of the vertex lighting: need to reset stream position after zipStream reading
   U32 zipStreamEnd;
   if(!stream.read(&zipStreamEnd))
      return(false);
   zipStreamEnd += stream.getPosition();

   // vertex lighting
   ZipSubRStream zipStream;
   if(!zipStream.attachStream(&stream))
      return(false);

   if(!zipStream.read(&size))
		return(false);
   mHasAlarmState = bool(size);

   if(!zipStream.read(&size))
      return(false);
   mDetailVertexCount.setSize(size);
	for(i = 0; i < size; i++)
      if(!zipStream.read(&mDetailVertexCount[i]))
         return(false);

   size = 0;
   for(i = 0; i < mDetailVertexCount.size(); i++)
      size += mDetailVertexCount[i];

   mVertexColorsNormal.setSize(size);

   if(mHasAlarmState)
      mVertexColorsAlarm.setSize(size);

   U32 curPos = 0;
   for(i = 0; i < mDetailVertexCount.size(); i++)
   {
      U32 count = mDetailVertexCount[i];
      for(U32 j = 0; j < count; j++)
         if(!zipStream.read(&mVertexColorsNormal[curPos + j]))
            return(false);

      // read in the alarm info
      if(mHasAlarmState)
	{
         // same?
         if(!zipStream.read(&size))
            return(false);
         if(bool(size))
            dMemcpy(&mVertexColorsAlarm[curPos], &mVertexColorsNormal[curPos], count * sizeof(ColorI));
         else
		{
            for(U32 j = 0; j < count; j++)
               if(!zipStream.read(&mVertexColorsAlarm[curPos + j]))
				return(false);
		}
	}

      curPos += count;
   }

   zipStream.detachStream();

	// since there is no resizeFilterStream the zipStream happily reads
	// off the end of the compressed block... reset the position
	stream.setPosition(zipStreamEnd);

	return(true);
}

bool PersistInfo::InteriorChunk::write(Stream & stream)
{
	if(!Parent::write(stream))
		return(false);

	// lightmaps
	U32 startPos = stream.getPosition();
	if(!stream.write(U32(0)))
		return(false);

	U32 i;
	if(!stream.write(U32(mDetailLightmapCount.size())))
		return(false);
	for(i = 0; i < mDetailLightmapCount.size(); i++)
		if(!stream.write(mDetailLightmapCount[i]))
			return(false);

	if(!stream.write(U32(mDetailLightmapIndices.size())))
		return(false);
	for(i = 0; i < mDetailLightmapIndices.size(); i++)
		if(!stream.write(mDetailLightmapIndices[i]))
			return(false);

	if(!stream.write(U32(mLightmaps.size())))
		return(false);
	for(i = 0; i < mLightmaps.size(); i++)
	{
		AssertFatal(mLightmaps[i], "SceneLighting::SceneLighting::PersistInfo::InteriorChunk::Write: Invalid bitmap!");
		if(LightManager::sgAllowFullLightMaps())
		{
			if(!mLightmaps[i]->writePNG(stream))
				return(false);
		}
		else
		{
			if(!mLightmaps[i]->write(stream))
				return(false);
		}
	}


	// write out the lightmap portions size
	U32 endPos = stream.getPosition();
	if(!stream.setPosition(startPos))
		return(false);

	// don't include the offset in the size
	if(!stream.write(U32(endPos - startPos - sizeof(U32))))
		return(false);
	if(!stream.setPosition(endPos))
		return(false);


	// vertex lighting: needs the size of the vertex info because the
	// zip stream may read off the end of the chunk
	startPos = stream.getPosition();
	if(!stream.write(U32(0)))
		return(false);

   ZipSubWStream zipStream;
   if(!zipStream.attachStream(&stream))
      return(false);

   if(!zipStream.write(U32(mHasAlarmState)))
      return(false);
   if(!zipStream.write(U32(mDetailVertexCount.size())))
      return(false);

   for(U32 i = 0; i < mDetailVertexCount.size(); i++)
      if(!zipStream.write(mDetailVertexCount[i]))
         return(false);

   U32 curPos = 0;
   for(i = 0; i < mDetailVertexCount.size(); i++)
   {
      U32 count = mDetailVertexCount[i];
      for(U32 j = 0; j < count; j++)
         if(!zipStream.write(mVertexColorsNormal[curPos + j]))
            return(false);

      // do alarm.. check if the same
      if(mHasAlarmState)
      {
         if(!dMemcmp(&mVertexColorsNormal[curPos], &mVertexColorsAlarm[curPos], count * sizeof(ColorI)))
         {
            if(!zipStream.write(U32(1)))
               return(false);
         }
         else
         {
            if(!zipStream.write(U32(0)))
               return(false);
            for(U32 j = 0; j < count; j++)
               if(!zipStream.write(mVertexColorsAlarm[curPos + j]))
                  return(false);
         }
      }

      curPos += count;
   }
   zipStream.detachStream();

	// write out the vertex lighting portions size
	endPos = stream.getPosition();
	if(!stream.setPosition(startPos))
		return(false);

	// don't include the offset in the size
	if(!stream.write(U32(endPos - startPos - sizeof(U32))))
		return(false);
	if(!stream.setPosition(endPos))
		return(false);

	return(true);
}

//------------------------------------------------------------------------------
// Class SceneLighting::PersistInfo::TerrainChunk
//------------------------------------------------------------------------------
PersistInfo::TerrainChunk::TerrainChunk()
{
	mChunkType = PersistChunk::TerrainChunkType;
	mLightmap = NULL;
}

PersistInfo::TerrainChunk::~TerrainChunk()
{
	if(mLightmap)
		delete[] mLightmap;
}

//------------------------------------------------------------------------------

bool PersistInfo::TerrainChunk::read(Stream & stream)
{
	if(!Parent::read(stream))
		return(false);

   GBitmap bitmap;
   if(!bitmap.readPNG(stream))
      return(false);

   if((bitmap.getWidth() != TerrainBlock::LightmapSize) ||
      (bitmap.getHeight() != TerrainBlock::LightmapSize) ||
      (bitmap.getFormat() != GBitmap::RGB))
      return(false);

   mLightmap = new U16[TerrainBlock::LightmapSize * TerrainBlock::LightmapSize];
   dMemset(mLightmap, 0, sizeof(U16) * TerrainBlock::LightmapSize * TerrainBlock::LightmapSize);

   // copy the bitmap: bits should already be clamped
   U8 * bp = bitmap.getAddress(0,0);
   for(U32 i = 0; i < TerrainBlock::LightmapSize * TerrainBlock::LightmapSize; i++)
   {
      mLightmap[i] = (U16(bp[0]) << 11) | (U16(bp[1]) << 6) | (U16(bp[2]) << 1) | 1;
      bp += 3;
   }

   return(true);
}

bool PersistInfo::TerrainChunk::write(Stream & stream)
{
	if(!Parent::write(stream))
		return(false);

	if(!mLightmap)
		return(false);

   // write out an RGB bitmap so it can be PNG compressed
   GBitmap bitmap(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, false, GBitmap::RGB);
   U8 * bp = bitmap.getAddress(0,0);

   for(U32 i = 0; i < TerrainBlock::LightmapSize * TerrainBlock::LightmapSize; i++)
   {
      bp[0] = mLightmap[i] >> 11;
      bp[1] = (mLightmap[i] >> 6) & 0x1f;
      bp[2] = (mLightmap[i] >> 1) & 0x1f;
      bp += 3;
   }

   if(!bitmap.writePNG(stream))
		return(false);

	return(true);
}



