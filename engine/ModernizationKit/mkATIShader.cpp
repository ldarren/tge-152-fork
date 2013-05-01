/*  mkATIShader.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

/*#include "ModernizationKit/mkShaderManager.h"
#include "console/console.h"
#include "platform/platform.h"
#include "core/resManager.h"
#include "core/stringTable.h"
#include "core/frameAllocator.h" 

#ifndef GL_FRAGMENT_SHADER_ATI
#define GL_FRAGMENT_SHADER_ATI                   0x8920
#define GL_REG_0_ATI                             0x8921
#define GL_REG_1_ATI                             0x8922
#define GL_REG_2_ATI                             0x8923
#define GL_REG_3_ATI                             0x8924
#define GL_REG_4_ATI                             0x8925
#define GL_REG_5_ATI                             0x8926
#define GL_CON_0_ATI                             0x8941
#define GL_CON_1_ATI                             0x8942
#define GL_CON_2_ATI                             0x8943
#define GL_CON_3_ATI                             0x8944
#define GL_CON_4_ATI                             0x8945
#define GL_CON_5_ATI                             0x8946
#define GL_CON_6_ATI                             0x8947
#define GL_CON_7_ATI                             0x8948
#define GL_MOV_ATI                               0x8961
#define GL_ADD_ATI                               0x8963
#define GL_MUL_ATI                               0x8964
#define GL_SUB_ATI                               0x8965
#define GL_DOT3_ATI                              0x8966
#define GL_DOT4_ATI                              0x8967
#define GL_MAD_ATI                               0x8968
#define GL_LERP_ATI                              0x8969
#define GL_CND_ATI                               0x896A
#define GL_CND0_ATI                              0x896B
#define GL_DOT2_ADD_ATI                          0x896C
#define GL_SECONDARY_INTERPOLATOR_ATI            0x896D
#define GL_SWIZZLE_STR_ATI                       0x8976
#define GL_SWIZZLE_STQ_ATI                       0x8977
#define GL_SWIZZLE_STR_DR_ATI                    0x8978
#define GL_SWIZZLE_STQ_DQ_ATI                    0x8979
#define GL_RED_BIT_ATI                           0x00000001
#define GL_GREEN_BIT_ATI                         0x00000002
#define GL_BLUE_BIT_ATI                          0x00000004
#define GL_2X_BIT_ATI                            0x00000001
#define GL_4X_BIT_ATI                            0x00000002
#define GL_8X_BIT_ATI                            0x00000004
#define GL_HALF_BIT_ATI                          0x00000008
#define GL_QUARTER_BIT_ATI                       0x00000010
#define GL_EIGHTH_BIT_ATI                        0x00000020
#define GL_SATURATE_BIT_ATI                      0x00000040
#define GL_COMP_BIT_ATI                          0x00000002
#define GL_NEGATE_BIT_ATI                        0x00000004
#define GL_BIAS_BIT_ATI                          0x00000008

void glColorFragmentOp1ATI(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
}

void glColorFragmentOp2ATI(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
}

void glColorFragmentOp3ATI(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
}

void glAlphaFragmentOp1ATI(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
}

void glAlphaFragmentOp2ATI(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
}

void glAlphaFragmentOp3ATI(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
}

void glSampleMapATI(GLuint dst, GLuint interp, GLenum swizzle)
{
}

void glPassTexCoordATI(GLuint dst, GLuint coord, GLenum swizzle)
{
}

#endif

#define ATI_UNARY_OP_SIZE 1
#define ATI_BINARY_OP_SIZE 5
#define ATI_TERNARY_OP_SIZE 5
#define ATI_DST_MOD_SETTING_SIZE 6
#define ATI_OPTIONAL_SATURATE_SIZE 1
#define AIT_ARG_MOD_SIZE 4
#define ATI_TEX_COORD_SIZE 6
#define ATI_THREE_TUPLE_SELECT_SIZE 4
#define ATI_REG_OP_SIZE 6
#define ATI_PROGRAM_CONSTANT_SIZE 8
#define ATI_OPTIONAL_DST_MASK_NAME_SIZE 15
#define ATI_OPTIONAL_DST_MASK_OP_SIZE 7
#define ATI_FIXED_CONSTANT_SIZE 2
#define ATI_COLOR_SIZE 2
#define ATI_ARG_REPLICATE_SIZE 4

#define ATI_INVALID 0xdecafbad

static const char* constNumbers = "01234567";

static const char* unaryOp[ATI_UNARY_OP_SIZE]     = {"MOV"};
static const char* binaryOp[ATI_BINARY_OP_SIZE]   = {"ADD", "MUL", "SUB", "DOT3", "DOT4"};
static const char* ternaryOp[ATI_TERNARY_OP_SIZE] = {"MAD", "LERP", "CND", "CND0", "DOT2ADD"};

static const GLenum unaryEnum[ATI_UNARY_OP_SIZE] =     { GL_MOV_ATI };
static const GLenum binaryEnum[ATI_BINARY_OP_SIZE] =   { GL_ADD_ATI, GL_MUL_ATI, GL_SUB_ATI, GL_DOT3_ATI, GL_DOT4_ATI };
static const GLenum ternaryEnum[ATI_TERNARY_OP_SIZE] = { GL_MAD_ATI, GL_LERP_ATI, GL_CND_ATI, GL_CND0_ATI, GL_DOT2_ADD_ATI };

static const char* dstModSetting[ATI_DST_MOD_SETTING_SIZE] = { "2x", "4x", "8x", "half", "quarter", "eigth" };
static const GLenum dstModSettingOp[ATI_DST_MOD_SETTING_SIZE] = { GL_2X_BIT_ATI, GL_4X_BIT_ATI, GL_8X_BIT_ATI, GL_HALF_BIT_ATI, GL_QUARTER_BIT_ATI, GL_EIGHTH_BIT_ATI};
static const char* optionalSaturate[ATI_OPTIONAL_SATURATE_SIZE] = { "sat" };
static const GLenum optionalSaturateOp[ATI_OPTIONAL_SATURATE_SIZE] = { GL_SATURATE_BIT_ATI };
static const char* argMod[AIT_ARG_MOD_SIZE] = { "neg", "2x", "bias", "comp" };
static const GLenum argModOp[AIT_ARG_MOD_SIZE] = { GL_2X_BIT_ATI, GL_COMP_BIT_ATI, GL_NEGATE_BIT_ATI, GL_BIAS_BIT_ATI };
static const char* texCoordName[ATI_TEX_COORD_SIZE] = { "t0", "t1", "t2", "t3", "t4", "t5" };
static const GLenum texCoordOp[ATI_TEX_COORD_SIZE] = { GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB, GL_TEXTURE3_ARB, GL_TEXTURE4_ARB, GL_TEXTURE5_ARB };
static const char* threeTupleSelect[ATI_THREE_TUPLE_SELECT_SIZE] = { "str", "stq", "str_dr", "stq_dq" };
static const GLenum threeTupleOp[ATI_THREE_TUPLE_SELECT_SIZE] = { GL_SWIZZLE_STR_ATI, GL_SWIZZLE_STQ_ATI, GL_SWIZZLE_STR_DR_ATI, GL_SWIZZLE_STQ_DQ_ATI };
static const char* regName[ATI_REG_OP_SIZE] = { "r0", "r1", "r2", "r3", "r4", "r5" };
static const GLenum regOp[ATI_REG_OP_SIZE] = { GL_REG_0_ATI, GL_REG_1_ATI, GL_REG_2_ATI, GL_REG_3_ATI, GL_REG_4_ATI, GL_REG_5_ATI };
static const char* programConstantName[ATI_PROGRAM_CONSTANT_SIZE] = { "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7" };
static const GLenum programConstantOp[ATI_PROGRAM_CONSTANT_SIZE] = { GL_CON_0_ATI, GL_CON_1_ATI, GL_CON_2_ATI, GL_CON_3_ATI, GL_CON_4_ATI, GL_CON_5_ATI, GL_CON_6_ATI, GL_CON_7_ATI };
static const char* optionalDstMask[ATI_OPTIONAL_DST_MASK_NAME_SIZE] = { "r", "g", "rg", "b", "rb", "gb", "rgb", "a", "ra", "ga", "rga", "ba", "rba", "gba", "rgba" };
static const GLenum optionalDstMaskOp[ATI_OPTIONAL_DST_MASK_OP_SIZE] = { GL_RED_BIT_ATI, GL_GREEN_BIT_ATI, GL_RED_BIT_ATI | GL_GREEN_BIT_ATI, GL_BLUE_BIT_ATI, 
                                            GL_RED_BIT_ATI | GL_BLUE_BIT_ATI, GL_GREEN_BIT_ATI | GL_BLUE_BIT_ATI, GL_RED_BIT_ATI | GL_GREEN_BIT_ATI | GL_BLUE_BIT_ATI };
static const char* fixedConstantName[ATI_FIXED_CONSTANT_SIZE] = { "0", "1" };
static const GLenum fixedConstantOp[ATI_FIXED_CONSTANT_SIZE] = { GL_ZERO, GL_ONE };
static const char* colorName[ATI_COLOR_SIZE] = { "color0", "color1" };
static const GLenum colorOp[ATI_COLOR_SIZE] = { 0, GL_SECONDARY_INTERPOLATOR_ATI };
static const char* optionalArgReplicate[ATI_ARG_REPLICATE_SIZE] = { "r", "g", "b", "a" };
static const GLenum optionalArgReplicateOp[ATI_ARG_REPLICATE_SIZE] = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };



ATIShader::ATIShader()
{
   mVertexProgram = 0;
   mFragmentProgram = 0;
   
   mPixelSourceString = NULL;
   mVertexSourceString = NULL;
   mPixelSourceStringLength = 0;
   mVertexSourceStringLength = 0;
}

bool ATIShader::loadShaderFromFile(const char* path, bool includeLibs)
{
   char fileNameBufferPix[512];
   char fileNameBufferVert[512];
   Con::expandScriptFilename(fileNameBufferPix, sizeof(fileNameBufferPix), path);
   Con::expandScriptFilename(fileNameBufferVert, sizeof(fileNameBufferVert), path);
   U32 len = dStrlen(fileNameBufferPix);
   dStrcpy(fileNameBufferPix + len, "Pixl.atit");
   dStrcpy(fileNameBufferVert + len, "Vert.atit");
   
   const char* vs;
   const char* ps;
   
   Stream *p = ResourceManager->openStream(fileNameBufferPix);
   Stream *v = ResourceManager->openStream(fileNameBufferVert);
   
   mPixelSourceStringLength = ResourceManager->getSize(fileNameBufferPix);
   mVertexSourceStringLength = ResourceManager->getSize(fileNameBufferVert);
   
   mPixelSourceString = (char *)dMalloc((mPixelSourceStringLength + 1) * sizeof(U8));
   mVertexSourceString = (char *)dMalloc((mVertexSourceStringLength + 1) * sizeof(U8));
   
   p->read(mPixelSourceStringLength, mPixelSourceString);
   v->read(mVertexSourceStringLength, mVertexSourceString);
   
   ResourceManager->closeStream(v);
   ResourceManager->closeStream(p);
   
   glEnable(GL_VERTEX_PROGRAM_ARB);
   glGenProgramsARB(1, &mVertexProgram);
   glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mVertexProgram);
   glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, mVertexSourceStringLength, mVertexSourceString);
   glDisable(GL_VERTEX_PROGRAM_ARB);
   
   // Parse ATI_text_fragment_shader into ATI_fragment_shader.
   // Then go stab someone for not just implementing ATI_text_fragment_shader into the windows ATi drivers.
   
   char buf[1024];
   char lex[1024];
   //mFragmentProgram = glGenFragmentShadersATI(1);
   //glBindFragmentShaderATI(mFragmentProgram);
   //glBeginFragmentShaderATI();
   p = ResourceManager->openStream(fileNameBufferPix);
   for(;;)
   {
      if(p->getStatus() == Stream::EOS)
         break;
      p->readLine((U8*)(buf), sizeof(buf));
      char *line = &buf[dStrlen(buf)];
      U32 offset = 0;
      U32 length = dStrlen(buf);
      for(U32 i = 0; i < length; i++)
      {
         if(buf[i] == ' ' || buf[i] == ',')
         {
            if(!offset)
               continue;
            lex[offset] = NULL;
            offset = 0;
            ATIShaderParser->parseValue(lex);
            continue;
         }
         if(buf[i] == ';')
         {
            lex[offset] = NULL;
            if(offset)
               ATIShaderParser->parseValue(lex);
            ATIShaderParser->parseValue("EOC");
            offset = 0;
            continue;
         }
         if(buf[i] == '#')
         {
            lex[offset] = NULL;
            if(offset)
               ATIShaderParser->parseValue(lex);
            offset = 0;
            i = 2048;
            continue;
         }
         if(buf[i] == '.')
         {
            lex[offset] = NULL;
            if(offset)
               ATIShaderParser->parseValue(lex);
            ATIShaderParser->parseValue("SWZ");
            offset = 0;
            continue;
         }
         if(buf[i] == '[')
         {
            lex[offset] = NULL;
            if(offset)
               ATIShaderParser->parseValue(lex);
            ATIShaderParser->parseValue("OBR");
            offset = 0;
            continue;
         }
         if(buf[i] == ']')
         {
            lex[offset] = NULL;
            if(offset)
               ATIShaderParser->parseValue(lex);
            ATIShaderParser->parseValue("CBR");
            offset = 0;
            continue;
         }
         lex[offset] = buf[i];
         offset++;
      }
      if(offset)
      {
        lex[offset] = NULL;
        ATIShaderParser->parseValue(lex);
      }
   } 
   //glEndFragmentShaderATI();
   //glBindFragmentShaderATI(0);
   return true;
}

_ATIShaderParser::_ATIShaderParser()
{
   mIsProperVersion = false;
   mIsSettingProperVersion = false;
   mCurrInstructionType = PassTexCoord;
   mCurrInstructionIsColor = false;
   mCurrInstructionIsImplicitPair = false;
   mIsConstantDeclaration = false;
   mIsSampleMap = false;
   mIsPassTexCoord = false;
   mIsFragmentOp = false;
   mCurrColorInstruction = NULL;
   mCurrAlphaInstruction = NULL;
   mCurrPassTexCoord = NULL;
   mCurrSampleMap = NULL;
   mInPrelimPass = false;
   mInOutputPass = false;
   mCompletedPrelimPass = false;
   mCompletedOutputPass = false;
   allowedValues[0] = "!!ATIfs1";
   numAllowedValues = 1;
   mRegisteredConstants.clear();
}

_ATIShaderParser::~_ATIShaderParser()
{
   if(mCurrColorInstruction)
      delete mCurrColorInstruction;
   if(mCurrAlphaInstruction)
      delete mCurrAlphaInstruction;
   if(mCurrPassTexCoord)
      delete mCurrPassTexCoord;
   if(mCurrSampleMap)
      delete mCurrSampleMap;

   mRegisteredConstants.clear();
}     

void _ATIShaderParser::parseValue(char value[])
{
   if(dStrcmp(value, "EOC") == 0)
   {
      processEOC();
      return;
   }
   for(U32 i = 0; i < numAllowedValues; i++)
   {
      if(dStrcmp(value, allowedValues[i]) == 0)
      {
         if(!mIsProperVersion && !mIsSettingProperVersion)
         {
            mIsSettingProperVersion = true;
            allowedValues[0] = "SWZ";
            numAllowedValues = 1;
            return;
         }
         if(mIsSettingProperVersion && !mIsProperVersion)
         {
            if(dStrcmp(value, "SWZ") == 0)
            {
               allowedValues[0] = "0";
               numAllowedValues = 1;
               return;
            }
            mIsSettingProperVersion = false;
            mIsProperVersion = true;
            allowedValues[0] = "StartConstants";
            allowedValues[1] = "StartPrelimPass";
            allowedValues[2] = "StartOutputPass";
            numAllowedValues = 3;
            return;
         }
         if(dStrcmp(value, "StartConstants") == 0)
         {
            mIsConstantDeclaration = true;
            allowedValues[0] = "EOC";
            numAllowedValues = 1;
            return;
         }
         if(dStrcmp(value, "StartPrelimPass") == 0)
         {
            mInPrelimPass = true;
            allowedValues[0] = "EOC";
            numAllowedValues = 1;
            return;
         }
         if(dStrcmp(value, "StartOutputPass") == 0)
         {
            mInOutputPass = true;
            allowedValues[0] = "EOC";
            numAllowedValues = 1;
            return;
         }
         if(dStrcmp(value, "EndConstants") == 0)
         {
            mIsConstantDeclaration = false;
            allowedValues[0] = "EOC";
            numAllowedValues = 1;
            return;
         }
         if(dStrcmp(value, "EndPass") == 0)
         {
            mInOutputPass = false;
            mInPrelimPass = false;
            allowedValues[0] = "EOC";
            numAllowedValues = 1;
            return;
         }
         if(mCurrColorInstruction)
         {
            if(mCurrColorInstruction->dst == ATI_INVALID)
            {
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrColorInstruction->dst = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "r0";
                     allowedValues[numAllowedValues++] = "r1";
                     allowedValues[numAllowedValues++] = "r2";
                     allowedValues[numAllowedValues++] = "r3";
                     allowedValues[numAllowedValues++] = "r4";
                     allowedValues[numAllowedValues++] = "r5";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }
                     return;
                  }
               }
            }
            if(mCurrColorInstruction->dstMask == ATI_INVALID)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "2x";
                  allowedValues[numAllowedValues++] = "4x";
                  allowedValues[numAllowedValues++] = "8x";
                  allowedValues[numAllowedValues++] = "half";
                  allowedValues[numAllowedValues++] = "quarter";
                  allowedValues[numAllowedValues++] = "eigth";
                  allowedValues[numAllowedValues++] = "r";
                  allowedValues[numAllowedValues++] = "g";
                  allowedValues[numAllowedValues++] = "rg";
                  allowedValues[numAllowedValues++] = "b";
                  allowedValues[numAllowedValues++] = "rb";
                  allowedValues[numAllowedValues++] = "gb";
                  allowedValues[numAllowedValues++] = "rgb";
                  allowedValues[numAllowedValues++] = "a";
                  allowedValues[numAllowedValues++] = "ra";
                  allowedValues[numAllowedValues++] = "ga";
                  allowedValues[numAllowedValues++] = "rga";
                  allowedValues[numAllowedValues++] = "ba";
                  allowedValues[numAllowedValues++] = "rba";
                  allowedValues[numAllowedValues++] = "gba";
                  allowedValues[numAllowedValues++] = "rgba";
                  return;
               }
               for(U32 j = 0; j < ATI_OPTIONAL_DST_MASK_NAME_SIZE; j++)
               {
                  if(dStrcmp(value, optionalDstMask[j]) == 0)
                  {
                     if(j == ATI_OPTIONAL_DST_MASK_OP_SIZE) // pure alpha
                     {
                        mCurrAlphaInstruction = new FragmentInstruction;
                        mCurrAlphaInstruction->op = mCurrColorInstruction->op;
                        mCurrAlphaInstruction->dst = mCurrColorInstruction->dst;
                        mCurrAlphaInstruction->saturate = 2;
                        mCurrAlphaInstruction->arg1 = ATI_INVALID;
                        mCurrAlphaInstruction->arg1Rep = ATI_INVALID;
                        mCurrAlphaInstruction->arg1Mod = ATI_INVALID;
                        mCurrAlphaInstruction->arg2 = ATI_INVALID;
                        mCurrAlphaInstruction->arg2Rep = ATI_INVALID;
                        mCurrAlphaInstruction->arg2Mod = ATI_INVALID;
                        mCurrAlphaInstruction->arg3 = ATI_INVALID;
                        mCurrAlphaInstruction->arg3Rep = ATI_INVALID;
                        mCurrAlphaInstruction->arg3Mod = ATI_INVALID;
                        delete mCurrColorInstruction;
                        mCurrColorInstruction = NULL;
                        numAllowedValues = 0;
                        allowedValues[numAllowedValues++] = "SWZ";
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                        return;
                     }
                     if(j > ATI_OPTIONAL_DST_MASK_OP_SIZE)
                     {
                        mCurrAlphaInstruction = new FragmentInstruction;
                        mCurrAlphaInstruction->op = mCurrColorInstruction->op;
                        mCurrAlphaInstruction->dst = mCurrColorInstruction->dst;
                        mCurrAlphaInstruction->arg1 = ATI_INVALID;
                        mCurrAlphaInstruction->arg1Rep = ATI_INVALID;
                        mCurrAlphaInstruction->arg1Mod = ATI_INVALID;
                        mCurrAlphaInstruction->arg2 = ATI_INVALID;
                        mCurrAlphaInstruction->arg2Rep = ATI_INVALID;
                        mCurrAlphaInstruction->arg2Mod = ATI_INVALID;
                        mCurrAlphaInstruction->arg3 = ATI_INVALID;
                        mCurrAlphaInstruction->arg3Rep = ATI_INVALID;
                        mCurrAlphaInstruction->arg3Mod = ATI_INVALID;
                        mCurrColorInstruction->dstMask = optionalDstMaskOp[j - ATI_OPTIONAL_DST_MASK_OP_SIZE - 1];
                        numAllowedValues = 0;
                        allowedValues[numAllowedValues++] = "SWZ";
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                        return;
                     }
                     mCurrColorInstruction->dstMask = optionalDstMaskOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "r0";
                     allowedValues[numAllowedValues++] = "r1";
                     allowedValues[numAllowedValues++] = "r2";
                     allowedValues[numAllowedValues++] = "r3";
                     allowedValues[numAllowedValues++] = "r4";
                     allowedValues[numAllowedValues++] = "r5";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }  
                     return;
                  }
               }
               // No dstmask
               mCurrColorInstruction->dstMask = GL_NONE;
               mCurrAlphaInstruction = new FragmentInstruction;
               mCurrAlphaInstruction->op = mCurrColorInstruction->op;
               mCurrAlphaInstruction->dst = mCurrColorInstruction->dst;
               mCurrAlphaInstruction->dstMask = GL_NONE;
               mCurrAlphaInstruction->arg1 = ATI_INVALID;
               mCurrAlphaInstruction->arg1Rep = ATI_INVALID;
               mCurrAlphaInstruction->arg1Mod = ATI_INVALID;
               mCurrAlphaInstruction->arg2 = ATI_INVALID;
               mCurrAlphaInstruction->arg2Rep = ATI_INVALID;
               mCurrAlphaInstruction->arg2Mod = ATI_INVALID;
               mCurrAlphaInstruction->arg3 = ATI_INVALID;
               mCurrAlphaInstruction->arg3Rep = ATI_INVALID;
               mCurrAlphaInstruction->arg3Mod = ATI_INVALID;
            }
            if(mCurrColorInstruction->dstMod == ATI_INVALID)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "2x";
                  allowedValues[numAllowedValues++] = "4x";
                  allowedValues[numAllowedValues++] = "8x";
                  allowedValues[numAllowedValues++] = "half";
                  allowedValues[numAllowedValues++] = "quarter";
                  allowedValues[numAllowedValues++] = "eighth";
                  return;
               }
               for(U32 j = 0; j < ATI_DST_MOD_SETTING_SIZE; j++)
               {
                  if(dStrcmp(value, dstModSetting[j]) == 0)
                  {
                     mCurrColorInstruction->dstMod = dstModSettingOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->dstMod = dstModSettingOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "r0";
                     allowedValues[numAllowedValues++] = "r1";
                     allowedValues[numAllowedValues++] = "r2";
                     allowedValues[numAllowedValues++] = "r3";
                     allowedValues[numAllowedValues++] = "r4";
                     allowedValues[numAllowedValues++] = "r5";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }
                     return;
                 }
               }
               //No dst mod
               mCurrColorInstruction->dstMod = GL_NONE;
            }
            if(mCurrColorInstruction->saturate == 2)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "sat";
                  return;
               }
               if(dStrcmp(value, "sat") == 0)
               {
                  mCurrColorInstruction->saturate = 1;
                  allowedValues[numAllowedValues++] = "r0";
                  allowedValues[numAllowedValues++] = "r1";
                  allowedValues[numAllowedValues++] = "r2";
                  allowedValues[numAllowedValues++] = "r3";
                  allowedValues[numAllowedValues++] = "r4";
                  allowedValues[numAllowedValues++] = "r5";
                  for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                  {
                     allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                  }
                  return;
               }
               //No saturate
               mCurrColorInstruction->saturate = 0;
               numAllowedValues = 0;
               allowedValues[numAllowedValues++] = "r0";
               allowedValues[numAllowedValues++] = "r1";
               allowedValues[numAllowedValues++] = "r2";
               allowedValues[numAllowedValues++] = "r3";
               allowedValues[numAllowedValues++] = "r4";
               allowedValues[numAllowedValues++] = "r5";
               for(U32 x = 0; x < mRegisteredConstants.size(); x++)
               {
                  allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
               }
            }
            if(mCurrColorInstruction->arg1 == ATI_INVALID)
            {
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrColorInstruction->arg1 = regOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg1 = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Unary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                  }
               }
               for(U32 j = 0; j < mRegisteredConstants.size(); j++)
               {
                  if(dStrcmp(value, mRegisteredConstants[j].regName) == 0)
                  {
                     mCurrColorInstruction->arg1 = mRegisteredConstants[j].regEnum;
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg1 = mRegisteredConstants[j].regEnum;
                     mCurrColorInstruction->numConstants = 1;
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Unary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                  }
               }
            }
            if(mCurrColorInstruction->arg1Rep == ATI_INVALID)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "r";
                  allowedValues[numAllowedValues++] = "g";
                  allowedValues[numAllowedValues++] = "b";
                  allowedValues[numAllowedValues++] = "a";
                  return;
               }
               for(U32 j = 0; j < ATI_ARG_REPLICATE_SIZE; j++)
               {
                  if(dStrcmp(value, optionalArgReplicate[j]) == 0)
                  {
                     mCurrColorInstruction->arg1Rep = optionalArgReplicateOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg1Rep = optionalArgReplicateOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Unary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                  }
               }
               // No arg rep
               mCurrColorInstruction->arg1Rep = GL_NONE;
               if(mCurrAlphaInstruction)
                  mCurrAlphaInstruction->arg1Rep = GL_NONE;
            }
            if(mCurrColorInstruction->arg1Mod == ATI_INVALID)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "neg";
                  allowedValues[numAllowedValues++] = "2x";
                  allowedValues[numAllowedValues++] = "bias";
                  allowedValues[numAllowedValues++] = "comp";
                  return;
               }
               for(U32 j = 0; j < AIT_ARG_MOD_SIZE; j++)
               {
                  if(dStrcmp(value, dstModSetting[j]) == 0)
                  {
                     mCurrColorInstruction->arg1Mod = argModOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->dstMod = argModOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Unary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                 }
               }
               //No arg mod
               mCurrColorInstruction->arg1Mod = GL_NONE;
               if(mCurrAlphaInstruction)
                  mCurrAlphaInstruction->arg1Mod = GL_NONE;
            }
            if(mCurrColorInstruction->arg2 == ATI_INVALID && mCurrInstructionType > Unary)
            {
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrColorInstruction->arg2 = regOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg2 = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Binary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                  }
               }
               for(U32 j = 0; j < mRegisteredConstants.size(); j++)
               {
                  if(dStrcmp(value, mRegisteredConstants[j].regName) == 0)
                  {
                     mCurrColorInstruction->arg2 = mRegisteredConstants[j].regEnum;
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg2 = mRegisteredConstants[j].regEnum;
                     mCurrColorInstruction->numConstants++;
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Binary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                  }
               }
            }
            if(mCurrColorInstruction->arg2Rep == ATI_INVALID && mCurrInstructionType > Unary)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "r";
                  allowedValues[numAllowedValues++] = "g";
                  allowedValues[numAllowedValues++] = "b";
                  allowedValues[numAllowedValues++] = "a";
                  return;
               }
               for(U32 j = 0; j < ATI_ARG_REPLICATE_SIZE; j++)
               {
                  if(dStrcmp(value, optionalArgReplicate[j]) == 0)
                  {
                     mCurrColorInstruction->arg2Rep = optionalArgReplicateOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg2Rep = optionalArgReplicateOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Binary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                  }
               }
               // No arg rep
               mCurrColorInstruction->arg2Rep = GL_NONE;
               if(mCurrAlphaInstruction)
                  mCurrAlphaInstruction->arg2Rep = GL_NONE;
            }
            if(mCurrColorInstruction->arg2Mod == ATI_INVALID && mCurrInstructionType > Unary)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "neg";
                  allowedValues[numAllowedValues++] = "2x";
                  allowedValues[numAllowedValues++] = "bias";
                  allowedValues[numAllowedValues++] = "comp";
                  return;
               }
               for(U32 j = 0; j < AIT_ARG_MOD_SIZE; j++)
               {
                  if(dStrcmp(value, dstModSetting[j]) == 0)
                  {
                     mCurrColorInstruction->arg2Mod = argModOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->dstMod = argModOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     if(mCurrInstructionType > Binary)
                     {
                        allowedValues[numAllowedValues++] = "r0";
                        allowedValues[numAllowedValues++] = "r1";
                        allowedValues[numAllowedValues++] = "r2";
                        allowedValues[numAllowedValues++] = "r3";
                        allowedValues[numAllowedValues++] = "r4";
                        allowedValues[numAllowedValues++] = "r5";
                        for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                        {
                           allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                        }
                     }
                     else
                     {
                        allowedValues[numAllowedValues++] = "EOC";
                     }
                     return;
                 }
               }
               //No arg mod
               mCurrColorInstruction->arg2Mod = GL_NONE;
               if(mCurrAlphaInstruction)
                  mCurrAlphaInstruction->arg2Mod = GL_NONE;
            }
            if(mCurrColorInstruction->arg3 == ATI_INVALID && mCurrInstructionType > Binary)
            {
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrColorInstruction->arg3 = regOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg3 = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "EOC";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }
                     return;
                  }
               }
               for(U32 j = 0; j < mRegisteredConstants.size(); j++)
               {
                  if(dStrcmp(value, mRegisteredConstants[j].regName) == 0)
                  {
                     mCurrColorInstruction->arg3 = mRegisteredConstants[j].regEnum;
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg3 = mRegisteredConstants[j].regEnum;
                     if(mCurrColorInstruction->numConstants == 2)
                        Con::errorf("Too many constants in instruction!");
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "EOC";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }
                     return;
                  }
               }
            }
            if(mCurrColorInstruction->arg3Rep == ATI_INVALID && mCurrInstructionType > Binary)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "r";
                  allowedValues[numAllowedValues++] = "g";
                  allowedValues[numAllowedValues++] = "b";
                  allowedValues[numAllowedValues++] = "a";
                  return;
               }
               for(U32 j = 0; j < ATI_ARG_REPLICATE_SIZE; j++)
               {
                  if(dStrcmp(value, optionalArgReplicate[j]) == 0)
                  {
                     mCurrColorInstruction->arg3Rep = optionalArgReplicateOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->arg3Rep = optionalArgReplicateOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "EOC";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }
                     return;
                  }
               }
               // No arg rep
               mCurrColorInstruction->arg3Rep = GL_NONE;
               if(mCurrAlphaInstruction)
                  mCurrAlphaInstruction->arg3Rep = GL_NONE;
            }
            if(mCurrColorInstruction->arg3Mod == ATI_INVALID && mCurrInstructionType > Binary)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "neg";
                  allowedValues[numAllowedValues++] = "2x";
                  allowedValues[numAllowedValues++] = "bias";
                  allowedValues[numAllowedValues++] = "comp";
                  return;
               }
               for(U32 j = 0; j < AIT_ARG_MOD_SIZE; j++)
               {
                  if(dStrcmp(value, dstModSetting[j]) == 0)
                  {
                     mCurrColorInstruction->arg3Mod = argModOp[j];
                     if(mCurrAlphaInstruction)
                        mCurrAlphaInstruction->dstMod = argModOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     allowedValues[numAllowedValues++] = "EOC";
                     for(U32 x = 0; x < mRegisteredConstants.size(); x++)
                     {
                        allowedValues[numAllowedValues++] = mRegisteredConstants[x].regName;
                     }
                     return;
                 }
               }
               //No arg mod
               mCurrColorInstruction->arg3Mod = GL_NONE;
               if(mCurrAlphaInstruction)
                  mCurrAlphaInstruction->arg3Mod = GL_NONE;
            }
         }
         if(mCurrAlphaInstruction && !mCurrColorInstruction)
         {
            if(mCurrAlphaInstruction->dstMod == ATI_INVALID)
            {
            }
            if(mCurrAlphaInstruction->arg1 == ATI_INVALID)
            {
            }
            if(mCurrAlphaInstruction->arg1Rep == ATI_INVALID)
            {
            }
            if(mCurrAlphaInstruction->arg1Mod == ATI_INVALID)
            {
            }
            if(mCurrAlphaInstruction->arg2 == ATI_INVALID && mCurrInstructionType > Unary)
            {
            }
            if(mCurrAlphaInstruction->arg2Rep == ATI_INVALID && mCurrInstructionType > Unary)
            {
            }
            if(mCurrAlphaInstruction->arg2Mod == ATI_INVALID && mCurrInstructionType > Unary)
            {
            }
            if(mCurrAlphaInstruction->arg3 == ATI_INVALID && mCurrInstructionType > Binary)
            {
            }
            if(mCurrAlphaInstruction->arg3Rep == ATI_INVALID && mCurrInstructionType > Binary)
            {
            }
            if(mCurrAlphaInstruction->arg3Mod == ATI_INVALID && mCurrInstructionType > Binary)
            {
            }
         }
         if(mCurrSampleMap)
         {
            if(mCurrSampleMap->dst = ATI_INVALID)
            {
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrSampleMap->dst = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "t0";
                     allowedValues[numAllowedValues++] = "t1";
                     allowedValues[numAllowedValues++] = "t2";
                     allowedValues[numAllowedValues++] = "t3";
                     allowedValues[numAllowedValues++] = "t4";
                     allowedValues[numAllowedValues++] = "t5";
                     return;
                  }
               }
            }
            if(mCurrSampleMap->coord = ATI_INVALID)
            {
               for(U32 j = 0; j < ATI_TEX_COORD_SIZE; j++)
               {
                  if(dStrcmp(value, texCoordName[j]) == 0)
                  {
                     mCurrSampleMap->coord = texCoordOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     return;
                  }
               }
            }
            if(mCurrSampleMap->swizzle = ATI_INVALID)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "str";
                  allowedValues[numAllowedValues++] = "stq";
                  allowedValues[numAllowedValues++] = "str_dr";
                  allowedValues[numAllowedValues++] = "stq_dq";
                  return;
               }
               for(U32 j = 0; j < ATI_THREE_TUPLE_SELECT_SIZE; j++)
               {
                  if(dStrcmp(value, threeTupleSelect[j]) == 0)
                  {
                     mCurrSampleMap->swizzle = threeTupleOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "EOC";
                     return;
                  }
               }
            }
         }
         if(mCurrPassTexCoord)
         {
            if(mCurrPassTexCoord->dst = ATI_INVALID)
            {
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrPassTexCoord->dst = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "t0";
                     allowedValues[numAllowedValues++] = "t1";
                     allowedValues[numAllowedValues++] = "t2";
                     allowedValues[numAllowedValues++] = "t3";
                     allowedValues[numAllowedValues++] = "t4";
                     allowedValues[numAllowedValues++] = "t5";
                     allowedValues[numAllowedValues++] = "r0";
                     allowedValues[numAllowedValues++] = "r1";
                     allowedValues[numAllowedValues++] = "r2";
                     allowedValues[numAllowedValues++] = "r3";
                     allowedValues[numAllowedValues++] = "r4";
                     allowedValues[numAllowedValues++] = "r5";
                     return;
                  }
               }
            }
            if(mCurrPassTexCoord->coord = ATI_INVALID)
            {
               for(U32 j = 0; j < ATI_TEX_COORD_SIZE; j++)
               {
                  if(dStrcmp(value, texCoordName[j]) == 0)
                  {
                     mCurrPassTexCoord->coord = texCoordOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     return;
                  }
               }
               for(U32 j = 0; j < ATI_REG_OP_SIZE; j++)
               {
                  if(dStrcmp(value, regName[j]) == 0)
                  {
                     mCurrPassTexCoord->coord = regOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "SWZ";
                     return;
                  }
               }
            }
            if(mCurrPassTexCoord->swizzle = ATI_INVALID)
            {
               if(dStrcmp(value, "SWZ") == 0)
               {
                  numAllowedValues = 0;
                  allowedValues[numAllowedValues++] = "str";
                  allowedValues[numAllowedValues++] = "stq";
                  allowedValues[numAllowedValues++] = "str_dr";
                  allowedValues[numAllowedValues++] = "stq_dq";
                  return;
               }
               for(U32 j = 0; j < ATI_THREE_TUPLE_SELECT_SIZE; j++)
               {
                  if(dStrcmp(value, threeTupleSelect[j]) == 0)
                  {
                     mCurrPassTexCoord->swizzle = threeTupleOp[j];
                     numAllowedValues = 0;
                     allowedValues[numAllowedValues++] = "EOC";
                     return;
                  }
               }
            }
         }
         for(U32 j = 0; j < ATI_UNARY_OP_SIZE; j++)
         {
            if(dStrcmp(value, unaryOp[j]) == 0)
            {
               mCurrColorInstruction = new FragmentInstruction;
               mCurrInstructionType = Unary;
               mCurrColorInstruction->op = unaryEnum[j];
               mCurrColorInstruction->dst = ATI_INVALID;
               mCurrColorInstruction->dstMask = ATI_INVALID;
               mCurrColorInstruction->dstMod = ATI_INVALID;
               mCurrColorInstruction->arg1 = ATI_INVALID;
               mCurrColorInstruction->arg1Rep = ATI_INVALID;
               mCurrColorInstruction->arg1Mod = ATI_INVALID;
               mCurrColorInstruction->saturate = 2;
               mCurrColorInstruction->numConstants = 0;
               numAllowedValues = 0;
               allowedValues[numAllowedValues++] = "r0";
               allowedValues[numAllowedValues++] = "r1";
               allowedValues[numAllowedValues++] = "r2";
               allowedValues[numAllowedValues++] = "r3";
               allowedValues[numAllowedValues++] = "r4";
               allowedValues[numAllowedValues++] = "r5";
               return;
            }
         }
         for(U32 j = 0; j < ATI_BINARY_OP_SIZE; j++)
         {
            if(dStrcmp(value, binaryOp[j]) == 0)
            {
               mCurrColorInstruction = new FragmentInstruction;
               mCurrInstructionType = Binary;
               mCurrColorInstruction->op = binaryEnum[j];
               mCurrColorInstruction->dst = ATI_INVALID;
               mCurrColorInstruction->dstMask = ATI_INVALID;
               mCurrColorInstruction->dstMod = ATI_INVALID;
               mCurrColorInstruction->arg1 = ATI_INVALID;
               mCurrColorInstruction->arg1Rep = ATI_INVALID;
               mCurrColorInstruction->arg1Mod = ATI_INVALID;
               mCurrColorInstruction->arg2 = ATI_INVALID;
               mCurrColorInstruction->arg2Rep = ATI_INVALID;
               mCurrColorInstruction->arg2Mod = ATI_INVALID;
               mCurrColorInstruction->saturate = 2;
               mCurrColorInstruction->numConstants = 0;
               numAllowedValues = 0;
               allowedValues[numAllowedValues++] = "r0";
               allowedValues[numAllowedValues++] = "r1";
               allowedValues[numAllowedValues++] = "r2";
               allowedValues[numAllowedValues++] = "r3";
               allowedValues[numAllowedValues++] = "r4";
               allowedValues[numAllowedValues++] = "r5";
               return;
            }
         }
         for(U32 j = 0; j < ATI_TERNARY_OP_SIZE; j++)
         {
            if(dStrcmp(value, ternaryOp[j]) == 0)
            {
               mCurrColorInstruction = new FragmentInstruction;
               mCurrInstructionType = Ternary;
               mCurrColorInstruction->op = ternaryEnum[j];
               mCurrColorInstruction->dst = ATI_INVALID;
               mCurrColorInstruction->dstMask = ATI_INVALID;
               mCurrColorInstruction->dstMod = ATI_INVALID;
               mCurrColorInstruction->arg1 = ATI_INVALID;
               mCurrColorInstruction->arg1Rep = ATI_INVALID;
               mCurrColorInstruction->arg1Mod = ATI_INVALID;
               mCurrColorInstruction->arg2 = ATI_INVALID;
               mCurrColorInstruction->arg2Rep = ATI_INVALID;
               mCurrColorInstruction->arg2Mod = ATI_INVALID;
               mCurrColorInstruction->arg3 = ATI_INVALID;
               mCurrColorInstruction->arg3Rep = ATI_INVALID;
               mCurrColorInstruction->arg3Mod = ATI_INVALID;
               mCurrColorInstruction->saturate = 2;
               mCurrColorInstruction->numConstants = 0;
               numAllowedValues = 0;
               allowedValues[numAllowedValues++] = "r0";
               allowedValues[numAllowedValues++] = "r1";
               allowedValues[numAllowedValues++] = "r2";
               allowedValues[numAllowedValues++] = "r3";
               allowedValues[numAllowedValues++] = "r4";
               allowedValues[numAllowedValues++] = "r5";
               return;
            }
         }
         if(dStrcmp(value, "SampleMap") == 0)
         {
            mCurrSampleMap = new ATISampleMap;
            mCurrSampleMap->dst = ATI_INVALID;
            mCurrSampleMap->coord = ATI_INVALID;
            mCurrSampleMap->swizzle = ATI_INVALID;
            numAllowedValues = 0;
            allowedValues[numAllowedValues++] = "r0";
            allowedValues[numAllowedValues++] = "r1";
            allowedValues[numAllowedValues++] = "r2";
            allowedValues[numAllowedValues++] = "r3";
            allowedValues[numAllowedValues++] = "r4";
            allowedValues[numAllowedValues++] = "r5";
            mCurrInstructionType = SampleMap;
            return;
         }
         if(dStrcmp(value, "PassTexCoord") == 0)
         {
            mCurrPassTexCoord = new ATIPassTexCoord;
            mCurrPassTexCoord->dst = ATI_INVALID;
            mCurrPassTexCoord->coord = ATI_INVALID;
            mCurrPassTexCoord->swizzle = ATI_INVALID;
         }
          if(dStrcmp(value, "CONSTANT") == 0)
          {
             numAllowedValues = 0;
             allowedValues[numAllowedValues++] = "c0";
             allowedValues[numAllowedValues++] = "c1";
             allowedValues[numAllowedValues++] = "c2";
             allowedValues[numAllowedValues++] = "c3";
             allowedValues[numAllowedValues++] = "c4";
             allowedValues[numAllowedValues++] = "c5";
             allowedValues[numAllowedValues++] = "c6";
             allowedValues[numAllowedValues++] = "c7";
             return;
          }
          if(mIsConstantDeclaration) // declaraing constants, didn't hit an EOC or "CONSTANT"
          {
             if(dStrcmp(value, "=") == 0)
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "program";
                return;
             }
             if(dStrcmp(value, "program") == 0)
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "SWZ";
                return;
             }
             if(dStrcmp(value, "SWZ") == 0)
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "env";
                return;
             }
             if(dStrcmp(value, "env") == 0)
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "OBR";
                return;
             }
             if(dStrcmp(value, "OBR") == 0)
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "0";
                allowedValues[numAllowedValues++] = "1";
                allowedValues[numAllowedValues++] = "2";
                allowedValues[numAllowedValues++] = "3";
                allowedValues[numAllowedValues++] = "4";
                allowedValues[numAllowedValues++] = "5";
                allowedValues[numAllowedValues++] = "6";
                allowedValues[numAllowedValues++] = "7";
                return;
             }
             if(dIsdigit(value[0]))
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "CBR";
                mRegisteredConstants.last().regNum = dAtoi(value);
                return;
             }
             if(dStrcmp(value, "CBR") == 0)
             {
                numAllowedValues = 0;
                allowedValues[numAllowedValues++] = "EOC";
                return;
             }
             for(U32 j = 0; j < ATI_PROGRAM_CONSTANT_SIZE; j++)
             {
                if(dStrcmp(value, programConstantName[j]) == 0)
                {
                   numAllowedValues = 0;
                   allowedValues[numAllowedValues++] = "=";
                   ATIConstant newConst;
                   newConst.regName = value;
                   newConst.regEnum = programConstantOp[j];
                   mRegisteredConstants.push_back(newConst);
                   return;
                }
             }
          }
      }
   }
   Con::errorf("Error in parsing shader");
}

void _ATIShaderParser::processEOC()
{
   if(mCurrColorInstruction)
   {
      emitColorInstruction();
      delete mCurrColorInstruction;
      mCurrColorInstruction = NULL;
   }
   if(mCurrAlphaInstruction)
   {
      emitAlphaInstruction();
      delete mCurrAlphaInstruction;
      mCurrAlphaInstruction = NULL;
   }
   if(mCurrSampleMap)
   {
      emitSampleMap();
      delete mCurrSampleMap;
      mCurrSampleMap = NULL;
   }
   if(mCurrPassTexCoord)
   {
      emitPassTexCoord();
      delete mCurrPassTexCoord;
      mCurrPassTexCoord = NULL;
   }
   if(mIsConstantDeclaration)
   {
      allowedValues[0] = "CONSTANT";
      allowedValues[1] = "EndConstants";
      numAllowedValues = 2;
      return;
   }
   if(mInPrelimPass)
   {
      numAllowedValues = 0;
      if(mCurrInstructionType == PassTexCoord || mCurrInstructionType == SampleMap)
      {
         allowedValues[numAllowedValues++] = "SampleMap";
         allowedValues[numAllowedValues++] = "PassTexCoord";
         allowedValues[numAllowedValues++] = "MOV";
         allowedValues[numAllowedValues++] = "ADD";
         allowedValues[numAllowedValues++] = "MUL";
         allowedValues[numAllowedValues++] = "SUB";
         allowedValues[numAllowedValues++] = "DOT3";
         allowedValues[numAllowedValues++] = "DOT4";
         allowedValues[numAllowedValues++] = "MAD";
         allowedValues[numAllowedValues++] = "LERP";
         allowedValues[numAllowedValues++] = "CND";
         allowedValues[numAllowedValues++] = "CND0";
         allowedValues[numAllowedValues++] = "DOT2ADD";
      }
      else
      {
         allowedValues[numAllowedValues++] = "MOV";
         allowedValues[numAllowedValues++] = "ADD";
         allowedValues[numAllowedValues++] = "MUL";
         allowedValues[numAllowedValues++] = "SUB";
         allowedValues[numAllowedValues++] = "DOT3";
         allowedValues[numAllowedValues++] = "DOT4";
         allowedValues[numAllowedValues++] = "MAD";
         allowedValues[numAllowedValues++] = "LERP";
         allowedValues[numAllowedValues++] = "CND";
         allowedValues[numAllowedValues++] = "CND0";
         allowedValues[numAllowedValues++] = "DOT2ADD";
         allowedValues[numAllowedValues++] = "EndPass";
      }
      return;
   }
   if(mInOutputPass)
   {
      numAllowedValues = 0;
      if(mCurrInstructionType == PassTexCoord || mCurrInstructionType == SampleMap)
      {
         allowedValues[numAllowedValues++] = "SampleMap";
         allowedValues[numAllowedValues++] = "PassTexCoord";
      }
      allowedValues[numAllowedValues++] = "MOV";
      allowedValues[numAllowedValues++] = "ADD";
      allowedValues[numAllowedValues++] = "MUL";
      allowedValues[numAllowedValues++] = "SUB";
      allowedValues[numAllowedValues++] = "DOT3";
      allowedValues[numAllowedValues++] = "DOT4";
      allowedValues[numAllowedValues++] = "MAD";
      allowedValues[numAllowedValues++] = "LERP";
      allowedValues[numAllowedValues++] = "CND";
      allowedValues[numAllowedValues++] = "CND0";
      allowedValues[numAllowedValues++] = "DOT2ADD";
      allowedValues[numAllowedValues++] = "EndPass";
      return;
   }
   //We must have just left a pass.
   allowedValues[0] = "StartOutputPass";
   allowedValues[1] = "StartPrelimPass";
   mCurrInstructionType = PassTexCoord;
   numAllowedValues = 2;
   return;
}

void _ATIShaderParser::reset()
{
   if(mCurrColorInstruction)
      delete mCurrColorInstruction;
   if(mCurrAlphaInstruction)
      delete mCurrAlphaInstruction;
   if(mCurrPassTexCoord)
      delete mCurrPassTexCoord;
   if(mCurrSampleMap)
      delete mCurrSampleMap;

   mRegisteredConstants.clear();
   mIsProperVersion = false;
   mCurrInstructionType = Unary;
   mCurrInstructionIsColor = false;
   mCurrInstructionIsImplicitPair = false;
   mIsConstantDeclaration = false;
   mIsSampleMap = false;
   mIsPassTexCoord = false;
   mIsFragmentOp = false;
   mCurrColorInstruction = NULL;
   mCurrAlphaInstruction = NULL;
   mCurrPassTexCoord = NULL;
   mCurrSampleMap = NULL;
}

void _ATIShaderParser::emitColorInstruction()
{
   switch(mCurrInstructionType)
   {
      case Unary:
         glColorFragmentOp1ATI(mCurrColorInstruction->op, mCurrColorInstruction->dst, mCurrColorInstruction->dstMask,
                               mCurrColorInstruction->dstMod, mCurrColorInstruction->arg1, mCurrColorInstruction->arg1Rep,
                               mCurrColorInstruction->arg1Mod);
         break;
      case Binary:
         glColorFragmentOp2ATI(mCurrColorInstruction->op, mCurrColorInstruction->dst, mCurrColorInstruction->dstMask,
                               mCurrColorInstruction->dstMod, mCurrColorInstruction->arg1, mCurrColorInstruction->arg1Rep,
                               mCurrColorInstruction->arg1Mod, mCurrColorInstruction->arg2, mCurrColorInstruction->arg2Rep,
                               mCurrColorInstruction->arg2Mod);
         break;
      case Ternary:
         glColorFragmentOp3ATI(mCurrColorInstruction->op, mCurrColorInstruction->dst, mCurrColorInstruction->dstMask,
                               mCurrColorInstruction->dstMod, mCurrColorInstruction->arg1, mCurrColorInstruction->arg1Rep,
                               mCurrColorInstruction->arg1Mod, mCurrColorInstruction->arg2, mCurrColorInstruction->arg2Rep,
                               mCurrColorInstruction->arg2Mod, mCurrColorInstruction->arg3, mCurrColorInstruction->arg3Rep,
                               mCurrColorInstruction->arg3Mod);
         break;
   }
}

void _ATIShaderParser::emitAlphaInstruction()
{
   switch(mCurrInstructionType)
   {
      case Unary:
         glAlphaFragmentOp1ATI(mCurrAlphaInstruction->op, mCurrAlphaInstruction->dst,
                               mCurrAlphaInstruction->dstMod, mCurrAlphaInstruction->arg1, mCurrAlphaInstruction->arg1Rep,
                               mCurrAlphaInstruction->arg1Mod);
         break;
      case Binary:
         glAlphaFragmentOp2ATI(mCurrAlphaInstruction->op, mCurrAlphaInstruction->dst,
                               mCurrAlphaInstruction->dstMod, mCurrAlphaInstruction->arg1, mCurrAlphaInstruction->arg1Rep,
                               mCurrAlphaInstruction->arg1Mod, mCurrAlphaInstruction->arg2, mCurrAlphaInstruction->arg2Rep,
                               mCurrAlphaInstruction->arg2Mod);
         break;
      case Ternary:
         glAlphaFragmentOp3ATI(mCurrAlphaInstruction->op, mCurrAlphaInstruction->dst,
                               mCurrAlphaInstruction->dstMod, mCurrAlphaInstruction->arg1, mCurrAlphaInstruction->arg1Rep,
                               mCurrAlphaInstruction->arg1Mod, mCurrAlphaInstruction->arg2, mCurrAlphaInstruction->arg2Rep,
                               mCurrAlphaInstruction->arg2Mod, mCurrAlphaInstruction->arg3, mCurrAlphaInstruction->arg3Rep,
                               mCurrAlphaInstruction->arg3Mod);
         break;
   }
}

void _ATIShaderParser::emitSampleMap()
{
   glSampleMapATI(mCurrSampleMap->dst, mCurrSampleMap->coord, mCurrSampleMap->swizzle);
}

void _ATIShaderParser::emitPassTexCoord()
{
   glPassTexCoordATI(mCurrPassTexCoord->dst, mCurrPassTexCoord->coord, mCurrPassTexCoord->swizzle);
}

ASMShaderParameter* ATIShader::getNamedParameter(StringTableEntry name)
{
   return NULL;
}

bool ATIShader::setParameter1f(StringTableEntry name, F32 value0)
{
   return false;
}

bool ATIShader::setParameter2f(StringTableEntry name, F32 value0, F32 value1)
{
   return false;
}

bool ATIShader::setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2)
{
   return false;
}

bool ATIShader::setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3)
{
   return false;
}

bool ATIShader::setParameter1i(StringTableEntry name, U32 value0)
{
   return false;
}

bool ATIShader::setParameter2i(StringTableEntry name, U32 value0, U32 value1)
{
   return false;
}

bool ATIShader::setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2)
{
   return false;
}

bool ATIShader::setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3)
{
}

bool ATIShader::setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m)
{
   return false;
}

bool ATIShader::setTextureSampler(StringTableEntry name, U32 value0)
{
}

void ATIShader::reset()
{
}

void ATIShader::resurrect()
{
}

void ATIShader::kill()
{
}

void ATIShader::bind()
{
}

void ATIShader::unbind()
{
} */



