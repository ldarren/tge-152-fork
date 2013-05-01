/*  mkVBOTests.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#ifndef _MKVBOTESTS_H_
#define _MKVBOTESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class MKVBOTests : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE( MKVBOTests );
   
   CPPUNIT_TEST( testDataReload );
   
   CPPUNIT_TEST_SUITE_END();
   
   public:
   
   static bool mkRunTests();
   
   void setUp();
   void tearDown();
   
   void testDataReload();

};

#endif