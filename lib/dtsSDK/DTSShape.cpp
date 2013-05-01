
#pragma warning ( disable: 4786 4018 )

#include "DTSShape.h"
#include "DTSBrushMesh.h"
#include "DTSOutputStream.h"
#include "DTSInputStream.h"
#include "DTSEndian.h"
#include "DTSTypes.h"

#include <map>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper macro -- assumes output stream "out"
#define writeEndian(a,sz) out.write((char*) &FIX_ENDIAN(a),sz)

namespace DTS
{

   // -----------------------------------------------------------------------
   //  Constructor (create an empty shape)
   // -----------------------------------------------------------------------

   Shape::Shape()
   {
   }

   Sequence::Sequence()
   {
      baseDecalState   = 0 ;
      baseObjectState  = 0 ;
      baseRotation     = 0 ;
      baseScale        = 0 ;
      baseTranslation  = 0 ;
      duration         = 1.0f ;
      firstGroundFrame = 0 ;
      flags            = 0 ;
      nameIndex        = 0 ;
      numGroundFrames  = 0 ;
      numKeyFrames     = 0 ;
      priority         = 1 ;
      toolBegin        = 0.0f ;
   }

   // -----------------------------------------------------------------------
   //  Loads a DTS file
   // -----------------------------------------------------------------------

   void Shape::read (std::istream &in) 
   {
      InputStream stream(in) ;
      int version = stream.version() ;

      if (version < 19) 
      {
#ifdef _WIN32
         MessageBox (NULL, "The DTS file is of a older, unsupported version", "Error", 
                     MB_ICONHAND | MB_OK) ;
#else
		 assert(0 && "The DTS file is of a older, unsupported version");
#endif

         return ;
      }

      // Header

      int numNodes ;
      int numObjects ;
      int numDecals ;
      int numSubshapes ;
      int numIFLmaterials ;
      int numNodeRotations ;
      int numNodeTranslations ;
      int numNodeScalesUniform ;
      int numNodeScalesAligned ;
      int numNodeScalesArbitrary ;
      int numGroundFrames ;
      int numObjectStates ;
      int numDecalStates ;
      int numTriggers ;
      int numDetailLevels ;
      int numMeshes ;
      int numSkins ;
      int numNames ;

      stream >> numNodes >> numObjects >> numDecals >> numSubshapes >> numIFLmaterials ;

      if (version < 22)
      {
         stream >> numNodeRotations ;
         numNodeRotations -= numNodes ;
         numNodeTranslations = numNodeRotations ;
         numNodeScalesUniform = 0 ;
         numNodeScalesAligned = 0 ;
         numNodeScalesArbitrary = 0 ;      
         numGroundFrames = 0 ;
      }
      else
      {
         stream >> numNodeRotations ;
         stream >> numNodeTranslations ;
         stream >> numNodeScalesUniform ;
         stream >> numNodeScalesAligned ;
         stream >> numNodeScalesArbitrary ;
         if (version > 23)
            stream >> numGroundFrames ;
      }

      stream >> numObjectStates ;
      stream >> numDecalStates ;
      stream >> numTriggers ;
      stream >> numDetailLevels ;
      stream >> numMeshes ;

      if (version < 23)
         stream >> numSkins ;
      else
         numSkins = 0 ;

      stream >> numNames ;

      int smallestSizeInt ;
      stream >> smallestSizeInt ;
      stream >> smallestDetailLevel ;
      smallestSize = (float)smallestSizeInt ;

      stream.readCheck(0) ;

      // Bounds
      
      stream >> radius ;
      stream >> tubeRadius ;
      stream >> center ;
      stream >> bounds ;

      stream.readCheck(1) ;

      // Nodes

      nodes.resize(numNodes) ;
      stream >> nodes ;
      stream.readCheck(2) ;

      // Objects 

      objects.resize(numObjects) ;
      stream >> objects ;
      stream.readCheck(3) ;

      // Decals

      decals.resize(numDecals) ;
      stream >> decals ;
      stream.readCheck(4) ;

      // IFL Materials

      IFLmaterials.resize(numIFLmaterials) ;
      stream >> IFLmaterials ;
      stream.readCheck(5) ;

      // Subshapes

      subshapes.resize(numSubshapes) ;
      std::vector<Subshape>::iterator shape ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream >> shape->firstNode ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream >> shape->firstObject ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream >> shape->firstDecal ;
      stream.readCheck(6) ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream >> shape->numNodes ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream >> shape->numObjects ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream >> shape->numDecals ;
      stream.readCheck(7) ;

      // MeshIndexList (obsolete data)

      if (version < 16)
      {
         int size, temp ;
         stream >> size ;
         while (size--) stream >> temp ;
      }

      // Default translations and rotations

      nodeDefRotations.resize(numNodes) ;
      nodeDefTranslations.resize(numNodes) ;

      for (int n = 0 ; n < nodes.size() ; n++)
      {
         stream >> nodeDefRotations[n] ;
         stream >> nodeDefTranslations[n] ;
      }

      // Animation translations and rotations

      nodeTranslations.resize(numNodeTranslations) ;
      nodeRotations.resize(numNodeRotations) ;
      stream >> nodeTranslations ;
      stream >> nodeRotations ;
      stream.readCheck(8) ;

      // Default scales

      nodeScalesUniform.resize (numNodeScalesUniform) ;
      nodeScalesAligned.resize (numNodeScalesAligned) ;
      nodeScalesArbitrary.resize (numNodeScalesArbitrary) ;

      if (version > 21)
      {
         stream >> numNodeScalesUniform ;
         stream >> numNodeScalesAligned ;
         stream >> numNodeScalesArbitrary ;
         stream.readCheck(9) ;
      }

      // Ground transformations

      groundTranslations.resize (numGroundFrames) ;
      groundRotations.resize (numGroundFrames) ;

      if (version > 23)
      {
         stream >> groundTranslations ;
         stream >> groundRotations ;
         stream.readCheck() ;
      }

      // Object states

      objectStates.resize (numObjectStates) ;
      stream >> objectStates ;
      stream.readCheck() ;

      // Decal states

      decalStates.resize (numDecalStates) ;
      stream >> decalStates ;
      stream.readCheck() ;

      // Triggers

      triggers.resize (numTriggers) ;
      stream >> triggers ;
      stream.readCheck() ;

      // Detail Levels

      detailLevels.resize (numDetailLevels) ;
      stream >> detailLevels ;
      stream.readCheck() ;

      // Meshes

      meshes.resize (numMeshes) ;
      stream >> meshes ;
      stream.readCheck() ;

   }

   // -----------------------------------------------------------------------
   //  Saves a DTS file
   // -----------------------------------------------------------------------

   void Shape::save (std::ostream & out) const
   {
      OutputStream stream (out, 24) ;

      // Header

      stream << (int) nodes.size() ;
      stream << (int) objects.size() ;
      stream << (int) decals.size() ;
      stream << (int) subshapes.size() ;
      stream << (int) IFLmaterials.size() ;
      stream << (int) nodeRotations.size() ;
      stream << (int) nodeTranslations.size() ;
      stream << (int) nodeScalesUniform.size() ;
      stream << (int) nodeScalesAligned.size() ;
      stream << (int) nodeScalesArbitrary.size() ;
      stream << (int) groundTranslations.size() ;
      stream << (int) objectStates.size() ;
      stream << (int) decalStates.size() ;
      stream << (int) triggers.size() ;
      stream << (int) detailLevels.size() ;
      stream << (int) meshes.size() ;
      stream << (int) names.size() ;
      stream << (int) smallestSize ;
      stream << smallestDetailLevel ;

      stream.storeCheck(0) ;

      // Bounds

      stream << radius ;
      stream << tubeRadius ;
      stream << center ;
      stream << bounds ;

      stream.storeCheck(1) ;

      // Nodes

      stream << nodes ;
      stream.storeCheck(2) ;
      
      // Objects
      
      stream << objects ;
      stream.storeCheck(3) ;
      
      // Decals
      
      stream << decals ;
      stream.storeCheck(4) ;

      // IFL Materials

      stream << IFLmaterials ;
      stream.storeCheck(5) ;

      // Subshapes
      
      std::vector<Subshape>::const_iterator shape ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream << shape->firstNode ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream << shape->firstObject ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream << shape->firstDecal ;
      stream.storeCheck(6) ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream << shape->numNodes ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream << shape->numObjects ;
      for (shape = subshapes.begin() ; shape != subshapes.end() ; shape++)
         stream << shape->numDecals ;
      stream.storeCheck(7) ;

      // Default translations and rotations

      assert (nodeDefRotations.size() == nodes.size()) ;
      assert (nodeDefTranslations.size() == nodes.size()) ;

      for (int n = 0 ; n < nodes.size() ; n++)
      {
         stream << nodeDefRotations[n] ;
         stream << nodeDefTranslations[n] ;
      }

      // Animation translations and rotations

      stream << nodeTranslations ;      // As current position, we store the same values
      stream << nodeRotations ;
      stream.storeCheck(8) ;

      // Default scales

      stream << nodeScalesUniform ;
      stream << nodeScalesAligned ;
      stream << nodeScalesArbitrary ;
      stream << nodeScaleRotsArbitrary ;
      stream.storeCheck(9) ;

      // Ground transformations

      stream << groundTranslations ;
      stream << groundRotations ;
      stream.storeCheck(10) ;

      // Object states

      stream << objectStates ;
      stream.storeCheck(11) ;
      
      // Decal states

      stream << decalStates ;
      stream.storeCheck(12) ;

      // Triggers

      stream << triggers ;
      stream.storeCheck(13) ;
      
      // Detail levels
      
      stream << detailLevels ;
      stream.storeCheck(14) ;
      
      // Meshes 

      stream << meshes ;
      stream.storeCheck() ;      // Many checks stored within the meshes
      
      // Names
      
      stream << names ;
      stream.storeCheck() ;

      // Finished with the 3-buffer stuff

      stream.flush() ;

      // Sequences

      int numSequences = sequences.size() ;
      out.write ((char *)&FIX_ENDIAN(numSequences), 4) ;
      
      for (int seq = 0 ; seq < sequences.size() ; seq++)
      {
         const Sequence &p = sequences[seq] ;
         
         out.write ((char *) &FIX_ENDIAN(p.nameIndex), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.flags), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.numKeyFrames), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.duration), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.priority), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.firstGroundFrame), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.numGroundFrames), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.baseRotation), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.baseTranslation), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.baseScale), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.baseObjectState), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.baseDecalState), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.firstTrigger), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.numTriggers), 4) ;
         out.write ((char *) &FIX_ENDIAN(p.toolBegin), 4) ;

         write(out,&p.matters.rotation);
         write(out,&p.matters.translation);
         write(out,&p.matters.scale);
         write(out,&p.matters.decal);
         write(out,&p.matters.ifl);
         write(out,&p.matters.vis);
         write(out,&p.matters.frame);
         write(out,&p.matters.matframe);
      }

      // Materials

      char materialListVersion = 1 ;
      out.write (&materialListVersion, 1) ;
      int count = materials.size() ;
      out.write ((char *) &FIX_ENDIAN(count), 4) ;

      std::vector<Material>::const_iterator mat ;
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
      {
         char length = mat->name.size() ;
         out.write (&length, 1) ;
         std::string::const_iterator pos = mat->name.begin() ;
         while (pos != mat->name.end())
            out.write (&*(pos++), 1) ;
      }
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
         out.write ((char *) &FIX_ENDIAN(mat->flags), 4) ;
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
         out.write ((char *) &FIX_ENDIAN(mat->reflectance), 4) ;
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
         out.write ((char *) &FIX_ENDIAN(mat->bump), 4) ;
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
         out.write ((char *) &FIX_ENDIAN(mat->detail), 4) ;
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
         out.write ((char *) &FIX_ENDIAN(mat->detailScale), 4) ;
      for (mat = materials.begin() ; mat != materials.end() ; mat++)
         out.write ((char *) &FIX_ENDIAN(mat->reflection), 4) ;

      // Done!
   }

   void Shape::saveSequences(std::ostream & out) const 
   {
      int i;

      // write version info
      writeEndian((int)24,4);

      // write node names
      // -- this is how we will map imported sequence nodes to shape nodes
      writeEndian(nodes.size(),4);
      for (i=0; i<nodes.size(); i++)
      {
         const char * name = names[nodes[i].name].c_str();
         writeEndian(strlen(name),4);
         out.write(name,strlen(name));
      }

      // legacy write -- write zero objects, don't pretend to support object export anymore
      writeEndian((int)0,4);

      // on import, we will need to adjust keyframe data based on number of
      // nodes/objects in this shape...number of nodes can be inferred from
      // above, but number of objects cannot be.  Write that quantity here:
      writeEndian(objects.size(),4);
      
      writeEndian(nodeRotations.size(),4);
      for (i=0; i<nodeRotations.size(); i++)
      {
         writeEndian((short)(nodeRotations[i].x() * 32767.0f),2);
         writeEndian((short)(nodeRotations[i].y() * 32767.0f),2);
         writeEndian((short)(nodeRotations[i].z() * 32767.0f),2);
         writeEndian((short)(nodeRotations[i].w() * 32767.0f),2);
      }

      writeEndian(nodeTranslations.size(),4);
      for (i=0; i<nodeTranslations.size(); i++)
      {
         writeEndian(nodeTranslations[i].x(),4);
         writeEndian(nodeTranslations[i].y(),4);
         writeEndian(nodeTranslations[i].z(),4);
      }

      writeEndian(nodeScalesUniform.size(),4);
      for (i=0; i<nodeScalesUniform.size(); i++)
      {
         writeEndian(nodeScalesUniform[i],4);
      }

      writeEndian(nodeScalesAligned.size(),4);
      for (i=0; i<nodeScalesAligned.size(); i++)
      {
         writeEndian(nodeScalesAligned[i].x(),4);
         writeEndian(nodeScalesAligned[i].y(),4);
         writeEndian(nodeScalesAligned[i].z(),4);
      }

      writeEndian(nodeScaleRotsArbitrary.size(),4);
      for (i=0; i<nodeScaleRotsArbitrary.size(); i++)
      {
         writeEndian((short)(nodeScaleRotsArbitrary[i].x() * 32767.0f),2);
         writeEndian((short)(nodeScaleRotsArbitrary[i].y() * 32767.0f),2);
         writeEndian((short)(nodeScaleRotsArbitrary[i].z() * 32767.0f),2);
         writeEndian((short)(nodeScaleRotsArbitrary[i].w() * 32767.0f),2);
      }
      // same number as above...
      for (i=0; i<nodeScalesArbitrary.size(); i++)
      {
         writeEndian(nodeScalesArbitrary[i].x(),4);
         writeEndian(nodeScalesArbitrary[i].y(),4);
         writeEndian(nodeScalesArbitrary[i].z(),4);
      }

      writeEndian(groundTranslations.size(),4);
      for (i=0; i<groundTranslations.size(); i++)
      {
         writeEndian(groundTranslations[i].x(),4);
         writeEndian(groundTranslations[i].y(),4);
         writeEndian(groundTranslations[i].z(),4);
      }
      // same number as above
      for (i=0; i<groundRotations.size(); i++)
      {
         writeEndian((short)(groundRotations[i].x() * 32767.0f),2);
         writeEndian((short)(groundRotations[i].y() * 32767.0f),2);
         writeEndian((short)(groundRotations[i].z() * 32767.0f),2);
         writeEndian((short)(groundRotations[i].w() * 32767.0f),2);
      }

      // legacy write -- write zero objects, don't pretend to support object export anymore
      writeEndian((int)0,4);

      // Sequences

      int numSequences = sequences.size() ;
      writeEndian(numSequences,4);
      
      for (int seq = 0 ; seq < sequences.size() ; seq++)
      {
         const Sequence &p = sequences[seq] ;

         // write sequence name
         const char * seqname = names[p.nameIndex].c_str();
         writeEndian(strlen(seqname),4);
         out.write((char*)seqname,strlen(seqname));

         // skip name index...
         writeEndian(p.flags,4);
         writeEndian(p.numKeyFrames,4);
         writeEndian(p.duration,4);
         writeEndian(p.priority,4);
         writeEndian(p.firstGroundFrame,4);
         writeEndian(p.numGroundFrames,4);
         writeEndian(p.baseRotation,4);
         writeEndian(p.baseTranslation,4);
         writeEndian(p.baseScale,4);
         writeEndian(p.baseObjectState,4);
         writeEndian(p.baseDecalState,4);
         writeEndian(p.firstTrigger,4);
         writeEndian(p.numTriggers,4);
         writeEndian(p.toolBegin,4);

         write(out,&p.matters.rotation);
         write(out,&p.matters.translation);
         write(out,&p.matters.scale);
         write(out,&p.matters.decal);
         write(out,&p.matters.ifl);
         write(out,&p.matters.vis);
         write(out,&p.matters.frame);
         write(out,&p.matters.matframe);
      }

      writeEndian(triggers.size(),4);
      for (i=0; i<triggers.size(); i++)
      {
         writeEndian(triggers[i].state,4);
         writeEndian(triggers[i].pos,4);
      }
   }

   void Shape::write (std::ostream & out,const std::vector<bool> * ptr) const
   {
      // Save out the bool array as an array of bits, in 32bit chunks.
      int words[32], use = ptr->size() / 32 ;
      if (use * 32 < ptr->size()) use++ ;

      memset(words, 0, sizeof(words));
      for (int i = 0 ; i < ptr->size() ; i++)
          if ((*ptr)[i])
              words[i >> 5] |= 1 << (i & 31);
      for (int j = 0; j < sizeof(words)/sizeof(int); j++)
         words[j] = FIX_ENDIAN(words[j]);


      out.write ((char *)&FIX_ENDIAN(use), 4) ;
      out.write ((char *)&FIX_ENDIAN(use), 4) ;
      if (use > 0)
         out.write ((char *)&words, 4*use) ;
   }

   // -----------------------------------------------------------------------
   //  Utility methods
   // -----------------------------------------------------------------------

   int Shape::addName (std::string s)
   {
      // Don't store duplicated names

      for (int i = 0 ; i < names.size() ; i++)
      {
#if _WIN32
         if (!_stricmp(names[i].data(),s.data()))
#else
         if (!strcasecmp(names[i].data(),s.data()))
#endif
            return i ;
      }
      names.push_back(s) ;
      return names.size() - 1 ;
   }

   void Shape::calculateBounds()
   {
      if (!objects.size()) return ;

      bounds.max = Point(-10E30F, -10E30F, -10E30F) ;
      bounds.min = Point( 10E30F,  10E30F,  10E30F) ;

      // Iterate through the objects instead of the meshes
      // so we can easily get the default transforms.
      for (int i = 0 ; i < objects.size() ; i++)
      {
         const Object &obj = objects[i];

         Point trans;
         Quaternion rot;
         getNodeWorldPosRot(obj.node,trans,rot);

         for (int j = 0 ; j < obj.numMeshes ; j++)
         {
            Box bounds2 = meshes[obj.firstMesh + j].getBounds(trans,rot) ;
            bounds.min.x(min(bounds.min.x(), bounds2.min.x())) ;
            bounds.min.y(min(bounds.min.y(), bounds2.min.y())) ;
            bounds.min.z(min(bounds.min.z(), bounds2.min.z())) ;
            bounds.max.x(max(bounds.max.x(), bounds2.max.x())) ;
            bounds.max.y(max(bounds.max.y(), bounds2.max.y())) ;
            bounds.max.z(max(bounds.max.z(), bounds2.max.z())) ;
         }
      }
   }
   
   void Shape::calculateCenter()
   {
      center = bounds.min.midpoint(bounds.max) ;
   }

   void Shape::calculateTubeRadius()
   {
      float maxRadius = 0.0f ;

      for (int i = 0 ; i < objects.size() ; i++)
      {
         const Object &obj = objects[i];

         Point trans;
         Quaternion rot;
         getNodeWorldPosRot(obj.node,trans,rot);

         for (int j = 0 ; j < obj.numMeshes ; j++)
         {
            const Mesh &m = meshes[obj.firstMesh + j];
            float meshRadius = m.getTubeRadiusFrom(trans,rot,center) ;
            if (meshRadius > maxRadius)
               maxRadius = meshRadius ;
         }
      }
      tubeRadius = maxRadius ;
   }

   void Shape::calculateRadius()
   {
      float maxRadius = 0.0f ;

      for (int i = 0 ; i < objects.size() ; i++)
      {
         const Object &obj = objects[i];

         Point trans;
         Quaternion rot;
         getNodeWorldPosRot(obj.node,trans,rot);

         for (int j = 0 ; j < obj.numMeshes ; j++)
         {
            const Mesh &m = meshes[obj.firstMesh + j];
            float meshRadius = m.getRadiusFrom(trans,rot,center) ;
            if (meshRadius > maxRadius)
               maxRadius = meshRadius ;
         }
      }
      radius = maxRadius ;
   }

   void Shape::setSmallestSize(int pixels)
   {
      if (pixels < 1) 
         pixels = 1;
      
      smallestSize = float(pixels);

      int i=0;

      for (i = 0; i < detailLevels.size(); i++)
      {
         if (detailLevels[i].size < pixels)
            break ;
      }
      smallestDetailLevel = i;
   }

   void Shape::getNodeWorldPosRot(int n, Point &trans, Quaternion &rot)
   {
      // Build total translation & rotation for this node
      std::vector <int> nidx;
      nidx.push_back(n);
      while ((n = nodes[n].parent) >= 0)
         nidx.push_back(n);

      trans = Point(0,0,0);
      rot = Quaternion::identity;
      for (int i = nidx.size() - 1; i >= 0; i--)
      {
         trans += rot.apply(nodeDefTranslations[nidx[i]]);
         rot = nodeDefRotations[nidx[i]] * rot;
      }
   }
}