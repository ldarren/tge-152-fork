
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "math/mMath.h"
#include "math/mMathFn.h"
#include "afxEase.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

F32
afxEase::t(F32 t, F32 ein, F32 eout)
{
  if (t == 0.0)
    return 0.0;
  
  if (t == 1.0)
    return 1.0;
  
  F32 ee = eout - ein + 1.0;
  
  // ease in section
  if (t <= ein)
  {
    F32 tin = t/ein;
    return (mSin(M_PI_F*(tin - 1.0)) + M_PI_F*tin)*ein*(1.0/M_PI_F)/ee;
  }

  // middle linear section
  else if (t <= eout)
  {
    return (2.0*t - ein)/ee;
  }

  // ease out section
  else 
  {
    F32 iout = 1.0 - eout;
    F32 g	= (t - eout)*M_PI_F/iout;
    return ((mSin(g) + g)*(iout)/M_PI_F + 2.0*eout - ein)*1.0/ee + 0.0;
  }
}

F32
afxEase::eq(F32 t, F32 a, F32 b, F32 ein, F32 eout)
{
  if (t == 0.0)
    return a;
  
  if (t == 1.0)
    return b;
  
  F32 ab = b - a;
  F32 ee = eout - ein + 1.0;
  
  // ease in section
  if (t <= ein)
  {
    F32 tin = t/ein;
    return a + (mSin(M_PI_F*(tin - 1.0)) + M_PI_F*tin)*ab*ein*(1.0/M_PI_F)/ee;
  }

  // middle linear section
  else if (t <= eout)
  {
    return a + ab*(2.0*t - ein)/ee;
  }

  // ease out section
  else 
  {
    F32 iout = 1.0 - eout;
    F32 g	= (t - eout)*M_PI_F/iout;
    return ((mSin(g) + g)*(iout)/M_PI_F + 2.0*eout - ein)*ab/ee + a;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
