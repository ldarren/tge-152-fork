
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/util/afxCurveEval.h"

Point2F afxHermiteEval::evaluateCurve( Point2F &v0, Point2F &v1,
                                         Point2F &t0, Point2F &t1, F32 t )
{
  F32 t_3 = t*t*t;
  F32 t_2 = t*t;
  F32 h1 = ( 2.0f * t_3 ) - ( 3.0f * t_2 ) + 1;
  F32 h2 = (-2.0f * t_3 ) + ( 3.0f * t_2 );
  F32 h3 = t_3 - ( 2.0f * t_2 ) + t;
  F32 h4 = t_3 - t_2;

  Point2F v( 
    (h1*v0.x)+(h2*v1.x)+(h3*t0.x)+(h4*t1.x),
    (h1*v0.y)+(h2*v1.y)+(h3*t0.y)+(h4*t1.y) );

  return v;
}

Point2F afxHermiteEval::evaluateCurve( Point2F &v0, Point2F &v1, F32 t )
{
  Point2F tangent( 1, 0 );
  return( evaluateCurve( v0, v1, tangent, tangent, t ) );
}

Point2F afxHermiteEval::evaluateCurveTangent( Point2F &v0, Point2F &v1,
                                                Point2F &t0, Point2F &t1, F32 t )
{
  F32 t_2 = t*t;
  F32 h1_der = ( 6.0f * t_2 ) - ( 6.0f * t );
  F32 h2_der = (-6.0f * t_2 ) + ( 6.0f * t );
  F32 h3_der = ( 3.0f * t_2 ) - ( 4.0f * t ) + 1;
  F32 h4_der = ( 3.0f * t_2 ) - ( 2.0f * t );

  Point2F tangent( 
    (h1_der*v0.x)+(h2_der*v1.x)+(h3_der*t0.x)+(h4_der*t1.x),
    (h1_der*v0.y)+(h2_der*v1.y)+(h3_der*t0.y)+(h4_der*t1.y) );

  return tangent;
}

Point2F afxHermiteEval::evaluateCurveTangent( Point2F &v0, Point2F &v1, F32 t )
{
  Point2F tangent( 1, 0 );
  return( evaluateCurveTangent( v0, v1, tangent, tangent, t ) );
}

Point3F afxHermiteEval::evaluateCurve( Point3F &v0, Point3F &v1,
                                         Point3F &t0, Point3F &t1, F32 t )
{
  F32 t_3 = t*t*t;
  F32 t_2 = t*t;
  F32 h1 = ( 2.0f * t_3 ) - ( 3.0f * t_2 ) + 1;
  F32 h2 = (-2.0f * t_3 ) + ( 3.0f * t_2 );
  F32 h3 = t_3 - ( 2.0f * t_2 ) + t;
  F32 h4 = t_3 - t_2;

  Point3F v( 
    (h1*v0.x)+(h2*v1.x)+(h3*t0.x)+(h4*t1.x),
    (h1*v0.y)+(h2*v1.y)+(h3*t0.y)+(h4*t1.y),
    (h1*v0.z)+(h2*v1.z)+(h3*t0.z)+(h4*t1.z) );

  return v;
}

Point3F afxHermiteEval::evaluateCurve( Point3F &v0, Point3F &v1, F32 t )
{
  Point3F tangent( 1, 0, 0 );
  return( evaluateCurve( v0, v1, tangent, tangent, t ) );
}

Point3F afxHermiteEval::evaluateCurveTangent( Point3F &v0, Point3F &v1,
                                                Point3F &t0, Point3F &t1, F32 t )
{
  F32 t_2 = t*t;
  F32 h1_der = ( 6.0f * t_2 ) - ( 6.0f * t );
  F32 h2_der = (-6.0f * t_2 ) + ( 6.0f * t );
  F32 h3_der = ( 3.0f * t_2 ) - ( 4.0f * t ) + 1;
  F32 h4_der = ( 3.0f * t_2 ) - ( 2.0f * t );

  Point3F tangent( 
    (h1_der*v0.x)+(h2_der*v1.x)+(h3_der*t0.x)+(h4_der*t1.x),
    (h1_der*v0.y)+(h2_der*v1.y)+(h3_der*t0.y)+(h4_der*t1.y),
    (h1_der*v0.z)+(h2_der*v1.z)+(h3_der*t0.z)+(h4_der*t1.z) );

  return tangent;
}

Point3F afxHermiteEval::evaluateCurveTangent( Point3F &v0, Point3F &v1, F32 t )
{
  Point3F tangent( 1, 0, 0 );
  return( evaluateCurveTangent( v0, v1, tangent, tangent, t ) );
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

