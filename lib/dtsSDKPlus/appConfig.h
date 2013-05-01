//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef DTSAPPCONFIG_H_
#define DTSAPPCONFIG_H_

#include "DTSTypes.h"
#include "DTSShape.h"
#include "DTSPlusTypes.h"
#include "DTSUtil.h"
#include <fstream>

namespace DTS
{
	typedef void (*progressfnptr)(F32,F32,const char*);

	// enum for printDump
	enum
	{
		PDPass1              = 1 << 0, // collect useful nodes
		PDPass2              = 1 << 1, // put together shape structure
		PDPass3              = 1 << 2, // cull un-needed nodes
		PDObjectOffsets      = 1 << 3, // display object offset transform during 2nd pass
		PDNodeStates         = 1 << 4, // display as added
		PDObjectStates       = 1 << 5, // ""
		PDNodeStateDetails   = 1 << 6, // details of above
		PDObjectStateDetails = 1 << 7, // ""
		PDSequences          = 1 << 8,
		PDShapeHierarchy     = 1 << 9,
		PDAlways             = 0xFFFFFFFF
	};
	
   class AppNode;
   class AppMessage;

   class AppConfig
   {
      static AppConfig * smConfig;

      F32 mAnimationDelta;
      F32 mSameVertTOL;
      F32 mSameTVertTOL;
      F32 mSameNormTOL;
      F32 mWeightThreshhold;
      S32 mWeightsPerVertex;
      F32 mCyclicSequencePadding;
      F32 mAppFramesPerSec;

      bool mEnableSequences;
      bool mExportOptimized;
      bool mAllowUnusedMeshes;
      bool mAllowCollapseTransform;
      bool mNoMipMap;
      bool mNoMipMapTranslucent;
      bool mZapBorder;
      bool mIgnoreSmoothingGroupDuringCollapse;
      bool mIgnoreSmoothingGroupOnSkinMesh;
      U32  mDumpConfig;
      char * mErrorString;
      std::ofstream *mDumpFile;

		progressfnptr mProgressCallback;

      std::vector<char *> mBoolParamNames;
      std::vector<U32 *> mBoolParams;
      std::vector<U32> mBoolParamBit;
      std::vector<char *> mFloatParamNames;
      std::vector<F32 *> mFloatParams;
      std::vector<char *> mIntParamNames;
      std::vector<S32 *> mIntParams;
      std::vector<char *> mStringParamNames;
      std::vector<char *> mStringParams;
      std::vector<S32> mStringParamMaxLen;

      std::vector<char *> mAlwaysExport;
      std::vector<char *> mNeverExport;
      std::vector<char *> mNeverAnimate;
	  std::vector<AppMessage *>mWarningMessages;
	  std::vector<AppMessage *>mErrorMessages;

      virtual void printDump(U32 mask, const char * str);
		virtual void printWarning(U32 mask, const char * warningId, const char * warningMessage);
		virtual void printError(U32 mask, const char * errorId, const char * errorMessage);
      virtual bool setDumpFile(const char * path, const char * name = NULL);
      virtual bool closeDumpFile();

      virtual bool alwaysExport(AppNode *);
      virtual bool neverExport(AppNode *);
      virtual bool neverAnimate(AppNode *);

      bool searchConfigFile(const char * filename);
      bool readConfigFile(const char * filename);
      void writeConfigFile(const char * filename);

      void setInitialDefaults();
      S32 getParamEntry(const char * name, std::vector<char *> & nameTable);
      void setupConfigParams();
      void clearConfigParams();
      void clearConfigLists();
		void setAppFramesPerSec(F32 fps) { mAppFramesPerSec = fps; };

		// progress handling
		void setProgressCallback(progressfnptr callback);
		virtual void setProgress(F32 minor, F32 major, const char* message);

      // error handling
      void setExportError(const char * errorId, const char * str) { printError( PDAlways, errorId, str ); }
      bool isExportError() { return mErrorMessages.size() > 0; }
      const char * getExportError() { return "Errors during export, please check dump file."; }

      public:

      AppConfig();
      virtual ~AppConfig();

      void setConfig(AppConfig * config);

      // access configuration parameters statically...

      static bool SetDumpFile(const char * path, const char * name = NULL) { return smConfig->setDumpFile(path,name); }
      static bool CloseDumpFile() { return smConfig->closeDumpFile(); }
      static void PrintDump(U32 mask, const char * str) { smConfig->printDump(mask,str); }

		static void PrintWarning( U32 mask, const char * warningId, const char* warningMessage ) { smConfig->printWarning( mask, warningId, warningMessage ); }
		static void PrintError( U32 mask, const char * errorId, const char* errorMessage ) { smConfig->printError( mask, errorId, errorMessage ); }
	  
      static U32  GetDumpMask()          { return smConfig->mDumpConfig; }

      static bool AlwaysExport(AppNode * node) { return smConfig->alwaysExport(node); }
      static bool NeverExport(AppNode * node) { return smConfig->neverExport(node); }
      static bool NeverAnimate(AppNode * node) { return smConfig->neverAnimate(node); }

      static void SetExportError(const char * errorId, const char * str) { smConfig->setExportError(errorId, str); }
      static bool IsExportError() { return smConfig->isExportError(); }
      static const char * GetExportError() { return smConfig->getExportError(); }

      static bool GetEnableSequences()   { return smConfig->mEnableSequences; }
      static bool GetExportOptimized()   { return smConfig->mExportOptimized; }
      static bool GetAllowUnusedMeshes() { return smConfig->mAllowUnusedMeshes; }
      static bool GetAllowCollapse() { return smConfig->mAllowCollapseTransform; }
      static bool GetNoMipMap() { return smConfig->mNoMipMap; }
      static bool GetNoMipMapTranslucent() { return smConfig->mNoMipMapTranslucent; }
      static bool GetZapBorder() { return smConfig->mZapBorder; }
      static bool IgnoreSmoothingGroupDuringCollapse() { return smConfig->mIgnoreSmoothingGroupDuringCollapse; }
      static bool IgnoreSmoothingGroupOnSkinMesh() { return smConfig->mIgnoreSmoothingGroupOnSkinMesh; }
      static F32  AnimationDelta() { return smConfig->mAnimationDelta; }
      static F32  SameVertTOL() { return smConfig->mSameVertTOL; }
      static F32  SameNormTOL() { return smConfig->mSameNormTOL; }
      static F32  SameTVertTOL() { return smConfig->mSameVertTOL; }
      static F32  WeightThreshhold() { return smConfig->mWeightThreshhold; }
      static S32  WeightsPerVertex() { return smConfig->mWeightsPerVertex; }
      static F32  CyclicSequencePadding() { return smConfig->mCyclicSequencePadding; }
      static F32  AppFramesPerSec() { return smConfig->mAppFramesPerSec; }

		static void  SetAppFramesPerSec(F32 fps) { smConfig->setAppFramesPerSec(fps); }

      static void SetExportOptimized(bool opt) { smConfig->mExportOptimized = opt; }

      static void SetDefaults() { smConfig->setInitialDefaults(); }
      static bool SearchConfigFile(const char * filename) { return smConfig->searchConfigFile(filename); }
      static bool ReadConfigFile(const char * filename) { return smConfig->readConfigFile(filename); }
      static void WriteConfigFile(const char * filename) { smConfig->writeConfigFile(filename); }

		// progress handling
		static void SetProgressCallback(progressfnptr callback) { smConfig->setProgressCallback(callback); }
		static void SetProgress(F32 minor, F32 major, const char* message) { smConfig->setProgress(minor, major, message); }
   };

};


#endif // DTSAPPMATERIAL_H_
