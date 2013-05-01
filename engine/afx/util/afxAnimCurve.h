
#ifndef _AFX_ANIM_CURVE_H_
#define _AFX_ANIM_CURVE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "core/tVector.h"
#include "afx/util/afxCurveEval.h"


class afxAnimCurve
{
	class Key
	{
		public:
			F32 time;
			F32 value;
	};

	private:
		afxCurveEval* evaluator;

		F32  final_value;
		F32  start_value;
		F32  final_time;
		F32	 start_time;
		bool usable;

		//std::vector<Key> keys;
		Vector<Key> keys;

		//static bool compare_Key( const Key &a, const Key &b ); 
		static S32 QSORT_CALLBACK compare_Key( const void* a, const void* b );

	public:
		afxAnimCurve();
		~afxAnimCurve();

		void    addKey( Point2F &v );
		void    addKey( F32 time, F32 value );
		void    setKeyTime( int index, F32 t );
		void    setKeyValue( int index, F32 v );
		void    sort( );
		int     numKeys();
		F32     getKeyTime( int index );
		F32     getKeyValue( int index );
		Point2F getSegment( F32 time );
		F32     evaluate( F32 time );

		void print();
		void printKey( int index );

	private:
		Point2F computeTangentK0( Point2F &k0, Point2F &k1, int start_index );
		Point2F computeTangentK1( Point2F &k0, Point2F &k1, int end_index );
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ANIM_CURVE_H_