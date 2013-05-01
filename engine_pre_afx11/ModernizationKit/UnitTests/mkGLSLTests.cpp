/*  mkGLSLTests.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/UnitTests/mkGLSLTests.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ModernizationKit/mkShaderManager.h"

bool MKGLSLShaderTests::mkRunTests()
{
   printf("Running GLSL unit tests\n");
   CppUnit::TextTestRunner runner;
   runner.addTest( MKGLSLShaderTests::suite() );
   bool wasSuccessful = runner.run( "", false );
   return wasSuccessful;
}

void MKGLSLShaderTests::setUp()
{
   // Some fluff shaders in the shader manager's list, just to mix things up a bit
   fluff0 = ShaderManager->createShader("Shaders/UnitTests/fluffV.glsl", "Shaders/UnitTests/fluffP.glsl", GLSLShaderType);
   fluff1 = ShaderManager->createShader("Shaders/UnitTests/fluffV.glsl", NULL, GLSLShaderType);
   fluff2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/fluffP.glsl", GLSLShaderType);
}

void MKGLSLShaderTests::tearDown()
{
   ShaderManager->deleteShader(fluff0);
   ShaderManager->deleteShader(fluff1);
   ShaderManager->deleteShader(fluff2);
}

void MKGLSLShaderTests::testVFLoad()
{
   printf("Testing basic shader loading...");

   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == GLSLShaderType);
   
   // Ensure it really is a GLSL shader
   GLSLShader* glsl = dynamic_cast<GLSLShader*>(test);
   CPPUNIT_ASSERT(glsl != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testVLoad()
{
   printf("Testing vertex only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == GLSLShaderType);
   
   // Ensure it really is a GLSL shader
   GLSLShader* glsl = dynamic_cast<GLSLShader*>(test);
   CPPUNIT_ASSERT(glsl != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testFLoad()
{
   printf("Testing fragment only loading...");
   // Ensure the shader is created
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   // Ensure type is properly set
   ShaderType type = test->getType();
   CPPUNIT_ASSERT(type == GLSLShaderType);
   
   // Ensure it really is a GLSL shader
   GLSLShader* glsl = dynamic_cast<GLSLShader*>(test);
   CPPUNIT_ASSERT(glsl != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testVFCacheLoad()
{
   printf("Testing loading a cached shader with vertex and fragment files...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testVCacheLoad()
{
   printf("Testing loading a cached shader with a vertex file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the fragment shader, see if things change
   test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testFCacheLoad()
{
   printf("Testing loading a cached shader with a fragment file...");
   // Load the same shader twice.  The shader manager should cache the first shader, and return the same pointer for the second.
   Shader* test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   Shader* test2 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test == test2);
   
   // Load vertex and fragment, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Just load the vertex shader, see if things change
   test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   CPPUNIT_ASSERT(test != test2);
   ShaderManager->deleteShader(test2);
   
   // Cleanup
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testFilepathReturn()
{ 
   printf("Testing that filepaths are properly set...");
   // Make sure the returned filepaths are accurate
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   U32 vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   fr = dStrcmp(fpath, "Shaders/UnitTests/basicTestP.glsl");
   
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   vr = dStrcmp(vpath, "Shaders/UnitTests/basicTestV.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fpath == NULL);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testInvalidVFLoad()
{
   printf("Testing loading of nonexistant vertex and fragment files...");
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.glsl", "Shaders/UnitTests/DoNotCreateP.glsl", GLSLShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testInvalidFLoad()
{
   printf("Testing loading of nonexistant fragment file...");
   // Vertex shader is valid, pixel shader is invalid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/DoNotCreateP.glsl", GLSLShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testInvalidVLoad()
{
   printf("Testing loading of nonexistant vertex file...");
   // Vertex shader is invalid, pixel shader is valid
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/DoNotCreateV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   
   // Now then, a shader should load, but it should be the error shader.  Let's check all that, m'kay?
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testNoVFSupportLoad()
{
   printf("Testing loading GLSL with no GLSL support...");
   // Force no GLSL support
   ShaderManager->forceGLSLSupport(false, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->forceGLSLSupport(true, true);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testNoVSupportLoad()
{
   printf("Testing loading GLSL with no vertex shader support...");
   // Force no GLSLV support
   ShaderManager->forceGLSLSupport(false, true);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 == NULL);
   CPPUNIT_ASSERT(test3 != NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test3);
   ShaderManager->forceGLSLSupport(true, true);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testNoFSupportLoad()
{
   printf("Testing loading GLSL with no fragment shader support...");
   // Force no GLSLF support
   ShaderManager->forceGLSLSupport(true, false);
   
   // Attempt to load a shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   Shader* test2 = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", NULL, GLSLShaderType);
   Shader* test3 = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   
   // test should be NULL
   CPPUNIT_ASSERT(test == NULL);
   CPPUNIT_ASSERT(test2 != NULL);
   CPPUNIT_ASSERT(test3 == NULL);
   
   // Cleanup
   ShaderManager->deleteShader(test2);
   ShaderManager->forceGLSLSupport(true, true);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testVError()
{
   printf("Testing loading a vertex shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.glsl", "Shaders/UnitTests/basicTestP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the vertex error shader!!!
   test = ShaderManager->createShader("Shaders/UnitTests/basicErrorV.glsl", NULL, GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the vertex path!
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath == NULL);
   
   // Check that they are, in fact, the error shader
   vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testFError()
{
   printf("Testing loading a fragment shader which generates a compilation error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/basicTestV.glsl", "Shaders/UnitTests/basicErrorP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   
   // This should load *ONLY* the fragment error shader!
   test = ShaderManager->createShader(NULL, "Shaders/UnitTests/basicErrorP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   vpath = test->getVertexFilepath();
   fpath = test->getFragmentFilepath();
   
   // Ensure that we have only the fragment path
   CPPUNIT_ASSERT(vpath == NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}

void MKGLSLShaderTests::testVFError()
{
   printf("Testing loading a shader which generates a linker error...");
   // This should load the error shader
   Shader* test = ShaderManager->createShader("Shaders/UnitTests/progErrorV.glsl", "Shaders/UnitTests/progErrorP.glsl", GLSLShaderType);
   CPPUNIT_ASSERT(test != NULL);
   
   const char* vpath = test->getVertexFilepath();
   const char* fpath = test->getFragmentFilepath();
   
   // Ensure that we have both of those
   CPPUNIT_ASSERT(vpath != NULL);
   CPPUNIT_ASSERT(fpath != NULL);
   
   // Check that they are, in fact, the error shader
   U32 vr = dStrcmp(vpath, "Shaders/Debug/errorVert.glsl");
   U32 fr = dStrcmp(fpath, "Shaders/Debug/errorPixl.glsl");
   
   CPPUNIT_ASSERT(vr == 0);
   CPPUNIT_ASSERT(fr == 0);
   
   ShaderManager->deleteShader(test);
   printf("PASS!\n");
}
