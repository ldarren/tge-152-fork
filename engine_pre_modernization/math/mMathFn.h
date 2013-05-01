//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MMATHFN_H_
#define _MMATHFN_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _MCONSTANTS_H_
#include "math/mConstants.h"
#endif
#include <math.h>

// Remove a couple of annoying macros, if they are present (In VC 6, they are.)
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

class MatrixF;
class PlaneF;

extern void MathConsoleInit();

//--------------------------------------
// Installable Library Prototypes
extern S32  (*m_mulDivS32)(S32 a, S32 b, S32 c);
extern U32  (*m_mulDivU32)(S32 a, S32 b, U32 c);

extern F32  (*m_catmullrom)(F32 t, F32 p0, F32 p1, F32 p2, F32 p3);

extern void (*m_point2F_normalize)(F32 *p);
extern void (*m_point2F_normalize_f)(F32 *p, F32 len);
extern void (*m_point2D_normalize)(F64 *p);
extern void (*m_point2D_normalize_f)(F64 *p, F64 len);
extern void (*m_point3F_normalize)(F32 *p);
extern void (*m_point3F_normalize_f)(F32 *p, F32 len);
extern void (*m_point3F_interpolate)(const F32 *from, const F32 *to, F32 factor, F32 *result);

extern void (*m_point3D_normalize)(F64 *p);
extern void (*m_point3D_normalize_f)(F64 *p, F64 len);
extern void (*m_point3D_interpolate)(const F64 *from, const F64 *to, F64 factor, F64 *result);

extern void (*m_point3F_bulk_dot)(const F32* refVector,
                                  const F32* dotPoints,
                                  const U32  numPoints,
                                  const U32  pointStride,
                                  F32*       output);
extern void (*m_point3F_bulk_dot_indexed)(const F32* refVector,
                                          const F32* dotPoints,
                                          const U32  numPoints,
                                          const U32  pointStride,
                                          const U32* pointIndices,
                                          F32*       output);

extern void (*m_quatF_set_matF)( F32 x, F32 y, F32 z, F32 w, F32* m );

extern void (*m_matF_set_euler)(const F32 *e, F32 *result);
extern void (*m_matF_set_euler_point)(const F32 *e, const F32 *p, F32 *result);
extern void (*m_matF_identity)(F32 *m);
extern void (*m_matF_inverse)(F32 *m);
extern void (*m_matF_affineInverse)(F32 *m);
extern void (*m_matF_transpose)(F32 *m);
extern void (*m_matF_scale)(F32 *m,const F32* p);
extern void (*m_matF_normalize)(F32 *m);
extern F32  (*m_matF_determinant)(const F32 *m);
extern void (*m_matF_x_matF)(const F32 *a, const F32 *b, F32 *mresult);
// extern void (*m_matF_x_point3F)(const F32 *m, const F32 *p, F32 *presult);
// extern void (*m_matF_x_vectorF)(const F32 *m, const F32 *v, F32 *vresult);
extern void (*m_matF_x_point4F)(const F32 *m, const F32 *p, F32 *presult);
extern void (*m_matF_x_scale_x_planeF)(const F32 *m, const F32* s, const F32 *p, F32 *presult);
extern void (*m_matF_x_box3F)(const F32 *m, F32 *min, F32 *max);

// Note that x must point to at least 4 values for quartics, and 3 for cubics
extern U32 (*mSolveQuadratic)(F32 a, F32 b, F32 c, F32* x);
extern U32 (*mSolveCubic)(F32 a, F32 b, F32 c, F32 d, F32* x);
extern U32 (*mSolveQuartic)(F32 a, F32 b, F32 c, F32 d, F32 e, F32* x);

extern S32 mRandI(S32 i1, S32 i2); // random # from i1 to i2 inclusive
extern F32 mRandF(F32 f1, F32 f2); // random # from f1 to f2 inclusive


inline void m_matF_x_point3F(const F32 *m, const F32 *p, F32 *presult)
{
   AssertFatal(p != presult, "Error, aliasing matrix mul pointers not allowed here!");
   
#ifdef TORQUE_COMPILER_GCC
	const F32	p0 = p[0], p1 = p[1], p2 = p[2];
	const F32	m0 = m[0], m1 = m[1], m2 = m[2];
	const F32	m3 = m[3], m4 = m[4], m5 = m[5];
	const F32	m6 = m[6], m7 = m[7], m8 = m[8];
	const F32	m9 = m[9], m10 = m[10], m11 = m[11];
	
	presult[0] = m0*p0 + m1*p1 + m2*p2  + m3;
	presult[1] = m4*p0 + m5*p1 + m6*p2  + m7;
	presult[2] = m8*p0 + m9*p1 + m10*p2 + m11;
#else
   presult[0] = m[0]*p[0] + m[1]*p[1] + m[2]*p[2]  + m[3];
   presult[1] = m[4]*p[0] + m[5]*p[1] + m[6]*p[2]  + m[7];
   presult[2] = m[8]*p[0] + m[9]*p[1] + m[10]*p[2] + m[11];
#endif
}


//--------------------------------------
inline void m_matF_x_vectorF(const F32 *m, const F32 *v, F32 *vresult)
{
   AssertFatal(v != vresult, "Error, aliasing matrix mul pointers not allowed here!");

#ifdef TORQUE_COMPILER_GCC
	const F32	v0 = v[0], v1 = v[1], v2 = v[2];
	const F32	m0 = m[0], m1 = m[1], m2 = m[2];
	const F32	m4 = m[4], m5 = m[5], m6 = m[6];
	const F32	m8 = m[8], m9 = m[9], m10 = m[10];
	
	vresult[0] = m0*v0 + m1*v1 + m2*v2;
	vresult[1] = m4*v0 + m5*v1 + m6*v2;
	vresult[2] = m8*v0 + m9*v1 + m10*v2;
#else
   vresult[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
   vresult[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2];
   vresult[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2];
#endif
}


//--------------------------------------
// Inlines
inline bool mIsZero(const F32 val, const F32 epsilon = 1e-8f )
{
   return (val > -epsilon) && (val < epsilon);
}

inline F32 mFloor(const F32 val)
{
   return (F32) floor(val);
}

inline F32 mCeil(const F32 val)
{
   return (F32) ceil(val);
}

inline F32 mFabs(const F32 val)
{
   return (F32) fabs(val);
}

inline F64 mFabs(const F64 val)
{
   return fabs(val);
}

inline F32 mFmod(const F32 val, const F32 mod)
{
   return (F32) fmod(val, mod);
}

inline S32 mAbs(const S32 val)
{
   // Kinda lame, and disallows intrinsic inlining by the compiler.  Maybe fix?
   //  DMM
   if (val < 0)
      return -val;

   return val;
}

inline S32 mClamp(S32 val, S32 low, S32 high)
{
   return getMax(getMin(val, high), low);
}

inline F32 mClampF(F32 val, F32 low, F32 high)
{
   return (F32) getMax(getMin(val, high), low);
}

inline S32 mMulDiv(S32 a, S32 b, S32 c)
{
   return m_mulDivS32(a, b, c);
}

inline U32 mMulDiv(S32 a, S32 b, U32 c)
{
   return m_mulDivU32(a, b, c);
}


inline F32 mSin(const F32 angle)
{
   return (F32) sin(angle);
}

inline F32 mCos(const F32 angle)
{
   return (F32) cos(angle);
}

inline F32 mTan(const F32 angle)
{
   return (F32) tan(angle);
}

inline F32 mAsin(const F32 val)
{
   return (F32) asin(val);
}

inline F32 mAcos(const F32 val)
{
   return (F32) acos(val);
}

inline F32 mAtan(const F32 x, const F32 y)
{
   return (F32) atan2(x, y);
}

inline void mSinCos(const F32 angle, F32 &s, F32 &c)
{
   s = mSin(angle);
   c = mCos(angle);
}

inline F32 mTanh(const F32 angle)
{
   return (F32) tanh(angle);
}

inline F32 mSqrt(const F32 val)
{
   return (F32) sqrt(val);
}

inline F32 mPow(const F32 x, const F32 y)
{
   return (F32) pow(x, y);
}

inline F32 mLog(const F32 val)
{
   return (F32) log(val);
}


inline F64 mSin(const F64 angle)
{
   return (F64) sin(angle);
}

inline F64 mCos(const F64 angle)
{
   return (F64) cos(angle);
}

inline F64 mTan(const F64 angle)
{
   return (F64) tan(angle);
}

inline F64 mAsin(const F64 val)
{
   return (F64) asin(val);
}

inline F64 mAcos(const F64 val)
{
   return (F64) acos(val);
}

inline F64 mAtan(const F64 x, const F64 y)
{
   return (F64) atan2(x, y);
}

inline void mSinCos(const F64 angle, F64 &sin, F64 &cos)
{
   sin = mSin(angle);
   cos = mCos(angle);
}

inline F64 mTanh(const F64 angle)
{
   return (F64) tanh(angle);
}

inline F64 mPow(const F64 x, const F64 y)
{
   return (F64) pow(x, y);
}

inline F64 mLog(const F64 val)
{
   return (F64) log(val);
}


inline F32 mCatmullrom(F32 t, F32 p0, F32 p1, F32 p2, F32 p3)
{
   return m_catmullrom(t, p0, p1, p2, p3);
}


inline F64 mFabsD(const F64 val)
{
   return (F64) fabs(val);
}

inline F64 mFmodD(const F64 val, const F64 mod)
{
   return (F64) fmod(val, mod);
}

inline F64 mSqrtD(const F64 val)
{
   return (F64) sqrt(val);
}

inline F64 mFloorD(const F64 val)
{
   return (F64) floor(val);
}

inline F64 mCeilD(const F64 val)
{
   return (F64) ceil(val);
}


//--------------------------------------
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

inline F32 mDot(const Point3F &p1, const Point3F &p2)
{
   return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

inline void mCross(const Point3F &a, const Point3F &b, Point3F *res)
{
#ifdef TORQUE_COMPILER_GCC
// remove possibility of aliases
	const F32	ax = a.x, ay = a.y, az = a.z;
	const F32	bx = b.x, by = b.y, bz = b.z;

	res->x = (ay * bz) - (az * by);
	res->y = (az * bx) - (ax * bz);
	res->z = (ax * by) - (ay * bx);
#else
   res->x = (a.y * b.z) - (a.z * b.y);
   res->y = (a.z * b.x) - (a.x * b.z);
   res->z = (a.x * b.y) - (a.y * b.x);
#endif
}

inline F64 mDot(const Point3D &p1, const Point3D &p2)
{
   return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

inline void mCross(const Point3D &a, const Point3D &b, Point3D *res)
{
#ifdef TORQUE_COMPILER_GCC
// remove possibility of aliases
	const F64	ax = a.x, ay = a.y, az = a.z;
	const F64	bx = b.x, by = b.y, bz = b.z;

	res->x = (ay * bz) - (az * by);
	res->y = (az * bx) - (ax * bz);
	res->z = (ax * by) - (ay * bx);
#else
   res->x = (a.y * b.z) - (a.z * b.y);
   res->y = (a.z * b.x) - (a.x * b.z);
   res->z = (a.x * b.y) - (a.y * b.x);
#endif
}

inline Point3F mCross(const Point3F &a, const Point3F &b)
{
   Point3F ret;
   mCross(a,b,&ret);
   return ret;
}

inline void mCross(const F32* a, const F32* b, F32 *res)
{
#ifdef TORQUE_COMPILER_GCC
// remove possibility of aliases
	const F32	ax = a[0], ay = a[1], az = a[2];
	const F32	bx = b[0], by = b[1], bz = b[2];

	res[0] = (ay * bz) - (az * by);
	res[1] = (az * bx) - (ax * bz);
	res[2] = (ax * by) - (ay * bx);
#else
   res[0] = (a[1] * b[2]) - (a[2] * b[1]);
   res[1] = (a[2] * b[0]) - (a[0] * b[2]);
   res[2] = (a[0] * b[1]) - (a[1] * b[0]);
#endif
}

inline void mCross(const F64* a, const F64* b, F64* res)
{
#ifdef TORQUE_COMPILER_GCC
// remove possibility of aliases
	const F64	ax = a[0], ay = a[1], az = a[2];
	const F64	bx = b[0], by = b[1], bz = b[2];

	res[0] = (ay * bz) - (az * by);
	res[1] = (az * bx) - (ax * bz);
	res[2] = (ax * by) - (ay * bx);
#else
   res[0] = (a[1] * b[2]) - (a[2] * b[1]);
   res[1] = (a[2] * b[0]) - (a[0] * b[2]);
   res[2] = (a[0] * b[1]) - (a[1] * b[0]);
#endif
}

void mTransformPlane(const MatrixF& mat, const Point3F& scale, const PlaneF& plane, PlaneF* result);


//--------------------------------------
inline F32 mDegToRad(F32 d)
{
   return((d * M_PI_F) / 180.0f);
}

inline F32 mRadToDeg(F32 r)
{
   return((r * 180.0f) / M_PI_F);
}

inline F64 mDegToRad(F64 d)
{
   return (d * M_PI) / 180.0;
}

inline F64 mRadToDeg(F64 r)
{
   return (r * 180.0) / M_PI;
}

#endif //_MMATHFN_H_
