
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "ts/tsShapeInstance.h"

#include "afx/ce/afxZodiac.h"
#include "afx/ce/afxModel.h"
#include "afx/afxResidueMgr.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

int QSORT_CALLBACK afxResidueMgr::ResidueList::compare_residue(const void* p1, const void* p2)
{
  const afxResidueMgr::Residue** pd1 = (const afxResidueMgr::Residue**)p1;
  const afxResidueMgr::Residue** pd2 = (const afxResidueMgr::Residue**)p2;
  
  return int(((char*)(*pd1)->data.simobject) - ((char*)(*pd2)->data.simobject));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

inline void afxResidueMgr::ResidueList::swap_array_ptrs()
{
  Vector<Residue*>* tmp = m_array;
  m_array = m_scratch_array;
  m_scratch_array = tmp;
}

void afxResidueMgr::ResidueList::free_residue(Residue* residue)
{
  if (afxResidueMgr::the_mgr->requires_delete_tracking(residue))
    afxResidueMgr::the_mgr->disable_delete_tracking(residue);
  afxResidueMgr::the_mgr->free_residue(residue);
}

afxResidueMgr::ResidueList::ResidueList()
{
  VECTOR_SET_ASSOCIATION(m_array_a);
  VECTOR_SET_ASSOCIATION(m_array_b);

  m_array = &m_array_a;
  m_scratch_array = &m_array_b ;

  m_dirty = false;
  m_pending = -1;
}

afxResidueMgr::ResidueList::~ResidueList()
{
  m_array_a.clear();
  m_array_b.clear();
}

void afxResidueMgr::ResidueList::sort()
{
  dQsort(m_array->address(), m_array->size(), sizeof(Residue*), compare_residue);
  m_dirty = false;
}

void afxResidueMgr::ResidueList::fadeAndCull(U32 now)
{
  for (S32 i = 0; i < m_array->size(); i++)
  {
    Residue* r = (*m_array)[i];

    // done
    if (now >= r->stop_time)
    {
      free_residue(r);
    }
    // fading
    else if (now >= r->fade_time)
    {
      r->fade = 1.0f - ((F32)(now - r->fade_time))/((F32)(r->stop_time - r->fade_time));
      m_scratch_array->push_back(r);
    }
    // opaque
    else
    {
      r->fade = 1.0f;
      m_scratch_array->push_back(r);
    }
  }

  m_array->clear();
  swap_array_ptrs();
}

// removes all residue with datablock matching obj
void afxResidueMgr::ResidueList::stripMatchingObjects(SimObject* db, bool del_notify)
{
  if (del_notify)
  {
    for (S32 i = 0; i < m_array->size(); i++)
    {
      Residue* r = (*m_array)[i];
      if (db == r->data.simobject)
        afxResidueMgr::the_mgr->free_residue(r);
      else
        m_scratch_array->push_back(r);
    }
  }
  else
  {
    for (S32 i = 0; i < m_array->size(); i++)
    {
      Residue* r = (*m_array)[i];
      if (db == r->data.simobject)
        free_residue(r);
      else
        m_scratch_array->push_back(r);
    }
  }

  m_array->clear();
  swap_array_ptrs();
}

void afxResidueMgr::ResidueList::add(Residue* residue)
{
  m_array->push_back(residue);
  m_dirty = true;
}

void afxResidueMgr::manage_residue(const Residue* r)
{
  if (r == NULL || r->fade < 0.01f)
    return;

  if (r->type == ZODIAC)
  {
    ColorF zode_color = ColorI(r->params.zodiac.r, r->params.zodiac.g, r->params.zodiac.b, r->params.zodiac.a);

    afxZodiacData* zd = (afxZodiacData*) r->data.zodiac;
    if (zd->blend_flags == afxZodiacDefs::BLEND_SUBTRACTIVE)
       zode_color *= r->fade;
    else
       zode_color.alpha *= r->fade;

    Point3F zode_pos(r->params.zodiac.pos_x, r->params.zodiac.pos_y, r->params.zodiac.pos_z);
    afxZodiacMgr::addZodiac(zode_pos, r->params.zodiac.rad, zode_color, r->params.zodiac.ang, 
                            &zd->txr, zd->zflags);
  }
  else if (r->type == MODEL)
  {
    r->data.model->setFadeAmount(r->fade);
  }
}

void afxResidueMgr::ResidueList::manage()
{
  S32 n_residue = m_array->size(); 

  for (S32 x = 0; x < n_residue; x++)
    the_mgr->manage_residue((*m_array)[x]);
}

U32 afxResidueMgr::ResidueList::findPendingBestBump(U32 look_max)
{
  U32 soonest = 1000*60*60*24;
  m_pending = -1;

  U32 n = m_array->size();
  for (U32 i = 0; i < n && i < look_max; i++)
  {
    Residue* r = (*m_array)[i];
    if (r->stop_time < soonest)
    {
      soonest = r->stop_time;
      m_pending = i;
    }
  }

  return soonest;
}

void afxResidueMgr::ResidueList::bumpPending()
{
  if (m_pending >= 0 && m_pending < m_array->size())
  {
    Residue* r = (*m_array)[m_pending];
    m_array->erase(m_pending);
    free_residue(r);
  }

  m_pending = -1;
}

bool afxResidueMgr::requires_delete_tracking(Residue* r)
{
  return (r->type == MODEL);
}

void afxResidueMgr::enable_delete_tracking(Residue* r)
{
  deleteNotify(r->data.simobject);
}

void afxResidueMgr::disable_delete_tracking(Residue* r)
{
  clearNotify(r->data.simobject);
  r->data.simobject->deleteObject();
  r->data.simobject = 0;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

afxResidueMgr*  afxResidueMgr::the_mgr = NULL;
U32             afxResidueMgr::m_max_residue_objs = 256;
bool            afxResidueMgr::enabled = true;

IMPLEMENT_CONOBJECT(afxResidueMgr);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// free-list management

afxResidueMgr::Residue* afxResidueMgr::alloc_free_pool_block()
{
  // allocate new block for the free-list
  m_free_pool_blocks.push_back(new Residue[FREE_POOL_BLOCK_SIZE]);
  
  // link them onto the free-list
  Residue* new_block = m_free_pool_blocks.last();
  for (U32 i = 0; i < FREE_POOL_BLOCK_SIZE - 1; i++)
    new_block[i].next = &new_block[i + 1];
  
  // tail of free-list points to NULL
  new_block[FREE_POOL_BLOCK_SIZE - 1].next = NULL;

  return new_block;
}

afxResidueMgr::Residue* afxResidueMgr::alloc_residue()
{
  // need new free-list-block if m_next_free is null
  if (!m_next_free)
    m_next_free = alloc_free_pool_block();
  
  // pop new residue from head of free-list
  Residue* residue = m_next_free;
  m_next_free = residue->next;
  residue->next = NULL;

  return residue;
}

void afxResidueMgr::free_residue(Residue* residue)
{
  // push residue onto head of free-list
  residue->next = m_next_free;
  m_next_free = residue;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxResidueMgr::deleteResidueObject(SimObject* obj, bool del_notify)
{
  m_managed.stripMatchingObjects(obj, del_notify);
}

void afxResidueMgr::bump_residue()
{
  if (m_managed.findPendingBestBump())
    m_managed.bumpPending();
}

void afxResidueMgr::add_residue(Residue* residue)
{
  AssertFatal(residue != NULL, "residue pointer is NULL.");

  if (/*m_rendered.size() + */m_managed.size() >= m_max_residue_objs)
    bump_residue();

  m_managed.add(residue);
  manage_residue(residue);

  if (requires_delete_tracking(residue))
    enable_delete_tracking(residue);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

afxResidueMgr::afxResidueMgr()
{ 
  mObjBox.min.set(-1e7, -1e7, -1e7);
  mObjBox.max.set( 1e7,  1e7,  1e7);
  mWorldBox.min.set(-1e7, -1e7, -1e7);
  mWorldBox.max.set( 1e7,  1e7,  1e7);
  
  m_next_free = NULL;

  VECTOR_SET_ASSOCIATION(m_free_pool_blocks);
}

afxResidueMgr::~afxResidueMgr()
{
  m_next_free = NULL;

  for (S32 i = 0; i < m_free_pool_blocks.size(); i++)
    delete [] m_free_pool_blocks[i];

  m_free_pool_blocks.clear();
}

void afxResidueMgr::onDeleteNotify(SimObject* obj)
{
  deleteResidueObject(obj, true);
  Parent::onDeleteNotify(obj);
}

void afxResidueMgr::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  U32 now = Platform::getVirtualMilliseconds();
  m_managed.fadeAndCull(now);
  m_managed.sortIfDirty();
  m_managed.manage();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// add ZODIAC residue

void afxResidueMgr::add(F32 dur, F32 fade_dur, afxZodiacData* zode, 
                        Point3F pos, F32 rad, ColorF col, F32 ang)
{
  if (m_max_residue_objs == 0 || dur <= 0)
    return;

  ColorI col_i = col;
  U32 now = Platform::getVirtualMilliseconds();

  Residue* residue = the_mgr->alloc_residue();
  //
  residue->type = ZODIAC;
  residue->data.zodiac = zode;
  residue->fade_time = now + (U32)(dur*1000);
  residue->stop_time = residue->fade_time + (U32)(fade_dur*1000);
  residue->fade = 1.0f;
  //
  residue->params.zodiac.pos_x = pos.x;
  residue->params.zodiac.pos_y = pos.y;
  residue->params.zodiac.pos_z = pos.z;
  residue->params.zodiac.rad = rad;
  residue->params.zodiac.r = col_i.red;
  residue->params.zodiac.g = col_i.green;
  residue->params.zodiac.b = col_i.blue;
  residue->params.zodiac.a = col_i.alpha;
  residue->params.zodiac.ang = ang;
  //
  residue->next = 0;
  
  the_mgr->add_residue(residue);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// add MODEL residue

void afxResidueMgr::add(F32 dur, F32 fade_dur, afxModel* model)
{
  if (m_max_residue_objs == 0 || dur <= 0)
    return;

  U32 now = Platform::getVirtualMilliseconds();
  
  Residue* residue = the_mgr->alloc_residue();
  //
  residue->type = MODEL;
  residue->data.model = model;
  residue->fade_time = now + (U32)(dur*1000);
  residue->stop_time = residue->fade_time + (U32)(fade_dur*1000);
  residue->fade = 1.0f;
  //
  residue->next = 0;

  the_mgr->add_residue(residue);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
