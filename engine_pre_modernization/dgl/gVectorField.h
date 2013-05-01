//------------------------------------------------------------------------------
// Vector Field
//------------------------------------------------------------------------------

#ifndef _GVECTORFIELD_H_
#define _GVECTORFIELD_H_

#include "math/mPoint.h"
#include "core/stream.h"

// Comment this line out to disable the ability to render out the field
#define ENABLE_FIELD_VISUALIZE

/// Function pointer that is used for functions to init vector fields
/// @note This function will be passed the normal texture coordinates in outVec
typedef void(FN_CDECL *VectorFieldInitFn)( const int x, const int y, const Point2I &resolution, const F32 maxX, const F32 maxY, Point2F *outVec );

class VectorField
{

private:
   Point2I mFieldResolution;  ///< Resolution of the vector field, x * y = number of quads
   Point2F *mVectorField;     ///< Vector Field

   Point2F *mFieldVerts;      ///< Vertices for rendering the field
   U16     *mFieldIndices;    ///< Indices for rendering the field

   Point2F mMax;              ///< Maximum texture coordinate values

#ifdef ENABLE_FIELD_VISUALIZE
   Point2F *mUnflippedVecField;  ///< If the coords get flipped, then this will be unflipped
#endif

   /// Helper function to create the vector field
   ///
   /// @param  resolution     Vector Field resolution
   void allocVectorField( const Point2I &resolution );

   /// Helper function to destroy the vector field
   void destroyVectorField();

   /// Helper function to null the pointers
   void nullField();

   /// Helper inline just so I can store the vector field as a 1d array but access
   /// it as a 2d array
   Point2F &getVector( const U32 x, const U32 y );
   const Point2F &getVector( const U32 x, const U32 y ) const;

   /// Put this field out to a stream
   void serialize( Stream *stream ) const;

   /// Read a stream and set up this field
   void unserialize( Stream *stream );

public:
   /// This is used to specify flip options for creating the texture coord array
   enum TextureCoordFlip
   {
      Flip_None = 0,
      Flip_X    = BIT(0),
      Flip_Y    = BIT(1),
      Flip_XY   = Flip_X | Flip_Y
   };

   /// Default Constructor
   VectorField();

   /// Constructor
   ///
   /// @param  resolution     Vector Field resolution
   /// @param  maxX           Maximum value that can be assigned to an x-coord
   /// @param  maxY           Maximum value that can be assigned to a y-coord
   /// @param  flip           Flipping options for the texture coordinates
   /// @param  initFn         Function pointer to init the vector field values with
   VectorField( const Point2I &resolution, const F32 maxX = 1.f, const F32 maxY = 1.f,
                TextureCoordFlip flip = Flip_None, VectorFieldInitFn initFn = NULL );

   /// Constructor with file name
   ///
   /// @param  fileName       File name to load from
   VectorField( const char *fileName );

   /// Destructor
   ~VectorField();

   /// Init the vector field
   /// @param  maxX           Maximum value that can be assigned to an x-coord
   /// @param  maxY           Maximum value that can be assigned to a y-coord
   /// @param  flip           Flipping options for the texture coordinates
   /// @param  initFn         Function pointer to init the vector field values with
   void initVectorField( F32 maxX = 1.f, F32 maxY = 1.f, TextureCoordFlip flip = Flip_None, VectorFieldInitFn initFn = NULL );

   /// Load the vector field from a file
   /// @see saveField
   ///
   /// @param  fileName       File name to load from
   bool loadField( const char *fileName );

   /// Save the vector field to a file
   ///
   /// @param fileName        File name to save to
   bool saveField( const char *fileName ) const;

   /// Render out the vector field
   ///
   /// @param  texture        True if this should enable texturing
   void renderField( bool texture = false ) const;

   /// Render the field to let someone visualize what it is doing
   void visualizeField( F32 alpha = 1.f ) const;
};

//------------------------------------------------------------------------------

inline Point2F &VectorField::getVector( const U32 x, const U32 y )
{
   AssertFatal( x < mFieldResolution.x && y < mFieldResolution.y, "Vector Field access out of bounds" );

   return mVectorField[x + y * mFieldResolution.x];
}

inline const Point2F &VectorField::getVector( const U32 x, const U32 y ) const
{
   AssertFatal( x < mFieldResolution.x && y < mFieldResolution.y, "Vector Field access out of bounds" );

   return mVectorField[x + y * mFieldResolution.x];
}

//------------------------------------------------------------------------------

#ifndef ENABLE_FIELD_VISUALIZE
inline void VectorField::visualizeField( F32 alpha /* = 0.5f */ ) const
{
}
#endif

//------------------------------------------------------------------------------

inline void VectorField::nullField()
{
#ifdef ENABLE_FIELD_VISUALIZE
   mUnflippedVecField = NULL;
#endif
   mFieldVerts = NULL;
   mFieldIndices = NULL;
   mVectorField = NULL;
}

#endif