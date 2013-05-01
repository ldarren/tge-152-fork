//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#ifndef _FORESTCOLLISION_H_
#define _FORESTCOLLISION_H_

class Forest;
class ForestItem;
class ForestItemData;
class TSShapeInstance;

#include "collision/convex.h"

//----------------------------------------------------------------------------

class ForestConvex : public Convex
{
   typedef Convex Parent;
   friend class Forest;

protected:
   Forest*          mForest;
   ForestItem*      mItem;
   ForestItemData  *mData;
   TSShapeInstance* mShapeInstance;
   MatrixF          mTransform;
   Point3F          mObjScale;

public:
   U32       hullId;
   Box3F     box;

   ForestConvex() 
   {
      mType = ForestConvexType;
   }

   ForestConvex(const ForestConvex& cv) 
   {
      mType       = ForestConvexType;
      mObject     = cv.mObject;
      mForest     = cv.mForest;
      mData       = cv.mData;
      mItem       = cv.mItem;
      mShapeInstance = cv.mShapeInstance;
      mTransform  = cv.mTransform;
      hullId      = cv.hullId;
      box         = cv.box;
      mObjScale   = cv.mObjScale;
   }

   // Helper code
   void calculateTransform(const MatrixF &worldXfrm);

   // Convex interface
   const MatrixF& getTransform() const;
   Box3F          getBoundingBox() const;
   Box3F          getBoundingBox(const MatrixF& mat, const Point3F& scale) const;
   Point3F        support(const VectorF& v) const;
   void           getFeatures(const MatrixF& mat,const VectorF& n, ConvexFeature* cf);
   void           getPolyList(AbstractPolyList* list);
};

inline const MatrixF &ForestConvex::getTransform() const 
{
   return mTransform;
}

#endif