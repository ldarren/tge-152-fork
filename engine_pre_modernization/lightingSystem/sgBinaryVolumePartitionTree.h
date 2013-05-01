//---------------------------------------------------------------
// Synapse Gaming - Binary Volume Partition Tree
// Copyright © Synapse Gaming 2004 - 2005
// Written by John Kabus
//
// Overview:
//  The Binary Volume Partition Tree (BVPT) is a hybrid of the
//  BSP and Oct-tree spacial partitioning schemes that combines
//  the strengths of both into a system that can efficiently
//  processes large amounts of geometry and geometry with large
//  surface area, which allows optimal handling of both brush
//  and mesh geometry.
//
//  Similar to Oct-tree partitioning, BVPT splits the geometric
//  'world' into equal volumes and each volume into sub-volumes
//  creating a tree that assigns each surface or object into the
//  smallest volume that completely contains it.
//
//  The main disadvantage to using Oct-trees is that all geometry
//  that rides the border between *any* two octants is maintained
//  by the parent volume, which means the geometry is used every
//  time the parent volume is traversed regardless of the
//  relationship to the requested octant.
//
//  BVPT avoids Oct-tree inefficiencies by splitting volumes in
//  half (instead of into eighths).  Because of this all geometry
//  maintained by the parent volume spans both of the two sub-volumes,
//  meaning that the parent geometry has a definite relationship
//  to the requested sub-volume.
//---------------------------------------------------------------
#ifndef BVPT_H_
#define BVPT_H_

#include "platform/types.h"
#include "core/color.h"
#include "math/mPoint.h"
#include "math/mBox.h"
#include "math/mPlane.h"
#include "core/tVector.h"


template<class Tstoreobj> class BVPT
{
public:
	enum axisType
	{
		atX = 0,
		atY = 1,
		atZ = 2,
		atNone = 3
	};
	
	typedef Vector<Tstoreobj> objectList;
	
	// used for spacial partitioning...
	PlaneF plane;
	
	// objects contained in this partition...
	objectList object;
	
	// children in the tree...
	BVPT *positive;
	BVPT *negative;
	
	// only used for passing info into children...
	axisType axis;
   axisType avoidAxis;
	// only used for passing info into children...
	Box3F volume;


	BVPT()
	{
		positive = negative = NULL;
		clear();
	}	
	BVPT(const Box3F &vol)
	{
		positive = negative = NULL;
		init(vol);
	}
	BVPT(const Box3F &vol, axisType avoidaxis)
	{
		positive = negative = NULL;
		init(vol, avoidaxis);
	}
	~BVPT() {clear();}
   void init(const Box3F &vol)
   {
      init(vol, atNone);
   }
	void init(const Box3F &vol, axisType avoidaxis)
	{
		clear();
		volume = vol;
      avoidAxis = avoidaxis;
		calculatePartition();
	}
	void clear()
	{
		axis = atNone;
      avoidAxis = atNone;
		plane.set(0.0f, 0.0f, 0.0f);
      plane.d = 0.0f;
		volume.min.set(F32_MAX, F32_MAX, F32_MAX);
		volume.max.set(-F32_MAX, -F32_MAX, -F32_MAX);
		object.clear();
		if(positive)
		{
			delete positive;
			positive = NULL;
		}
		if(negative)
		{
			delete negative;
			negative = NULL;
		}
	}
	void calculatePartition()
	{
		Point3F span = volume.max - volume.min;
		F32 bestspan = 0;
		axisType bestaxis = atNone;
		
		for(S32 i=0; i<3; i++)
		{
			if((bestspan < span[i]) && (axisType(i) != avoidAxis))
			{
				bestspan = span[i];
				bestaxis = axisType(i);
			}
		}
		
		axis = bestaxis;
		plane.set(0.0f, 0.0f, 0.0f);
		plane[axis] = 1.0f;
		plane.d = -(volume.min[axis] + (bestspan * 0.5f));
	}
	void storeObject(const Box3F &boundingbox, const Tstoreobj obj)
	{
		bool frontmin = plane.distToPlane(boundingbox.min) > 0.0f;
		bool frontmax = plane.distToPlane(boundingbox.max) > 0.0f;
		
		if(frontmin != frontmax)
		{
			// only fully contained by this volume...
			object.push_back(obj);
		}
		else if(frontmin)
		{
			// positive side...
			// setup child...
			Box3F vol = volume;
			vol.min[axis] = -plane.d;
			if(!positive)
				positive = new BVPT(vol, avoidAxis);
				
			// send down the object...
			positive->storeObject(boundingbox, obj);
		}
		else
		{
			// negative side...
			// setup child...
			Box3F vol = volume;
			vol.max[axis] = -plane.d;
			if(!negative)
				negative = new BVPT(vol, avoidAxis);
				
			// send down the object...
			negative->storeObject(boundingbox, obj);
		}
	}
	void collectObjects(const Box3F &boundingbox, objectList &objectslist)
	{
		// we're here so collect the objects...
		U32 originalcount = objectslist.size();
		objectslist.increment(object.size());
		if(object.size() > 0)
		{
			dMemcpy((objectslist.address() + originalcount), object.address(), (object.size() * sizeof(Tstoreobj)));
		}

		bool frontmin = plane.distToPlane(boundingbox.min) > 0.0f;
		bool frontmax = plane.distToPlane(boundingbox.max) > 0.0f;
		
		if((frontmin || frontmax) && positive)
			positive->collectObjects(boundingbox, objectslist);

		if((!frontmin || !frontmax) && negative)
			negative->collectObjects(boundingbox, objectslist);
	}
	// clips line to outer volume...
	void collectObjectsClipped(Point3F start, Point3F end, objectList &objectslist)
	{
		F32 t;
		Point3F vect;
		if(!volume.isContained(start))
		{
			if(!volume.collideLine(start, end, &t, &vect))
				return;// we missed the whole volume...
			if((t < 0.0f) || (t > 1.0f))
				return;// we missed the whole volume...
			vect = end - start;
			start += (vect * t);
		}
		if(!volume.isContained(end))
		{
			if(!volume.collideLine(end, start, &t, &vect))
				return;// we missed the whole volume...
			if((t < 0.0f) || (t > 1.0f))
				return;// we missed the whole volume...
			vect = start - end;
			end += (vect * t);
		}
		collectObjectsUnclipped(start, end, objectslist);
	}
	// assumes the line has been clipped to the outer volume!!!
	void collectObjectsUnclipped(const Point3F &start, const Point3F &end, objectList &objectslist)
	{
		// we're here so collect the objects...
		U32 originalcount = objectslist.size();
		objectslist.increment(object.size());
		if(object.size() > 0)
		{
			dMemcpy((objectslist.address() + originalcount), object.address(), (object.size() * sizeof(Tstoreobj)));
		}
			
		// do we have children?
		if((!positive) && (!negative))
			return;
			
		// test for sides...
		F32 diststart = plane.distToPlane(start);
		bool fronts = diststart > 0.0f;
		bool fronte = plane.distToPlane(end) > 0.0f;
		
		if(fronts == fronte)// same side?
		{
			if(fronts && positive)// in the front...
				positive->collectObjectsUnclipped(start, end, objectslist);
			else if((!fronts) && negative)// might be the back...
				negative->collectObjectsUnclipped(start, end, objectslist);
		}
		else
		{
			// find the split...
			Point3F split = end - start;
			F32 t = mDot(split, plane);
			if(t == 0)
				return;
			t = -diststart / t;
			if(t > 0.0f)
			{
				split *= t;
				split += start;
			}
			else
			{
				split = start;
			}

			// setup the orientation...
			const Point3F *f;
			const Point3F *b;
			if(fronts)
			{
				f = &start;
				b = &end;
			}
			else
			{
				f = &end;
				b = &start;
			}

			// run the two new lines through the children...
			if(positive)
				positive->collectObjectsUnclipped((*f), split, objectslist);
			if(negative)
				negative->collectObjectsUnclipped(split, (*b), objectslist);
		}
	}
};


#endif//BVPT_H_

