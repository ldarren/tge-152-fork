/*  mkInterior.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#ifndef _MKINTERIOR_H_
#define _MKINTERIOR_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class Interior;
class VertexBuffer;
class TextureHandle;
class FileStream;
class Point2F;
struct MaterialList;
class Point3F;
class Material;

/// mkInterior is a nice wrapper class to make integration of the ModernizationKit into
/// existing Torque projects easier.  It also provides a single unified place
/// for all Modernization Kit additions to the Interior class.
///
/// mkInterior is Interior's primary point of contact with the MK.
class mkInterior
{
   friend class Interior;
   
   /// Constructor
   mkInterior();
   
   /// Destructor
   ~mkInterior();
   
   VertexBuffer *mVertexBuffer, *mFogCoordBuffer; ///< MK vertex buffers
   TextureHandle *mBlack, *mWhite; ///< Default textures
   
   /// Sets up rendering including VBO and material setup
   ///
   /// @param vbo       Vertex buffer to render the interior
   /// @param fogActive Do we need to fog the interior?
   /// @param fogCoords Really just a passed in pointer to sgFogCoords
   /// @param windings  The interior windings for doing lookups into the VBO
   /// @param camPos    The object space camera position
   void prepRender(VertexBuffer* vbo, bool fogActive, Point2F *fogCoords, Vector<U32> &windings, Point3F camPos);
   
   /// Resets all GL states (takes fogging/shaders into account)
   bool endRender();
   
   /// Streams fog coordinates into mFogCoordBuffer
   ///
   /// @param fogCoords    The fog texture coordinates
   /// @param windings     The interior windings
   void streamFogCoords(Point2F *fogCoords, Vector<U32> &windings); 
   
   /// Initializes the mkInterior, loads up/generates normal maps.
   ///
   /// @param materials       The interior's material list
   /// @param path            The path to the interior file
   void init(MaterialList *materials, const char* path);
   
   /// Binds the proper normal map given the surface index
   ///
   /// @param surfaceIndex       Used to index into the normal map array
   void bindMaterial(U32 surfaceIndex);
   
   Vector<Material *> mMaterials; ///< All of our normal maps
   
   /// Loads up the proper material.  Returns true if a shader was bound, otherwise returns false
   bool initShader();
   
   bool mIsUsingShader; ///< Are we doing shaderized rendering?
   
   /// Generates a normal map using 3x3 sobel filtering
   ///
   /// @param handle       The texture we are generating a normal map for
   /// @param name         The name of the texture
   /// @param path         The path to the interior file
   /// @param pMapStream   The filestream we are writing the script commands to
   void generateMaterial(TextureHandle& handle, const char* name, const char* path, FileStream &pMapStream); // Generate a normal map from the passed texture
};

#endif

