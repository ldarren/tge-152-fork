/*  mkTesting.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/UnitTests/mkTesting.h"
#include "ModernizationKit/UnitTests/mkGLSLTests.h"
//#include "ModernizationKit/UnitTests/mkCgTests.h"
#include "ModernizationKit/UnitTests/mkASMTests.h"
#include "ModernizationKit/UnitTests/mkATITTests.h"
#include "ModernizationKit/UnitTests/mkVBOTests.h"

bool mkRunTests()
{
   bool wasSuccessful;
   wasSuccessful  = MKGLSLShaderTests::mkRunTests();
   //wasSuccessful |= MKCgShaderTests::mkRunTests();
   wasSuccessful |= MKASMShaderTests::mkRunTests();
   wasSuccessful |= MKATITShaderTests::mkRunTests();
   wasSuccessful |= MKVBOTests::mkRunTests();
   return wasSuccessful;
}
   