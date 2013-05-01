//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "constructor/constructorSimpleMesh.h"

#ifdef IS_CONSTRUCTOR
#include "constructor/lightingSystem/lightingSystem.h"
#else
#include "interior/interiorLMManager.h"
#endif

#include "console/console.h"
#include "sim/sceneObject.h"
#include "util/triRayCheck.h"
#include "math/mathIO.h"

#ifdef IS_CONSTRUCTOR
ConstructorSimpleMesh *ConstructorSimpleMeshLoader::currentMesh = NULL;
#endif

// *** DAW: Checks for polygon level collision with given planes
U32 _whichSide(PlaneF pln, Point3F* verts)
{
   Point3F currv, nextv;
   S32 csd, nsd;

   // Find out which side the first vert is on
   U32 side = PlaneF::On;
   currv = verts[0];
   csd = pln.whichSide(currv);
   if(csd != PlaneF::On)
      side = csd;

   for(U32 k = 1; k < 3; k++)
   {
      nextv = verts[k];
      nsd = pln.whichSide(nextv);
      if((csd == PlaneF::Back && nsd == PlaneF::Front) ||
         (csd == PlaneF::Front && nsd == PlaneF::Back))
         return 2;
      else if (nsd != PlaneF::On)
         side = nsd;
      currv = nextv;
      csd = nsd;
   }

   // Loop back to the first vert
   nextv = verts[0];
   nsd = pln.whichSide(nextv);
   if((csd == PlaneF::Back && nsd == PlaneF::Front) ||
      (csd == PlaneF::Front && nsd == PlaneF::Back))
      return 2;
   else if(nsd != PlaneF::On)
      side = nsd;
   return side;

}


bool ConstructorSimpleMesh::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   bool found = false;
   F32 best_t = F32_MAX;
   Point3F best_normal = Point3F(0, 0, 1);
   Point3F dir = end - start;

   for(U32 p=0; p<primitives.size(); p++)
   {
      primitive &prim = primitives[p];
      for(U32 t=2; t<prim.count; t++)
      {
         Point3F &v1 = verts[prim.start+t-2];
         Point3F &v2 = verts[prim.start+t-1];
         Point3F &v3 = verts[prim.start+t];

         F32 cur_t = 0;
         Point2F b;

         if(castRayTriangle(start, dir, v1, v2, v3, cur_t, b))
         {
            if(cur_t < best_t)
            {
               best_t = cur_t;
               best_normal = norms[prim.start+t];
               found = true;
            }
         }
      }
   }

   if(found && info)
   {
      info->t = best_t;
      info->normal = best_normal;
      info->material = 0;
   }

   return found;
}

bool ConstructorSimpleMesh::castPlanes(PlaneF left, PlaneF right, PlaneF top, PlaneF bottom)
{
   for(U32 p=0; p<primitives.size(); p++)
   {
      primitive &prim = primitives[p];
      for(U32 t=2; t<prim.count; t++)
      {
         Point3F v[3];
         v[0] = verts[prim.start+t-2];
         v[1] = verts[prim.start+t-1];
         v[2] = verts[prim.start+t];

         if(_whichSide(left, v) == PlaneF::Front)
            continue;
         if(_whichSide(right, v) == PlaneF::Front)
            continue;
         if(_whichSide(top, v) == PlaneF::Front)
            continue;
         if(_whichSide(bottom, v) == PlaneF::Front)
            continue;

         return true;
      }
   }

   return false;
}

#ifdef IS_CONSTRUCTOR
void ConstructorSimpleMesh::render(bool transparent, bool texture, bool lightmap)
{
#else
void ConstructorSimpleMesh::render(bool transparent, bool texture, bool lightmap, bool lightmapinunit0,
                                   U32 interiorlmhandle, U32 instancelmhandle)
{
   if(!materialList)
      return;
#endif
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, verts.address());
   glEnableClientState(GL_NORMAL_ARRAY);
   glNormalPointer(GL_FLOAT, 0, norms.address());

   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   if(lightmapinunit0)
      glTexCoordPointer(2, GL_FLOAT, 0, lightmapUVs.address());
   else
      glTexCoordPointer(2, GL_FLOAT, 0, diffuseUVs.address());

   //if (lightmap)
   //{
   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   if(lightmapinunit0)
      glTexCoordPointer(2, GL_FLOAT, 0, diffuseUVs.address());
   else
      glTexCoordPointer(2, GL_FLOAT, 0, lightmapUVs.address());
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   //}

   U32 diffuseid = -1;
   U32 lightmapid = -1;

   for(S32 i=0; i<primitives.size(); i++)
   {
      primitive &draw = primitives[i];

      if(draw.alpha != transparent)
         continue;

      if(texture)
      {
         U32 id = materialList->getMaterial(draw.diffuseIndex).getGLName();
         if(diffuseid != id)
         {
            diffuseid = id;

            if(lightmapinunit0)
               glActiveTextureARB(GL_TEXTURE1_ARB);

            glBindTexture(GL_TEXTURE_2D, diffuseid);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, draw.texS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, draw.texT);

            if(lightmapinunit0)
               glActiveTextureARB(GL_TEXTURE0_ARB);
         }
      }

      if(lightmap)
      {
         U32 id = gInteriorLMManager.getHandle(interiorlmhandle, instancelmhandle, draw.lightMapIndex)->getGLName();
         if(lightmapid != id)
         {
            lightmapid = id;
            if(!lightmapinunit0)
               glActiveTextureARB(GL_TEXTURE1_ARB);

            glBindTexture(GL_TEXTURE_2D, lightmapid);

            if(!lightmapinunit0)
               glActiveTextureARB(GL_TEXTURE0_ARB);
         }
      }

      glDrawElements(GL_TRIANGLE_STRIP, draw.count, GL_UNSIGNED_SHORT,
         &indices[draw.start]);
   }

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
}

bool ConstructorSimpleMesh::read(Stream& stream)
{
   // Simple serialization
   S32 vectorSize = 0;

   // Primitives
   stream.read(&vectorSize);
   primitives.setSize(vectorSize);
   for (U32 i = 0; i < primitives.size(); i++)
   {
      stream.read(&primitives[i].alpha);
      stream.read(&primitives[i].texS);
      stream.read(&primitives[i].texT);
      stream.read(&primitives[i].diffuseIndex);
      stream.read(&primitives[i].lightMapIndex);
      stream.read(&primitives[i].start);
      stream.read(&primitives[i].count);

      mathRead(stream, &primitives[i].lightMapEquationX);
      mathRead(stream, &primitives[i].lightMapEquationY);
      mathRead(stream, &primitives[i].lightMapOffset);
      mathRead(stream, &primitives[i].lightMapSize);
   }

   // Indices
   stream.read(&vectorSize);
   indices.setSize(vectorSize);
   for (U32 i = 0; i < indices.size(); i++)
      stream.read(&indices[i]);

   // Vertices
   stream.read(&vectorSize);
   verts.setSize(vectorSize);
   for (U32 i = 0; i < verts.size(); i++)
      mathRead(stream, &verts[i]);

   // Normals
   stream.read(&vectorSize);
   norms.setSize(vectorSize);
   for (U32 i = 0; i < norms.size(); i++)
      mathRead(stream, &norms[i]);

   // Diffuse UVs
   stream.read(&vectorSize);
   diffuseUVs.setSize(vectorSize);
   for (U32 i = 0; i < diffuseUVs.size(); i++)
      mathRead(stream, &diffuseUVs[i]);

   // Lightmap UVs
   stream.read(&vectorSize);
   lightmapUVs.setSize(vectorSize);
   for (U32 i = 0; i < lightmapUVs.size(); i++)
      mathRead(stream, &lightmapUVs[i]);

   // Material list
   bool hasMaterialList = false;
   stream.read(&hasMaterialList);
   if (hasMaterialList)
   {
      // Since we are doing this externally to a TSShape read we need to
      // make sure that our read version is the same as our write version.
      // It is possible that it was changed along the way by a loaded TSShape.
      TSShape::smReadVersion = TSShape::smVersion;

      if (materialList)
         delete materialList;

      materialList = new TSMaterialList;
      materialList->read(stream);
   }
   else
      materialList = NULL;

   // Diffuse bitmaps
   stream.read(&vectorSize);
   for (U32 i = 0; i < vectorSize; i++)
   {
      bool hasBitmap = false;

      stream.read(&hasBitmap);

      if (hasBitmap)
      {
         GBitmap* bitMap = new GBitmap;
         bitMap->readPNG(stream);

         TextureHandle& handle = materialList->getMaterial(i);
         handle.set(materialList->getMaterialName(i), bitMap);
      }
   }

   // Misc data
   stream.read(&hasSolid);
   stream.read(&hasTranslucency);
   mathRead(stream, &bounds);
   mathRead(stream, &transform);
   mathRead(stream, &scale);

   calculateBounds();

   return true;
}

bool ConstructorSimpleMesh::write(Stream& stream) const
{
   // Simple serialization
   // Primitives
   stream.write(primitives.size());
   for (U32 i = 0; i < primitives.size(); i++)
   {
      stream.write(primitives[i].alpha);
      stream.write(primitives[i].texS);
      stream.write(primitives[i].texT);
      stream.write(primitives[i].diffuseIndex);
      stream.write(primitives[i].lightMapIndex);
      stream.write(primitives[i].start);
      stream.write(primitives[i].count);

      mathWrite(stream, primitives[i].lightMapEquationX);
      mathWrite(stream, primitives[i].lightMapEquationY);
      mathWrite(stream, primitives[i].lightMapOffset);
      mathWrite(stream, primitives[i].lightMapSize);
   }

   // Indices
   stream.write(indices.size());
   for (U32 i = 0; i < indices.size(); i++)
      stream.write(indices[i]);

   // Vertices
   stream.write(verts.size());
   for (U32 i = 0; i < verts.size(); i++)
      mathWrite(stream, verts[i]);

   // Normals
   stream.write(norms.size());
   for (U32 i = 0; i < norms.size(); i++)
      mathWrite(stream, norms[i]);

   // Diffuse UVs
   stream.write(diffuseUVs.size());
   for (U32 i = 0; i < diffuseUVs.size(); i++)
      mathWrite(stream, diffuseUVs[i]);

   // Lightmap UVs
   stream.write(lightmapUVs.size());
   for (U32 i = 0; i < lightmapUVs.size(); i++)
      mathWrite(stream, lightmapUVs[i]);

   // Material list
   if (materialList)
   {
      stream.write(true);
      materialList->write(stream);
   }
   else
      stream.write(false);

   // Diffuse bitmaps
   if (!materialList)
      stream.write(0);
   else
   {
      stream.write(materialList->getMaterialCount());

      for (U32 i = 0; i < materialList->getMaterialCount(); i++)
      {
         TextureHandle& handle = materialList->getMaterial(i);

         if (handle.isValid())
         {
            GBitmap* bitMap = handle.getBitmap();

            if (bitMap)
            {
               stream.write(true);
               bitMap->writePNG(stream);
            }
            else
               stream.write(false);
         }
         else
            stream.write(false);
      }
   }

   // Misc data
   stream.write(hasSolid);
   stream.write(hasTranslucency);
   mathWrite(stream, bounds);
   mathWrite(stream, transform);
   mathWrite(stream, scale);

   return true;
}

//-----------------------------------------------------------------------------
#ifdef IS_CONSTRUCTOR
bool ConstructorSimpleMeshLoader::loadSimpleMesh(const char *filename, ConstructorSimpleMesh &mesh)
{
   if(!filename)
      return false;

   const char *ext = dStrrchr(filename, '.');
   if(!ext || (ext[0] == 0))
      return false;

   FileStream file;
   if(!file.open(filename, FileStream::Read))
   {
      Con::errorf("Unable to open file '%s'.", filename);
      return false;
   }

   currentMesh = &mesh;
   currentMesh->clear();

   bool res = false;
   if(dStricmp(ext, ".dts") == 0)
      res = loadDTSFile(filename);
   //else if(dStricmp(ext, ".x") == 0)
   //	res = loadDirectXFile(&file);
   else
   {
      ext = &ext[1];
      if(ext[0] != 0)
      {
         char func[256];
         func[0] = 0;
         dStrcat(func, "SimpleMeshLoader_load");
         dStrcat(func, ext);
         dStrcat(func, "File");
         const char *ret = Con::executef(1, func);
         if(!ret || (dStricmp(ret, "1") != 0))
            res = false;
         else
            res = true;
      }
   }

   currentMesh->calculateBounds();
   currentMesh = NULL;

   return res;
}

bool ConstructorSimpleMeshLoader::loadDTSFile(const char *filename)
{
   Resource<TSShape> shape;
   shape = ResourceManager->load(filename);
   if(shape.isNull())
      return false;

   TSShapeInstance *shapeinst = new TSShapeInstance(shape, true);
   if(!shapeinst)
      return false;

   // load up...
   currentMesh->materialList = new TSMaterialList(shapeinst->getMaterialList());

   // only interested in the top detail...
   const TSDetail *detail = &shapeinst->getShape()->details[0];
   S32 ss = detail->subShapeNum;
   S32 od = detail->objectDetailNum;
   S32 start = shapeinst->getShape()->subShapeFirstObject[ss];
   S32 end = start + shapeinst->getShape()->subShapeNumObjects[ss];

   shapeinst->animate(0);
   shapeinst->setCurrentDetail(0);
   shapeinst->setStatics(0);

   currentMesh->hasSolid = true;

   for(U32 i=start; i<end; i++)
   {
      TSMesh *mesh = shapeinst->mMeshObjects[i].getMesh(od);
      MatrixF *mat = shapeinst->mMeshObjects[i].getTransform();

      if(!mesh)
         continue;

      MatrixF m;
      if(mat)
         m = *mat;
      else
         m.identity();

      ToolVector<Point3F> *vertsp;
      ToolVector<Point3F> *normsp;
      TSSkinMesh *smesh = dynamic_cast<TSSkinMesh *>(mesh);
      if(smesh)
      {
         vertsp = &smesh->initialVerts;
         normsp = &smesh->initialNorms;
      }
      else
      {
         vertsp = &mesh->verts;
         normsp = &mesh->norms;
      }
      ToolVector<Point3F> &verts = *vertsp;
      ToolVector<Point3F> &norms = *normsp;

      for(U32 p=0; p<mesh->primitives.size(); p++)
      {
         TSDrawPrimitive &oldprim = mesh->primitives[p];

         currentMesh->primitives.increment();
         ConstructorSimpleMesh::primitive &newprim = currentMesh->primitives.last();

         U32 flags = currentMesh->materialList->getFlags(oldprim.matIndex & TSDrawPrimitive::MaterialMask);
         newprim.alpha = (flags & TSMaterialList::Translucent);
         newprim.texS = (flags & TSMaterialList::S_Wrap) ? GL_REPEAT : GL_CLAMP;
         newprim.texT = (flags & TSMaterialList::T_Wrap) ? GL_REPEAT : GL_CLAMP;
         newprim.start = currentMesh->indices.size();
         newprim.count = 0;
         newprim.lightMapId = 0;
         newprim.lightMapIndex = 0;

         if(newprim.alpha)
            currentMesh->hasTranslucency = true;

         newprim.diffuseIndex = oldprim.matIndex & TSDrawPrimitive::MaterialMask;
         TextureHandle &tex = currentMesh->materialList->getMaterial(newprim.diffuseIndex);
         newprim.diffuseId = tex.getGLName();

         for(U32 v=0; v<oldprim.numElements; v++)
         {
            AssertFatal(((S32(oldprim.start + v) > -1) &&
               (S32(oldprim.start + v) < mesh->indices.size())), "!");
            S32 srcindex = mesh->indices[oldprim.start + v];
            S32 dstindex = currentMesh->verts.size();

            currentMesh->indices.increment();
            currentMesh->norms.increment();
            currentMesh->verts.increment();
            currentMesh->diffuseUVs.increment();
            currentMesh->lightmapUVs.increment();

            currentMesh->indices.last() = dstindex;
            currentMesh->verts[dstindex] = verts[srcindex];
            currentMesh->norms[dstindex] = norms[srcindex];

            m.mulP(currentMesh->verts[dstindex]);
            m.mulV(currentMesh->norms[dstindex]);

            ToolVector<Point2F> uvs;
            mesh->getUVs(TSMesh::tDiffuse, uvs);

            if(uvs.size() > 0)
               currentMesh->diffuseUVs[dstindex] = uvs[srcindex];

            newprim.count++;
         }
      }
   }

   // Now grab the collision meshes and shove them into ConvexBrushes
   for (U32 i = 0; i < shapeinst->getShape()->details.size(); i++)
   {
      const TSDetail * detail = &shapeinst->getShape()->details[i];

      char* name = (char*)shapeinst->getShape()->names[detail->nameIndex];

      if (dStrstr(dStrlwr(name), "collision-"))
      {
         shapeinst->animate(i);
         shapeinst->setCurrentDetail(i);
         shapeinst->setStatics(i);

         S32 ss = detail->subShapeNum;
         S32 od = detail->objectDetailNum;

         S32 start = shapeinst->getShape()->subShapeFirstObject[ss];
         S32 end   = shapeinst->getShape()->subShapeNumObjects[ss] + start;
         if (start < end)
         {
            // Run through objects and collide
            for (S32 j = start; j < end; j++)
            {
               TSShapeInstance::MeshObjectInstance * mesh = &shapeinst->mMeshObjects[j];

               if (od >= mesh->object->numMeshes)
                  continue;

               ConcretePolyList polys;

               // Get a valid transform for the polylist
               MatrixF* mat = mesh->getTransform();

               MatrixF m;
               if(mat)
                  m = *mat;
               else
                  m.identity();

               polys.setTransform(&m, Point3F(1.0f, 1.0f, 1.0f));

               // collide...
               U32 surfaceKey = 0;
               mesh->buildPolyList(od, &polys, surfaceKey);

               if (polys.mPolyList.size() == 0)
                  continue;

               // Now build our actual ConvexBrush
               ConvexBrush* hull = new ConvexBrush;

               for (U32 k = 0; k < polys.mPolyList.size(); k++)
               {
                  ConcretePolyList::Poly& srcPoly = polys.mPolyList[k];
                  OptimizedPolyList::Poly dstPoly;

                  dstPoly.plane = hull->mFaces.addPlane(srcPoly.plane);
                  dstPoly.vertexStart = hull->mFaces.mIndexList.size();
                  dstPoly.vertexCount = srcPoly.vertexCount;

                  for (U32 m = 0; m < srcPoly.vertexCount; m++)
                  {
                     U32 srcIndex = polys.mIndexList[srcPoly.vertexStart + m];
                     Point3F& pt = polys.mVertexList[srcIndex];

                     U32 dstIndex = hull->mFaces.addPoint(pt);
                     hull->mFaces.mIndexList.push_back(dstIndex);
                  }

                  dstPoly.textureData.texture = StringTable->insert("null");
                  dstPoly.isNull = true;

                  hull->mFaces.setUpTextureMatrix(dstPoly);

                  hull->mFaces.mPolyList.push_back(dstPoly);
               }

               // We need to do a merging pass on the polylist since TSMesh's
               // buildPolyList returns a list of individual triangles
               // Setup the surface id's
               for (U32 k = 0; k < hull->mFaces.mPlaneList.size(); k++)
               {
                  for (U32 m = 0; m < hull->mFaces.size(); m++)
                  {
                     OptimizedPolyList::Poly& poly = hull->mFaces[m];

                     if (poly.plane == k)
                        poly.surfaceID = k;
                  }
               }

               // And do the actual merge
               hull->mFaces.mergeSurfaces();

               // Need to do a couple operations on our ConvexBrush
               hull->mFaces.generateEdgelist();
               hull->calcBounds();
               hull->calcCentroid();
               hull->mStatus = ConvexBrush::Good;
               hull->mType = InteriorMapResource::Detail;
               hull->mBrushScale = 32.0f;

               // And now push it back into our collisionHulls list
               currentMesh->collisionHulls.push_back(hull);
            }
         }
      }
   }

   shapeinst->clearStatics();

   delete shapeinst;

   return true;
}

/*bool ConstructorSimpleMeshLoader::loadDirectXFile(Stream *stream)
{
return false;
}*/


ConsoleFunction(SimpleMeshLoader_createTriStrip, bool, 3, 3, "bool SimpleMeshLoader_createTriStrip(textureFileName, transparent)")
{
   ConstructorSimpleMesh *mesh = ConstructorSimpleMeshLoader::currentMesh;
   if(!mesh)
      return false;

   if(!mesh->materialList)
      mesh->materialList = new TSMaterialList();
   mesh->materialList->push_back(argv[2], 0);

   mesh->primitives.increment();
   ConstructorSimpleMesh::primitive &prim = mesh->primitives.last();
   prim.start = mesh->indices.size();
   prim.count = 0;

   prim.diffuseIndex = mesh->materialList->size() - 1;
   TextureHandle &tex = mesh->materialList->getMaterial(prim.diffuseIndex);
   prim.diffuseId = tex.getGLName();

   prim.lightMapId = 0;
   prim.lightMapIndex = 0;

   prim.alpha = ((dAtoi(argv[2]) == 1) || (dStrcmp(argv[2], "true") == 0));
   prim.texS = GL_REPEAT;
   prim.texT = GL_REPEAT;

   return true;
}

ConsoleFunction(SimpleMeshLoader_createVert, bool, 4, 4, "bool SimpleMeshLoader_createVert(pos, norm, uv)")
{
   ConstructorSimpleMesh *mesh = ConstructorSimpleMeshLoader::currentMesh;
   if(!mesh || (mesh->primitives.size() < 1))
      return false;

   Point3F pos, norm;
   Point2F uv;

   dSscanf(argv[1], "%f %f %f", &pos.x, &pos.y, &pos.z);
   dSscanf(argv[2], "%f %f %f", &norm.x, &norm.y, &norm.z);
   dSscanf(argv[3], "%f %f", &uv.x, &uv.y);

   ConstructorSimpleMesh::primitive &prim = mesh->primitives.last();
   prim.count++;

   mesh->indices.increment();
   mesh->indices.last() = mesh->verts.size();

   mesh->verts.increment();
   mesh->verts.last() = pos;

   mesh->norms.increment();
   mesh->norms.last() = norm;

   mesh->diffuseUVs.increment();
   mesh->diffuseUVs.last() = uv;

   mesh->lightmapUVs.increment();

   return true;
}
#endif

