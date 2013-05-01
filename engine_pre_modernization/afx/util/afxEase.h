
#ifndef _AFX_EASE_H_
#define _AFX_EASE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "platform/platform.h"

class afxEase
{
public:
  static F32   t(F32 t, F32 ein, F32 eout);
  static F32   eq(F32 t, F32 a, F32 b, F32 ein, F32 eout);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_EASE_H_

