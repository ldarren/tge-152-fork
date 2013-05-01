
#ifndef _AFX_CURVE_3D_H_
#define _AFX_CURVE_3D_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "core/tVector.h"
#include "math/mPoint.h"

class afxCurveEval;

class afxCurve3D
{
	class CurvePoint
	{
		public:
			F32			parameter;
			Point3F point;
		
		   // new:
         Point3F tangent;
	};

	private:
		afxCurveEval* evaluator;
		Point3F start_value;
		Point3F final_value;
		Point3F start_tangent;
		Point3F final_tangent;
		bool	  usable;

		//std::vector<CurvePoint> points;
		Vector<CurvePoint> points;

		Point3F default_vector;

		//static bool compare_CurvePoint( const CurvePoint &a, const CurvePoint &b ); 
		static S32 QSORT_CALLBACK compare_CurvePoint( const void* a, const void* b );

      // new
      Point3F last_tangent;
      bool flip;

	public:
		afxCurve3D();
		~afxCurve3D();

		void    addPoint( F32 param, Point3F &v );
		void    setPoint( int index, Point3F &v );
		void    sort( );
		int     numPoints();
		F32     getParameter( int index );
		Point3F getPoint( int index );
		Point3F evaluate( F32 param );
		Point3F evaluateTangent( F32 param );

		void print();

      void computeTangents();

      //MatrixF createOrientFromDir( Point3F &direction );

	private:
		Point3F computeTangentP0( Point3F &p0, Point3F &p1, int start_index );
		Point3F computeTangentP1( Point3F &p0, Point3F &p1, int end_index );
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_CURVE_3D_H_