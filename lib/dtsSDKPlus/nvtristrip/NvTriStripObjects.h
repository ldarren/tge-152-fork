//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
// Copied from NVidia Triangle Strip SDK, available from NVidia website
//-----------------------------------------------------------------------------

#ifndef NV_TRISTRIP_OBJECTS_H
#define NV_TRISTRIP_OBJECTS_H

#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#else
   typedef U32 UINT;
   typedef short WORD;
#endif
#include <vector>
#include <list>
#include "VertexCache.h"
#include "../DTSPlusTypes.h"

/////////////////////////////////////////////////////////////////////////////////
//
// Types defined for stripification
//
/////////////////////////////////////////////////////////////////////////////////

struct MyVertex {
	F32 x, y, z;
	F32 nx, ny, nz;
};

typedef MyVertex MyVector;

struct MyFace {
	S32 v1, v2, v3;
	F32 nx, ny, nz;
};


class NvFaceInfo {
public:
	
	// vertex indices
	NvFaceInfo(S32 v0, S32 v1, S32 v2){
		m_v0 = v0; m_v1 = v1; m_v2 = v2;
		m_stripId      = -1;
		m_testStripId  = -1;
		m_experimentId = -1;
	}
	
	// data members are left public
	S32   m_v0, m_v1, m_v2;
	S32   m_stripId;      // real strip Id
	S32   m_testStripId;  // strip Id in an experiment
	S32   m_experimentId; // in what experiment was it given an experiment Id?
};

// nice and dumb edge class that points knows its
// indices, the two faces, and the next edge using
// the lesser of the indices
class NvEdgeInfo {
public:
	
	// constructor puts 1 ref on us
	NvEdgeInfo (S32 v0, S32 v1){
		m_v0       = v0;
		m_v1       = v1;
		m_face0    = NULL;
		m_face1    = NULL;
		m_nextV0   = NULL;
		m_nextV1   = NULL;
		
		// we will appear in 2 lists.  this is a good
		// way to make sure we delete it the second time
		// we hit it in the edge infos
		m_refCount = 2;    
		
	}
	
	// ref and unref
	void Unref () { if (--m_refCount == 0) delete this; }
	
	// data members are left public
	UINT         m_refCount;
	NvFaceInfo  *m_face0, *m_face1;
	S32          m_v0, m_v1;
	NvEdgeInfo  *m_nextV0, *m_nextV1;
};


// This class is a quick summary of parameters used
// to begin a triangle strip.  Some operations may
// want to create lists of such items, so they were
// pulled out into a class
class NvStripStartInfo {
public:
	NvStripStartInfo(NvFaceInfo *startFace, NvEdgeInfo *startEdge, bool toV1){
		m_startFace    = startFace;
		m_startEdge    = startEdge;
		m_toV1         = toV1;
	}
	NvFaceInfo    *m_startFace;
	NvEdgeInfo    *m_startEdge;
	bool           m_toV1;      
};


typedef std::vector<NvFaceInfo*>     NvFaceInfoVec;
typedef std::list  <NvFaceInfo*>     NvFaceInfoList;
typedef std::list  <NvFaceInfoVec*>  NvStripList;
typedef std::vector<NvEdgeInfo*>     NvEdgeInfoVec;

typedef std::vector<WORD> WordVec;
typedef std::vector<S32> IntVec;
typedef std::vector<MyVertex> MyVertexVec;
typedef std::vector<MyFace> MyFaceVec;

template<class T> 
inline void SWAP(T& first, T& second) 
{
	T temp = first;
	first = second;
	second = temp;
}

// This is a summary of a strip that has been built
class NvStripInfo {
public:
	
	// A little information about the creation of the triangle strips
	NvStripInfo(const NvStripStartInfo &startInfo, S32 stripId, S32 experimentId = -1) :
	  m_startInfo(startInfo)
	{
		m_stripId      = stripId;
		m_experimentId = experimentId;
		visited = false;
		m_numDegenerates = 0;
	}
	  
	// This is an experiment if the experiment id is >= 0
	inline bool IsExperiment () const { return m_experimentId >= 0; }
	  
	inline bool IsInStrip (const NvFaceInfo *faceInfo) const 
	{
		if(faceInfo == NULL)
			return false;
		  
		return (m_experimentId >= 0 ? faceInfo->m_testStripId == m_stripId : faceInfo->m_stripId == m_stripId);
	}
	  
	bool SharesEdge(const NvFaceInfo* faceInfo, NvEdgeInfoVec &edgeInfos);
	  
	// take the given forward and backward strips and combine them together
	void Combine(const NvFaceInfoVec &forward, const NvFaceInfoVec &backward);
	  
	//returns true if the face is "unique", i.e. has a vertex which doesn't exist in the faceVec
	bool Unique(NvFaceInfoVec& faceVec, NvFaceInfo* face);
	  
	// mark the triangle as taken by this strip
	bool IsMarked    (NvFaceInfo *faceInfo);
	void MarkTriangle(NvFaceInfo *faceInfo);
	  
	// build the strip
	void Build(NvEdgeInfoVec &edgeInfos, NvFaceInfoVec &faceInfos);
	  
	// public data members
	NvStripStartInfo m_startInfo;
	NvFaceInfoVec    m_faces;
	S32              m_stripId;
	S32              m_experimentId;
	  
	bool visited;

	S32 m_numDegenerates;
};

typedef std::vector<NvStripInfo*>    NvStripInfoVec;


//The actual stripifier
class NvStripifier {
public:
	
	// Constructor
	NvStripifier();
	~NvStripifier();
	
	//the target vertex cache size, the structure to place the strips in, and the input indices
	void Stripify(const WordVec &in_indices, const S32 in_cacheSize, const S32 in_minStripLength, 
				  const U16 maxIndex, NvStripInfoVec &allStrips, NvFaceInfoVec &allFaces);
	void CreateStrips(const NvStripInfoVec& allStrips, IntVec& stripIndices, const bool bStitchStrips, U32& numSeparateStrips);
	
	static S32 GetUniqueVertexInB(NvFaceInfo *faceA, NvFaceInfo *faceB);
	//static S32 GetSharedVertex(NvFaceInfo *faceA, NvFaceInfo *faceB);
	static void GetSharedVertices(NvFaceInfo *faceA, NvFaceInfo *faceB, S32* vertex0, S32* vertex1);

	static bool IsDegenerate(const NvFaceInfo* face);
	
protected:
	
	WordVec indices;
	S32 cacheSize;
	S32 minStripLength;
	F32 meshJump;
	bool bFirstTimeResetPoint;
	
	/////////////////////////////////////////////////////////////////////////////////
	//
	// Big mess of functions called during stripification
	//
	/////////////////////////////////////////////////////////////////////////////////

	//********************
	bool IsMoneyFace(const NvFaceInfo& face);
	bool FaceContainsIndex(const NvFaceInfo& face, const U32 index);

	bool IsCW(NvFaceInfo *faceInfo, S32 v0, S32 v1);
	bool NextIsCW(const S32 numIndices);
	
	bool IsDegenerate(const U16 v0, const U16 v1, const U16 v2);
	
	static S32  GetNextIndex(const WordVec &indices, NvFaceInfo *face);
	static NvEdgeInfo *FindEdgeInfo(NvEdgeInfoVec &edgeInfos, S32 v0, S32 v1);
	static NvFaceInfo *FindOtherFace(NvEdgeInfoVec &edgeInfos, S32 v0, S32 v1, NvFaceInfo *faceInfo);
	NvFaceInfo *FindGoodResetPoint(NvFaceInfoVec &faceInfos, NvEdgeInfoVec &edgeInfos);
	
	void FindAllStrips(NvStripInfoVec &allStrips, NvFaceInfoVec &allFaceInfos, NvEdgeInfoVec &allEdgeInfos, S32 numSamples);
	void SplitUpStripsAndOptimize(NvStripInfoVec &allStrips, NvStripInfoVec &outStrips, NvEdgeInfoVec& edgeInfos, NvFaceInfoVec& outFaceList);
	void RemoveSmallStrips(NvStripInfoVec& allStrips, NvStripInfoVec& allBigStrips, NvFaceInfoVec& faceList);
	
	bool FindTraversal(NvFaceInfoVec &faceInfos, NvEdgeInfoVec &edgeInfos, NvStripInfo *strip, NvStripStartInfo &startInfo);
	S32  CountRemainingTris(std::list<NvStripInfo*>::iterator iter, std::list<NvStripInfo*>::iterator  end);
	
	void CommitStrips(NvStripInfoVec &allStrips, const NvStripInfoVec &strips);
	
	F32 AvgStripSize(const NvStripInfoVec &strips);
	S32 FindStartPoint(NvFaceInfoVec &faceInfos, NvEdgeInfoVec &edgeInfos);
	
	void UpdateCacheStrip(VertexCache* vcache, NvStripInfo* strip);
	void UpdateCacheFace(VertexCache* vcache, NvFaceInfo* face);
	F32 CalcNumHitsStrip(VertexCache* vcache, NvStripInfo* strip);
	S32 CalcNumHitsFace(VertexCache* vcache, NvFaceInfo* face);
	S32 NumNeighbors(NvFaceInfo* face, NvEdgeInfoVec& edgeInfoVec);
	
	void BuildStripifyInfo(NvFaceInfoVec &faceInfos, NvEdgeInfoVec &edgeInfos, const U16 maxIndex);
	bool AlreadyExists(NvFaceInfo* faceInfo, NvFaceInfoVec& faceInfos);
	
	// let our strip info classes and the other classes get
	// to these protected stripificaton methods if they want
	friend class NvStripInfo;
};

#endif
