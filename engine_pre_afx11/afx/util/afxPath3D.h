
#ifndef _AFX_PATH3D_UTIL_H_
#define _AFX_PATH3D_UTIL_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/util/afxCurve3D.h"
#include "afx/util/afxAnimCurve.h"

#include "math/mMatrix.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxPath3D
{
private:
    // Path-related data
  afxCurve3D   curve;
  afxAnimCurve curve_parameters;
  int       num_points;
  
  // Time data
  F32       start_time;
  F32       end_time;
  
public:
  /*C*/     afxPath3D( );
  /*D*/     ~afxPath3D();
  
  void      sortAll();
  
  void      setStartTime(F32 time);
  
  F32       getEndTime();
  int       getNumPoints();
  Point3F   getPointPosition(int index);
  F32       getPointTime(int index);
  F32       getPointParameter(int index);
  Point2F   getParameterSegment(F32 time);
  
  void      setPointPosition(int index, Point3F &p);
  
  Point3F   evaluateAtTime(F32 time);
  Point3F   evaluateTangentAtTime(F32 time);
  Point3F   evaluateTangentAtPoint(int index);
  
  void      buildPath(int num_points, Point3F curve_points[], F32 start_time, F32 end_time);
  void      buildPath(int num_points, Point3F curve_points[], F32 speed);
  void      buildPath(int num_points, Point3F curve_points[], Point2F curve_params[]);
  
  void      reBuildPath();
  
  void      print();
   
  enum // looping types
  {      
    LOOP_CONSTANT,
    LOOP_CYCLE,
    LOOP_OSCILLATE
  };
  
  U32       loop_type;
  void      setLoopType(U32);
  
private:
  void      initPathParameters(Point3F curve_points[], F32 speed);
  void      initPathParametersNEW(Point3F curve_points[], F32 start_time, F32 end_time);
    
  F32       calcCurveTime(F32 time);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_PATH3D_UTIL_H_
