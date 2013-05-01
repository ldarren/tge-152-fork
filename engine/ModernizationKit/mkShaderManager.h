/*  mkShaderManager.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */
 
#ifndef _MKSHADERMANAGER_H_
#define _MKSHADERMANAGER_H_

#ifndef _DGL_H_
#include "dgl/dgl.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _MMATRIX_H_
#include "math/mMatrix.h"
#endif

#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif

/// Somtimes Cg just hates the world, and it has to be taken out
//#define SHADER_MANAGER_USE_CG

/// If defined, shaders are run through error checking after being compiled
#define SHADER_MANAGER_DEBUG

#ifdef SHADER_MANAGER_USE_CG
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif

class Shader;
class _ShaderManager;

/// A handy enum for tracking what kind of shader we're dealing with without nasty dynamic_cast
/// Primarily used by script to core engine code.
enum ShaderType
{
   GLSLShaderType = 0,
   CGShaderType = 1,
   ASMShaderType = 2,
   ATITextShaderType = 3
};

#ifdef SHADER_MANAGER_USE_CG
/// A parameter struct from a Cg shader.
struct CGShaderParameter
{
   StringTableEntry mName;
   CGparameter mVertexId;
   CGparameter mFragmentId;
};
#endif

/// A parameter struct from a GLSL shader.
struct GLSLShaderParameter
{
   StringTableEntry mName;
   GLuint mId;
};

/// Parameter struct for an ARB_*_program shader
struct ASMShaderParameter
{
   StringTableEntry mName;
   GLuint mVertexId;
   GLuint mFragmentId;
   bool mVertexIsEnv;
   bool mFragmentIsEnv;
};

/// Does absolutely nothing.  Pure virtual (well almost) class. 
class Shader
{
   friend class _ShaderManager;
   
protected:
   /// Most of these should be self explanatory
   char* mPixelSourceString;
   char* mVertexSourceString;
   /// Filepath -Vert/Pixl.blah  For shader pairs.
   /// @note Deprecated.  Will be removed before shipping
   StringTableEntry mFilepath;
   
   /// The filepath to the vertex shader
   StringTableEntry mVertexFilepath;
   
   /// The filepath to the fragment shader
   StringTableEntry mFragFilepath;
   
   /// Length of the source of the fragment shader
   U32 mPixelSourceStringLength;
   
   /// Length of the source of the vertex shader
   U32 mVertexSourceStringLength;
   
   /// Binds the shader
   virtual void bind();
   
   /// Unbinds the shader
   virtual void unbind();
    
   /// @name Parameter setting
   /// Cg and GLSL need to know how many values were dealing with
   /// so there are lots of different functions for setting parameters
   /// @{
   virtual bool setParameter1f(StringTableEntry name, F32 value0);
   virtual bool setParameter2f(StringTableEntry name, F32 value0, F32 value1);
   virtual bool setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2);
   virtual bool setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3);
    
   virtual bool setParameter1i(StringTableEntry name, U32 value0);
   virtual bool setParameter2i(StringTableEntry name, U32 value0, U32 value1);
   virtual bool setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2);
   virtual bool setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3);
    
   virtual bool setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m);
    
   /// This function is for GLSL's benefit.
   virtual bool setTextureSampler(StringTableEntry name, U32 value0);
   /// @}

   /// Loads the shader from the given files.
   ///
   /// @param vertPath           path to the vertex shader file
   /// @param fragPath           path to the fragment shader file
   virtual bool loadShaderFromFiles(const char* vertPath, const char* fragPath);
   
   /// Doesn't even load a file.
   /// @note Used by the shader IDE.
   virtual bool compileShaderFromSource(const char* vertSource, const char* fragSource, bool includeLibs, bool link);
   
   /// What kind of shader we are.
   ShaderType mType;
   
   /// Kills off all context senstive shader information
   virtual void reset();
   /// Restores all context sensitive shader information
   virtual void resurrect();
   /// Kills the shader
   virtual void kill();

public:
   /// @name Debugging information
   /// Used by the shader IDE
   /// @{
   U32 mVertCompileStatus;
   U32 mFragCompileStatus;
   U32 mLinkStatus;

   virtual const char* getProgramLog();
   virtual const char* getVertexLog();
   virtual const char* getFragmentLog();
   
   /// Checks for success/failure in compilation and linking
   virtual void updateStatus();
   /// @}
   
   ShaderType getType() { return mType; }
   
   StringTableEntry getVertexFilepath() { return mVertexFilepath; };
   StringTableEntry getFragmentFilepath() { return mFragFilepath; };
   
   /// This function is for Cg's benefit.  Updates the state matrices
   /// Currently only called before glDrawElements in TSMesh::render
   virtual void updateStateMatrices();
   
   /// Reloads all parameters.
   virtual void reloadParameters();
};

#ifdef SHADER_MANAGER_USE_CG
/// The Cg shader class.
class CGShader : public Shader
{
   friend class _ShaderManager;
   private:

   CGShader();

   /// Internal fragment shader object
   CGprogram mFragmentShader;
   
   /// Internal vertex shader object
   CGprogram mVertexShader;
   
   /// @name Logs
   /// @{
   StringTableEntry mVertErrorLog;
   StringTableEntry mFragErrorLog;
   
   StringTableEntry mFragCompilationLog;
   StringTableEntry mVertCompilationLog;
   /// @}
   
   /// Shader profiles
   CGprofile mVertexProfile, mFragmentProfile;

   /// @name Parameter setting
   /// @{
   virtual bool setParameter1f(StringTableEntry name, F32 value0);
   virtual bool setParameter2f(StringTableEntry name, F32 value0, F32 value1);
   virtual bool setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2);
   virtual bool setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3);
    
   virtual bool setParameter1i(StringTableEntry name, U32 value0);
   virtual bool setParameter2i(StringTableEntry name, U32 value0, U32 value1);
   virtual bool setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2);
   virtual bool setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3);
   
   virtual bool setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m);
   
   virtual bool setTextureSampler(StringTableEntry name, U32 value0);
   /// @}
   
   /// Loads a vertex shader from the given file
   /// @returns true if the shader loaded, false otherwise
   bool loadVertexShaderFromFile(const char* vertPath);
   
   /// Loads a fragment shader from the given file
   /// @returns true if the shader loaded, false otherwise
   bool loadFragmentShaderFromFile(const char* fragPath);
   
   /// Loads a shader from teh given files
   virtual bool loadShaderFromFiles(const char* vertPath, const char* fragPath);
   virtual bool compileShaderFromSource(const char* vertSource, const char* fragSource, bool includeLibs, bool link);
   
   /// Returns a CGShaderParameter which matches the given name
   virtual CGShaderParameter* getNamedParameter(StringTableEntry name);
   
   /// A list of all of our parameters
   Vector<CGShaderParameter*> mParameters;
   
   virtual void reset();
   virtual void resurrect();
   virtual void kill();
   
   virtual void bind();
   virtual void unbind();
   
public:
   virtual void updateStateMatrices();
   virtual void reloadParameters();
   
   virtual const char* getProgramLog();
   virtual const char* getVertexLog();
   virtual const char* getFragmentLog();
   
   virtual void updateStatus();
};
#endif

/// The ARB_*_program shader class
class ASMShader : public Shader
{
   friend class _ShaderManager;
   private:

   ASMShader();

   GLuint mVertexProgram;
   GLuint mFragmentProgram;
   
   virtual bool setParameter1f(StringTableEntry name, F32 value0);
   virtual bool setParameter2f(StringTableEntry name, F32 value0, F32 value1);
   virtual bool setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2);
   virtual bool setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3);
    
   virtual bool setParameter1i(StringTableEntry name, U32 value0);
   virtual bool setParameter2i(StringTableEntry name, U32 value0, U32 value1);
   virtual bool setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2);
   virtual bool setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3);
   
   virtual bool setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m);
   
   virtual bool setTextureSampler(StringTableEntry name, U32 value0);
   
   Vector<ASMShaderParameter*> mParameters;
   
   virtual ASMShaderParameter* getNamedParameter(StringTableEntry name);
   
   bool loadVertexShaderFromFile(const char* vertPath);
   bool loadFragmentShaderFromFile(const char* fragPath);
   
   virtual bool loadShaderFromFiles(const char* vertPath, const char* fragPath);
        
   virtual void reset();
   virtual void resurrect();
   virtual void kill();
   
   virtual void bind();
   virtual void unbind();
   
};

/// The GLSL shader class
class GLSLShader : public Shader
{
   friend class _ShaderManager;
   private:

   GLSLShader();

   GLhandleARB mProgram;
   GLhandleARB mFragmentShader;
   GLhandleARB mVertexShader;
   
   virtual bool setParameter1f(StringTableEntry name, F32 value0);
   virtual bool setParameter2f(StringTableEntry name, F32 value0, F32 value1);
   virtual bool setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2);
   virtual bool setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3);
    
   virtual bool setParameter1i(StringTableEntry name, U32 value0);
   virtual bool setParameter2i(StringTableEntry name, U32 value0, U32 value1);
   virtual bool setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2);
   virtual bool setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3);
   
   virtual bool setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m);
   
   virtual bool setTextureSampler(StringTableEntry name, U32 value0);
    
   virtual bool loadShaderFromFiles(const char* vertPath, const char* fragPath);
   bool loadVertexShaderFromFile(const char* vertPath);
   bool loadFragmentShaderFromFile(const char* fragPath);
   virtual bool compileShaderFromSource(const char* vertSource, const char* fragSource, bool includeLibs, bool link);
   
   virtual GLSLShaderParameter* getNamedParameter(StringTableEntry name);
   
   Vector<GLSLShaderParameter*> mParameters;
   
   virtual void reset();
   virtual void resurrect();
   virtual void kill();
   
   virtual void bind();
   virtual void unbind();
   
public:
   virtual const char* getProgramLog();
   virtual const char* getVertexLog();
   virtual const char* getFragmentLog();
   
   virtual void updateStatus();
   virtual void reloadParameters();
};

/// The ATI_text_fragment_shader shader class.
/// Ironically, this is the cleanest class.  Most of it was written
/// after I reworked how shaders should be loaded.
class ATITShader : public Shader
{
   friend class _ShaderManager;
private:

   ATITShader();

   GLuint mFragmentProgram;
   GLuint mVertexProgram;
   
   virtual bool setParameter1f(StringTableEntry name, F32 value0);
   virtual bool setParameter2f(StringTableEntry name, F32 value0, F32 value1);
   virtual bool setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2);
   virtual bool setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3);
    
   virtual bool setParameter1i(StringTableEntry name, U32 value0);
   virtual bool setParameter2i(StringTableEntry name, U32 value0, U32 value1);
   virtual bool setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2);
   virtual bool setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3);
   
   virtual bool setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m);
   
   virtual bool setTextureSampler(StringTableEntry name, U32 value0);
       
   bool loadVertexShaderFromFile(const char* vertPath);
   bool loadFragmentShaderFromFile(const char* fragPath);
   virtual bool loadShaderFromFiles(const char* vertPath, const char* fragPath);
   
   virtual ASMShaderParameter* getNamedParameter(StringTableEntry name);
   
   Vector<ASMShaderParameter*> mParameters;
   
   virtual void reset();
   virtual void resurrect();
   virtual void kill();
   
   virtual void bind();
   virtual void unbind();
};

/// A global 4x4 matrix parameter
class Global4x4f
{
   public:

   MatrixF m;
   bool mTranspose;
   StringTableEntry mName;
};

/// A global scalar parameter
class Global1f
{
   public:

   F32 value0;
   StringTableEntry mName;
   
   F32 minX, maxX;
   
   bool mShouldSave;
   
   void save(FileStream &fStream);
};

/// A global 2f parameter (float2, vec2)
class Global2f
{
   public:

   F32 value0, value1;
   StringTableEntry mName;
   
   F32 minX, maxX, minY, maxY;
   
   bool mShouldSave;
   
   void save(FileStream &fStream);
};

/// A global 3f parameter (float3, vec3)
class Global3f
{
   public:

   F32 value0, value1, value2;
   StringTableEntry mName;
   
   F32 minX, maxX, minY, maxY, minZ, maxZ;
   
   bool mShouldSave;
   
   void save(FileStream &fStream);
};

/// A global 4f parameter (float4, vec4)
class Global4f
{
   public:

   F32 value0, value1, value2, value3;
   StringTableEntry mName;
   
   F32 minX, maxX, minY, maxY, minZ, maxZ, minW, maxW;
   
   bool mShouldSave;
   
   void save(FileStream &fStream);
};

/// The ShaderManager is by far the oldest piece of the Modernization Kit as it exists today, dating
/// back to July, 2006 (Most of the MK was created in December of 2006).
///
/// The ShaderManager is responsible for handling all things relating to shaders (gee, ya think?)
/// which includes creating them, binding them, and setting parameters.
/// In addition to these routine functions, the ShaderManager also handles
/// global parameters, which apply to all shaders, and GLSL libraries.
/// 
/// The ShaderManager is only loosely aware of the concrete shader classes.
/// The only time the concrete classes are referenced is for shader creation (duh)
/// Beyond that all interaction occurs through the abstract base class Shader
/// just like in the rest of Torque.
///
/// Ya, the ShaderManager is pretty boring really.  It provides a nice common
/// interface for all things shader related, but that's about it.  Given that
/// the modernization kit supports four kinds of shaders though (eight if you count the
/// various versions of Cg) this isn't exactly a trivial task.
class _ShaderManager
{
   friend class CGShader;
   public:
   
   static void create();
   static void destroy();
   
   F32 getPixelShaderVersion() { return mPixelShaderVersion; };
   void forcePixelShaderVersion(F32 version) { mPixelShaderVersion = mClampF(version, 0.0f, 3.0f); };
   
   F32 getVertexShaderVersion() { return mVertexShaderVersion; };
   void forceVertexShaderVersion(F32 version) { mVertexShaderVersion = mClampF(version, 0.0f, 3.0f); };
   
   /// Forces creation of the specified type of shader
   /// This is the only method used to create shaders in the MK
   Shader* createShader(const char* vertexPath, const char* fragPath, ShaderType type);
   
   /// calls shader::compileShaderFromSource
   /// Used by the shader IDE
   Shader* compileShaderFromSource(const char* vertSource, const char* fragSource, 
                                   bool includeLibs, bool link, ShaderType type);
  
   /// Deletes shd, removes it from our list of shaders.
   void deleteShader(Shader* shd);
   
   /// kills all shaders, libraries, and global parameters in the shader manager
   void kill();
   
   /// Calls reset on all loaded shaders
   void reset();
   
   /// calls resurrect on all loaded shaders
   void resurrect();
   
   /// All global 4x4 parameters
   Vector<Global4x4f> mGlobal4x4fParameters;
   
   /// All global 1f parameters
   Vector<Global1f> mGlobal1fParameters;
   
   /// All global 2f parameters
   Vector<Global2f> mGlobal2fParameters;
   
   /// All global 3f parameters
   Vector<Global3f> mGlobal3fParameters;
   
   /// All global 4f parameters
   Vector<Global4f> mGlobal4fParameters;
   
   /// Validates the GLSL implementation
   /// This really shouldn't be part of the shader manager...
   void validateGLSL();
   
   /// Calls mCurrBound->unbind()
   void unbindShader();
   
   /// Binds shd and sets all global parameters
   void bindShader(Shader* shd);
   
   /// Checks if a shader is bound
   bool isShaderBound() {return mCurrBound != NULL; }
   
   /// Do we support shaders of any kind?
   bool doesSupportShaders();
   
   /// @name Parameter setting
   /// Function overloading is an awesome awesome thing.
   /// @{
   bool setParameter(StringTableEntry name, F32 value0);
   bool setParameter(StringTableEntry name, F32 value0, F32 value1);
   bool setParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2);
   bool setParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3);
    
   bool setParameter(StringTableEntry name, U32 value0);
   bool setParameter(StringTableEntry name, U32 value0, U32 value1);
   bool setParameter(StringTableEntry name, U32 value0, U32 value1, U32 value2);
   bool setParameter(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3);
   
   bool setParameter(StringTableEntry name, bool transpose, MatrixF *m);
   bool setTextureSampler(StringTableEntry name, U32 value0);
   
   /// @}
   
   /// @name Global parameter setting
   /// These are parameters which apply to ALL shaders, not just the currently bound one.
   /// @{
   void setGlobalParameter(StringTableEntry name, bool transpose, MatrixF m);
   void setGlobalParameter(StringTableEntry name, F32 value0, bool save = true);
   void setGlobalParameter(StringTableEntry name, F32 value0, F32 value1, bool save = true);
   void setGlobalParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2, bool save = true);
   void setGlobalParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3, bool save = true);
   
   void setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX);
   void setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX, F32 minY, F32 maxY);
   void setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX, F32 minY, F32 maxY, F32 minZ, F32 maxZ);
   void setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX, F32 minY, F32 maxY, 
                                                        F32 minZ, F32 maxZ, F32 minW, F32 maxW);
   /// @}
   
   /// Deletes a global parameter (after 0'ing it on all shaders)
   /// @note This is SLOW!
   void deleteGlobalParameter(StringTableEntry name);
   
   /// reloads all shaders (from their files)
   void reloadShaders();
   
   /// Saves global parameters to a script file
   void exportGlobalParameters();
   
   /// @name Support checking
   /// These functions check support for the specified shader type
   /// @{
   void getGLSLSupport(bool &vert, bool &frag)  {  vert = mSupportsGLSLVert;   frag = mSupportsGLSLPixl;  };
   void getCgSupport(bool &vert, bool &frag)    {  vert = mSupportsCgVert;     frag = mSupportsCgPixl;    };
   void getASMSupport(bool &vert, bool &frag)   {  vert = mSupportsASMVert;    frag = mSupportsASMPixl;   };
   void getATITSupport(bool &vert, bool &frag)  {  vert = mSupportsATITVert;   frag = mSupportsATITPixl;  };
   /// @}
   
   /// @name Force shader support
   /// Force to false, not to true.  Forcing to true = crash!
   /// Primarily used by unit tests
   /// @{
   void forceGLSLSupport(bool vert, bool frag)  {  mSupportsGLSLVert = vert;  mSupportsGLSLPixl = frag;  };
   void forceCgSupport(bool vert, bool frag)    {  mSupportsCgVert = vert;    mSupportsCgPixl = frag;    };
   void forceASMSupport(bool vert, bool frag)   {  mSupportsASMVert = vert;   mSupportsASMPixl = frag;   };
   void forceATITSupport(bool vert, bool frag)  {  mSupportsATITVert = vert;  mSupportsATITPixl = frag;  };
   /// @}
   
   private:
   
   /// D3D vertex shader version
   F32 mVertexShaderVersion;
   
   /// D3D pixel shader version
   F32 mPixelShaderVersion;
   
   /// Need to look up what this does
   bool mGLSLValidate;
   
   /// Need to look up what this does
   GLhandleARB mValidateVertex;
   GLhandleARB mValidateFragment;
   
   /// Confirms that we can load the specified combination of shaders of the given type
   ///
   /// @param vertShader         bool indicating if we are loading a vertex shader
   /// @param fragShader         bool indication if we are loading a fragment shader
   /// @param type               The type of shader we are loading
   ///
   /// @return                   bool indicating if the user's GPU can load these shaders
   bool confirmSupport(bool vertShader, bool fragShader, ShaderType type);
   
   /// Deprecated
   F32 mDefaultParamValue;
   
   /// @name shader support bools
   /// @{
   bool mSupportsGLSLPixl;
   bool mSupportsASMPixl;
   bool mSupportsCgPixl;
   bool mSupportsATITPixl;
   
   bool mSupportsGLSLVert;
   bool mSupportsASMVert;
   bool mSupportsCgVert;
   bool mSupportsATITVert;
   /// @}
   
#ifdef SHADER_MANAGER_USE_CG

   /// @name Cg specific stuff.
   /// @{
   CGcontext mCGContext;
   CGprofile mCGFragmentProfile, mCGVertexProfile;
   bool mCGInit;

   void initCG();
   /// @}
#endif
   
   /// Initializes the shader manager, checks shader support, etc.
   void init();
   
   _ShaderManager();
   ~_ShaderManager();
   
   /// All of our current shaders
   Vector<Shader*> mShaders;
   
   /// Our currently bound shader
   Shader* mCurrBound;
   
   /// Compiles a specific validation shader
   bool validateGLSLShader(StringTableEntry file, bool vertex);
};

/// ShaderManager global pointer
extern _ShaderManager *ShaderManager;
    
#endif

