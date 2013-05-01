//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


#ifndef GL_GROUP_BEGIN
#define GL_GROUP_BEGIN( flag )
#define UNDEF_BEGIN
#endif

#ifndef GL_GROUP_END
#define GL_GROUP_END()
#define UNDEF_END
#endif

//ARB_multitexture
GL_GROUP_BEGIN(ARB_multitexture)
GL_FUNCTION(void,       glActiveTextureARB, (GLenum target), return; )
GL_FUNCTION(void,       glClientActiveTextureARB, (GLenum target), return; )
//GL_FUNCTION(void,       glMultiTexCoord1dARB, (GLenum target, GLdouble s), return; )
//GL_FUNCTION(void,       glMultiTexCoord1dvARB, (GLenum target, const GLdouble *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord1fARB, (GLenum target, GLfloat s), return; )
//GL_FUNCTION(void,       glMultiTexCoord1fvARB, (GLenum target, const GLfloat *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord1iARB, (GLenum target, GLint s), return; )
//GL_FUNCTION(void,       glMultiTexCoord1ivARB, (GLenum target, const GLint *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord1sARB, (GLenum target, GLshort s), return; )
//GL_FUNCTION(void,       glMultiTexCoord1svARB, (GLenum target, const GLshort *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord2dARB, (GLenum target, GLdouble s, GLdouble t), return; )
//GL_FUNCTION(void,       glMultiTexCoord2dvARB, (GLenum target, const GLdouble *v), return; )
GL_FUNCTION(void,       glMultiTexCoord2fARB, (GLenum target, GLfloat s, GLfloat t), return; )
GL_FUNCTION(void,       glMultiTexCoord2fvARB, (GLenum target, const GLfloat *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord2iARB, (GLenum target, GLint s, GLint t), return; )
//GL_FUNCTION(void,       glMultiTexCoord2ivARB, (GLenum target, const GLint *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord2sARB, (GLenum target, GLshort s, GLshort t), return; )
//GL_FUNCTION(void,       glMultiTexCoord2svARB, (GLenum target, const GLshort *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord3dARB, (GLenum target, GLdouble s, GLdouble t, GLdouble r), return; )
//GL_FUNCTION(void,       glMultiTexCoord3dvARB, (GLenum target, const GLdouble *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord3fARB, (GLenum target, GLfloat s, GLfloat t, GLfloat r), return; )
//GL_FUNCTION(void,       glMultiTexCoord3fvARB, (GLenum target, const GLfloat *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord3iARB, (GLenum target, GLint s, GLint t, GLint r), return; )
//GL_FUNCTION(void,       glMultiTexCoord3ivARB, (GLenum target, const GLint *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord3sARB, (GLenum target, GLshort s, GLshort t, GLshort r), return; )
//GL_FUNCTION(void,       glMultiTexCoord3svARB, (GLenum target, const GLshort *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord4dARB, (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q), return; )
//GL_FUNCTION(void,       glMultiTexCoord4dvARB, (GLenum target, const GLdouble *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord4fARB, (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q), return; )
//GL_FUNCTION(void,       glMultiTexCoord4fvARB, (GLenum target, const GLfloat *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord4iARB, (GLenum target, GLint s, GLint t, GLint r, GLint q), return; )
//GL_FUNCTION(void,       glMultiTexCoord4ivARB, (GLenum target, const GLint *v), return; )
//GL_FUNCTION(void,       glMultiTexCoord4sARB, (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q), return; )
//GL_FUNCTION(void,       glMultiTexCoord4svARB, (GLenum target, const GLshort *v), return; )
GL_GROUP_END()

//ARB_texture_compression
GL_GROUP_BEGIN(ARB_texture_compression)
GL_FUNCTION(void,			glCompressedTexImage3DARB, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void*), return; )
GL_FUNCTION(void,			glCompressedTexImage2DARB, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void*), return; )
GL_FUNCTION(void,			glCompressedTexImage1DARB, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLsizei imageSize, const void*), return; )
GL_FUNCTION(void,			glCompressedTexSubImage3DARB, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void*), return; )
GL_FUNCTION(void,			glCompressedTexSubImage2DARB, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void*), return; )
GL_FUNCTION(void,			glCompressedTexSubImage1DARB, (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void*), return; )
GL_FUNCTION(void,			glGetCompressedTexImageARB, (GLenum target, GLint lod, void* img), return; )
GL_GROUP_END()

//EXT_compiled_vertex_array
GL_GROUP_BEGIN(EXT_compiled_vertex_array)
GL_FUNCTION(void,       glLockArraysEXT, (GLint first, GLsizei count), return; )
GL_FUNCTION(void,       glUnlockArraysEXT, (void), return; )
GL_GROUP_END()

//EXT_fog_coord
GL_GROUP_BEGIN(EXT_fog_coord)
GL_FUNCTION(void,			glFogCoordfEXT, (GLfloat), return; )
GL_FUNCTION(void ,		glFogCoordPointerEXT, (GLenum, GLsizei, void*), return; )
GL_GROUP_END()

//EXT_paletted_texture
GL_GROUP_BEGIN(EXT_paletted_texture)
GL_FUNCTION(void,       glColorTableEXT, (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* data), return; )
GL_GROUP_END()


// EXT_blend_color
// http://www.msi.unilim.fr/~porquet/glexts/GL_EXT_blend_color.txt.html
#define GL_CONSTANT_COLOR_EXT                0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT      0x8002
#define GL_CONSTANT_ALPHA_EXT                0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT      0x8004
#define GL_BLEND_COLOR_EXT                   0x8005

GL_GROUP_BEGIN(EXT_blend_color)
GL_FUNCTION(void,       glBlendColorEXT, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha), return; )
GL_GROUP_END()

// EXT_blend_minmax
// http://www.msi.unilim.fr/~porquet/glexts/GL_EXT_blend_minmax.txt.html
#define GL_FUNC_ADD_EXT                      0x8006
#define GL_MIN_EXT                           0x8007
#define GL_MAX_EXT                           0x8008
#define GL_BLEND_EQUATION_EXT                0x8009

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

// EXT_blend_subtract
// http://www.msi.unilim.fr/~porquet/glexts/GL_EXT_blend_subtract.txt.html
#define GL_FUNC_SUBTRACT_EXT                 0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT         0x800B

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

GL_GROUP_BEGIN(EXT_blend_minmax)
GL_FUNCTION(void,       glBlendEquationEXT, (GLenum mode), return; )
GL_GROUP_END()

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

//ARB_texture_rectangle
// http://www.msi.unilim.fr/~porquet/glexts/GL_ARB_texture_rectangle.txt.html
#define GL_TEXTURE_RECTANGLE_ARB            0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB    0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_ARB      0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB   0x84F8

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

//NV_vertex_array_range
#ifdef TORQUE_OS_WIN32
GL_GROUP_BEGIN(NV_vertex_array_range)
GL_FUNCTION(void, glVertexArrayRangeNV, (GLsizei length, void* pointer), return; )
GL_FUNCTION(void, glFlushVertexArrayRangeNV, (), return; )
GL_FUNCTION(void*, wglAllocateMemoryNV, (GLsizei, GLfloat, GLfloat, GLfloat), return NULL; )
GL_FUNCTION(void, wglFreeMemoryNV, (void*), return; )
GL_GROUP_END()
#endif

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

//GL_ARB_depth_texture
#define GL_DEPTH_COMPONENT16_ARB	0x81A5
#define GL_DEPTH_COMPONENT24_ARB	0x81A6
#define GL_DEPTH_COMPONENT32_ARB	0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB	0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB	0x884B

//GL_EXT_framebuffer_object
#define GL_FRAMEBUFFER_EXT                     0x8D40
#define GL_RENDERBUFFER_EXT                    0x8D41
#define GL_STENCIL_INDEX1_EXT                  0x8D46
#define GL_STENCIL_INDEX4_EXT                  0x8D47
#define GL_STENCIL_INDEX8_EXT                  0x8D48
#define GL_STENCIL_INDEX16_EXT                 0x8D49
#define GL_RENDERBUFFER_WIDTH_EXT              0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT             0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT    0x8D44
#define GL_RENDERBUFFER_RED_SIZE_EXT           0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE_EXT         0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE_EXT          0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT         0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT         0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT       0x8D55
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4
#define GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT               0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT               0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT               0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT               0x8CED
#define GL_COLOR_ATTACHMENT14_EXT               0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT               0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT               0x8D20
#define GL_FRAMEBUFFER_COMPLETE_EXT                          0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT             0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT     0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT             0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT            0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT            0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                       0x8CDD
#define GL_FRAMEBUFFER_BINDING_EXT             0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT            0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT           0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT           0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT   0x0506

GL_GROUP_BEGIN(EXT_framebuffer_object)
GL_FUNCTION(GLboolean, glIsRenderbufferEXT, (GLuint renderbuffer), return false; )
GL_FUNCTION(void, glBindRenderbufferEXT, (GLenum target, GLuint renderbuffer), return; )
GL_FUNCTION(void, glDeleteRenderbuffersEXT, (GLsizei n, const GLuint *renderbuffers), return; )
GL_FUNCTION(void, glGenRenderbuffersEXT, (GLsizei n, GLuint *renderbuffers), return; )
GL_FUNCTION(void, glRenderbufferStorageEXT, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height), return; )
GL_FUNCTION(void, glGetRenderbufferParameterivEXT, (GLenum target, GLenum pname, GLint *params), return; )
GL_FUNCTION(GLboolean, glIsFramebufferEXT, (GLuint framebuffer), return false; )
GL_FUNCTION(void, glBindFramebufferEXT, (GLenum target, GLuint framebuffer), return; )
GL_FUNCTION(void, glDeleteFramebuffersEXT, (GLsizei n, const GLuint *framebuffers), return; )
GL_FUNCTION(void, glGenFramebuffersEXT, (GLsizei n, GLuint *framebuffers), return; )
GL_FUNCTION(GLenum, glCheckFramebufferStatusEXT, (GLenum target), return GL_FRAMEBUFFER_UNSUPPORTED_EXT; )
GL_FUNCTION(void, glFramebufferTexture1DEXT, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level), return; )
GL_FUNCTION(void, glFramebufferTexture2DEXT, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level), return; )
GL_FUNCTION(void, glFramebufferTexture3DEXT, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset), return; )
GL_FUNCTION(void, glFramebufferRenderbufferEXT, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer), return; )
GL_FUNCTION(void, glGetFramebufferAttachmentParameterivEXT, (GLenum target, GLenum attachment, GLenum pname, GLint *params), return; )
GL_FUNCTION(void, glGenerateMipmapEXT, (GLenum target), return; )
GL_GROUP_END()

//GL_ARB_vertex_buffer_objects
#define GL_ARRAY_BUFFER_ARB                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
#define GL_ARRAY_BUFFER_BINDING_ARB                     0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB             0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB              0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB              0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB               0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB               0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB       0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB           0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB     0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB      0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB              0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB       0x889F
#define GL_STREAM_DRAW_ARB                              0x88E0
#define GL_STREAM_READ_ARB                              0x88E1
#define GL_STREAM_COPY_ARB                              0x88E2
#define GL_STATIC_DRAW_ARB                              0x88E4
#define GL_STATIC_READ_ARB                              0x88E5
#define GL_STATIC_COPY_ARB                              0x88E6
#define GL_DYNAMIC_DRAW_ARB                             0x88E8
#define GL_DYNAMIC_READ_ARB                             0x88E9
#define GL_DYNAMIC_COPY_ARB                             0x88EA
#define GL_READ_ONLY_ARB                                0x88B8
#define GL_WRITE_ONLY_ARB                               0x88B9
#define GL_READ_WRITE_ARB                               0x88BA
#define GL_BUFFER_SIZE_ARB                              0x8764
#define GL_BUFFER_USAGE_ARB                             0x8765
#define GL_BUFFER_ACCESS_ARB                            0x88BB
#define GL_BUFFER_MAPPED_ARB                            0x88BC
#define GL_BUFFER_MAP_POINTER_ARB                       0x88BD

GL_GROUP_BEGIN(ARB_vertex_buffer_object)
GL_FUNCTION(void, glBindBufferARB, (GLenum target, GLuint buffer), return; )
GL_FUNCTION(void, glDeleteBuffersARB, (GLsizei n, const GLuint *buffers), return; )
GL_FUNCTION(void, glGenBuffersARB, (GLsizei n, GLuint *buffers), return; )
GL_FUNCTION(GLboolean, glIsBufferARB, (GLuint buffer), return false; )
GL_FUNCTION(void, glBufferDataARB, (GLenum target, GLsizei size, const GLvoid *data, GLenum usage), return; )
GL_FUNCTION(void, glBufferSubDataARB, (GLenum target, GLint offset, GLsizei size, const GLvoid *data), return; )
GL_FUNCTION(void* , glMapBufferARB, (GLenum target, GLenum access), return NULL; )
GL_FUNCTION(GLboolean, glUnmapBufferARB, (GLenum target), return false; )
GL_FUNCTION(void, glGetBufferParameterivARB, (GLenum target, GLenum pname, GLint *params), return; )
GL_FUNCTION(void, glGetBufferPointervARB, (GLenum target, GLenum pname, GLvoid **params), return; )
GL_FUNCTION(void, glGetBufferSubDataARB, (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data), return; )
GL_GROUP_END()

//GL_ARB_shader_objects
#define GL_PROGRAM_OBJECT_ARB				0x8B40
#define GL_OBJECT_TYPE_ARB					0x8B4E
#define GL_OBJECT_SUBTYPE_ARB				0x8B4F
#define GL_OBJECT_DELETE_STATUS_ARB			0x8B80
#define GL_OBJECT_COMPILE_STATUS_ARB			0x8B81
#define GL_OBJECT_LINK_STATUS_ARB				0x8B82
#define GL_OBJECT_VALIDATE_STATUS_ARB			0x8B83
#define GL_OBJECT_INFO_LOG_LENGTH_ARB			0x8B84
#define GL_OBJECT_ATTACHED_OBJECTS_ARB			0x8B85
#define GL_OBJECT_ACTIVE_UNIFORMS_ARB			0x8B86
#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB		0x8B87
#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB			0x8B88
#define GL_SHADER_OBJECT_ARB				0x8B48
#define GL_FLOAT						0x1406
#define GL_FLOAT_VEC2_ARB					0x8B50
#define GL_FLOAT_VEC3_ARB					0x8B51
#define GL_FLOAT_VEC4_ARB					0x8B52
#define GL_INT						0x1404
#define GL_INT_VEC2_ARB					0x8B53
#define GL_INT_VEC3_ARB					0x8B54
#define GL_INT_VEC4_ARB					0x8B55
#define GL_BOOL_ARB					0x8B56
#define GL_BOOL_VEC2_ARB					0x8B57
#define GL_BOOL_VEC3_ARB					0x8B58
#define GL_BOOL_VEC4_ARB					0x8B59
#define GL_FLOAT_MAT2_ARB					0x8B5A
#define GL_FLOAT_MAT3_ARB					0x8B5B
#define GL_FLOAT_MAT4_ARB					0x8B5C
#define GL_SAMPLER_1D_ARB					0x8B5D
#define GL_SAMPLER_2D_ARB					0x8B5E
#define GL_SAMPLER_3D_ARB					0x8B5F
#define GL_SAMPLER_CUBE_ARB				0x8B60
#define GL_SAMPLER_1D_SHADOW_ARB				0x8B61
#define GL_SAMPLER_2D_SHADOW_ARB				0x8B62
#define GL_SAMPLER_2D_RECT_ARB				0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW_ARB			0x8B64

//GL_ARB_shading_language_100
#ifndef GL_SHADING_LANGUAGE_VERSION_ARB
#define GL_SHADING_LANGUAGE_VERSION_ARB		0x8B8C
#endif

//GL_ARB_vertex_shader
#define GL_VERTEX_SHADER_ARB				0x8B31
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB		0x8B4A
#define GL_MAX_VARYING_FLOATS_ARB				0x8B4B
#define GL_MAX_VERTEX_ATTRIBS_ARB				0x8869
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB			0x8872
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB		0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB		0x8B4D
#define GL_MAX_TEXTURE_COORDS_ARB				0x8871
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB			0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB			0x8643
#define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB			0x8B89
#define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB		0x8B8A
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB			0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB			0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB			0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB			0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB		0x886A
#define GL_CURRENT_VERTEX_ATTRIB_ARB			0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB			0x8645

//GL_ARB_fragment_shader
#define GL_FRAGMENT_SHADER_ARB				0x8B30
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB		0x8B49
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB		0x8B8B

GL_GROUP_BEGIN(ARB_shader_objects)
GL_FUNCTION(void, glDeleteObjectARB, (GLenum obj), return; )
GL_FUNCTION(GLenum, glGetHandleARB, (GLenum pname), return 0; )
GL_FUNCTION(void, glDetachObjectARB, (GLenum containerObj, GLenum attachedObj), return; )
GL_FUNCTION(GLenum, glCreateShaderObjectARB, (GLenum shaderType), return 0; )
GL_FUNCTION(void, glShaderSourceARB, (GLenum shaderObj, GLsizei count, const char **string, const GLint *length), return; )
GL_FUNCTION(void, glCompileShaderARB, (GLenum shaderObj), return; )
GL_FUNCTION(GLenum,  glCreateProgramObjectARB, (), return 0; )
GL_FUNCTION(void, glAttachObjectARB, (GLenum containerObj, GLenum obj), return; )
GL_FUNCTION(void, glLinkProgramARB, (GLenum programObj), return; )
GL_FUNCTION(void, glUseProgramObjectARB, (GLenum programObj), return; )
GL_FUNCTION(void, glValidateProgramARB, (GLenum programObj), return; )
GL_FUNCTION(void, glUniform1fARB, (GLint location, GLfloat v0), return; )
GL_FUNCTION(void, glUniform2fARB, (GLint location, GLfloat v0, GLfloat v1), return; )
GL_FUNCTION(void, glUniform3fARB, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2), return; )
GL_FUNCTION(void, glUniform4fARB, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3), return; )
GL_FUNCTION(void, glUniform1iARB, (GLint location, GLint v0), return; )
GL_FUNCTION(void, glUniform2iARB, (GLint location, GLint v0, GLint v1), return; )
GL_FUNCTION(void, glUniform3iARB, (GLint location, GLint v0, GLint v1, GLint v2), return; )
GL_FUNCTION(void, glUniform4iARB, (GLint location, GLint v0, GLint v1, GLint v2, GLint v3), return; )
GL_FUNCTION(void, glUniform1fvARB, (GLint location, GLsizei count, const GLfloat *value), return; )
GL_FUNCTION(void, glUniform2fvARB, (GLint location, GLsizei count, const GLfloat *value), return; )
GL_FUNCTION(void, glUniform3fvARB, (GLint location, GLsizei count, const GLfloat *value), return; )
GL_FUNCTION(void, glUniform4fvARB, (GLint location, GLsizei count, const GLfloat *value), return; )
GL_FUNCTION(void, glUniform1ivARB, (GLint location, GLsizei count, const GLint *value), return; )
GL_FUNCTION(void, glUniform2ivARB, (GLint location, GLsizei count, const GLint *value), return; )
GL_FUNCTION(void, glUniform3ivARB, (GLint location, GLsizei count, const GLint *value), return; )
GL_FUNCTION(void, glUniform4ivARB, (GLint location, GLsizei count, const GLint *value), return; )
GL_FUNCTION(void, glUniformMatrix2fvARB, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), return; )
GL_FUNCTION(void, glUniformMatrix3fvARB, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), return; )
GL_FUNCTION(void, glUniformMatrix4fvARB, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), return; )
GL_FUNCTION(void, glGetObjectParameterfvARB, (GLenum obj, GLenum pname, GLfloat *params), return; )
GL_FUNCTION(void, glGetObjectParameterivARB, (GLenum obj, GLenum pname, GLint *params), return; )
GL_FUNCTION(void, glGetInfoLogARB, (GLenum obj, GLsizei maxLength, GLsizei *length, char *infoLog), return; )
GL_FUNCTION(void, glGetAttachedObjectsARB, (GLenum containerObj, GLsizei maxCount, GLsizei *count, GLenum *obj), return; )
GL_FUNCTION(GLint,  glGetUniformLocationARB, (GLenum programObj, const char *name), return 0; )
GL_FUNCTION(void, glGetActiveUniformARB, (GLenum programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, char *name), return; )
GL_FUNCTION(void, glGetUniformfvARB, (GLenum programObj, GLint location, GLfloat *params), return; )
GL_FUNCTION(void, glGetUniformivARB, (GLenum programObj, GLint location, GLint *params), return; )
GL_FUNCTION(void, glGetShaderSourceARB, (GLenum obj, GLsizei maxLength, GLsizei *length, char *source), return; )
GL_FUNCTION(void, glGetProgramivARB, (GLenum obj, GLenum pname, GLint *params), return; )
GL_FUNCTION(void, glGenProgramsARB, (GLsizei n, GLuint *programs), return; )
GL_FUNCTION(void, glBindProgramARB, (GLenum obj, GLuint program), return; )
GL_FUNCTION(void, glProgramStringARB, (GLenum obj, GLenum format, GLsizei len, const GLvoid *string), return; )
GL_FUNCTION(void, glDeleteProgramsARB, (GLsizei n, const GLuint *programs), return; )
GL_FUNCTION(void, glProgramEnvParameter4fARB, (GLenum obj, GLuint idx, GLfloat x, GLfloat y, GLfloat z, GLfloat w), return; )
GL_FUNCTION(void, glProgramLocalParameter4fARB, (GLenum obj, GLuint idx, GLfloat x, GLfloat y, GLfloat z, GLfloat w), return; )
GL_GROUP_END()

GL_GROUP_BEGIN(ARB_vertex_shader)
GL_FUNCTION(void, glVertexAttrib1fARB, (GLuint index, GLfloat v0), return; )
GL_FUNCTION(void, glVertexAttrib1sARB, (GLuint index, GLshort v0), return; )
GL_FUNCTION(void, glVertexAttrib1dARB, (GLuint index, GLdouble v0), return; )
GL_FUNCTION(void, glVertexAttrib2fARB, (GLuint index, GLfloat v0, GLfloat v1), return; )
GL_FUNCTION(void, glVertexAttrib2sARB, (GLuint index, GLshort v0, GLshort v1), return; )
GL_FUNCTION(void, glVertexAttrib2dARB, (GLuint index, GLdouble v0, GLdouble v1), return; )
GL_FUNCTION(void, glVertexAttrib3fARB, (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2), return; )
GL_FUNCTION(void, glVertexAttrib3sARB, (GLuint index, GLshort v0, GLshort v1, GLshort v2), return; )
GL_FUNCTION(void, glVertexAttrib3dARB, (GLuint index, GLdouble v0, GLdouble v1, GLdouble v2), return; )
GL_FUNCTION(void, glVertexAttrib4fARB, (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3), return; )
GL_FUNCTION(void, glVertexAttrib4sARB, (GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3), return; )
GL_FUNCTION(void, glVertexAttrib4dARB, (GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3), return; )
GL_FUNCTION(void, glVertexAttrib4NubARB, (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w), return; )
GL_FUNCTION(void, glVertexAttrib1fvARB, (GLuint index, const GLfloat *v), return; )
GL_FUNCTION(void, glVertexAttrib1svARB, (GLuint index, const GLshort *v), return; )
GL_FUNCTION(void, glVertexAttrib1dvARB, (GLuint index, const GLdouble *v), return; )
GL_FUNCTION(void, glVertexAttrib2fvARB, (GLuint index, const GLfloat *v), return; )
GL_FUNCTION(void, glVertexAttrib2svARB, (GLuint index, const GLshort *v), return; )
GL_FUNCTION(void, glVertexAttrib2dvARB, (GLuint index, const GLdouble *v), return; )
GL_FUNCTION(void, glVertexAttrib3fvARB, (GLuint index, const GLfloat *v), return; )
GL_FUNCTION(void, glVertexAttrib3svARB, (GLuint index, const GLshort *v), return; )
GL_FUNCTION(void, glVertexAttrib3dvARB, (GLuint index, const GLdouble *v), return; )
GL_FUNCTION(void, glVertexAttrib4fvARB, (GLuint index, const GLfloat *v), return; )
GL_FUNCTION(void, glVertexAttrib4svARB, (GLuint index, const GLshort *v), return; )
GL_FUNCTION(void, glVertexAttrib4dvARB, (GLuint index, const GLdouble *v), return; )
GL_FUNCTION(void, glVertexAttrib4ivARB, (GLuint index, const GLint *v), return; )
GL_FUNCTION(void, glVertexAttrib4bvARB, (GLuint index, const GLbyte *v), return; )
GL_FUNCTION(void, glVertexAttrib4ubvARB, (GLuint index, const GLubyte *v), return; )
GL_FUNCTION(void, glVertexAttrib4usvARB, (GLuint index, const GLushort *v), return; )
GL_FUNCTION(void, glVertexAttrib4uivARB, (GLuint index, const GLuint *v), return; )
GL_FUNCTION(void, glVertexAttrib4NbvARB, (GLuint index, const GLbyte *v), return; )
GL_FUNCTION(void, glVertexAttrib4NsvARB, (GLuint index, const GLshort *v), return; )
GL_FUNCTION(void, glVertexAttrib4NivARB, (GLuint index, const GLint *v), return; )
GL_FUNCTION(void, glVertexAttrib4NubvARB, (GLuint index, const GLubyte *v), return; )
GL_FUNCTION(void, glVertexAttrib4NusvARB, (GLuint index, const GLushort *v), return; )
GL_FUNCTION(void, glVertexAttrib4NuivARB, (GLuint index, const GLuint *v), return; )
GL_FUNCTION(void, glVertexAttribPointerARB, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer), return; )
GL_FUNCTION(void, glEnableVertexAttribArrayARB, (GLuint index), return; )
GL_FUNCTION(void, glDisableVertexAttribArrayARB, (GLuint index), return; )
GL_FUNCTION(void, glBindAttribLocationARB, (GLenum programObj, GLuint index, const char *name), return; )
GL_FUNCTION(void, glGetActiveAttribARB, (GLenum programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, char *name), return; )
GL_FUNCTION(GLint, glGetAttribLocationARB, (GLenum programObj, const char *name), return 0; )
GL_FUNCTION(void, glGetVertexAttribdvARB, (GLuint index, GLenum pname, GLdouble *params), return; )
GL_FUNCTION(void, glGetVertexAttribfvARB, (GLuint index, GLenum pname, GLfloat *params), return; )
GL_FUNCTION(void, glGetVertexAttribivARB, (GLuint index, GLenum pname, GLint *params), return; )
GL_FUNCTION(void, glGetVertexAttribPointervARB, (GLuint index, GLenum pname, void **pointer), return; )
GL_GROUP_END()

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

#ifdef UNDEF_BEGIN
#undef GL_GROUP_BEGIN
#undef UNDEF_BEGIN
#endif

#ifdef UNDEF_END
#undef GL_GROUP_END
#undef UNDEF_END
#endif
