//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//           enhanced-physical-zone -- PhysicalZone object enhanced to allow orientation
//               add radial forces.
//           pz-opt -- PhysicalZone network optimizations.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _H_PHYSICALZONE
#define _H_PHYSICALZONE

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif
#ifndef _EARLYOUTPOLYLIST_H_
#include "collision/earlyOutPolyList.h"
#endif

#ifndef _H_TRIGGER
#include "game/trigger.h"
#endif

#define AFX_CAP_PHYSICAL_ZONE_OPTS

class Convex;

// -------------------------------------------------------------------------
class PhysicalZone : public SceneObject
{
   typedef SceneObject Parent;

  protected:
   bool onAdd();
   void onRemove();

 #ifdef AFX_CAP_PHYSICAL_ZONE_OPTS // AFX CODE BLOCK (pz-opt) <<
   enum UpdateMasks {
      InitialUpdateMask = BIT(0),
      ActiveMask        = BIT(1),
      SettingsMask      = BIT(2),
      FadeMask          = BIT(3),
      PolyhedronMask    = BIT(4),
      MoveMask          = BIT(5),
   };
#else // ORIGINAL CODE
   enum UpdateMasks {
      InitialUpdateMask = 1 << 0,
      ActiveMask        = 1 << 1
   };
#endif // AFX CODE BLOCK (pz-opt) >>

  public:
   void setTransform(const MatrixF &mat);

  protected:
   F32        mVelocityMod;
   F32        mGravityMod;
   Point3F    mAppliedForce;

   // Basically ripped from trigger
   Polyhedron           mPolyhedron;
   EarlyOutPolyList     mClippedList;

   bool mActive;

   Convex* mConvexList;
   void buildConvex(const Box3F& box, Convex* convex);

  public:
   PhysicalZone();
   ~PhysicalZone();

   F32 getVelocityMod() const      { return mVelocityMod; }
   F32 getGravityMod()  const      { return mGravityMod;  }
   // AFX CODE BLOCK (enhanced-physical-zone) <<
   // the scene object is now passed in to getForce() where
   // it is needed to calculate the applied force when the
   // force is radial.
   const Point3F& getForce(const SceneObject*) const;
   /* ORIGINAL CODE
   const Point3F& getForce() const { return mAppliedForce; }
   */
   // AFX CODE BLOCK (enhanced-physical-zone) >>

   void setPolyhedron(const Polyhedron&);
   bool testObject(SceneObject*);

   void activate();
   void deactivate();
   bool isActive() const { return mActive; }

   DECLARE_CONOBJECT(PhysicalZone);
   static void initPersistFields();

   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);
   // AFX CODE BLOCK (enhanced-physical-zone) <<
public:
   enum { VECTOR, SPHERICAL, CYLINDRICAL };
   enum { FORCE_TYPE_BITS = 2 };
   virtual void onStaticModified(const char* slotName);
protected:
   friend class afxPhysicalZoneData;
   friend class afxEA_PhysicalZone;
   S32    force_type;
   F32    force_mag;
   bool   orient_force;
   F32    fade_amt;
#ifdef AFX_CAP_PHYSICAL_ZONE_OPTS // AFX CODE BLOCK (pz-opt) <<
   void   setFadeAmount(F32 amt) { fade_amt = amt; if (fade_amt < 1.0f) setMaskBits(FadeMask); }
#else
   void   setFadeAmount(F32 amt) { fade_amt = amt; if (fade_amt < 1.0f) setMaskBits(InitialUpdateMask); }
#endif // AFX CODE BLOCK (pz-opt) >>
   // AFX CODE BLOCK (enhanced-physical-zone) >>
};

#endif // _H_PHYSICALZONE

