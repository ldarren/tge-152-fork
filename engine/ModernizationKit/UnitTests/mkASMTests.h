/*  mkASMTests.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#ifndef _MKASMTESTS_H_
#define _MKASMTESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class Shader;

class MKASMShaderTests : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( MKASMShaderTests );
   
   CPPUNIT_TEST( testVFLoad );
   CPPUNIT_TEST( testVLoad );
   CPPUNIT_TEST( testFLoad );
   CPPUNIT_TEST( testVFCacheLoad );
   CPPUNIT_TEST( testVCacheLoad );
   CPPUNIT_TEST( testFCacheLoad );
   
   CPPUNIT_TEST( testFilepathReturn );
   
   CPPUNIT_TEST( testInvalidVFLoad );
   CPPUNIT_TEST( testInvalidFLoad );
   CPPUNIT_TEST( testInvalidVLoad );
   
   CPPUNIT_TEST( testNoVFSupportLoad );
   CPPUNIT_TEST( testNoVSupportLoad );
   CPPUNIT_TEST( testNoFSupportLoad );
   
   CPPUNIT_TEST( testVError );
   CPPUNIT_TEST( testFError );
   CPPUNIT_TEST( testVFError );
   CPPUNIT_TEST_SUITE_END();
   
   Shader *fluff0, *fluff1, *fluff2;
   
   public:
   
   static bool mkRunTests();
   
   void setUp();
   void tearDown();
   
   void testVFLoad();
   void testVLoad();
   void testFLoad();
   void testVFCacheLoad();
   void testVCacheLoad();
   void testFCacheLoad();
   
   void testFilepathReturn();
   void testInvalidVFLoad();
   void testInvalidFLoad();
   void testInvalidVLoad();
   
   void testNoVFSupportLoad();
   void testNoVSupportLoad();
   void testNoFSupportLoad();
   
   void testVError();
   void testFError();
   void testVFError();
};

#endif
