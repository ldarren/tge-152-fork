
#ifndef _AFX_COMPONENT_EFFECT_H_
#define _AFX_COMPONENT_EFFECT_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "core/tVector.h"
#include "afx/afxConstraint.h"

class afxComponentEffectData
{
public:
  virtual void  gather_cons_defs(Vector<afxConstraintDef>& defs) { };
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_COMPONENT_EFFECT_H_
