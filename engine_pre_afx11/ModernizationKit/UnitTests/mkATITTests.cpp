/*  mkATITTests.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/UnitTests/mkATITTests.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ModernizationKit/mkShaderManager.h"

bool MKATITShaderTests::mkRunTests()
{
   printf("Running ATIT unit tests\n");
   CppUnit::TextTestRunner runner;
   runner.addTest( MKATITShaderTests::suite() );
   bool wasSuccessful = runner.run( "", false );
   return wasSuccessful;
}

void MKATITShaderTests::setUp()
{
   // Some fluff shaders in the shader manager's list, just to mix things up a bit
   fluff0 = ShaderManager->createShader("Shaders/UnitTests/fluffV.arb", "Shaders/UnitTests/fluffP.atit", ATITextShaderType);
   fluff1 = ShaderManager->createShader("Shaders/UnitTests/fluffV.arb", NULL, ATITextShaderType);
   fluff2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/fluffP.atit", ATITextShaderType);
}

void MKATITShaderTests::tearDown()
{
   ShaderManager->deleteShader(fluff0);
   ShaderManager->deleteShader(fluff1);
   ShaderManager->deleteShader(fluff2);
}

void MKATITShaderTests::testVFLoad()
{
   printf("Testing basic shader loading...");

   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == ATITextShaderType);
   
   // Ensure it really is a ATIT shader
   ATITShader* atitShd = dynamic_cast<ATITShader*>(test);
   CPPUNIT_ASSERT(atitShd != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testVLoad()
{
   printf("Testing vertex only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == ATITextShaderType);
   
   // Ensure it really is a GLSL shader
   ATITShader* atitShd = dynamic_cast<ATITShader*>(test);
   CPPUNIT_ASSERT(atitShd != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testFLoad()
{
   printf("Testing fragment only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == ATITextShaderType);
   
   // Ensure it really is a GLSL shader
   ATITShader* atitShd = dynamic_cast<ATITShader*>(test);
   CPPUNIT_ASSERT(atitShd != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testVFCacheLoad()
{
   printf("Testing loading a cached shader with vertex and fragment files...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testVCacheLoad()
{
   printf("Testing loading a cached shader with a vertex file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testFCacheLoad()
{
   printf("Testing loading a cached shader with a fragment file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   Shader* test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testFilepathReturn()
{ 
   printf("Testing that filepaths are properly set...");
   // Make sure the returned filepaths are accurate
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   U32 vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.arb");
   U32 fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.atit");
   
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.arb");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fpath == NULL);
   printf("PASS!\n");
}

void MKATITShaderTests::testInvalidVFLoad()
{
   printf("Testing loading of nonexistant vertex and fragment files...");
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.arb", "Shaders/UnitTests/DoNotCreateP.atit", ATITextShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testInvalidFLoad()
{
   printf("Testing loading of nonexistant fragment file...");
   // Vertex shader is valid, pixel shader is invalid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/DoNotCreateP.atit", ATITextShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testInvalidVLoad()
{
   printf("Testing loading of nonexistant vertex file...");
   // Vertex shader is invalid, pixel shader is valid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testNoVFSupportLoad()
{
   printf("Testing loading ATI_text_fragment_shader with no ATI_text_fragment_shader support...");
   // Force no ATIT support
   ShaderManager->forceATITSupport(false, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->forceATITSupport(true, true);
   printf("PASS!\n");
}

void MKATITShaderTests::testNoVSupportLoad()
{
   printf("Testing loading ATI_text_fragment_shader with no vertex shader support...");
   // Force no ATITV support
   ShaderManager->forceATITSupport(false, true);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test3);
   ShaderManager->forceATITSupport(true, true);
   printf("PASS!\n");
}

void MKATITShaderTests::testNoFSupportLoad()
{
   printf("Testing loading ATI_text_fragment_shader with no fragment shader support...");
   // Force no ATITF support
   ShaderManager->forceATITSupport(true, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", NULL, ATITextShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 != NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test2);
   ShaderManager->forceATITSupport(true, true);
   printf("PASS!\n");
}

void MKATITShaderTests::testVError()
{
   printf("Testing loading a vertex shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.arb", "Shaders/UnitTests/basicTestP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the vertex error shader!!!
   test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.arb", NULL, ATITextShaderType);
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

void MKATITShaderTests::testFError()
{
   printf("Testing loading a fragment shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.arb", "Shaders/UnitTests/basicErrorP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the fragment error shader!
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicErrorP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the fragment path
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKATITShaderTests::testVFError()
{
   printf("Testing loading a shader which generates a linker error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.arb", "Shaders/UnitTests/basicErrorP.atit", ATITextShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.arb");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.atit");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}


