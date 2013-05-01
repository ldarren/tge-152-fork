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

#include "game/physicalZone.h"
#include "core/bitStream.h"
#include "collision/boxConvex.h"
#include "collision/clippedPolyList.h"
#include "console/consoleTypes.h"
#include "math/mathIO.h"
#include "dgl/dgl.h"
#include "sceneGraph/sceneState.h"

IMPLEMENT_CO_NETOBJECT_V1(PhysicalZone);

ConsoleMethod(PhysicalZone, activate, void, 2, 2, "Activate the physical zone's effects.")
{
   if (object->isClientObject())
      return;

   object->activate();
}

ConsoleMethod(PhysicalZone, deactivate, void, 2, 2, "Deactivate the physical zone's effects.")
{
   if (object->isClientObject())
      return;

   object->deactivate();
}


//--------------------------------------------------------------------------
//--------------------------------------
//
PhysicalZone::PhysicalZone()
{
   mNetFlags.set(Ghostable | ScopeAlways);

   mTypeMask |= PhysicalZoneObjectType;

   mVelocityMod = 1.0f;
   mGravityMod  = 1.0f;
   mAppliedForce.set(0, 0, 0);

   mConvexList = new Convex;
   mActive = true;

   // AFX CODE BLOCK (enhanced-physical-zone) <<
   force_type = VECTOR;
   force_mag = 0.0f;
   orient_force = false;
   fade_amt = 1.0f;
   // AFX CODE BLOCK (enhanced-physical-zone) >>
}

PhysicalZone::~PhysicalZone()
{
   delete mConvexList;
   mConvexList = NULL;
}

// AFX CODE BLOCK (enhanced-physical-zone) <<
static EnumTable::Enums forces_enums[] =
{
   { PhysicalZone::VECTOR,          "vector"        },
   { PhysicalZone::SPHERICAL,       "spherical"     },
   { PhysicalZone::CYLINDRICAL,     "cylindrical"   },
   // aliases
   { PhysicalZone::SPHERICAL,       "sphere"        },
   { PhysicalZone::CYLINDRICAL,     "cylinder"      },
};
static EnumTable forces_tbl(sizeof(forces_enums) / sizeof(forces_enums[0]), forces_enums);
// AFX CODE BLOCK (enhanced-physical-zone) >>

//--------------------------------------------------------------------------
void PhysicalZone::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Misc");	
   addField("velocityMod",  TypeF32,               Offset(mVelocityMod,  PhysicalZone));
   addField("gravityMod",   TypeF32,               Offset(mGravityMod,   PhysicalZone));
   addField("appliedForce", TypePoint3F,           Offset(mAppliedForce, PhysicalZone));
   addField("polyhedron",   TypeTriggerPolyhedron, Offset(mPolyhedron,   PhysicalZone));
   endGroup("Misc");	
   // AFX CODE BLOCK (enhanced-physical-zone) <<
   addField("forceType",    TypeEnum,              Offset(force_type,    PhysicalZone), 1, &forces_tbl);
   addField("orientForce",  TypeBool,              Offset(orient_force,  PhysicalZone));
   // AFX CODE BLOCK (enhanced-physical-zone) >>
}

//--------------------------------------------------------------------------
bool PhysicalZone::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (mVelocityMod < -40.0f || mVelocityMod > 40.0f) {
      Con::errorf("PhysicalZone: velocity mod out of range.  [-40, 40]");
      mVelocityMod = mVelocityMod < -40.0f ? -40.0f : 40.0f;
   }
   if (mGravityMod < -40.0f || mGravityMod > 40.0f) {
      Con::errorf("PhysicalZone: GravityMod out of range.  [-40, 40]");
      mGravityMod = mGravityMod < -40.0f ? -40.0f : 40.0f;
   }
   static const char* coordString[] = { "x", "y", "z" };
   F32* p = mAppliedForce;
   for (U32 i = 0; i < 3; i++) {
      if (p[i] < -40000.0f || p[i] > 40000.0f) {
         Con::errorf("PhysicalZone: applied force: %s out of range.  [-40000, 40000]", coordString[i]);
         p[i] = p[i] < -40000.0f ? -40000.0f : 40000.0f;
      }
   }

   Polyhedron temp = mPolyhedron;
   setPolyhedron(temp);

   // AFX CODE BLOCK (enhanced-physical-zone) <<
   switch (force_type)
   {
   case SPHERICAL:
     force_mag = mAppliedForce.magnitudeSafe();
     break;
   case CYLINDRICAL:
     {
       Point3F force_vec = mAppliedForce;
       force_vec.z = 0.0;
       force_mag = force_vec.magnitudeSafe();
     }
     break;
   }
   // AFX CODE BLOCK (enhanced-physical-zone) >>

   addToScene();

   return true;
}


void PhysicalZone::onRemove()
{
   mConvexList->nukeList();

   removeFromScene();
   Parent::onRemove();
}


//------------------------------------------------------------------------------
void PhysicalZone::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   MatrixF base(true);
   base.scale(Point3F(1.0/mObjScale.x,
                        1.0/mObjScale.y,
                        1.0/mObjScale.z));
   base.mul(mWorldToObj);
   mClippedList.setBaseTransform(base);

 #ifdef AFX_CAP_PHYSICAL_ZONE_OPTS // AFX CODE BLOCK (pz-opt) <<
   if (isServerObject())
      setMaskBits(MoveMask);
 #else // ORIGINAL CODE
   if (isServerObject())
      setMaskBits(InitialUpdateMask);
 #endif  // AFX CODE BLOCK (pz-opt) >>
}


//--------------------------------------------------------------------------
U32 PhysicalZone::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
   U32 i;
   U32 retMask = Parent::packUpdate(con, mask, stream);

 #ifdef AFX_CAP_PHYSICAL_ZONE_OPTS // AFX CODE BLOCK (pz-opt) <<
   if (stream->writeFlag(mask & PolyhedronMask)) 
   {
      //Con::printf("### PZ UPDATE POLYHEDRON (S)");

      // Write the polyhedron
      stream->write(mPolyhedron.pointList.size());
      for (i = 0; i < mPolyhedron.pointList.size(); i++)
         mathWrite(*stream, mPolyhedron.pointList[i]);

      stream->write(mPolyhedron.planeList.size());
      for (i = 0; i < mPolyhedron.planeList.size(); i++)
         mathWrite(*stream, mPolyhedron.planeList[i]);

      stream->write(mPolyhedron.edgeList.size());
      for (i = 0; i < mPolyhedron.edgeList.size(); i++) {
         const Polyhedron::Edge& rEdge = mPolyhedron.edgeList[i];

         stream->write(rEdge.face[0]);
         stream->write(rEdge.face[1]);
         stream->write(rEdge.vertex[0]);
         stream->write(rEdge.vertex[1]);
      }
   }

   if (stream->writeFlag(mask & MoveMask))
   {
      //Con::printf("### PZ UPDATE TRANSFORM (S)");
      stream->writeAffineTransform(mObjToWorld);
      mathWrite(*stream, mObjScale);
   }

   if (stream->writeFlag(mask & SettingsMask))
   {
      //Con::printf("### PZ UPDATE SETTINGS (S)");

      stream->write(mVelocityMod);
      stream->write(mGravityMod);
      mathWrite(*stream, mAppliedForce);
      stream->writeInt(force_type, FORCE_TYPE_BITS); // AFX
      stream->writeFlag(orient_force); // AFX
   }

   if (stream->writeFlag(mask & FadeMask))
   {
      //Con::printf("### PZ UPDATE FADE (S)");
      U8 fade_byte = (U8)(fade_amt*255.0f);
      stream->write(fade_byte);
   }

   //Con::printf("### PZ UPDATE ACTIVE (S)");
   stream->writeFlag(mActive);
 #else // ORIGINAL CODE
   if (stream->writeFlag((mask & InitialUpdateMask) != 0)) {
      // Note that we don't really care about efficiency here, since this is an
      //  edit-only ghost...
      mathWrite(*stream, mObjToWorld);
      mathWrite(*stream, mObjScale);

      // Write the polyhedron
      stream->write(mPolyhedron.pointList.size());
      for (i = 0; i < mPolyhedron.pointList.size(); i++)
         mathWrite(*stream, mPolyhedron.pointList[i]);

      stream->write(mPolyhedron.planeList.size());
      for (i = 0; i < mPolyhedron.planeList.size(); i++)
         mathWrite(*stream, mPolyhedron.planeList[i]);

      stream->write(mPolyhedron.edgeList.size());
      for (i = 0; i < mPolyhedron.edgeList.size(); i++) {
         const Polyhedron::Edge& rEdge = mPolyhedron.edgeList[i];

         stream->write(rEdge.face[0]);
         stream->write(rEdge.face[1]);
         stream->write(rEdge.vertex[0]);
         stream->write(rEdge.vertex[1]);
      }

      stream->write(mVelocityMod);
      stream->write(mGravityMod);
      mathWrite(*stream, mAppliedForce);
      stream->writeFlag(mActive);
      // AFX CODE BLOCK (enhanced-physical-zone) <<
      U8 fade_byte = (U8)(fade_amt*255.0f);
      stream->write(fade_byte);
      stream->writeInt(force_type, FORCE_TYPE_BITS);
      stream->writeFlag(orient_force);
      // AFX CODE BLOCK (enhanced-physical-zone) >>
   } else {
      stream->writeFlag(mActive);
   }
 #endif  // AFX CODE BLOCK (pz-opt) >>

   return retMask;
}

void PhysicalZone::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

 #ifdef AFX_CAP_PHYSICAL_ZONE_OPTS // AFX CODE BLOCK (pz-opt) <<
   bool new_ph = false;
   if (stream->readFlag()) // PolyhedronMask
   {
      //Con::printf("### PZ UPDATE POLYHEDRON (C)");

      U32 i, size;
      Polyhedron tempPH;

      // Read the polyhedron
      stream->read(&size);
      tempPH.pointList.setSize(size);
      for (i = 0; i < tempPH.pointList.size(); i++)
         mathRead(*stream, &tempPH.pointList[i]);

      stream->read(&size);
      tempPH.planeList.setSize(size);
      for (i = 0; i < tempPH.planeList.size(); i++)
         mathRead(*stream, &tempPH.planeList[i]);

      stream->read(&size);
      tempPH.edgeList.setSize(size);
      for (i = 0; i < tempPH.edgeList.size(); i++) {
         Polyhedron::Edge& rEdge = tempPH.edgeList[i];

         stream->read(&rEdge.face[0]);
         stream->read(&rEdge.face[1]);
         stream->read(&rEdge.vertex[0]);
         stream->read(&rEdge.vertex[1]);
      }

      setPolyhedron(tempPH);
      new_ph = true;
   }

   if (stream->readFlag()) // MoveMask
   {
      //Con::printf("### PZ UPDATE TRANSFORM (C)");

      MatrixF temp;
      stream->readAffineTransform(&temp);

      Point3F tempScale;
      mathRead(*stream, &tempScale);

      //if (!new_ph)
      //{
      //  Polyhedron rPolyhedron = mPolyhedron;
      //  setPolyhedron(rPolyhedron);
      //}
      setScale(tempScale);
      setTransform(temp);
   }

   if (stream->readFlag()) //SettingsMask
   {
      //Con::printf("### PZ UPDATE SETTINGS (C)");

      stream->read(&mVelocityMod);
      stream->read(&mGravityMod);
      mathRead(*stream, &mAppliedForce);
      force_type = stream->readInt(FORCE_TYPE_BITS); // AFX
      orient_force = stream->readFlag(); // AFX
   }

   if (stream->readFlag()) //FadeMask
   {
      //Con::printf("### PZ UPDATE FADE (C)");
      
      U8 fade_byte;
      stream->read(&fade_byte);
      fade_amt = ((F32)fade_byte)/255.0f;
   }
   else
     fade_amt = 1.0f;

   //Con::printf("### PZ UPDATE ACTIVE (C)");
   mActive = stream->readFlag();
 #else // ORIGINAL CODE
   if (stream->readFlag()) {
      U32 i, size;
      MatrixF temp;
      Point3F tempScale;
      Polyhedron tempPH;

      // Transform
      mathRead(*stream, &temp);
      mathRead(*stream, &tempScale);

      // Read the polyhedron
      stream->read(&size);
      tempPH.pointList.setSize(size);
      for (i = 0; i < tempPH.pointList.size(); i++)
         mathRead(*stream, &tempPH.pointList[i]);

      stream->read(&size);
      tempPH.planeList.setSize(size);
      for (i = 0; i < tempPH.planeList.size(); i++)
         mathRead(*stream, &tempPH.planeList[i]);

      stream->read(&size);
      tempPH.edgeList.setSize(size);
      for (i = 0; i < tempPH.edgeList.size(); i++) {
         Polyhedron::Edge& rEdge = tempPH.edgeList[i];

         stream->read(&rEdge.face[0]);
         stream->read(&rEdge.face[1]);
         stream->read(&rEdge.vertex[0]);
         stream->read(&rEdge.vertex[1]);
      }

      stream->read(&mVelocityMod);
      stream->read(&mGravityMod);
      mathRead(*stream, &mAppliedForce);

      setPolyhedron(tempPH);
      setScale(tempScale);
      setTransform(temp);
      mActive = stream->readFlag();
      // AFX CODE BLOCK (enhanced-physical-zone) <<
      U8 fade_byte;
      stream->read(&fade_byte);
      fade_amt = ((F32)fade_byte)/255.0f;
      force_type = stream->readInt(FORCE_TYPE_BITS);
      orient_force = stream->readFlag();
      // AFX CODE BLOCK (enhanced-physical-zone) >>
   } else {
      mActive = stream->readFlag();
   }
 #endif  // AFX CODE BLOCK (pz-opt) >>
}


//--------------------------------------------------------------------------
void PhysicalZone::setPolyhedron(const Polyhedron& rPolyhedron)
{
   mPolyhedron = rPolyhedron;

   if (mPolyhedron.pointList.size() != 0) {
      mObjBox.min.set(1e10, 1e10, 1e10);
      mObjBox.max.set(-1e10, -1e10, -1e10);
      for (U32 i = 0; i < mPolyhedron.pointList.size(); i++) {
         mObjBox.min.setMin(mPolyhedron.pointList[i]);
         mObjBox.max.setMax(mPolyhedron.pointList[i]);
      }
   } else {
      mObjBox.min.set(-0.5, -0.5, -0.5);
      mObjBox.max.set( 0.5,  0.5,  0.5);
   }

   MatrixF xform = getTransform();
   setTransform(xform);

   mClippedList.clear();
   mClippedList.mPlaneList = mPolyhedron.planeList;

   MatrixF base(true);
   base.scale(Point3F(1.0/mObjScale.x,
                      1.0/mObjScale.y,
                      1.0/mObjScale.z));
   base.mul(mWorldToObj);

   mClippedList.setBaseTransform(base);
}


//--------------------------------------------------------------------------
void PhysicalZone::buildConvex(const Box3F& box, Convex* convex)
{
   // These should really come out of a pool
   mConvexList->collectGarbage();

   Box3F realBox = box;
   mWorldToObj.mul(realBox);
   realBox.min.convolveInverse(mObjScale);
   realBox.max.convolveInverse(mObjScale);

   if (realBox.isOverlapped(getObjBox()) == false)
      return;

   // Just return a box convex for the entire shape...
   Convex* cc = 0;
   CollisionWorkingList& wl = convex->getWorkingList();
   for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
      if (itr->mConvex->getType() == BoxConvexType &&
          itr->mConvex->getObject() == this) {
         cc = itr->mConvex;
         break;
      }
   }
   if (cc)
      return;

   // Create a new convex.
   BoxConvex* cp = new BoxConvex;
   mConvexList->registerObject(cp);
   convex->addToWorkingList(cp);
   cp->init(this);

   mObjBox.getCenter(&cp->mCenter);
   cp->mSize.x = mObjBox.len_x() / 2.0f;
   cp->mSize.y = mObjBox.len_y() / 2.0f;
   cp->mSize.z = mObjBox.len_z() / 2.0f;
}


bool PhysicalZone::testObject(SceneObject* enter)
{
   if (mPolyhedron.pointList.size() == 0)
      return false;

   mClippedList.clear();

   SphereF sphere;
   sphere.center = (mWorldBox.min + mWorldBox.max) * 0.5;
   VectorF bv = mWorldBox.max - sphere.center;
   sphere.radius = bv.len();

   enter->buildPolyList(&mClippedList, mWorldBox, sphere);
   return mClippedList.isEmpty() == false;
}


//--------------------------------------
void PhysicalZone::activate()
{
   AssertFatal(isServerObject(), "Client objects not allowed in ForceFieldInstance::open()");

   if (mActive != true)
      setMaskBits(ActiveMask);
   mActive = true;
}

void PhysicalZone::deactivate()
{
   AssertFatal(isServerObject(), "Client objects not allowed in ForceFieldInstance::close()");

   if (mActive != false)
      setMaskBits(ActiveMask);
   mActive = false;
}

// AFX CODE BLOCK (enhanced-physical-zone) <<
void PhysicalZone::onStaticModified(const char* slotName)
{
  if (dStricmp(slotName, "appliedForce") == 0 || dStricmp(slotName, "forceType") == 0)
  {
    switch (force_type)
    {
    case SPHERICAL:
      force_mag = mAppliedForce.magnitudeSafe();
      break;
    case CYLINDRICAL:
      {
        Point3F force_vec = mAppliedForce;
        force_vec.z = 0.0;
        force_mag = force_vec.magnitudeSafe();
      }
      break;
    }
  }
}

const Point3F& PhysicalZone::getForce(const SceneObject* obj) const 
{ 
  static Point3F force_vec;

  if (force_type == VECTOR)
  {
    if (orient_force)
    {
      getTransform().mulV(mAppliedForce, &force_vec);
      force_vec *= fade_amt;
      return force_vec; 
    }
    force_vec = mAppliedForce;
    force_vec *= fade_amt;
    return force_vec;
  }

  if (!obj)
  {
    force_vec.zero();
    return force_vec; 
  }

  if (force_type == SPHERICAL)
  {
    force_vec = obj->getPosition() - getPosition();
    force_vec.normalizeSafe();
    force_vec *= force_mag*fade_amt;
    return force_vec;
  }

  // force_type == CYLINDRICAL

  if (orient_force)
  {
    force_vec = obj->getPosition() - getPosition();
    getWorldTransform().mulV(force_vec);
    force_vec.z = 0.0f;
    force_vec.normalizeSafe();
    force_vec *= force_mag;
    force_vec.z = mAppliedForce.z;
    getTransform().mulV(force_vec);
    force_vec *= fade_amt;
    return force_vec;
  }

  force_vec = obj->getPosition() - getPosition();
  force_vec.z = 0.0f;
  force_vec.normalizeSafe();
  force_vec *= force_mag;
  force_vec.z = mAppliedForce.z;
  force_vec *= fade_amt;
  return force_vec;
}
// AFX CODE BLOCK (enhanced-physical-zone) >>