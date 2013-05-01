//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//        enhanced-particle -- increased keys to 8.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "particle.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#include "math/mathIO.h"

static ParticleData gDefaultParticleData;


IMPLEMENT_CO_DATABLOCK_V1(ParticleData);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
ParticleData::ParticleData()
{
   dragCoefficient      = 0.0f;
   windCoefficient      = 1.0f;
   gravityCoefficient   = 0.0f;
   inheritedVelFactor   = 0.0f;
   constantAcceleration = 0.0f;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 0;
   spinSpeed            = 0.0f;
   spinRandomMin        = 0.0f;
   spinRandomMax        = 0.0f;
   useInvAlpha          = false;
   animateTexture       = false;
#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   allowLighting        = false;
   oldstyleAnimated     = false;
#endif

   numFrames            = 1;
   framesPerSec         = numFrames;

   S32 i;
   for( i=0; i<PDC_NUM_KEYS; i++ )
   {
      colors[i].set( 1.0f, 1.0f, 1.0f, 1.0f );
      sizes[i] = 1.0f;
   }

   times[0] = 0.0f;
   times[1] = 1.0f;
   // AFX CODE BLOCK (enhanced-particle) <<
   for (i = 2; i < PDC_NUM_KEYS; i++)
     times[i] = -1.0f;
   /* ORIGINAL CODE
   times[2] = 2.0f;
   times[3] = 2.0f;
   */
   // AFX CODE BLOCK (enhanced-particle) >>

   texCoords[0].set(0.0,0.0);   // texture coords at 4 corners
   texCoords[1].set(0.0,1.0);   // of particle quad
   texCoords[2].set(1.0,1.0);   // (defaults to entire particle)
   texCoords[3].set(1.0,0.0);
   animTexTiling.set(0,0);      // tiling dimensions 
   animTexFramesString = NULL;  // string of animation frame indices
   animTexUVs = NULL;           // array of tile vertex UVs
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   textureName = NULL;          // texture filename
   textureHandle = NULL;        // loaded texture handle
#else
   dMemset( textureNameList, 0, sizeof( textureNameList ) );
   dMemset( textureList, 0, sizeof( textureList ) );
#endif

   // AFX CODE BLOCK (enhanced-particle) <<
   constrain_pos = false;
   // AFX CODE BLOCK (enhanced-particle) >>
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
ParticleData::~ParticleData()
{
   if (animTexUVs)
   {
      delete [] animTexUVs;
   }
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleData::initPersistFields()
{
   Parent::initPersistFields();

   addField("dragCoefficient",      TypeF32,    Offset(dragCoefficient,      ParticleData));
   addField("windCoefficient",      TypeF32,    Offset(windCoefficient,      ParticleData));
   addField("gravityCoefficient",   TypeF32,    Offset(gravityCoefficient,   ParticleData));
   addField("inheritedVelFactor",   TypeF32,    Offset(inheritedVelFactor,   ParticleData));
   addField("constantAcceleration", TypeF32,    Offset(constantAcceleration, ParticleData));
   addField("lifetimeMS",           TypeS32,    Offset(lifetimeMS,           ParticleData));
   addField("lifetimeVarianceMS",   TypeS32,    Offset(lifetimeVarianceMS,   ParticleData));
   addField("spinSpeed",            TypeF32,    Offset(spinSpeed,            ParticleData));
   addField("spinRandomMin",        TypeF32,    Offset(spinRandomMin,        ParticleData));
   addField("spinRandomMax",        TypeF32,    Offset(spinRandomMax,        ParticleData));
   addField("useInvAlpha",          TypeBool,   Offset(useInvAlpha,          ParticleData));
   addField("animateTexture",       TypeBool,   Offset(animateTexture,       ParticleData));
   addField("framesPerSec",         TypeS32,    Offset(framesPerSec,         ParticleData));

   addField("textureCoords",        TypePoint2F,  Offset(texCoords,          ParticleData),  4);
   addField("animTexTiling",        TypePoint2I,  Offset(animTexTiling,      ParticleData));
   addField("animTexFrames",        TypeString,   Offset(animTexFramesString,ParticleData));
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   addField("textureName",          TypeFilename, Offset(textureName,        ParticleData));
   addField("animTexName",          TypeFilename, Offset(textureName,        ParticleData));
#else
   addField("textureName",          TypeFilename, Offset(textureNameList,    ParticleData));
   addField("animTexName",          TypeFilename, Offset(textureNameList,    ParticleData), PDC_MAX_TEX );
#endif

   // Interpolation variables
   addField("colors",               TypeColorF, Offset(colors,               ParticleData), PDC_NUM_KEYS );
   addField("sizes",                TypeF32,    Offset(sizes,                ParticleData), PDC_NUM_KEYS );
   addField("times",                TypeF32,    Offset(times,                ParticleData), PDC_NUM_KEYS );

#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   addField("allowLighting",      TypeBool,   Offset(allowLighting, ParticleData));
#endif

   // AFX CODE BLOCK (enhanced-particle) <<
   addField("constrainPos",         TypeBool,   Offset(constrain_pos,        ParticleData));
   // AFX CODE BLOCK (enhanced-particle) >>
}

//-----------------------------------------------------------------------------
// Pack data
//-----------------------------------------------------------------------------
void ParticleData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeFloat(dragCoefficient / 5, 10);
   if(stream->writeFlag(windCoefficient != gDefaultParticleData.windCoefficient))
      stream->write(windCoefficient);

   // AFX CODE BLOCK (bug-fix) <<
   // With original implementation, gravityCoefficient could never
   // be zero on client.
   if (stream->writeFlag(gravityCoefficient != 0.0f))
     stream->writeSignedFloat(gravityCoefficient / 10, 12); 
   /* ORIGINAL CODE
   stream->writeSignedFloat(gravityCoefficient / 10, 12);
   */
   // AFX CODE BLOCK (bug-fix) >>

   stream->writeFloat(inheritedVelFactor, 9);
   if(stream->writeFlag(constantAcceleration != gDefaultParticleData.constantAcceleration))
      stream->write(constantAcceleration);

   stream->write( lifetimeMS );
   stream->write( lifetimeVarianceMS );

   if(stream->writeFlag(spinSpeed != gDefaultParticleData.spinSpeed))
      stream->write(spinSpeed);
   if(stream->writeFlag(spinRandomMin != gDefaultParticleData.spinRandomMin || spinRandomMax != gDefaultParticleData.spinRandomMax))
   {
      stream->writeInt((S32)(spinRandomMin + 1000), 11);
      stream->writeInt((S32)(spinRandomMax + 1000), 11);
   }
   stream->writeFlag(useInvAlpha);

   S32 i, count;

   // see how many frames there are:
   for(count = 0; count < ParticleData::PDC_NUM_KEYS-1; count++)
      if(times[count] >= 1)
         break;

   count++;

   // AFX CODE BLOCK (enhanced-particle) <<
   // An extra bit is needed for 8 keys.
   stream->writeInt(count-1, 3);
   /* ORIGINAL CODE
   stream->writeInt(count-1, 2);
   */
   // AFX CODE BLOCK (enhanced-particle) >>

   for( i=0; i<count; i++ )
   {
      stream->writeFloat( colors[i].red, 7);
      stream->writeFloat( colors[i].green, 7);
      stream->writeFloat( colors[i].blue, 7);
      stream->writeFloat( colors[i].alpha, 7);
      stream->writeFloat( sizes[i]/MaxParticleSize, 14);
      stream->writeFloat( times[i], 8);
   }

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   if (stream->writeFlag(textureName && textureName[0]))
     stream->writeString(textureName);
#else
   for( count=0; count<PDC_MAX_TEX; count++ )
      if(!textureNameList[count])
         break;
   stream->writeInt(count, 6);
   for(i = 0; i < count; i++)
      stream->writeString( textureNameList[i] );
#endif

   for (i = 0; i < 4; i++)
      mathWrite(*stream, texCoords[i]);
   if (stream->writeFlag(animateTexture))
   {
      if (stream->writeFlag(animTexFramesString && animTexFramesString[0]))
      {
         stream->writeString(animTexFramesString);
      }
      mathWrite(*stream, animTexTiling);
      stream->writeInt(framesPerSec, 8);
   }
      
#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   stream->writeFlag(allowLighting);
#endif

   // AFX CODE BLOCK (enhanced-particle) <<
   stream->writeFlag(constrain_pos);
   // AFX CODE BLOCK (enhanced-particle) >>
}

//-----------------------------------------------------------------------------
// Unpack data
//-----------------------------------------------------------------------------
void ParticleData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   dragCoefficient = stream->readFloat(10) * 5;
   if(stream->readFlag())
      stream->read(&windCoefficient);
   else
      windCoefficient = gDefaultParticleData.windCoefficient;

   // AFX CODE BLOCK (bug-fix) <<
   // With original implementation, gravityCoefficient could never
   // be zero on client.
   if (stream->readFlag()) 
     gravityCoefficient = stream->readSignedFloat(12)*10; 
   else 
     gravityCoefficient = 0.0f; 
   /* ORIGINAL CODE
   gravityCoefficient = stream->readSignedFloat(12) * 10;
   */
   // AFX CODE BLOCK (bug-fix) >>

   inheritedVelFactor = stream->readFloat(9);
   if(stream->readFlag())
      stream->read(&constantAcceleration);
   else
      constantAcceleration = gDefaultParticleData.constantAcceleration;

   stream->read( &lifetimeMS );
   stream->read( &lifetimeVarianceMS );

   if(stream->readFlag())
      stream->read(&spinSpeed);
   else
      spinSpeed = gDefaultParticleData.spinSpeed;

   if(stream->readFlag())
   {
      spinRandomMin = (F32)(stream->readInt(11) - 1000);
      spinRandomMax = (F32)(stream->readInt(11) - 1000);
   }
   else
   {
      spinRandomMin = gDefaultParticleData.spinRandomMin;
      spinRandomMax = gDefaultParticleData.spinRandomMax;
   }

   useInvAlpha = stream->readFlag();

   S32 i;
   // AFX CODE BLOCK (enhanced-particle) <<
   // An extra bit is needed for 8 keys.
   S32 count = stream->readInt(3) + 1;
   /* ORIGINAL CODE
   S32 count = stream->readInt(2) + 1;
   */
   // AFX CODE BLOCK (enhanced-particle) >>
   for(i = 0;i < count; i++)
   {
      colors[i].red = stream->readFloat(7);
      colors[i].green = stream->readFloat(7);
      colors[i].blue = stream->readFloat(7);
      colors[i].alpha = stream->readFloat(7);
      sizes[i] = stream->readFloat(14) * MaxParticleSize;
      times[i] = stream->readFloat(8);
   }

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   textureName = (stream->readFlag()) ? stream->readSTString() : 0;
#else
   count = stream->readInt(6);
   for(i = 0; i < count;i ++)
      textureNameList[i] = stream->readSTString();
#endif

   for (i = 0; i < 4; i++)
      mathRead(*stream, &texCoords[i]);
   if (animateTexture = stream->readFlag())
   {
     animTexFramesString = (stream->readFlag()) ? stream->readSTString() : 0;
     mathRead(*stream, &animTexTiling);
     framesPerSec = stream->readInt(8);
   }
      
#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   allowLighting = stream->readFlag();
#endif

   // AFX CODE BLOCK (enhanced-particle) <<
   constrain_pos = stream->readFlag();
   // AFX CODE BLOCK (enhanced-particle) >>
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleData::onAdd()
{
   if (Parent::onAdd() == false)
      return false;

   return loadParameters();
}

bool ParticleData::loadParameters()
{
   if (dragCoefficient < 0.0f) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) drag coeff less than 0", getName());
      dragCoefficient = 0.0f;
   }
   
   if (lifetimeMS < 1) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) lifetime < 1 ms", getName());
      lifetimeMS = 1;
   }
   
   if (lifetimeVarianceMS >= lifetimeMS) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) lifetimeVariance >= lifetime", getName());
      lifetimeVarianceMS = lifetimeMS - 1;
   }
   
   if (spinSpeed > 10000.0f || spinSpeed < -10000.0f) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinSpeed invalid", getName());
      return false;
   }
   
   if (spinRandomMin > 10000.0f || spinRandomMin < -10000.0f) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinRandomMin invalid", getName());
      spinRandomMin = -360.0f;
      return false;
   }
   
   if (spinRandomMin > spinRandomMax) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinRandomMin greater than spinRandomMax", getName());
      spinRandomMin = spinRandomMax - (spinRandomMin - spinRandomMax );
      return false;
   }
   
   if (spinRandomMax > 10000.0f || spinRandomMax < -10000.0f) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) spinRandomMax invalid", getName());
      spinRandomMax = 360.0f;
      return false;
   }
   if (framesPerSec > 255)
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) framesPerSec > 255, too high", getName());
      framesPerSec = 255;
      return false;
   }

   times[0] = 0.0f;
   for (U32 i = 1; i < PDC_NUM_KEYS; i++) 
   {
     if (times[i] < 0.0f)
       break;
     if (times[i] < times[i-1]) 
     {
       Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) times[%d] < times[%d]", getName(), i, i-1);
       times[i] = times[i-1];
     }
   }

   times[0] = 0.0f;

   U32 last_idx = 0;
   for (U32 i = 1; i < PDC_NUM_KEYS; i++)
   {
     if (times[i] < 0.0f)
       break;
     else
       last_idx = i;
   }

   for (U32 i = last_idx+1; i < PDC_NUM_KEYS; i++) 
   {
      times[i] = times[last_idx];
      colors[i] = colors[last_idx];
      sizes[i] = sizes[last_idx];
   }

#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   if (animateTexture && animTexTiling.x == 0 && animTexTiling.y == 0 && animTexFramesString == NULL)
   {
      oldstyleAnimated = true;
      return true;
   }
#endif

   // Here we validate parameters
   if (animateTexture) 
   {
     // Tiling dimensions must be positive and non-zero
     if (animTexTiling.x <= 0 || animTexTiling.y <= 0)
     {
       Con::warnf(ConsoleLogEntry::General, 
                  "ParticleData(%s) bad value(s) for animTexTiling [%d or %d <= 0], invalid datablock", 
                  animTexTiling.x, animTexTiling.y, getName());
       return false;
     }

     // Indices must fit into a byte so these are also bad
     if (animTexTiling.x * animTexTiling.y > 256)
     {
       Con::warnf(ConsoleLogEntry::General, 
                  "ParticleData(%s) bad values for animTexTiling [%d*%d > %d], invalid datablock", 
                  animTexTiling.x, animTexTiling.y, 256, getName());
       return false;
     }

     // A list of frames is required
     if (!animTexFramesString || !animTexFramesString[0]) 
     {
       Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) no animTexFrames, invalid datablock", getName());
       return false;
     }

     // The frame list cannot be too long.
     if (animTexFramesString && dStrlen(animTexFramesString) > 255) 
     {
       Con::errorf(ConsoleLogEntry::General, "ParticleData(%s) animTexFrames string too long [> 255 chars]", getName());
       return false;
     }
   }

   return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool ParticleData::preload(bool server, char errorBuffer[256])
{
   if (Parent::preload(server, errorBuffer) == false)
      return false;

   if(!server)
   {
      if (!reload(errorBuffer))
         return false;
   }

   return true;
}

bool ParticleData::reload(char errorBuffer[256])
{
   bool error = false;

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   // Here we attempt to load the particle's texture if specified. An undefined
   // texture is *not* an error since the emitter may provide one.
   if (textureName && textureName[0])
   {
      textureHandle = GFXTexHandle(textureName, &GFXDefaultStaticDiffuseProfile);
      if (!textureHandle)
      {
         dSprintf(errorBuffer, 256, "Missing particle texture: %s", textureName);
         error = true;
      }
   }
#else
   numFrames = 0;
   for( int i=0; i<PDC_MAX_TEX; i++ )
   {
      if( textureNameList[i] && textureNameList[i][0] )
      {
         textureList[i] = TextureHandle( textureNameList[i], MeshTexture );
         if (!textureList[i].getName())
         {
            dSprintf(errorBuffer, 256, "Missing particle texture: %s", textureNameList[i]);
            error = true;
         }
         numFrames++;
      }
   }
#endif

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   if (animateTexture) 
#else
   if (animateTexture && !oldstyleAnimated) 
#endif
   {
      // Here we parse animTexFramesString into byte-size frame numbers in animTexFrames.
      // Each frame token must be separated by whitespace.
      // A frame token must be a positve integer frame number or a range of frame numbers
      // separated with a '-'. 
      // The range separator, '-', cannot have any whitspace around it.
      // Ranges can be specified to move through the frames in reverse as well as forward.
      // Frame numbers exceeding the number of tiles will wrap.
      //   example:
      //     "0-16 20 19 18 17 31-21"

      S32 n_tiles = animTexTiling.x * animTexTiling.y;
      AssertFatal(n_tiles > 0 && n_tiles <= 256, "Error, bad animTexTiling setting." );

      animTexFrames.clear();

      char* tokCopy = new char[dStrlen(animTexFramesString) + 1];
      dStrcpy(tokCopy, animTexFramesString);

      char* currTok = dStrtok(tokCopy, " \t");
      while (currTok != NULL) 
      {
         char* minus = dStrchr(currTok, '-');
         if (minus)
         { 
            // add a range of frames
            *minus = '\0';
            S32 range_a = dAtoi(currTok);
            S32 range_b = dAtoi(minus+1);
            if (range_b < range_a)
            {
               // reverse frame range
               for (S32 i = range_a; i >= range_b; i--)
                  animTexFrames.push_back((U8)(i % n_tiles));
            }
            else
            {
               // forward frame range
               for (S32 i = range_a; i <= range_b; i++)
                  animTexFrames.push_back((U8)(i % n_tiles));
            }
         }
         else
         {
            // add one frame
            animTexFrames.push_back((U8)(dAtoi(currTok) % n_tiles));
         }
         currTok = dStrtok(NULL, " \t");
      }

      // Here we pre-calculate the UVs for each frame tile, which are
      // tiled inside the UV region specified by texCoords. Since the
      // UVs are calculated using bilinear interpolation, the texCoords
      // region does *not* have to be an axis-aligned rectangle.

      if (animTexUVs)
         delete [] animTexUVs;

      animTexUVs = new Point2F[(animTexTiling.x+1)*(animTexTiling.y+1)];

      // interpolate points on the left and right edge of the uv quadrangle
      Point2F lf_pt = texCoords[0];
      Point2F rt_pt = texCoords[3];

      // per-row delta for left and right interpolated points
      Point2F lf_d = (texCoords[1] - texCoords[0])/(F32)animTexTiling.y;
      Point2F rt_d = (texCoords[2] - texCoords[3])/(F32)animTexTiling.y;

      S32 idx = 0;
      for (S32 yy = 0; yy <= animTexTiling.y; yy++)
      {
         Point2F p = lf_pt;
         Point2F dp = (rt_pt - lf_pt)/(F32)animTexTiling.x;
         for (S32 xx = 0; xx <= animTexTiling.x; xx++)
         {
            animTexUVs[idx++] = p;
            p += dp;
         }
         lf_pt += lf_d;
         rt_pt += rt_d;
      }

      // cleanup
      delete [] tokCopy;
      numFrames = animTexFrames.size();
   }

   return !error;
}

//-----------------------------------------------------------------------------
// Initialize particle
//-----------------------------------------------------------------------------
void ParticleData::initializeParticle(Particle* init, const Point3F& inheritVelocity)
{
   init->dataBlock = this;

   // Calculate the constant accleration...
   init->vel += inheritVelocity * inheritedVelFactor;
   init->acc  = init->vel * constantAcceleration;

   // Calculate this instance's lifetime...
   init->totalLifetime = lifetimeMS;
   if (lifetimeVarianceMS != 0)
      init->totalLifetime += S32(gRandGen.randI() % (2 * lifetimeVarianceMS + 1)) - S32(lifetimeVarianceMS);

   // assign spin amount
   init->spinSpeed = spinSpeed + gRandGen.randF( spinRandomMin, spinRandomMax );
}

// AFX CODE BLOCK (to-tgea-from-tge) <<
ConsoleMethod(ParticleData, reload, void, 2, 2, "(void)"
              "Reloads this particle")
{
   object->loadParameters();
   char errorBuffer[256];
   object->reload(errorBuffer);
}
// AFX CODE BLOCK (to-tgea-from-tge) >>

