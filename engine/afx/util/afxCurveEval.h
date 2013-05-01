
#ifndef _AFX_CURVE_EVAL_BASE_H_
#define _AFX_CURVE_EVAL_BASE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "math/mPoint.h"

class afxCurveEval
{
public:
  virtual Point2F evaluateCurve(Point2F& v0, Point2F& v1, F32 t)=0;
  virtual Point2F evaluateCurve(Point2F& v0, Point2F& v1, Point2F& t0, Point2F& t1, F32 t)=0;
  virtual Point2F evaluateCurveTangent(Point2F& v0, Point2F& v1, F32 t)=0;
  virtual Point2F evaluateCurveTangent(Point2F& v0, Point2F& v1, Point2F& t0, Point2F& t1, F32 t)=0;
  
  virtual Point3F evaluateCurve(Point3F& v0, Point3F& v1, F32 t)=0;
  virtual Point3F evaluateCurve(Point3F& v0, Point3F& v1, Point3F& t0, Point3F& t1, F32 t)=0;
  virtual Point3F evaluateCurveTangent(Point3F& v0, Point3F& v1, F32 t)=0;
  virtual Point3F evaluateCurveTangent(Point3F& v0, Point3F& v1, Point3F& t0, Point3F& t1, F32 t)=0;
};

class afxHermiteEval : public afxCurveEval
{
public:
  Point2F evaluateCurve(Point2F& v0, Point2F& v1, F32 t);
  Point2F evaluateCurve(Point2F& v0, Point2F& v1, Point2F& t0, Point2F& t1, F32 t);
  Point2F evaluateCurveTangent(Point2F& v0, Point2F& v1, F32 t);
  Point2F evaluateCurveTangent(Point2F& v0, Point2F& v1, Point2F& t0, Point2F& t1, F32 t);
  
  Point3F evaluateCurve(Point3F& v0, Point3F& v1, F32 t);
  Point3F evaluateCurve(Point3F& v0, Point3F& v1, Point3F& t0, Point3F& t1, F32 t);
  Point3F evaluateCurveTangent(Point3F& v0, Point3F& v1, F32 t);
  Point3F evaluateCurveTangent(Point3F& v0, Point3F& v1, Point3F& t0, Point3F& t1, F32 t);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_CURVE_EVAL_BASE_H_
