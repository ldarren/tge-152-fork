/*  mkVBOManager.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */
 
#ifndef _MKVBOMANAGER_H_
#define _MKVBOMANAGER_H_

#ifndef _DGL_H_
#include "dgl/dgl.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class MK;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

/// Enum for types of buffers.  May be later expanded for
/// pixel buffer objects, uniform buffer objects, texture buffer objects
/// etc.
enum BufferType
{
   MKVertex,
   MKIndex
};

/// Type of data we have
enum BufferDataType
{
   MKStatic,
   MKDynamic,
   MKStream,
   MKBDT_COUNT
};

/// This base class implements all buffer functionality.
/// OpenGL does not have different kinds of buffers.  It has buffer
/// objects which can be bound for different operations.  The
/// buffer API provided by the MK reflects that.
///
/// Okay, fine, I did it this way because it saves me time.
class BufferObject
{
   friend class _VBOManager;
   protected:
   
   /// Index or vertex buffer?
   BufferType mType;
   
   /// Static, dynamic, stream?
   BufferDataType mDataType;
   
   /// The actual GLenum of our data type
   GLenum mGLDataType;
   
   /// This stores the contents of the buffer during a context switch
   void* mStaticData;
   
   /// How much data we can put in the buffer
   U32 mDataSize;
   
   /// Internal GL object
   GLuint mBuffer;
   
   /// GLenum for our buffer type
   GLenum mGLBufferType;
   
   /// So GCC will stop complaining
   BufferObject();
   
   /// BufferObject constructor.  Size and dataType are provided here, and
   /// CAN NOT BE CHANGED AFTER CREATION!
   BufferObject(U32 size, BufferDataType dataType);
   ~BufferObject();
   
   /// Context dead
   void reset();
   
   /// Context back
   void resurrect();
   
   /// App dead
   void kill();
   
   /// Binds the buffer
   void bind();
   
   /// Binds 0 to whatever binding we represent
   void unbind();   
   
   public:
   /// Gets the proper pointer to use for OpenGL draw commands
   inline void* getBufferOffset(U32 offset) { return (void*)BUFFER_OFFSET(offset); }
   
   /// Maps the buffer to directly write data into it
   void* map();
   
   /// Unamps the buffer because we're done writing data
   bool unmap();
   
   /// Uploads new data to the buffer. DOES NOT RESIZE THE BUFFER
   ///
   /// @param size            How many bytes of data we are uploading
   /// @param startOffset     A byte offset into the buffer at which we start writing data
   /// @param data            The data to write
   void uploadData(U32 size, U32 startOffset, void* data);
   
   /// This downloads the buffer data from OpenGL into data
   ///
   /// @param size            How many bytes of data we want to download
   /// @param startOffset     How many bytes into the buffer we start reading data from
   /// @param data            Where we write the data to
   void getInternalGLData(U32 size, U32 startOffset, void* data);
};

/// This is a VertexBuffer.  It's constructor identifies the BufferObject as a
/// vertex buffer.  It's also handy for type checking.
class VertexBuffer : public BufferObject
{
   friend class _VBOManager;
   VertexBuffer(U32 size, BufferDataType dataType);
};

/// This is an IndexBuffer.  It's constructor identifies the BufferObject as an
/// index buffer.  It's also handy for type checking.
class IndexBuffer : public BufferObject
{
   friend class _VBOManager;
   IndexBuffer(U32 size, BufferDataType dataType);
};

/// The _VBOManager class manages VBOs.
class _VBOManager
{
   friend class MK;
   public:
   
   /// Creates an index buffer.
   ///
   /// @param size               The size of the buffer
   /// @param dataType           The type of data (static, dynamic, stream) the buffer stores
   /// @param data               The data to initially upload to the buffer (NULL is acceptable)
   IndexBuffer* createIndexBuffer(U32 size, BufferDataType dataType, void* data);
   
   /// Creates a vertex buffer.
   ///
   /// @param size               The size of the buffer
   /// @param dataType           The type of data (static, dynamic, stream) the buffer stores
   /// @param data               The data to initially upload to the buffer (NULL is acceptable)
   VertexBuffer* createVertexBuffer(U32 size, BufferDataType dataType, void* data);
   
   /// Deletes the specified buffer
   void deleteBuffer(BufferObject* buffer);
   
   /// Binds the given buffer.  If NULL, binds 0 to the vertex buffer attachment
   void bindVBO(VertexBuffer* buffer);
   
   /// Binds the given buffer.  If NULL, binds 0 to the index buffer attachment.
   void bindIBO(IndexBuffer* buffer);
   
   /// Context dead
   void reset();
   
   /// Context back
   void resurrect();
   
   /// App dead
   void kill();
   
   private:
   
   _VBOManager();
   ~_VBOManager();
   
   /// Currently bound VBO
   VertexBuffer* mCurrVBO;
   
   /// Currently bound IBO
   IndexBuffer* mCurrIBO;
   
   /// All of our created buffers.
   Vector<BufferObject*> mBuffers;
};

   
#endif

