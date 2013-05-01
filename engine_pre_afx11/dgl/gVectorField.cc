//------------------------------------------------------------------------------
// Vector Field Rendering Object
//------------------------------------------------------------------------------
#include "dgl/gVectorField.h"
#include "core/fileStream.h"
#include "dgl/dgl.h"
#include "math/mRect.h"
#include "core/color.h"
#include "core/frameAllocator.h"
#include "util/safeDelete.h"

#define NUM_VERTS    mFieldResolution.x * mFieldResolution.y
#define NUM_IDXS     mFieldResolution.x * 2
#define NUM_STRIPS   mFieldResolution.y - 1

#define INDEX(x, y) (x + y * mFieldResolution.x)
#define VERT(x, y)   mFieldVerts[INDEX(x,y)]

//------------------------------------------------------------------------------

VectorField::VectorField()
{
   nullField();
}

//------------------------------------------------------------------------------

VectorField::VectorField( const Point2I &resolution, F32 maxX /* = 1.f */,
                         F32 maxY /* = 1.f */, TextureCoordFlip flip /* = Flip_None */,
                         VectorFieldInitFn initFn /* = NULL  */ )
{
   nullField();

   allocVectorField( resolution );
   initVectorField( maxX, maxY, flip, initFn );
}

//------------------------------------------------------------------------------

VectorField::VectorField( const char *fileName )
{
   nullField();

   loadField( fileName );
}

//------------------------------------------------------------------------------

VectorField::~VectorField()
{
   destroyVectorField();
}

//------------------------------------------------------------------------------

void VectorField::destroyVectorField()
{
   SAFE_DELETE_ARRAY( mVectorField );
   SAFE_DELETE_ARRAY( mFieldIndices );
   SAFE_DELETE_ARRAY( mFieldVerts );

#ifdef ENABLE_FIELD_VISUALIZE
   SAFE_DELETE_ARRAY( mUnflippedVecField );
#endif
}

//------------------------------------------------------------------------------

void VectorField::allocVectorField( const Point2I &resolution )
{
   destroyVectorField();
   mFieldResolution = resolution;
   mVectorField = new Point2F[NUM_VERTS];

   // Create index buffer
   mFieldIndices = new U16[NUM_IDXS];

   bool up = false;
   U32 x = 0, y = 0;

   // It would be way more efficent if we did one index buffer with degenerate
   // polys but you know what, I hate writing that stuff, and after 3 hours on a
   // friday night, I'm going with the easy way.
   for( int i = 0; i < NUM_IDXS; i++ )
   {
      mFieldIndices[i] = INDEX(x, y);

      if( up )
      {
         x++;
         y--;
      }
      else
         y++;

      up = !up;
   }

#ifdef ENABLE_FIELD_VISUALIZE
   mUnflippedVecField = new Point2F[NUM_VERTS];
#endif
}

//------------------------------------------------------------------------------

void FN_CDECL defaultInitFn( const int x, const int y, const Point2I &resolution,
                            const F32 maxX, const F32 maxY, Point2F *outVec )
{
   outVec->x = ( (F32)x / ( (F32)resolution.x - 1.f ) ) * maxX;
   outVec->y = ( (F32)y / ( (F32)resolution.y - 1.f ) ) * maxY;
}

void VectorField::initVectorField( F32 maxX /* = 1.f */, F32 maxY /* = 1.f */, TextureCoordFlip flip /* = Flip_None */, VectorFieldInitFn initFn /* = NULL  */ )
{
   mMax.x = maxX;
   mMax.y = maxY;

   // Create the vertex buffer to render the field
   mFieldVerts = new Point2F[NUM_VERTS];

   // Initialize all this stuff at once since we are working in the same space
   for( int x = 0; x < mFieldResolution.x; x++ )
   {
      for( int y = 0; y < mFieldResolution.y; y++ )
      {
         // Vertices
         defaultInitFn( x, y, mFieldResolution, maxX, maxY, &VERT(x, y) );

         // Initialize the texture coordinates to their normal values
         defaultInitFn( x, y, mFieldResolution, maxX, maxY, &getVector( x, y ) );

         // If provided, pass the normal values on to a custom function
         if( initFn != NULL )
            (*initFn)( x, y, mFieldResolution, maxX, maxY, &getVector( x, y ) );
      }
   }

#ifdef ENABLE_FIELD_VISUALIZE
   dMemcpy( mUnflippedVecField, mVectorField, sizeof(Point2F) * NUM_VERTS );
#endif

   // Flip?
   if( flip != Flip_None )
   {
      FrameAllocatorMarker marker; // Ben = teh win

      // Make this easy, create a copy of the coord array
      Point2F *fieldCopy = (Point2F *)marker.alloc( sizeof(Point2F) * NUM_VERTS );
      dMemcpy( fieldCopy, mVectorField, sizeof(Point2F) * NUM_VERTS );

      for( int i = 0; i < NUM_VERTS; i++ )
      {
         if( flip & Flip_X )
            mVectorField[i].x = fieldCopy[NUM_VERTS - i - 1].x;

         if( flip & Flip_Y )
            mVectorField[i].y = fieldCopy[NUM_VERTS - i - 1].y;
      }
   }
}

//------------------------------------------------------------------------------

#define WRITEPOINT2X( p, s ) s->write( p.x ); s->write( p.y );
#define READPOINT2X( p, s ) s->read( &p.x ); s->read( &p.y );

void VectorField::serialize( Stream *stream ) const
{
   WRITEPOINT2X( mFieldResolution, stream );

   for( int x = 0; x < mFieldResolution.x; x++ )
   {
      for( int y = 0; y < mFieldResolution.y; y++ )
      {
         WRITEPOINT2X( getVector( x, y ), stream );
      }
   }
}

//------------------------------------------------------------------------------

void VectorField::unserialize( Stream *stream )
{
   READPOINT2X( mFieldResolution, stream );

   allocVectorField( mFieldResolution );

   for( int x = 0; x < mFieldResolution.x; x++ )
   {
      for( int y = 0; y < mFieldResolution.y; y++ )
      {
         READPOINT2X( getVector( x, y ), stream );
      }
   }
}

//------------------------------------------------------------------------------

bool VectorField::loadField( const char *fileName )
{
   FileStream fs;

   if( !fs.open( fileName, FileStream::Read ) )
      return false;

   unserialize( &fs );

   return true;
}

//------------------------------------------------------------------------------

bool VectorField::saveField( const char *fileName ) const
{
   FileStream fs;

   if( !fs.open( fileName, FileStream::Write ) )
      return false;

   serialize( &fs );

   return true;
}

//------------------------------------------------------------------------------

void VectorField::renderField( bool texture /* = false */ ) const
{
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D( 0.0, mMax.x, 0.0, mMax.y );


   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadIdentity();

   glEnableClientState( GL_VERTEX_ARRAY );

   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   if( texture )
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );

   glColor4f( 1.f, 1.f, 1.f, 1.f );
   for( int i = 0; i < NUM_STRIPS; i++ )
   {
      glTexCoordPointer( 2, GL_FLOAT, 0, &mVectorField[i * mFieldResolution.x] );
      glVertexPointer( 2, GL_FLOAT, 0, &mFieldVerts[i * mFieldResolution.x] );

      glDrawElements( GL_QUAD_STRIP, NUM_IDXS, GL_UNSIGNED_SHORT, mFieldIndices );
   }

   if( texture )
      glDisableClientState( GL_TEXTURE_COORD_ARRAY );

   glDisableClientState( GL_VERTEX_ARRAY );

   glDisable(GL_BLEND);

   glMatrixMode( GL_MODELVIEW );
   glPopMatrix();
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();
}

#ifdef ENABLE_FIELD_VISUALIZE

void VectorField::visualizeField( F32 alpha /* = 1.0f */ ) const
{
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D( 0.0, mMax.x, 0.0, mMax.y );


   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadIdentity();

   glDisable( GL_TEXTURE_2D );
   glEnable( GL_BLEND );

   // Draw the field in wireframe, apparently it's a back facing poly...whatever
   GLint mode[2];
   glGetIntegerv( GL_POLYGON_MODE, mode );
   glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

   glEnableClientState( GL_VERTEX_ARRAY );
   glColor4f( 1.f, 1.f, 1.f, alpha );

   for( int i = 0; i < NUM_STRIPS; i++ )
   {
      glVertexPointer( 2, GL_FLOAT, 0, &mFieldVerts[i * mFieldResolution.x] );
      glDrawElements( GL_QUAD_STRIP, NUM_IDXS, GL_UNSIGNED_SHORT, mFieldIndices );
   }

   glDisableClientState( GL_VERTEX_ARRAY );
   glPolygonMode( GL_FRONT, mode[0] );
   glPolygonMode( GL_BACK, mode[1] );

   // Draw the vectors
   glBegin( GL_LINES );
   glColor4f( 0.f, 1.f, 0.f, 1.f );
   for( int i = 0; i < NUM_VERTS; i++ )
   {
      // Base point
      glVertex2fv( (F32 *)&mFieldVerts[i] );
      glVertex2fv( (F32 *)&mUnflippedVecField[i]);
   }
   glEnd();

   // Draw the verts
   glPointSize( 3.f );
   glEnable( GL_POINT_SMOOTH );

   glBegin( GL_POINTS );
   glColor4f( 1.f, 0.f, 0.f, alpha );
   for( int i = 0; i < NUM_VERTS; i++ )
      glVertex2fv( (F32 *)&mFieldVerts[i] );
   glEnd();

   glDisable( GL_POINT_SMOOTH );
   glPointSize( 1.f );

   glDisable( GL_BLEND );

   glMatrixMode( GL_MODELVIEW );
   glPopMatrix();
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();
}

#endif