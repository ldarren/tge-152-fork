/*  mkVBOTests.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/UnitTests/mkVBOTests.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ModernizationKit/mkVBOManager.h"

bool MKVBOTests::mkRunTests()
{
   printf("Running VBO unit tests\n");
   CppUnit::TextTestRunner runner;
   runner.addTest( MKVBOTests::suite() );
   bool wasSuccessful = runner.run( "", false );
   return wasSuccessful;
}

void MKVBOTests::setUp()
{

}

void MKVBOTests::tearDown()
{

}

void MKVBOTests::testDataReload()
{
   printf("Testing data reloading...\n");
   F32 data0 = 0.3f;
   F32 data1 = 22.67f;
   F32 data2 = 4.89f;
   F32 data3 = 9.34f;
   F32* VBOData;

   VertexBuffer* buf = VBOManager->createVertexBuffer(4 * sizeof(F32), MKStream, NULL);
   VBOManager->bindVBO(buf);
   
   VBOData = (F32*)buf->map();
   VBOData[0] = data0;
   VBOData[1] = data1;
   VBOData[2] = data2;
   VBOData[3] = data3;
   buf->unmap();
   VBOManager->bindVBO(NULL);
   
   VBOManager->reset();
   VBOManager->resurrect();
   
   F32 newBuffData[4];
   buf->getInternalGLData(4 * sizeof(F32), 0, newBuffData);
   
   VBOManager->deleteBuffer(buf);
   
   CPPUNIT_ASSERT(newBuffData[0] == data0);
   CPPUNIT_ASSERT(newBuffData[1] == data1);
   CPPUNIT_ASSERT(newBuffData[2] == data2);
   CPPUNIT_ASSERT(newBuffData[3] == data3);
   
   printf("PASS!\n");
}