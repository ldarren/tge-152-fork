//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

GL_FUNCTION(void,		gluOrtho2D, (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top), return; )
GL_FUNCTION(void,		gluPerspective, (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar), return; )
GL_FUNCTION(void,		gluPickMatrix, (GLdouble x, GLdouble y, GLdouble width, GLdouble height, GLint viewport[4]), return; )
GL_FUNCTION(void,		gluLookAt, (GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz), return; )
GL_FUNCTION(int,		gluProject, (GLdouble objx, GLdouble objy, GLdouble objz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *winx, GLdouble *winy, GLdouble *winz), return 0; )
GL_FUNCTION(int,		gluUnProject, (GLdouble winx, GLdouble winy, GLdouble winz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *objx, GLdouble *objy, GLdouble *objz), return 0; )
GL_FUNCTION(int,		gluScaleImage, (GLenum format, GLint widthin, GLint heightin, GLenum typein, const void *datain, GLint widthout, GLint heightout, GLenum typeout, void *dataout), return 0; )
GL_FUNCTION(int,		gluBuild1DMipmaps, (GLenum target, GLint components, GLint width, GLenum format, GLenum type, const void *data), return 0; )
GL_FUNCTION(int,		gluBuild2DMipmaps, (GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void *data), return 0; )
