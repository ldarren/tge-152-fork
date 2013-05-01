//------------------------------------------------------------------------------
// Texture Sheet Object
// Pat Wilson
//------------------------------------------------------------------------------
#include "dgl/gTextureSheet.h"
#include "console/console.h"

// Uncomment this line to save-to-file the bitmap so you can look at it
//#define DEBUG_RENDER_SHEET


void textureSheetCB(const U32 eventCode, void *userData)
{
   GTextureSheet* ts = reinterpret_cast<GTextureSheet*>(userData);

   if(eventCode==TextureManager::BeginZombification)
   {
      SAFE_DELETE(ts->mTextureHandle);
   }
   else if(eventCode==TextureManager::CacheResurrected)
   {
      if(ts->mBitmapData)
         ts->mTextureHandle = new TextureHandle( NULL, ts->mBitmapData, TerrainTexture, true );
   }
}


//------------------------------------------------------------------------------

GTextureSheet::GTextureSheet( Vector<GBitmap *> bitmaps ) : mTextureHandle(NULL), mNumTextures(0)
{
   GTextureSheet();

   // Build the texture sheet
   buildTextureSheet( bitmaps );
}

//------------------------------------------------------------------------------

bool GTextureSheet::buildTextureSheet( Vector<GBitmap *> bitmaps )
{
   // Check to see if there is anything in the array
   if( bitmaps.size() < 1 )
      return false;

   // Check to make sure all images are the same size and format, just
   // because that is a reasonable limitation.
   U32 width = bitmaps[0]->getWidth();
   U32 height = bitmaps[0]->getHeight();
   GBitmap::BitmapFormat format = bitmaps[0]->getFormat();

   for( int i = 1; i < bitmaps.size(); i++ )
   {
      if( bitmaps[i]->getWidth() != width || 
          bitmaps[i]->getHeight() != height )
      {
         AssertFatal( false, "[GTextureSheet]: Bitmaps in a GTextureSheet must all be the same size." );
         return false;
      }
   }

   // Store this
   mTextureSize.x = width;
   mTextureSize.y = height;

   // Figure out, for this format, how many bytes per pixel there are
   U32 bytesPerPixel;

   switch( format ) 
   {
     case GBitmap::Alpha:     
     case GBitmap::Palettized:
     case GBitmap::Luminance:
     case GBitmap::Intensity:  bytesPerPixel = 1;
        break;
     case GBitmap::RGB:        bytesPerPixel = 3;
        break;
     case GBitmap::RGBA:       bytesPerPixel = 4;
        break;
     case GBitmap::RGB565:    
     case GBitmap::RGB5551:    bytesPerPixel = 2;
        break;
     default:
        AssertFatal( false, "GTextureSheet: GBitmap format not understood." );
        break;
   }

   GBitmap *newbitmap = NULL;

   // Ok so try to see if we can even fit this texture in the max texture size
   if( width * height * bitmaps.size() <= MAX_SHEET_SIZE * MAX_SHEET_SIZE )
   {
      // Ok it will fit, good, so if each texture was 512x512 and there are 4 textures
      // it would look like this:
      // |0|1|
      // |2|3|
      // If each texture was 256x256 and there are 5 textures, then it would look
      // like this:
      // |0|1|2|3|
      // |5|
      // If only 4 of the same textures:
      // |0|1|2|3|
      U32 numCols = U32( mFloor( MAX_SHEET_SIZE / (F32)width ) );
      U32 numRows = U32( mCeil( bitmaps.size() / (F32)numCols ) );

      mSheetDimensions.x = numCols;
      mSheetDimensions.y = numRows;

      // This is a pointer because we are going to pass it in to the
      // TextureHandle constructor, then it will own it, not us, so don't
      // delete it or anything.
      newbitmap = new GBitmap( width * numCols, height * numRows, false, format );

      U8 *newbits = newbitmap->getWritableBits();

      U32 curCol = 0;
      U32 curRow = 0;
      // Copy those suckers in
      for( U32 j = 0; j < bitmaps.size(); j++ )
      {
         for( U32 i = 0; i < height; i++ )
         {
            dMemcpy( &newbits[( i * width * bytesPerPixel * numCols )
                              + ( curCol * width * bytesPerPixel )
                              + ( curRow * width * height * numCols * bytesPerPixel )], 
                     &bitmaps[j]->getBits()[i * width * bytesPerPixel], 
                     width * bytesPerPixel );
         }

         // Check to see if we should jump down a row
         if( ++curCol == numCols )
         {
            curCol = 0;
            curRow++;
         }
      }
   }
   else
   {
      // It's too big, tell em to shove it
      AssertFatal( false, avar( "[GTextureSheet]: This texture sheet would exceed max size of %dx%d.", MAX_SHEET_SIZE, MAX_SHEET_SIZE ) );
      return false;
   }

   // It is all good, so set this
   mNumTextures = bitmaps.size();

   // Trash the old texture if it exists
   SAFE_DELETE( mTextureHandle );

   // This block will render out the texture sheet
#ifdef DEBUG_RENDER_SHEET
   FileStream fs;
   static S32 id=0;
   fs.open( avar("test%d.png", id++), FileStream::Write );
   newbitmap->writePNG( fs );
   fs.close();
#endif

   // Delete the input textures
   for( U32 i = 0; i < bitmaps.size(); i++ )
      delete bitmaps[i];

   // Create the new texture handle. We abuse TerrainTexture for this purpose.
   // TerrainTexture bitmaps get ignored; so as long as we get out of the
   // texturemanager at the appropriate time we're set. We do this, so it's
   // all good.
   newbitmap->extrudeMipLevels(true);
   mTextureHandle = new TextureHandle( NULL, newbitmap, TerrainTexture, true );
   mBitmapData = newbitmap;
   
   // Set up our call back.
   mTextureCBHandle = TextureManager::registerEventCallback(textureSheetCB, this);

   return true;
}

//------------------------------------------------------------------------------

bool GTextureSheet::getTextureCoords( const U32 bitmapIndex, Point2F *min, Point2F *max ) const
{
   if( bitmapIndex >= mNumTextures )
      return false;

   U32 row = bitmapIndex % mSheetDimensions.x;
   U32 col = U32( mFloor( bitmapIndex / (F32)mSheetDimensions.x ) );
   
   min->x = ( 1.f / mSheetDimensions.x ) * row + (1.f / this->mTextureHandle->getWidth());
   min->y = ( 1.f / mSheetDimensions.y ) * col + (1.f / this->mTextureHandle->getHeight());

   max->x = ( 1.f / mSheetDimensions.x ) * ( row + 1 ) - (1.f / this->mTextureHandle->getWidth());
   max->y = ( 1.f / mSheetDimensions.y ) * ( col + 1 ) - (1.f / this->mTextureHandle->getHeight());

   return true;
}

//------------------------------------------------------------------------------

GTextureSheet::~GTextureSheet()
{
   if(mTextureCBHandle)
      TextureManager::unregisterEventCallback(mTextureCBHandle);

   SAFE_DELETE( mTextureHandle );
}