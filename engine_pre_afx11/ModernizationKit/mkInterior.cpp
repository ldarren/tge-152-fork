/*  mkInterior.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkInterior.h"
#include "interior/interior.h"
#include "interior/itf.h"
#include "sceneGraph/sceneGraph.h"
#include "platform/profiler.h"
#include "dgl/materialPropertyMap.h"
#include "dgl/materialList.h"
#include "core/frameAllocator.h"
#include "ModernizationKit/ModernizationKit.h"

// If this is uncommented, we generate material definitions
#define MK_AUTOGEN_MATERIALS

// If this is uncommented, we set up the materials for parallax mapping.
#ifdef MK_AUTOGEN_MATERIALS
#define MK_AUTOGEN_NORMALMAP
#endif

Point3F curCamPos;

void Interior::fillSurfaceTexMats()
{
   for(U32 i = 0; i < mSurfaces.size(); i++)
   {
      Surface &surface = mSurfaces[i];
      
      const PlaneF & plane = getPlane(surface.planeIndex);
      
      Point3F planeNorm = plane;
      if(planeIsFlipped(surface.planeIndex))
      {
         planeNorm = -planeNorm;
      }
      
      OutputPoint pts[3];
      pts[0].point.x = mPoints[mWindings[surface.windingStart + 1]].point.x;
      pts[0].point.y = mPoints[mWindings[surface.windingStart + 1]].point.y;
      pts[0].point.z = mPoints[mWindings[surface.windingStart + 1]].point.z;
      pts[1].point.x = mPoints[mWindings[surface.windingStart + 0]].point.x;
      pts[1].point.y = mPoints[mWindings[surface.windingStart + 0]].point.y;
      pts[1].point.z = mPoints[mWindings[surface.windingStart + 0]].point.z;
      pts[2].point.x = mPoints[mWindings[surface.windingStart + 2]].point.x;
      pts[2].point.y = mPoints[mWindings[surface.windingStart + 2]].point.y;
      pts[2].point.z = mPoints[mWindings[surface.windingStart + 2]].point.z;
      
      TexGenPlanes texPlanes = mTexGenEQs[surface.texGenIndex];
      
      for( U32 j=0; j<3; j++ )
      {
         pts[j].texCoord.x = texPlanes.planeX.x * pts[j].point.x +
                             texPlanes.planeX.y * pts[j].point.y +
                             texPlanes.planeX.z * pts[j].point.z +
                             texPlanes.planeX.d;


         pts[j].texCoord.y = texPlanes.planeY.x * pts[j].point.x +
                             texPlanes.planeY.y * pts[j].point.y +
                             texPlanes.planeY.z * pts[j].point.z +
                             texPlanes.planeY.d;

      }
      
      Point3F edge1, edge2;
      Point3F cp;
      Point3F S,T,SxT;

      // x, s, t
      edge1.set( pts[1].point.x - pts[0].point.x, pts[1].texCoord.x - pts[0].texCoord.x, pts[1].texCoord.y - pts[0].texCoord.y );
      edge2.set( pts[2].point.x - pts[0].point.x, pts[2].texCoord.x - pts[0].texCoord.x, pts[2].texCoord.y - pts[0].texCoord.y );

      mCross( edge1, edge2, &cp );
      if( fabs(cp.x) > 0.0001)
      {
         S.x = -cp.y / cp.x;
         T.x = -cp.z / cp.x;
      }

      edge1.set( pts[1].point.y - pts[0].point.y, pts[1].texCoord.x - pts[0].texCoord.x, pts[1].texCoord.y - pts[0].texCoord.y );
      edge2.set( pts[2].point.y - pts[0].point.y, pts[2].texCoord.x - pts[0].texCoord.x, pts[2].texCoord.y - pts[0].texCoord.y );

      mCross( edge1, edge2, &cp );
      if( fabs(cp.x) > 0.0001 )
      {
         S.y = -cp.y / cp.x;
         T.y = -cp.z / cp.x;
      }

      edge1.set( pts[1].point.z - pts[0].point.z, pts[1].texCoord.x - pts[0].texCoord.x, pts[1].texCoord.y - pts[0].texCoord.y );
      edge2.set( pts[2].point.z - pts[0].point.z, pts[2].texCoord.x - pts[0].texCoord.x, pts[2].texCoord.y - pts[0].texCoord.y );

      mCross( edge1, edge2, &cp );
      if( fabs(cp.x) > 0.0001 )
      {
         S.z = -cp.y / cp.x;
         T.z = -cp.z / cp.x;
      }

      S.normalize();
      T.normalize();
      mCross( S, T, &SxT );


      if( mDot( SxT, planeNorm ) < 0.0 )
      {
         SxT = -SxT;
      }

      surface.T = S;
      surface.B = T;
      surface.N = SxT;
      surface.normal = planeNorm;
   }
}

void Interior::fillVertexTBN(U32 surfaceIndex)
{
   Surface & surface = mSurfaces[surfaceIndex];
   
   for(U32 j = surface.windingStart; j < surface.windingStart + surface.windingCount; j++)
   {
      mPoints[mWindings[j]].T = surface.T;
      mPoints[mWindings[j]].B = surface.B;
      mPoints[mWindings[j]].N = surface.N;
      mPoints[mWindings[j]].normal = surface.normal;
   }
}

void Interior::initVertexBuffer()
{
   TexGenPlanes texGen;
   TexGenPlanes texGenLM;
   U32 arraySize = mWindings.size();
   FrameTemp<OutputPoint> data(arraySize);
   Con::printf("Allocating %i bytes of data for %i verts (goal: %i) in the interior VBO", arraySize * sizeof(OutputPoint), arraySize, mPoints.size());
   Con::printf("%i texgen eqs and %i lmtexgen eqs", mTexGenEQs.size(), mLMTexGenEQs.size());
   for(U32 i = 0; i < mSurfaces.size(); i++)
   {
      Surface & surface = mSurfaces[i];
      fillVertexTBN(i);
      texGen = mTexGenEQs[surface.texGenIndex];
      texGenLM = mLMTexGenEQs[i];
      for(U32 j = surface.windingStart; j < surface.windingStart + surface.windingCount; j++)
      {
         U32 x = mWindings[j];
         ItrPaddedPoint point = mPoints[x];
         
         data[j].texCoord.x = (texGen.planeX.x * point.point.x) + (texGen.planeX.y * point.point.y) + (texGen.planeX.z * point.point.z) + (texGen.planeX.d);
         data[j].texCoord.y = (texGen.planeY.x * point.point.x) + (texGen.planeY.y * point.point.y) + (texGen.planeY.z * point.point.z) + (texGen.planeY.d);
         
         data[j].lmCoord.x = (texGenLM.planeX.x * point.point.x) + (texGenLM.planeX.y * point.point.y) + (texGenLM.planeX.z * point.point.z) + (texGenLM.planeX.d);
         data[j].lmCoord.y = (texGenLM.planeY.x * point.point.x) + (texGenLM.planeY.y * point.point.y) + (texGenLM.planeY.z * point.point.z) + (texGenLM.planeY.d);
         
         data[j].point.x = point.point.x;
         data[j].point.y = point.point.y;
         data[j].point.z = point.point.z;
         
         data[j].T.x = point.T.x;
         data[j].T.y = point.T.y;
         data[j].T.z = point.T.z;
         
         data[j].B.x = point.B.x;
         data[j].B.y = point.B.y;
         data[j].B.z = point.B.z;
         
         data[j].N.x = point.N.x;
         data[j].N.y = point.N.y;
         data[j].N.z = point.N.z;
      }
   }
   if(!mVertexBuffer)
   {
	   mVertexBuffer = MK::getVBO->createVertexBuffer(arraySize * sizeof(OutputPoint), MKStatic, &data[0]);
      mIndexBuffer = MK::getVBO->createIndexBuffer(getMin(4096, mWindings.size()) * sizeof(U16), MKStream, NULL);
      return;
   }
   mVertexBuffer->uploadData(arraySize * sizeof(OutputPoint), 0, &data[0]);
}

mkInterior::mkInterior()
{
   mBlack = NULL;
   mWhite = NULL;
   mVertexBuffer = NULL;
   mFogCoordBuffer = NULL;
   mIsUsingShader = false;
}

mkInterior::~mkInterior()
{
   delete mBlack;
   delete mWhite;

   mMaterials.clear();
   mBlack = NULL;
   mWhite = NULL;
}

void mkInterior::init(MaterialList *materials, const char* path)
{
   glActiveTextureARB(GL_TEXTURE0_ARB);
   mBlack        = new TextureHandle("common/lighting/black", MeshTexture);
   mWhite        = new TextureHandle("common/lighting/whiteAlpha255", MeshTexture);
   
   AssertFatal(materials, "mkInterior::init needs an actual material list");
   
   RectI storedView;
   dglGetViewport(&storedView);
   
   // In addition to generating normal maps, we even generate the Toruqe script necessary
   // to use them.  Aren't we nice?
   #ifdef MK_AUTOGEN_MATERIALS
   FileStream pMapStream;
   char scriptPath[256];
   dSprintf(scriptPath, sizeof(scriptPath), "%s/autogenMatList.cs", path);
   if(!pMapStream.open(scriptPath, FileStream::WriteAppend))
   {
      Con::printf("Failed to open file '%s'.", scriptPath);
   }
   #endif

   mMaterials.setSize(materials->getMaterialCount());
   
   for(U32 i = 0; i < materials->getMaterialCount(); i++)
   {
      const char* name = materials->getMaterialName(i);
      char texPath[256];
      dSprintf(texPath, sizeof(texPath), "%s/%s",path, name);
      Material* mat = MaterialManager->findMaterialByMapToTexture(texPath);
      
      // You WILL have a normal map, even if we have to make it ourselves
      if(!mat)
      {
         #ifdef MK_AUTOGEN_MATERIALS
         generateMaterial(materials->getMaterial(i), name, path, pMapStream);
         mMaterials[i] = MaterialManager->findMaterialByMapToTexture(texPath);
         #else
         mMaterials[i] = NULL;
         #endif
         continue;
      }
      else
      {
         mMaterials[i] = mat;
      }
   }
   #ifdef MK_AUTOGEN_NMAP
   pMapStream.close();
   #endif
   dglSetViewport(storedView);
}

bool mkInterior::initShader()
{
   // Ya, shader + reflection = weird reflection
   if(MKGFX->isReflectPass()) {return false; }
   return true;
}

void mkInterior::bindMaterial(U32 surfaceIndex)
{
   if(mMaterials.size() < surfaceIndex)
      return;
   
   if(mMaterials[surfaceIndex])
   {
      mMaterials[surfaceIndex]->bind();
      ShaderManager->setParameter("EyePos", curCamPos.x, curCamPos.y, curCamPos.z);
      return;
   }
}

void mkInterior::prepRender(VertexBuffer* vbo, bool fogActive, Point2F *fogCoords, Vector<U32> &windings, Point3F camPos)
{   
	MK::getVBO->bindVBO(vbo);
   
   if(initShader())
   {
      // Bunches of stuff to do if we are using shaders
      // Look at all those GL calls!
      curCamPos = camPos;
      mIsUsingShader = true;
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(4, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(0));
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(16));
      
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(28));
      
      glClientActiveTextureARB(GL_TEXTURE2_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(40));
      
      glClientActiveTextureARB(GL_TEXTURE4_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(52));
      
      glActiveTextureARB(GL_TEXTURE3_ARB);
      glClientActiveTextureARB(GL_TEXTURE3_ARB);
      if(fogActive)
      {
         glBindTexture(GL_TEXTURE_2D, gClientSceneGraph->getFogTexture().getGLName());
         glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		 MK::getVBO->bindVBO(NULL);
         streamFogCoords(fogCoords, windings);
         glTexCoordPointer(2, GL_FLOAT, sizeof(Point2F), mFogCoordBuffer->getBufferOffset(0));
         MK::getVBO->bindVBO(vbo);
      }
      else
      {
         glBindTexture(GL_TEXTURE_2D, mBlack->getGLName());
      }
   }
   else
   {
      // Oh, no shader for us.  Sad.  Let's set up for standard fixed function rendering
      mIsUsingShader = false;
      // Lightmaps
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(16));
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(0));

      // Base textures
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), vbo->getBufferOffset(8));
      
      // *gasp* It's the TLK!
      // What, you aren't using DRL either?  Why DID you download this?!
      LightManager::sgSetupExposureRendering();
   }
   
   glActiveTextureARB(GL_TEXTURE0_ARB);
}

void mkInterior::streamFogCoords(Point2F *fogCoords, Vector<U32> &windings)
{
   // Say hello to waaaay too much CPU time.
   // We should really generate fog coordinates in the vertex shader
   // and screw textured fog.
   if(!mFogCoordBuffer)
   {
      mFogCoordBuffer = MK::getVBO->createVertexBuffer(windings.size() * sizeof(Point2F), MKStream, NULL);
   }
   Point2F *f = (Point2F *)mFogCoordBuffer->map();
   if(f == NULL)
   {
     Con::errorf("Out of memory");
     mFogCoordBuffer->unmap();
     return;
   }
   U32 size = windings.size();
   for(U32 i = 0; i < size; i++)
   {
      U32 x = windings[i];
      f[i].x = fogCoords[x].x;
      f[i].y = fogCoords[x].y;
   }
   mFogCoordBuffer->unmap();
}

bool mkInterior::endRender()
{
   // Nothing to do if we aren't using shaderized rendering
   // 'cause we still have to fog... owie.
   if(!mIsUsingShader)
      return false;
   
   ShaderManager->unbindShader();
   glClientActiveTextureARB(GL_TEXTURE2_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glClientActiveTextureARB(GL_TEXTURE3_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glClientActiveTextureARB(GL_TEXTURE4_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      
   glClientActiveTextureARB(GL_TEXTURE5_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      
   glClientActiveTextureARB(GL_TEXTURE6_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      
   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glDisableClientState(GL_VERTEX_ARRAY);
   return true;
}


void mkInterior::generateMaterial(TextureHandle& handle, const char* name, const char* path, FileStream &pMapStream)
{
   #ifdef MK_AUTOGEN_NORMALMAP
   U32 texWidth = handle.getWidth();
   U32 texHeight = handle.getHeight();
   // Ya, no nonexistant textures.
   if(texWidth == 0 || texHeight == 0)
      return;
      
   Con::printf("Generating material for texture %s, width %i, height %i", name, texWidth, texHeight);
   RenderTextureFormat testFormat(RGBA8, Depth0, None, Stencil0, 0, true, false);
   RenderTextureFormat bestFit;
   
   RenderTextureManager->getClosestMatch(testFormat, bestFit);
   
   RenderTexture *heightmap = RenderTextureManager->createRenderTexture(texWidth, texHeight, bestFit);
   RenderTexture *nmap = RenderTextureManager->createRenderTexture(texWidth, texHeight, bestFit);
   Material* heightmapCreator = MaterialManager->findMaterialByName("heightmapCreator");
   Material *nmapCreator = MaterialManager->findMaterialByName("nmapCreator");
   
   // It'd be bad if we didn't have our materials
   if(!heightmapCreator || !nmapCreator || !heightmapCreator->getActiveShader() || !nmapCreator->getActiveShader())
      return;
   
   // Generate the heightmap (blurred greyscale)
   RenderTextureManager->renderToTexture(heightmap);
   heightmapCreator->bind();
   ShaderManager->setParameter("TextureSize", F32(texWidth), F32(texHeight));
   
   glBindTexture(GL_TEXTURE_2D, handle.getGLName());
   glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      glVertex2f(-1, -1);
      
      glTexCoord2f(0, 1);
      glVertex2f(-1, 1);
      
      glTexCoord2f(1, 1);
      glVertex2f(1, 1);
      
      glTexCoord2f(1, 0);
      glVertex2f(1, -1);
   glEnd();
   
   // Generate the normal map from the blurred heightmap
   // Makes parallax mapping (and normal mapping) look
   // so much nicer.
   RenderTextureManager->renderToTexture(nmap);
   heightmap->bindToUse(0);
   nmapCreator->bind();
   ShaderManager->setParameter("TextureSize", F32(texWidth), F32(texHeight));
   
   glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      glVertex2f(-1, -1);
      
      glTexCoord2f(0, 1);
      glVertex2f(-1, 1);
      
      glTexCoord2f(1, 1);
      glVertex2f(1, 1);
      
      glTexCoord2f(1, 0);
      glVertex2f(1, -1);
   glEnd();
   
   RenderTextureManager->renderToTexture(NULL);
   
   // I forgot this at first.  Bad things happened.  DON'T REMOVE IT!!!
   ShaderManager->unbindShader();
   
   FileStream fStream;
   char texturePath[256];
   // Download our fine work for saving.
   GBitmap *nmapTexture = nmap->download();
   dSprintf(texturePath, sizeof(texturePath), "%s/%s_height_auto.png",path, name);
   
   // Life sucks if we can't save all of our work
   if(!fStream.open(texturePath, FileStream::Write))
   {
      Con::printf("Failed to open file '%s'.", texturePath);
      return;
   }
   // Wrtie a PNG
   nmapTexture->writePNG(fStream);
   fStream.close();
   delete nmapTexture;
      
   // Bah, we don't need anything anymore
   RenderTextureManager->deleteRenderTexture(nmap);
   RenderTextureManager->deleteRenderTexture(heightmap);
   
   // Save the script command to load this next time.
   char nMapPath[256];
   char texPath[256];
   dSprintf(nMapPath, sizeof(nMapPath), "%s/%s_height_auto",path, name);
   dSprintf(texPath, sizeof(texPath), "%s/%s",path, name);
   char scriptCommand[2048];
   dSprintf(scriptCommand, sizeof(scriptCommand), "new Material()\n{\n   mapTo = \"%s\";\n   Shader20 = InteriorParallax20;\n   textures[0] = \"Lightmap\";\n   textures[1] = \"%s\";\n   textures[2] = \"%s\";\n   textures[3] = \"Fog\";\n\n   samplers[0] = \"LightMap\";\n   samplers[1] = \"BaseTexture\";\n   samplers[2] = \"NormalMap\";\n   samplers[3] = \"FogTexture\";\n};\n\n", texPath, texPath, nMapPath);
   pMapStream.write(dStrlen(scriptCommand), scriptCommand);
   Con::evaluate(scriptCommand);
   
   #elif defined(MK_AUTOGEN_MATERIALS)
   char texPath[256];
   dSprintf(texPath, sizeof(texPath), "%s/%s",path, name);
   char scriptCommand[2048];
   dSprintf(scriptCommand, sizeof(scriptCommand), "new Material()\n{\n   mapTo = \"%s\";\n   Shader20 = InteriorFF20;\n   textures[0] = \"Lightmap\";\n   textures[1] = \"%s\";\n   textures[3] = \"Fog\";\n\n   samplers[0] = \"LightMap\";\n   samplers[1] = \"BaseTexture\";\n   samplers[3] = \"FogTexture\";\n};\n\n", texPath, texPath);
   pMapStream.write(dStrlen(scriptCommand), scriptCommand);
   Con::evaluate(scriptCommand);
   
   #endif
}
