/*  mkCgTests.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/UnitTests/mkCgTests.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ModernizationKit/mkShaderManager.h"

bool MKCgShaderTests::mkRunTests()
{
   printf("Running Cg unit tests\n");
   CppUnit::TextTestRunner runner;
   runner.addTest( MKCgShaderTests::suite() );
   bool wasSuccessful = runner.run( "", false );
   return wasSuccessful;
}

void MKCgShaderTests::setUp()
{
   // Some fluff shaders in the shader manager's list, just to mix things up a bit
   fluff0 = ShaderManager->createShader("Shaders/UnitTests/fluffV.cg", "Shaders/UnitTests/fluffP.cg", CGShaderType);
   fluff1 = ShaderManager->createShader("Shaders/UnitTests/fluffV.cg", NULL, CGShaderType);
   fluff2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/fluffP.cg", CGShaderType);
}

void MKCgShaderTests::tearDown()
{
   ShaderManager->deleteShader(fluff0);
   ShaderManager->deleteShader(fluff1);
   ShaderManager->deleteShader(fluff2);
}

void MKCgShaderTests::testVFLoad()
{
   printf("Testing basic shader loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == CGShaderType);
   
   // Ensure it really is a Cg shader
   CGShader* cg = dynamic_cast<CGShader*>(test);
   CPPUNIT_ASSERT(cg != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testVLoad()
{
   printf("Testing vertex only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == CGShaderType);
   
   // Ensure it really is a Cg shader
   CGShader* cg = dynamic_cast<CGShader*>(test);
   CPPUNIT_ASSERT(cg != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testFLoad()
{
   printf("Testing fragment only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == CGShaderType);
   
   // Ensure it really is a Cg shader
   CGShader* cg = dynamic_cast<CGShader*>(test);
   CPPUNIT_ASSERT(cg != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testVFCacheLoad()
{
   printf("Testing loading a cached shader with vertex and fragment files...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testVCacheLoad()
{
   printf("Testing loading a cached shader with a vertex file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testFCacheLoad()
{  
   printf("Testing loading a cached shader with a fragment file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   Shader* test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testFilepathReturn()
{
   printf("Testing that filepaths are properly set...");
   // Make sure the returned filepaths are accurate
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   U32 vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.cg");
   U32 fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.cg");
   
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fpath == NULL);
   printf("PASS!\n");
}

void MKCgShaderTests::testInvalidVFLoad()
{
   printf("Testing loading of nonexistant vertex and fragment files...");
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.cg", "Shaders/UnitTests/DoNotCreateP.cg", CGShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testInvalidFLoad()
{
   printf("Testing loading of nonexistant fragment file...");
   // Vertex shader is valid, pixel shader is invalid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/DoNotCreateP.cg", CGShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testInvalidVLoad()
{
   printf("Testing loading of nonexistant vertex file...");
   // Vertex shader is invalid, pixel shader is valid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testNoVFSupportLoad()
{
   printf("Testing loading Cg with no Cg support...");
   // Force no Cg support
   ShaderManager->forceCgSupport(false, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->forceCgSupport(true, true);
   printf("PASS!\n");
}

void MKCgShaderTests::testNoVSupportLoad()
{
   printf("Testing loading Cg with no vertex shader support...");
   // Force no CgV support
   ShaderManager->forceCgSupport(false, true);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test3);
   ShaderManager->forceCgSupport(true, true);
   printf("PASS!\n");
}

void MKCgShaderTests::testNoFSupportLoad()
{
   printf("Testing loading Cg with no fragment shader support...");
   // Force no CgF support
   ShaderManager->forceCgSupport(true, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", NULL, CGShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 != NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test2);
   ShaderManager->forceCgSupport(true, true);
   printf("PASS!\n");
}

void MKCgShaderTests::testVError()
{
   printf("Testing loading a vertex shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.cg", "Shaders/UnitTests/basicTestP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the vertex error shader!!!
   test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.cg", NULL, CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the vertex path!
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath == NULL);
   
   // Check that they are, in fact, the error shader
   vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testFError()
{
   printf("Testing loading a fragment shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.cg", "Shaders/UnitTests/basicErrorP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the fragment error shader!
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicErrorP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the fragment path
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKCgShaderTests::testVFError()
{
   printf("Testing loading a shader which generates a linker error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.cg", "Shaders/UnitTests/basicErrorP.cg", CGShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.cg");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.cg");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}


