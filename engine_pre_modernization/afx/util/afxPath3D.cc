
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/util/afxPath3D.h"
#include "console/console.h"

afxPath3D::afxPath3D() : start_time(0), num_points(0), loop_type(LOOP_CONSTANT)
{
}

afxPath3D::~afxPath3D()
{
}

void afxPath3D::sortAll()
{
  curve.sort();
  curve_parameters.sort();
}

void afxPath3D::setStartTime( F32 time )
{
  start_time = time;
}

void afxPath3D::setLoopType( U32 loop_type )
{
  this->loop_type = loop_type;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

F32 afxPath3D::getEndTime()
{
  return end_time;
}

int afxPath3D::getNumPoints()
{
  return num_points;
}

Point3F afxPath3D::getPointPosition( int index )
{
  if (index < 0 || index >= num_points)
    return Point3F(0.0f, 0.0f, 0.0f);

  return curve.getPoint(index);
}

F32 afxPath3D::getPointTime( int index )
{
  if (index < 0 || index >= num_points)
    return 0.0f;

  return curve_parameters.getKeyTime(index);
}

F32 afxPath3D::getPointParameter( int index )
{
  if (index < 0 || index >= num_points)
    return 0.0f;

  return curve_parameters.getKeyValue(index);
}

Point2F afxPath3D::getParameterSegment( F32 time )
{
  return curve_parameters.getSegment(time);
}

void afxPath3D::setPointPosition( int index, Point3F &p )
{
  if (index < 0 || index >= num_points)
    return;

  curve.setPoint(index, p);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

F32 afxPath3D::calcCurveTime( F32 time )
{
   if( time <= start_time )
      return 0.0f;
   if( time <= end_time )
      return time-start_time;

   switch( loop_type )
   {
   case LOOP_CYCLE :
      {
         return mFmod( time-start_time, end_time-start_time );
      }
   case LOOP_OSCILLATE :
      {
         F32 t1 = time-start_time;
         F32 t2 = end_time-start_time;

         if( (int)(t1/t2) % 2 ) // odd segment
            return t2 - mFmod( t1, t2 );
         else                   // even segment
            return mFmod( t1, t2 );
      }
   case LOOP_CONSTANT :
   default:
      return end_time;
   }
}

Point3F afxPath3D::evaluateAtTime( F32 time )
{
  F32 ctime = calcCurveTime( time );
  F32 param = curve_parameters.evaluate( ctime );
  return curve.evaluate(param);
}

Point3F afxPath3D::evaluateTangentAtTime( F32 time )
{
  F32 ctime = calcCurveTime( time );
  F32 param = curve_parameters.evaluate( ctime );
  return curve.evaluateTangent(param);
}

Point3F afxPath3D::evaluateTangentAtPoint( int index )
{
  F32 param = curve_parameters.getKeyValue(index);
  return curve.evaluateTangent(param);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxPath3D::buildPath( int num_points, Point3F curve_points[], F32 start_time, F32 end_time )
{
  this->num_points = num_points;

  // Add points to path
  F32 param_inc = 1.0f / (F32)(num_points - 1);
  F32 param = 0.0f;
  for( int i = 0; i < num_points; i++, param += param_inc )
  {
    if( i == num_points-1 )
      param = 1.0f;
    curve.addPoint( param, curve_points[i] );
  }

  curve.computeTangents();

  initPathParametersNEW( curve_points, start_time, end_time );

  sortAll();
}

void afxPath3D::buildPath( int num_points, Point3F curve_points[], F32 speed )
{
  this->num_points = num_points;

  // Add points to path
  F32 param_inc = 1.0f / (F32)(num_points - 1);
  F32 param = 0.0f;
  for( int i = 0; i < num_points; i++, param += param_inc )
  {
    if( i == num_points-1 )
      param = 1.0f;
    curve.addPoint( param, curve_points[i] );
  }

  initPathParameters( curve_points, speed );

  sortAll();
}

void afxPath3D::buildPath( int num_points, Point3F curve_points[],
                                      Point2F curve_params[] )
{
  this->num_points = num_points;

  // Add points to path
  F32 param_inc = 1.0f / (F32)(num_points - 1);
  F32 param = 0.0f;
  for( int i = 0; i < num_points; i++, param += param_inc )
  {
    if( i == num_points-1 )
      param = 1.0f;
    curve.addPoint( param, curve_points[i] );
  }

  //
  for (int i = 0; i < num_points; i++)
    curve_parameters.addKey( curve_params[i] );

  // Set end time
  end_time = curve_params[num_points - 1].x;

  sortAll();
}

void afxPath3D::reBuildPath()
{
  curve.computeTangents();
  sortAll();
}

void afxPath3D::initPathParameters( Point3F curve_points[], F32 speed )
{
  // Compute the time for each point dependent on the speed of the character and the
  //  distance it must travel (approximately!)
  int num_segments = num_points - 1;
  F32 *point_distances = new F32[num_segments];
  for( int i = 0; i < num_segments; i++ )
  {
    Point3F p1 = curve_points[i+1];
    Point3F p0 = curve_points[i];
    
    point_distances[i] = (p1-p0).len();
  }

  F32 *times = new F32[num_segments];
  F32 last_time = 0;//start_time;
  for( int i = 0; i < num_segments; i++ )
  {
    times[i] = last_time + (point_distances[i] / speed);
    last_time = times[i];
  }

  curve_parameters.addKey( 0, 0.0f );//start_time, 0.0f );
  F32 param_inc = 1.0f / (F32)(num_points - 1);
  F32 param = 0.0f + param_inc;
  for( int i = 0; i < num_segments; i++, param += param_inc )
    curve_parameters.addKey( times[i], param );

  // Set end time
  end_time = times[num_segments-1];

  if (point_distances)
    delete [] point_distances;
  if (times)
    delete [] times;
}

void afxPath3D::initPathParametersNEW( Point3F curve_points[], F32 start_time, F32 end_time )
{
  int num_segments = num_points - 1;
  F32 *point_distances = new F32[num_segments];
  F32 total_distance = 0.0f;
  for( int i = 0; i < num_segments; i++ )
  {
    Point3F p1 = curve_points[i+1];
    Point3F p0 = curve_points[i];
    
    point_distances[i] = (p1-p0).len();
    total_distance += point_distances[i];
  }

  F32 duration = end_time - start_time;

  F32 time = 0.0f; //start_time;
  curve_parameters.addKey( time, 0.0f );
  F32 param_inc = 1.0f / (F32)(num_points - 1);
  F32 param = 0.0f + param_inc;
  for( int i=0; i < num_segments; i++, param += param_inc )
  {
    time += (point_distances[i]/total_distance) * duration;
    curve_parameters.addKey( time, param );
  }

  // Set end time ????
  //end_time = time;
  this->start_time = start_time;
  this->end_time = end_time;

  if (point_distances)
    delete [] point_distances;
}

void afxPath3D::print()
{
  // curve.print();
  curve_parameters.print();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
