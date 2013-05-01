/*  mkVBOManager.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkVBOManager.h"
#include "platform/platformGL.h"
#include "console/console.h"

GLenum BDT2GLBDT[MKBDT_COUNT] = {GL_STATIC_DRAW_ARB, GL_DYNAMIC_DRAW_ARB, GL_STREAM_DRAW_ARB};

BufferObject::BufferObject()
{
   mStaticData = NULL;
}

BufferObject::BufferObject(U32 size, BufferDataType dataType)
{
   mDataType = dataType;
   mDataSize = size;
   mGLDataType = BDT2GLBDT[mDataType];
   
   mBuffer = 0;
}

BufferObject::~BufferObject()
{

}

void BufferObject::bind()
{
   if(!mBuffer)
   {
      glGenBuffersARB(1, &mBuffer);
      glBindBufferARB(mGLBufferType, mBuffer);
      glBufferDataARB(mGLBufferType, mDataSize, NULL, mGLDataType);
      return;
   }
   
   glBindBufferARB(mGLBufferType, mBuffer);
}

void BufferObject::unbind()
{
   glBindBufferARB(mGLBufferType, 0);
}

void* BufferObject::map()
{
   if(!mBuffer)
      glGenBuffersARB(1, &mBuffer);
   
   glBindBufferARB(mGLBufferType, mBuffer);
   
   glBufferDataARB(mGLBufferType, mDataSize, NULL, mGLDataType);
   return glMapBufferARB(mGLBufferType, GL_WRITE_ONLY_ARB);
}

bool BufferObject::unmap()
{
   return glUnmapBufferARB(mGLBufferType);
}

void BufferObject::uploadData(U32 size, U32 startOffset, void *data)
{
   if(!mBuffer)
   {
      glGenBuffersARB(1, &mBuffer);
      glBindBufferARB(mGLBufferType, mBuffer);
      glBufferDataARB(mGLBufferType, mDataSize, data, mGLDataType);
      return;
   }
   
   glBindBufferARB(mGLBufferType, mBuffer);
   glBufferSubDataARB(mGLBufferType, startOffset, size, data);
   glBindBufferARB(mGLBufferType, 0);
}

void BufferObject::getInternalGLData(U32 size, U32 startOffset, void* data)
{
   AssertFatal(mBuffer, "Must have a buffer to get data from!");
   
   glBindBufferARB(mGLBufferType, mBuffer);
   glGetBufferSubDataARB(mGLBufferType, startOffset, size, data);
   glBindBufferARB(mGLBufferType, 0);
}

void BufferObject::reset()
{
   if(!mStaticData)
   {
      mStaticData = dMalloc(mDataSize);
   }
   glBindBufferARB(mGLBufferType, mBuffer);
   glGetBufferSubDataARB(mGLBufferType, 0, mDataSize, mStaticData);

   glBindBufferARB(mGLBufferType, 0);
   glDeleteBuffersARB(1, &mBuffer);
}

void BufferObject::resurrect()
{
   glGenBuffersARB(1, &mBuffer);
   glBindBufferARB(mGLBufferType, mBuffer);
   glBufferDataARB(mGLBufferType, mDataSize, mStaticData, mGLDataType);
   glBindBufferARB(mGLBufferType, 0);
}

void BufferObject::kill()
{
   glDeleteBuffersARB(1, &mBuffer);
   if(mStaticData)
      dFree(mStaticData);
}

VertexBuffer::VertexBuffer(U32 size, BufferDataType dataType)
{

   mDataType = dataType;
   mDataSize = size;
   mGLDataType = BDT2GLBDT[mDataType];
   
   mBuffer = 0;
   mType = MKVertex;
   mGLBufferType = GL_ARRAY_BUFFER_ARB;
}

IndexBuffer::IndexBuffer(U32 size, BufferDataType dataType)
{

   mDataType = dataType;
   mDataSize = size;
   mGLDataType = BDT2GLBDT[mDataType];
   
   mBuffer = 0;
   mType = MKIndex;
   mGLBufferType = GL_ELEMENT_ARRAY_BUFFER_ARB;
}


_VBOManager::_VBOManager()
{
   mCurrVBO = NULL;
   mCurrIBO = NULL;
}

_VBOManager::~_VBOManager()
{
   kill();
}

IndexBuffer* _VBOManager::createIndexBuffer(U32 size, BufferDataType dataType, void* data)
{
   IndexBuffer* buf = new IndexBuffer(size, dataType);
   if(data)
      buf->uploadData(size, 0, data);
   mBuffers.push_back(buf);
   return buf;
}

VertexBuffer* _VBOManager::createVertexBuffer(U32 size, BufferDataType dataType, void* data)
{
   VertexBuffer* buf = new VertexBuffer(size, dataType);
   if(data)
      buf->uploadData(size, 0, data);
   mBuffers.push_back(buf);
   return buf;
}

void _VBOManager::deleteBuffer(BufferObject* buffer)
{
   for(U32 i = 0; i < mBuffers.size(); i++)
   {
      if(mBuffers[i] == buffer)
      {
         mBuffers[i]->kill();
         delete mBuffers[i];
         mBuffers.erase(i);
         return;
      }
   }
}

void _VBOManager::bindVBO(VertexBuffer* buffer)
{
   if(buffer)
   {
      buffer->bind();
   }
   else if(mCurrVBO)
   {
      mCurrVBO->unbind();
   }
   mCurrVBO = buffer;
}

void _VBOManager::bindIBO(IndexBuffer* buffer)
{
   if(buffer)
   {
      buffer->bind();
   }
   else if(mCurrIBO)
   {
      mCurrIBO->unbind();
   }
   mCurrIBO = buffer;
}

void _VBOManager::reset()
{
   for(U32 i = 0; i < mBuffers.size(); i++)
   {
      mBuffers[i]->reset();
   }
}

void _VBOManager::resurrect()
{
   for(U32 i = 0; i < mBuffers.size(); i++)
   {
      mBuffers[i]->resurrect();
   }
}

void _VBOManager::kill()
{
   for(U32 i = 0; i < mBuffers.size(); i++)
   {
      mBuffers[i]->kill();
      delete mBuffers[i];
   }
   mBuffers.clear();
}
