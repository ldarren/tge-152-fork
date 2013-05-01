//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CONSTRUCTORSIMPLEMESH_H_
#define _CONSTRUCTORSIMPLEMESH_H_

#include "core/tVector.h"
#include "dgl/dgl.h"
#include "math/mBox.h"
#include "core/fileStream.h"
#include "ts/tsShapeInstance.h"

//#define IS_CONSTRUCTOR


class ConstructorSimpleMesh
{
public:
	class primitive
	{
	public:
		bool alpha;
		U32 texS;
		U32 texT;
		S32 diffuseIndex;
		S32 lightMapIndex;
		U32 start;
		U32 count;
		
      // used to relight the surface in-engine...
      PlaneF lightMapEquationX;
      PlaneF lightMapEquationY;
      Point2I lightMapOffset;
      Point2I lightMapSize;

#ifdef IS_CONSTRUCTOR
		S32 diffuseId;
      S32 lightMapId;
#endif

		primitive()
		{
			alpha = false;
			texS = GL_REPEAT;
			texT = GL_REPEAT;
			diffuseIndex = 0;
			lightMapIndex = 0;
			start = 0;
			count = 0;

         lightMapEquationX = PlaneF(0, 0, 0, 0);
         lightMapEquationY = PlaneF(0, 0, 0, 0);
         lightMapOffset = Point2I(0, 0);
         lightMapSize = Point2I(0, 0);

#ifdef IS_CONSTRUCTOR
			diffuseId = 0;
         lightMapId = 0;
#endif
		}
	};

	ConstructorSimpleMesh()
	{
		materialList = NULL;
		clear();
	}
	~ConstructorSimpleMesh(){clear();}
	void clear(bool wipeMaterials = true)
	{
		hasSolid = false;
		hasTranslucency = false;
		bounds = Box3F(-1, -1, -1, 1, 1, 1);
      transform.identity();
      scale.set(1.0f, 1.0f, 1.0f);

		primitives.clear();
		indices.clear();
		verts.clear();
		norms.clear();
		diffuseUVs.clear();
		lightmapUVs.clear();

		if(wipeMaterials && materialList)
			delete materialList;

      if (wipeMaterials)
		   materialList = NULL;
	}

#ifdef IS_CONSTRUCTOR
	void render(bool transparent, bool texture, bool lightmap);
#else
   void render(bool transparent, bool texture, bool lightmap, bool lightmapinunit0,
      U32 interiorlmhandle, U32 instancelmhandle);
#endif

	void calculateBounds()
	{
		bounds = Box3F(F32_MAX, F32_MAX, F32_MAX, -F32_MAX, -F32_MAX, -F32_MAX);
		for(U32 i=0; i<verts.size(); i++)
		{
			bounds.max.setMax(verts[i]);
			bounds.min.setMin(verts[i]);
		}
	}

	bool hasSolid;
	bool hasTranslucency;
	Box3F bounds;
   MatrixF transform;
   Point3F scale;

	Vector<primitive> primitives;

	// same index relationship...
	Vector<U16> indices;
	Vector<Point3F> verts;
	Vector<Point3F> norms;
	Vector<Point2F> diffuseUVs;
	Vector<Point2F> lightmapUVs;

	TSMaterialList *materialList;

   bool containsPrimitiveType(bool translucent)
   {
      for(U32 i=0; i<primitives.size(); i++)
      {
         if(primitives[i].alpha == translucent)
            return true;
      }
      return false;
   }

	void copyTo(ConstructorSimpleMesh &dest)
	{
		dest.clear();
		dest.hasSolid = hasSolid;
		dest.hasTranslucency = hasTranslucency;
		dest.bounds = bounds;
      dest.transform = transform;
      dest.scale = scale;
		dest.primitives = primitives;
		dest.indices = indices;
		dest.verts = verts;
		dest.norms = norms;
		dest.diffuseUVs = diffuseUVs;
		dest.lightmapUVs = lightmapUVs;

		if(materialList)
			dest.materialList = new TSMaterialList(materialList);
	}
	bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
	bool castPlanes(PlaneF left, PlaneF right, PlaneF top, PlaneF bottom);

   bool read(Stream& stream);
   bool write(Stream& stream) const;
};

#ifdef IS_CONSTRUCTOR
class ConstructorSimpleMeshLoader
{
public:
	static ConstructorSimpleMesh *currentMesh;
	static bool loadSimpleMesh(const char *filename, ConstructorSimpleMesh &mesh);

private:
	static bool loadDTSFile(const char *filename);
	//static bool loadDirectXFile(Stream *stream);
};
#endif

#endif //_CONSTRUCTORSIMPLEMESH_H_

