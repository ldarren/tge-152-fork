/*  mkMaterial.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#ifndef _MKMATERIAL_H_
#define _MKMATERIAL_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#ifndef _MKSHADERMANAGER_H_
#include "ModernizationKit/mkShaderManager.h"
#endif

#define MAX_SHADERS 10
class TextureHandle;

//-----------------------------------------------------------------

/// @defgroup mk_material Modernization Kit Material System
/// The Modernization Kit's material system, while simple, is also rather powerful (in my opinion at least).
///
/// OpenGL shaders are, as many of you know, a touchy area.  Certain extensions only work
/// on certain cards, and some shaders are better on some cards than others.
/// For example, while a Radeon 9600 can run an ATI_text_fragment_shader shader
/// it greatly prefers an ARB_fragment_program or GLSL shader.
///
/// To ensure that the best shader is always selected for a given card, based on
/// available shaders and the user set quality level, the material system uses this SubShader
/// class.  The SubShader class is responsible for loading the proper shader based on
/// the user set quality level, and informing the material of what kind of shader it is.
/// While up to ten distinct shaders may be loaded into a subshader, they must all be of the same type.
/// i.e. you cannot mix Cg and GLSL shaders within a subshader.
///
/// The Material class contains five SubShaders.  One for 1.1, 1.4, and 3.0 cards, and two for 2.0 cards.
/// @note While OpenGL does not have shader model numbers, the ShaderManager determines a D3D shader model number
///       based on supported extensions.
///
/// It is expected that 1.1 will be a Cg shader, 1.4 an ATI_text_fragment_shader, and 3.0 a GLSL shader.
/// There are two 2.0 shaders primarily to support the Intel GMA 9xx series (which can load ARB_*_program
/// but not GLSL) but it can be used for another purpose.  The GeForce FX series, while supporting GLSL, does
/// not run very well when using GLSL shaders.  Thus, you may specify a Cg, ARB_*_program, or specialized GLSL
/// shader for nVidia GPUs, and indicate to the material manager that it should prefer the alternate 2.0 shader
/// over the standard one.  Through this you can achieve optimal performance on any GPU.
///
/// While the material system can load a 1.4 shader on a given 2.0 card, or a 1.1 shader on another
/// this is in general not advised for the reasons given above.  ALWAYS have a 2.0 shader, the others are optional.

/// The SubShader class stores all the shaders of a given shader type for a material.
/// Because of this design, quality fallbacks apply to each shader model, allowing lower quality 3.0 shaders
/// before falling back to 2.0, for example.  Try to avoid having 2.0 shaders fall back to 1.4 or 1.1.
///
/// @note   SubShader is only a helper class for Material.  Beyond Material, no MK or Torque object is aware of its
///         existance
class SubShader : public SimObject
{
private:
   typedef SimObject Parent;
   friend class ConcreteClassRep<SubShader>;
   
protected:
   SubShader();
   ~SubShader();

public:
   /// Currently does nothing.
   bool onAdd();
   
   StringTableEntry mVertPaths[MAX_SHADERS]; ///< Filepaths for vertex shaders
   StringTableEntry mFragPaths[MAX_SHADERS]; ///< Filepaths for fragment (pixel) shaders
   U32 mShaderQualities[MAX_SHADERS]; ///< Quality level at which the shader can be used
   ShaderType mShaderType; ///< Type of shader
   
   static void initPersistFields();
   
   /// Loads proper shader based on quality level
   Shader* load(); 
   
   DECLARE_CONOBJECT( SubShader );
};

#define MAX_MATERIAL_SAMPLERS 16

/// The Material class contains SubShader instances for every major shader version
/// It is Torque Script's primary link to shaders, and materials are referenced in C++
/// to bind the proper shader.
class Material : public SimObject
{
protected:
   typedef SimObject Parent;
   /// Seriously, don't make a Material in C++.  Script only, okay?
   /// The constructor is protected
   friend class ConcreteClassRep<Material>;

   Material();
   ~Material();
public:   
   static void initPersistFields();
   
   /// Loads proper shader based on version number
   bool onAdd(); 
   
   /// Binds active shader and sets all material parameters
   bool bind(); 
   
   /// Reloads shader in the event of a change in shader quality (or the user rips out and replaces their GPU)
   void reload(); 
   
   /// Which samplers are bound to which names (For GLSL)
   StringTableEntry mTextureNames[MAX_MATERIAL_SAMPLERS];
   StringTableEntry mSamplerBindings[MAX_MATERIAL_SAMPLERS]; 
   StringTableEntry mMapToTexture;
   
   StringTableEntry mName; ///< Our name (should be depricated)
   
   SubShader* m11SubShader; ///< SubShader for 1.1 cards
   SubShader* m14SubShader; ///< SubShader for 1.4 cards
   SubShader* m20SubShader; ///< SubShader for 2.0 cards
   SubShader* m20aSubShader; ///< Alternate SubShader for 2.0 cards (in case the card/driver sucks)
   SubShader* m30SubShader; ///< SubShader for 3.0 cards
   
   /// Returns the shader which was loaded based on what the users GPU supports and the
   /// user set quality level.
   Shader* getActiveShader() { return mActiveShader; };
   
   U32 getLightmapTextureUnit();
   U32 getFogTextureUnit();
   
   private:
   Shader *mActiveShader; ///< Which shader we settled on loading.
   TextureHandle*    mTextures[MAX_MATERIAL_SAMPLERS]; ///< Our textures.
   
   public:
   DECLARE_CONOBJECT(Material);
};

/// The _MaterialManager, well, manages materials.  It also handles the shader quality level stuff.
class _MaterialManager
{
   friend class Material;
   public:
   static void create(); ///< Creates MaterialManager
   static void destroy(); ///< Delets MaterialManager
   
   /// Sets shader quality
   ///
   /// @param quality      New shader quality
   void setShaderQuality(U32 quality) { mShaderQuality = quality; };
   U32 getShaderQuality() { return mShaderQuality; }; ///< Returns current shader quality
   
   void reloadMaterials(); ///< Calls reload on all materials
   Shader* findShaderByMaterial(StringTableEntry name); ///< Returns a shader based on the material name
   Material* findMaterialByName(StringTableEntry name); ///< returns a material based on its name
   Material* findMaterialByMapToTexture(StringTableEntry tex); ///< Returns a material based on the texture it's mapped to.
   
   private:
   _MaterialManager();  ///< Private constructor so there's only one of us
   ~_MaterialManager(); ///< Private destructor so the programmer doesn't randomly delete us.
   
   /// Registers a material with the material manager
   ///
   /// @param mat The Material to register
   void registerMaterial(Material* mat); 
   Vector<Material*> mMaterials; ///< An array of all of the registered materials
   U32 mShaderQuality; ///< Current shader quality
};

DefineConsoleType( TypeSubShaderPtr ) /// HACK so SubShaders aren't datablocks

extern _MaterialManager *MaterialManager;

#endif
