
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_RESIDUE_MGR_H_
#define _AFX_RESIDUE_MGR_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "dgl/gTexManager.h"

class afxZodiacData;
class afxModel;

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxResidueMgr
//
//    Manage transient objects in the world.

class afxResidueMgr : public GameBase
{

  typedef GameBase Parent;
 
  enum { 
    ZODIAC,
    MODEL
  };

  struct Residue
  {
    struct ZodiacParams
    {
      F32   pos_x, pos_y, pos_z;
      F32   rad;
      U8    r,g,b,a;
      F32   ang;
    };  
    
    union ResidueParams
    {
      ZodiacParams  zodiac;
    };
    
    union ResidueData
    {
      afxZodiacData*  zodiac;
      afxModel*       model;
      SimObject*      simobject;
    };

    
    U32           type;
    ResidueData   data;
    ResidueParams params;
    U32           fade_time;
    U32           stop_time;
    F32           fade;
        
    Residue*      next;
  };

  class ResidueList
  {
    Vector<Residue*>  m_array_a;
    Vector<Residue*>  m_array_b;

    Vector<Residue*>* m_array;
    Vector<Residue*>* m_scratch_array;
    bool              m_dirty;
    S32               m_pending;

    void              swap_array_ptrs();
    void              free_residue(Residue*);

  public:
    /*C*/             ResidueList();
    /*D*/             ~ResidueList();

    S32               size() { return m_array->size(); }
    bool              empty() { return m_array->empty(); }
    void              sortIfDirty() { if (m_dirty) sort(); }

    void              sort();
    void              fadeAndCull(U32 now);
    void              stripMatchingObjects(SimObject* db, bool del_notify=false);
    void              add(Residue*);

    void              manage();

    U32               findPendingBestBump(U32 look_max=256);
    void              bumpPending();

    static int QSORT_CALLBACK compare_residue(const void* p1, const void* p2);
  };

  friend class ResidueList;

private:
  enum { FREE_POOL_BLOCK_SIZE = 256 };

  static afxResidueMgr* the_mgr;

  static U32        m_max_residue_objs;
  static bool       enabled;

  ResidueList       m_managed;
  
  Vector<Residue*>  m_free_pool_blocks;
  Residue*          m_next_free;

  Residue*          alloc_free_pool_block();
  Residue*          alloc_residue();
  void              free_residue(Residue*);

  void              bump_residue();
  void              add_residue(Residue*);
 
protected:
  virtual void      advanceTime(F32 dt);
  void              deleteResidueObject(SimObject* obj, bool del_notify=false); 

  void              manage_residue(const Residue* r);

  bool              requires_delete_tracking(Residue*);
  void              enable_delete_tracking(Residue*);
  void              disable_delete_tracking(Residue*);
   
public:                     
  /*C*/             afxResidueMgr();
  /*D*/             ~afxResidueMgr();

  virtual void      onDeleteNotify(SimObject *obj);
    
public:
                    // ZODIAC
  static void       add(F32 dur, F32 fade_dur, afxZodiacData*, Point3F pos, F32 rad, 
                        ColorF col, F32 ang);

                    // MODEL
  static void       add(F32 dur, F32 fade_dur, afxModel*);

  static afxResidueMgr* getMaster() { return the_mgr; }
  static void           setMaster(afxResidueMgr* m) { the_mgr = m; }

  DECLARE_CONOBJECT(afxResidueMgr);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_RESIDUE_MGR_H_
