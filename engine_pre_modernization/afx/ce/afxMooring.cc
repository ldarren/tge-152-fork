
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "sceneGraph/sceneState.h"
#include "dgl/dgl.h"
#include "math/mathIO.h"

#include "afx/afxChoreographer.h"
#include "afx/ce/afxMooring.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMooringData

IMPLEMENT_CONSOLETYPE(afxMooringData)
IMPLEMENT_GETDATATYPE(afxMooringData)
IMPLEMENT_SETDATATYPE(afxMooringData)
IMPLEMENT_CO_DATABLOCK_V1(afxMooringData);

afxMooringData::afxMooringData()
{
  track_pos_only = false;
  networking = SCOPE_ALWAYS;
  display_axis_marker = false;
}

#define myOffset(field) Offset(field, afxMooringData)

void afxMooringData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxMooringDataPtr", TypeafxMooringDataPtr, sizeof(afxMooringData*),
                    REF_GETDATATYPE(afxMooringData), REF_SETDATATYPE(afxMooringData));
#endif

  addField("displayAxisMarker",   TypeBool,     myOffset(display_axis_marker));
  addField("trackPosOnly",        TypeBool,     myOffset(track_pos_only));
  addField("networking",          TypeS8,       myOffset(networking));
}

bool afxMooringData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  return true;
}

void afxMooringData::packData(BitStream* stream)
{
	Parent::packData(stream);
  stream->write(display_axis_marker);
  stream->write(track_pos_only);
  stream->write(networking);
}

void afxMooringData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&display_axis_marker);
  stream->read(&track_pos_only);
  stream->read(&networking);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMooring

IMPLEMENT_CO_NETOBJECT_V1(afxMooring);

afxMooring::afxMooring()
{
  mNetFlags.set(Ghostable | ScopeAlways);

  chor_id = 0;
  hookup_with_chor = false;
  ghost_cons_name = ST_NULLSTRING;
}

afxMooring::afxMooring(U32 networking, U32 chor_id, StringTableEntry cons_name)
{
  if (networking & SCOPE_ALWAYS)
  {
    mNetFlags.clear();
    mNetFlags.set(Ghostable | ScopeAlways);
  }
  else if (networking & GHOSTABLE)
  {
    mNetFlags.clear();
    mNetFlags.set(Ghostable);
  }
  else if (networking & SERVER_ONLY)
  {
    mNetFlags.clear();
  }
  else // if (networking & CLIENT_ONLY)
  {
    mNetFlags.clear();
    mNetFlags.set(IsGhost);
  }

  this->chor_id = chor_id;
  hookup_with_chor = false;
  this->ghost_cons_name = cons_name;
}

afxMooring::~afxMooring()
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxMooring::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxMooringData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;

  return true;
}

void afxMooring::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  if (hookup_with_chor)
  {
    afxChoreographer* chor = arcaneFX::findClientChoreographer(chor_id);
    if (chor)
    {
      chor->setGhostConstraintObject(this, ghost_cons_name);
      hookup_with_chor = false;
    }
  }

  Point3F pos = getRenderPosition();
}

U32 afxMooring::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(conn, mask, stream);
  
  // InitialUpdate
  if (stream->writeFlag(mask & InitialUpdateMask)) 
  {
    stream->write(chor_id);
    stream->writeString(ghost_cons_name);
  }
  
  if (stream->writeFlag(mask & PositionMask)) 
  {
    if (mDataBlock->track_pos_only)
      mathWrite(*stream, mObjToWorld.getPosition());
    else
      stream->writeAffineTransform(mObjToWorld);
  } 
  
  return retMask;
}

//~~~~~~~~~~~~~~~~~~~~//

void afxMooring::setTransform(const MatrixF& mat)
{
   Parent::setTransform(mat);
   setMaskBits(PositionMask);
}

void afxMooring::unpackUpdate(NetConnection * conn, BitStream * stream)
{
  Parent::unpackUpdate(conn, stream);
  
  // InitialUpdate
  if (stream->readFlag())
  {
    stream->read(&chor_id);
    ghost_cons_name = stream->readSTString();
    
    if (chor_id != 0 && ghost_cons_name != ST_NULLSTRING)
      hookup_with_chor = true;
  }
  
  if (stream->readFlag()) 
  {
    if (mDataBlock->track_pos_only)
    {
      Point3F pos;
      mathRead(*stream, &pos);
      setPosition(pos);
    }
    else
    {
      MatrixF mat;
      stream->readAffineTransform(&mat);
      setTransform(mat);
      setRenderTransform(mat);
    }
  }
}

bool afxMooring::onAdd()
{
  if(!Parent::onAdd())
    return false;
  
  addToScene();
  
  return true;
}

void afxMooring::onRemove()
{
  removeFromScene();
  
  Parent::onRemove();
}

bool afxMooring::prepRenderImage(SceneState* state, const U32 stateKey,
                                 const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
  if (!mDataBlock->display_axis_marker || isLastState(state, stateKey))
    return false;
  setLastState(state, stateKey);
  
  // This should be sufficient for most objects that don't manage zones, and
  //  don't need to return a specialized RenderImage...
  if (state->isObjectRendered(this)) {
    SceneRenderImage* image = new SceneRenderImage;
    image->obj = this;
    image->isTranslucent = true;
    image->sortType = SceneRenderImage::Point;
    state->insertRenderImage(image);
  }
  
  return false;
}

void afxMooring::renderObject(SceneState* state, SceneRenderImage*)
{
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");
  
  RectI viewport;
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  dglGetViewport(&viewport);
  
  // Uncomment this if this is a "simple" (non-zone managing) object
  state->setupObjectProjection(this);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  dglMultMatrix(&getRenderTransform());
  glScalef(mObjScale.x, mObjScale.y, mObjScale.z);
  
  // DRAW HERE
  //glDisable(GL_DEPTH_TEST);

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(-0.5, 0, 0); glVertex3f(0.5, 0, 0);
  glColor3f(0, 1, 0);
  glVertex3f(0, -0.5, 0); glVertex3f(0, 0.5, 0);
  glColor3f(0, 0, 1);
  glVertex3f(0, 0, -0.5); glVertex3f(0, 0, 0.5);
  glEnd();

  //glEnable(GL_DEPTH_TEST);
  ////////
  
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  dglSetViewport(viewport);
  
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//