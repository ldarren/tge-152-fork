//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#ifndef _FORESTTEXTURESHEET_H_
#define _FORESTTEXTURESHEET_H_

#include "ts/tsshapeInstance.h"
#include "dgl/gTextureSheet.h"

class ForestTextureSheet : public GTextureSheet
{
private:
   U32 mNumEquatorSteps;
   U32 mNumPolarSteps;
   F32 mPolarAngle;
   bool mIncludePoles;
   U32 mTextureSize;

public:
   ForestTextureSheet( TSShapeInstance *shape, U32 numEquatorSteps, U32 numPolarSteps, F32 polarAngle, bool includePoles, S32 detailLevel, U32 textureSize );

   void createTextureSheet( TSShapeInstance *shape, U32 numEquatorSteps, U32 numPolarSteps, F32 polarAngle, bool includePoles, S32 detailLevel, U32 textureSize );
   const S32 getIndexFromMatrix( const F32 *m, F32 *outRotY ) const;
};

#endif
