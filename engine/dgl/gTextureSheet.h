//------------------------------------------------------------------------------
// Texture Sheet Object
// Pat Wilson
//------------------------------------------------------------------------------

#ifndef _GTEXTURE_SHEET_H_
#define _GTEXTURE_SHEET_H_

#include "math/mPoint.h"
#include "dgl/gBitmap.h"
#include "dgl/gTexManager.h"
#include "core/tVector.h"

// Because this is useful
#define SAFE_DELETE(x) if( x != NULL ) { delete x; x = NULL; }

/// This class will take a bunch of GBitmaps and stich them together into a large
/// texture sheet to avoid a bunch of texture state changes in specific applications
/// of this class.
class GTextureSheet
{
private:
   friend void textureSheetCB(const U32 eventCode, void *userData);

   U32 mTextureCBHandle;
   TextureHandle *mTextureHandle;   ///< The texture handle for this sheet
   U32 mNumTextures;                ///< Number of textures in the sheet
   Point2I mSheetDimensions;        ///< Size of the texture sheet
   Point2I mTextureSize;            ///< Size of each texture in the sheet

   GBitmap *mBitmapData; ///< Used to keep the bitmap we've generated around.
public:

   const static U32 MAX_SHEET_SIZE = 1024;

   /// Default Constructor
   GTextureSheet() : mTextureHandle(NULL), mNumTextures(0), mTextureCBHandle(0) {};

   /// Constructor that also builds the sheet.
   /// @see buildTextureSheet
   /// @param  bitmaps  Vector of GBitmaps to construct this sheet with
   GTextureSheet( Vector<GBitmap *> bitmaps );

   /// Destructor
   virtual ~GTextureSheet();
   
   /// Create the texture sheet
   /// @note This will delete the GBitmaps that are inputted
   /// @param  bitmaps  Vector of GBitmaps to construct this sheet with
   bool buildTextureSheet( Vector<GBitmap *> bitmaps );

   /// Get the texture handle for the sheet
   virtual const TextureHandle *getTextureHandle() const { return mTextureHandle; }

   /// Get adjusted min/max texture coordinates for a certain bitmap
   /// @param  bitmapIndex Index of the bitmap in the sheet
   /// @param  min         The minimum texture coordinate will be stored here
   /// @param  max         The maximum texture coordinate will be stored here
   virtual bool getTextureCoords( const U32 bitmapIndex, Point2F *min, Point2F *max ) const;

   /// Gets the number of textures in this sheet
   virtual U32 getNumTextures() const { return mNumTextures; }

   /// Returns true if this sheet is ready to be used
   virtual bool isValid() const { return mTextureHandle != NULL; }
};

#endif