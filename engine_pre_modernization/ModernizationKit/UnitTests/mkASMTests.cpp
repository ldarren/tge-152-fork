/*  mkASMTests.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/UnitTests/mkASMTests.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ModernizationKit/mkShaderManager.h"

bool MKASMShaderTests::mkRunTests()
{
   printf("Running ASM unit tests\n");
   CppUnit::TextTestRunner runner;
   runner.addTest( MKASMShaderTests::suite() );
   bool wasSuccessful = runner.run( "", false );
   return wasSuccessful;
}

void MKASMShaderTests::setUp()
{
   // Some fluff shaders in the shader manager's list, just to mix things up a bit
   fluff0 = ShaderManager->createShader("Shaders/UnitTests/fluffV.arb", "Shaders/UnitTests/fluffP.arb", ASMShaderType);
   fluff1 = ShaderManager->createShader("Shaders/UnitTests/fluffV.arb", NULL, ASMShaderType);
   fluff2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/fluffP.arb", ASMShaderType);
}

void MKASMShaderTests::tearDown()
{
   ShaderManager->deleteShader(fluff0);
   ShaderManager->deleteShader(fluff1);
   ShaderManager->deleteShader(fluff2);
}

void MKASMShaderTests::testVFLoad()
{
   printf("Testing basic shader loading...");

   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == ASMShaderType);
   
   // Ensure it really is a GLSL shader
   ASMShader* asmShd = dynamic_cast<ASMShader*>(test);
   CPPUNIT_ASSERT(asmShd != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testVLoad()
{
   printf("Testing vertex only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == ASMShaderType);
   
   // Ensure it really is a GLSL shader
   ASMShader* asmShd = dynamic_cast<ASMShader*>(test);
   CPPUNIT_ASSERT(asmShd != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testFLoad()
{
   printf("Testing fragment only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == ASMShaderType);
   
   // Ensure it really is a GLSL shader
   ASMShader* asmShd = dynamic_cast<ASMShader*>(test);
   CPPUNIT_ASSERT(asmShd != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testVFCacheLoad()
{
   printf("Testing loading a cached shader with vertex and fragment files...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testVCacheLoad()
{
   printf("Testing loading a cached shader with a vertex file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testFCacheLoad()
{
   printf("Testing loading a cached shader with a fragment file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   Shader* test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testFilepathReturn()
{ 
   printf("Testing that filepaths are properly set...");
   // Make sure the returned filepaths are accurate
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   U32 vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.arb");
   U32 fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.arb");
   
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fpath == NULL);
   printf("PASS!\n");
}

void MKASMShaderTests::testInvalidVFLoad()
{
   printf("Testing loading of nonexistant vertex and fragment files...");
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.arb", "Shaders/UnitTests/DoNotCreateP.arb", ASMShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testInvalidFLoad()
{
   printf("Testing loading of nonexistant fragment file...");
   // Vertex shader is valid, pixel shader is invalid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/DoNotCreateP.arb", ASMShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testInvalidVLoad()
{
   printf("Testing loading of nonexistant vertex file...");
   // Vertex shader is invalid, pixel shader is valid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testNoVFSupportLoad()
{
   printf("Testing loading ARB_*_program with no ARB_*_program support...");
   // Force no GLSL support
   ShaderManager->forceASMSupport(false, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->forceASMSupport(true, true);
   printf("PASS!\n");
}

void MKASMShaderTests::testNoVSupportLoad()
{
   printf("Testing loading ARB_*_program with no vertex shader support...");
   // Force no GLSLV support
   ShaderManager->forceASMSupport(false, true);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test3);
   ShaderManager->forceASMSupport(true, true);
   printf("PASS!\n");
}

void MKASMShaderTests::testNoFSupportLoad()
{
   printf("Testing loading ARB_*_program with no fragment shader support...");
   // Force no GLSLF support
   ShaderManager->forceASMSupport(true, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ASMShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 != NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test2);
   ShaderManager->forceASMSupport(true, true);
   printf("PASS!\n");
}

void MKASMShaderTests::testVError()
{
   printf("Testing loading a vertex shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.arb", "Shaders/UnitTests/basicTestP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the vertex error shader!!!
   test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.arb", NULL, ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the vertex path!
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath == NULL);
   
   // Check that they are, in fact, the error shader
   vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testFError()
{
   printf("Testing loading a fragment shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicErrorP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the fragment error shader!
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicErrorP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the fragment path
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKASMShaderTests::testVFError()
{
   printf("Testing loading a shader which generates a linker error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.arb", "Shaders/UnitTests/basicErrorP.arb", ASMShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}


