//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(disable : 4786 4018)
#endif

#include "appConfig.h"
#include "appNode.h"

namespace DTS {

	class AppMessage
	{
		char* mMessageId;
		char* mMessage;
	public:
		AppMessage( const char * messageId, const char * message );
		~AppMessage();

		char* MessageId() { return mMessageId; };
		char* Message() { return mMessage; };
	};

	AppMessage::AppMessage( const char * messageId, const char* message )
	{
		mMessageId = _strdup( messageId );
		mMessage = _strdup( message );
	}

	AppMessage::~AppMessage()
	{
		if( mMessageId )
			free( mMessageId );
		if( mMessage )
			free( mMessage );
	}

   static AppConfig gAppConfig;

   AppConfig * AppConfig::smConfig = &gAppConfig;

   AppConfig::AppConfig()
   {
      setInitialDefaults();
      setupConfigParams();
      mDumpFile = NULL;
   }

   AppConfig::~AppConfig()
   {      
      clearConfigLists();
      clearConfigParams();
      if( mDumpFile )
      {
         mDumpFile->close();
         delete mDumpFile;
      }
      if (smConfig==this)
         smConfig=&gAppConfig;
   }

   void AppConfig::setInitialDefaults()
   {
      mEnableSequences = true;
      mExportOptimized = true;
      mAllowUnusedMeshes = true;
      mAllowCollapseTransform = true;
      mNoMipMap = false;
      mNoMipMapTranslucent = false;
      mZapBorder = true;
      mAnimationDelta = 0.0001f;
      mSameVertTOL = 0.00005f;
      mSameNormTOL = 0.005f;
      mSameVertTOL = 0.00005f;
      mWeightThreshhold = 0.001f;
      mWeightsPerVertex = 10;
      mCyclicSequencePadding = 1.0f / 30.0f;
      mAppFramesPerSec = 30.0f;
      mDumpConfig = 0xFFFFFFFF;
      mErrorString = NULL;
		mProgressCallback = NULL;
      mIgnoreSmoothingGroupOnSkinMesh = true;
      mIgnoreSmoothingGroupDuringCollapse = false;
      clearConfigLists();
   }

   void AppConfig::clearConfigLists()
   {
      S32 i;

      for (i=0; i<mAlwaysExport.size(); i++)
         delete [] mAlwaysExport[i];
      mAlwaysExport.clear();

      for (i=0; i<mNeverExport.size(); i++)
         delete [] mNeverExport[i];
      mNeverExport.clear();

      for (i=0; i<mNeverAnimate.size(); i++)
         delete [] mNeverAnimate[i];
      mNeverAnimate.clear();

      for (i=0; i<mErrorMessages.size(); i++)
         delete mErrorMessages[i];
      mErrorMessages.clear();

      for (i=0; i<mWarningMessages.size(); i++)
         delete mWarningMessages[i];
      mWarningMessages.clear();
   }

   void AppConfig::setConfig(AppConfig * config)
   {
      assert(config != NULL);

      if (smConfig!=&gAppConfig)
         delete smConfig;
      smConfig = config;
   }

   bool AppConfig::setDumpFile(const char * path, const char * name)
   {
      // open dump file in path specified, with optional name (use dump.dmp by default)
      // path can include full file name...can also include \,/, or : as path delimitors
      assert(path && "No path set on dump file");
      if (name==NULL)
         name = "dump.html";

      char * fullpath = getFilePath(path,strlen(name)+1);
      strcat(fullpath,name);

      mDumpFile = new std::ofstream();
      mDumpFile->open(fullpath,std::ofstream::binary);
		if( !mDumpFile->fail() )
		{
			*mDumpFile << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n";
			*mDumpFile << "<html>\r\n<head>\r\n<title>DTS Exporter Report</title>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\r\n";
			*mDumpFile << "<style type=\"text/css\">\r\n<!--\r\n";
			*mDumpFile << "body {\r\nfont-family: Verdana, Arial, Helvetica, sans-serif;\r\nfont-size: 14px;\r\nfont-weight: normal;\r\ncolor: #000000;\r\n}\r\n";
			*mDumpFile << ".monoText {\r\nfont-size: 12px;\r\nline-height: normal;\r\nfont-family: \"Courier New\", Courier, mono;\r\n}\r\n";
			*mDumpFile << ".errors {\r\nbackground-color: #FF9999;\r\nborder-width: 5;\r\nborder-style: solid;\r\nborder-color: #FF0000;\r\npadding: 4;\r\n}\r\n";
			*mDumpFile << ".warnings {\r\nbackground-color: #FFF8AA;\r\nborder-width: 5;\r\nborder-style: solid;\r\nborder-color: #FFCC00;\r\npadding: 4;\r\n}\r\n";
			*mDumpFile << ".header {\r\nfont-size: 16px;\r\nline-height: 24px;\r\nfont-family:Verdana, Arial, Helvetica, sans-serif;\r\nfont-weight: bold;\r\npadding-left: 0px;\r\n}\r\n";
			*mDumpFile << "-->\r\n</style>\r\n</head>\r\n<body>\r\n<div class=\"header\">Exporter Report</div>\r\n<pre class=\"monoText\">\r\n";
			mDumpFile->flush();
		}
      return !mDumpFile->fail();
   }

   bool AppConfig::closeDumpFile()
   {
		S32 i;

		*mDumpFile << "</pre>";
		*mDumpFile << "<div class=\"warnings\">\r\n<div class=\"header\">Warnings:</div>\r\n";
		if( mWarningMessages.size() == 0 )
			*mDumpFile << "<div>No warnings.</div>";
		else
			for (i=0; i<mWarningMessages.size(); i++)
				*mDumpFile << "<div><a href=\"http://artist.garagegames.com/dts/warnings/" << mWarningMessages[i]->MessageId() << "\">Warning #" << mWarningMessages[i]->MessageId() << "</a>:" << mWarningMessages[i]->Message() << "</div>";
		*mDumpFile << "</div>\r\n<br>\r\n";
		*mDumpFile << "<div class=\"errors\">\r\n<div class=\"header\">Errors:</div>\r\n";
		if( mErrorMessages.size() == 0 )
			*mDumpFile << "<div>No errors.</div>";
		else
			for (i=0; i<mErrorMessages.size(); i++)
				*mDumpFile << "<div><a href=\"http://artist.garagegames.com/dts/errors/" << mErrorMessages[i]->MessageId() << "\">Error #" << mErrorMessages[i]->MessageId() << "</a>:" << mErrorMessages[i]->Message() << "</div>";
		*mDumpFile << "</div>\r\n";
		*mDumpFile << "</body>\r\n</html>\r\n";
      mDumpFile->flush();
      mDumpFile->close();
      delete mDumpFile;
      mDumpFile = NULL;
      return true;
   }

   void AppConfig::printDump(U32 mask, const char * str)
   {
      if (mask && AppConfig::GetDumpMask())
      {
         *mDumpFile << str;
         mDumpFile->flush();
      }
   }

	void AppConfig::printWarning( U32 mask, const char * warningId, const char* warningMessage )
	{
		if (mask && AppConfig::GetDumpMask())
		{
			mWarningMessages.push_back( new AppMessage( warningId, warningMessage ) );
			printDump( mask, warningMessage );
		}
	}

	void AppConfig::printError( U32 mask, const char * errorId, const char* errorMessage )
	{
		if (mask && AppConfig::GetDumpMask())
		{
			mErrorMessages.push_back( new AppMessage( errorId, errorMessage ) );
			printDump( mask, errorMessage );
		}
   }

   bool AppConfig::alwaysExport(AppNode * node)
   {
      const char * name = node->getName();
      for (S32 i=0; i<mAlwaysExport.size(); i++)
         if (stringEqual(name,mAlwaysExport[i]))
            return true;
      return false;
   }

   bool AppConfig::neverExport(AppNode * node)
   {
      const char * name = node->getName();
      for (S32 i=0; i<mNeverExport.size(); i++)
         if (stringEqual(name,mNeverExport[i]))
            return true;
      return false;
   }

   bool AppConfig::neverAnimate(AppNode * node)
   {
      const char * name = node->getName();
      for (S32 i=0; i<mNeverAnimate.size(); i++)
         if (stringEqual(name,mNeverAnimate[i]))
            return true;
      return false;
   }

   S32 AppConfig::getParamEntry(const char * name, std::vector<char *> & nameTable)
   {
      for (S32 i=0; i<nameTable.size(); i++)
      {
         if (!strcmp(name,nameTable[i]))
            return i;
      }
      return -1;
   }

   bool AppConfig::searchConfigFile(const char * filename)
      {
      const char * ext = strrchr(filename,'.');
      if (!ext)
         ext = ""; // just use filename as base

         const char * defaultName = "dtsScene.cfg";
         const char * pos = filename+strlen(filename)-strlen(ext);
         char * newname = new char[strlen(filename)+strlen(defaultName)+1];
         strcpy(newname,filename);

         // try using base filename with .cfg suffix
         strcpy(newname+(pos-filename),".cfg");
      if (readConfigFile(newname))
         {
            delete [] newname;
            return true;
         }

         // try using base filename except no number with .cfg suffix
         do
            pos--;
         while (pos>=filename && *pos<='9' && *pos>='0');
         pos++;
         strcpy(newname+(pos-filename),".cfg");
      if (readConfigFile(newname))
         {
            delete [] newname;
            return true;
         }

         // try using dtsScene.cfg
         do
            pos--;
         while (pos>=filename && *pos!='\\' && *pos!='/' && *pos!=':');
            pos++;
         strcpy(newname+(pos-filename),defaultName);
      if (readConfigFile(newname))
         {
            delete [] newname;
            return true;
         }

         // no config file
         delete [] newname;
         return false;
      }

   bool AppConfig::readConfigFile(const char * filename)
   {
      clearConfigLists();

      std::ifstream is;

      is.open(filename);
      if (!is.is_open())
      {
         printWarning(PDAlways,"101",avar("\r\nConfig file \"%s\" not found.\r\n",filename));
         return false;
      }

      printDump(PDAlways,avar("\r\nBegin reading config file \"%s\".\r\n",filename));

      S32 mode = 0; // 0=AlwaysExport:, 1=NeverExport:, 2=NeverAnimate:
      char buffer[256];
      do
      {
         is.getline(buffer,sizeof(buffer));
         if (stringEqual(buffer,"AlwaysExport:*"))
            mode = 0;
         else if (stringEqual(buffer,"NeverExport:*"))
            mode = 1;
         else if (stringEqual(buffer,"NeverAnimate:*"))
            mode = 2;
         else if (buffer[0]=='+' || buffer[0]=='-')
         {
            bool newVal = buffer[0]=='+';
            S32 idx = getParamEntry(buffer+1,mBoolParamNames);
            if (idx>=0)
            {
               if (mBoolParamBit[idx])
               {
                  if (newVal)
                     *mBoolParams[idx] |= mBoolParamBit[idx];
                  else
                     *mBoolParams[idx] &= ~mBoolParamBit[idx];
               }
               else
                  *mBoolParams[idx] = newVal;
               printDump(PDAlways,avar("%s %s.\r\n",mBoolParamNames[idx],newVal ? "enabled" : "disabled"));
            }
            else
               printDump(PDAlways,avar("Unknown bool parameter \"%s\"\r\n",buffer+1));
         }
         else if (buffer[0]=='=')
         {
            char * endName = strchr(buffer+1,' ');
            if (endName)
            {
               *endName = '\0';
               S32 idx1 = getParamEntry(buffer+1,mFloatParamNames);
               S32 idx2 = getParamEntry(buffer+1,mStringParamNames);
               S32 idx3 = getParamEntry(buffer+1,mIntParamNames);
               if (idx1>=0)
               {
                  // Float
                  *mFloatParams[idx1] = F32(atof(endName+1));
                  printDump(PDAlways,avar("%s = %f\r\n",mFloatParamNames[idx1],*mFloatParams[idx1]));
               }
               if (idx2>=0)
               {
                  // string
                  S32 maxLen = mStringParamMaxLen[idx2];
                  strncpy(mStringParams[idx2],endName+1,maxLen-1);
                  mStringParams[idx2][maxLen]='\0';
                  printDump(PDAlways,avar("%s = \"%s\"\r\n",mStringParamNames[idx2],mStringParams[idx2]));
               }
               if (idx3>=0)
               {
                  // S32
                  *mIntParams[idx3] = atoi(endName+1);
                  printDump(PDAlways,avar("%s = %i\r\n",mIntParamNames[idx3],*mIntParams[idx3]));
               }
               if (idx1<0 && idx2<0 && idx3<0)
                  printDump(PDAlways,avar("Unknown parameter \"%s\"\r\n",buffer+1));
            }
         }
         else if (buffer[0]!='\\' && buffer[0]!='/' && buffer[0]!=';')
         {
            char * name = buffer;
            while (*name==' ')
               name++;
            if (strlen(name))
            {
               if (mode == 0)
               {
                  mAlwaysExport.push_back(strnew(buffer));
                  printDump(PDAlways,avar("Always export node: \"%s\"\r\n",buffer));
               }
               else if (mode == 1)
               {
                  mNeverExport.push_back(strnew(buffer));
                  printDump(PDAlways,avar("Never export node: \"%s\"\r\n",buffer));
               }
               else if (mode == 2)
               {
                  mNeverAnimate.push_back(strnew(buffer));
                  printDump(PDAlways,avar("Never animate transform on node: \"%s\"\r\n",buffer));
               }
            }
         }
      } while (is.good());

      printDump(PDAlways,"End reading config file.\r\n");
      return true;
   }

   void AppConfig::writeConfigFile(const char * filename)
   {
      S32 i;

      std::ofstream os;

      os.open(filename);

      os << "AlwaysExport:" << std::endl;
      for (i=0; i<mAlwaysExport.size(); i++)
         os << mAlwaysExport[i] << std::endl;

      os << "NeverExport:" << std::endl;
      for (i=0; i<mNeverExport.size(); i++)
         os << mNeverExport[i] << std::endl;

      os << "NeverAnimate:" << std::endl;
      for (i=0; i<mNeverAnimate.size(); i++)
         os << mNeverAnimate[i] << std::endl;

      for (i=0; i<mBoolParamNames.size(); i++)
      {
         bool enabled = mBoolParamBit[i] ? (mBoolParamBit[i] & *mBoolParams[i]) != 0 : *mBoolParams[i]!=0;
         os << (enabled ? "+" : "-") << mBoolParamNames[i] << std::endl;
      }

      for (i=0; i<mFloatParamNames.size(); i++)
         os << mFloatParamNames[i] << "= " << *mFloatParams[i] << std::endl;

      for (i=0; i<mIntParamNames.size(); i++)
         os << mIntParamNames[i] << "= " << *mIntParams[i] << std::endl;

      for (i=0; i<mStringParamNames.size(); i++)
         os << mStringParamNames[i] << "= " << mStringParams[i] << std::endl;

      os.close();
   }

   void AppConfig::setupConfigParams()
   {
      // add bool config parameters
      mBoolParamNames.push_back(strnew("Dump::NodeCollection"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDPass1);

      mBoolParamNames.push_back(strnew("Dump::ShapeConstruction"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDPass2);

      mBoolParamNames.push_back(strnew("Dump::NodeCulling"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDPass3);

      mBoolParamNames.push_back(strnew("Dump::NodeStates"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDNodeStates);

      mBoolParamNames.push_back(strnew("Dump::NodeStateDetails"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDNodeStateDetails);

      mBoolParamNames.push_back(strnew("Dump::ObjectStates"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDObjectStates);

      mBoolParamNames.push_back(strnew("Dump::ObjectStateDetails"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDObjectStateDetails);

      mBoolParamNames.push_back(strnew("Dump::ObjectOffsets"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDObjectOffsets);

      mBoolParamNames.push_back(strnew("Dump::SequenceDetails"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDSequences);

      mBoolParamNames.push_back(strnew("Dump::ShapeHierarchy"));
      mBoolParams.push_back(&mDumpConfig);
      mBoolParamBit.push_back(PDShapeHierarchy);

      mBoolParamNames.push_back(strnew("Error::AllowUnusedMeshes"));
      mBoolParams.push_back((U32*)&mAllowUnusedMeshes);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("Param::CollapseTransforms"));
      mBoolParams.push_back((U32*)&mAllowCollapseTransform);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("Param::SequenceExport"));
      mBoolParams.push_back((U32*)&mEnableSequences);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("Materials::NoMipMap"));
      mBoolParams.push_back((U32*)&mNoMipMap);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("Materials::NoMipMapTranslucent"));
      mBoolParams.push_back((U32*)&mNoMipMapTranslucent);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("Materials::ZapBorder"));
      mBoolParams.push_back((U32*)&mZapBorder);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("SmoothingGroup::IgnoreOnSkinMesh"));
      mBoolParams.push_back((U32*)&mIgnoreSmoothingGroupOnSkinMesh);
      mBoolParamBit.push_back(0);

      mBoolParamNames.push_back(strnew("SmoothingGroup::IgnoreDuringCollapse"));
      mBoolParams.push_back((U32*)&mIgnoreSmoothingGroupDuringCollapse);
      mBoolParamBit.push_back(1);

      // add F32 config parameters
      mFloatParamNames.push_back(strnew("Params::AnimationDelta"));
      mFloatParams.push_back(&mAnimationDelta);

      mFloatParamNames.push_back(strnew("Params::SkinWeightThreshhold"));
      mFloatParams.push_back(&mWeightThreshhold);

      mFloatParamNames.push_back(strnew("Params::SameVertTOL"));
      mFloatParams.push_back(&mSameVertTOL);

      mFloatParamNames.push_back(strnew("Params::SameTVertTOL"));
      mFloatParams.push_back(&mSameTVertTOL);

      // add S32 config parameters
      mIntParamNames.push_back(strnew("Params::weightsPerVertex"));
      mIntParams.push_back(&mWeightsPerVertex);
   }

   void AppConfig::clearConfigParams()
   {
      S32 i;
      for (i=0; i<mBoolParamNames.size(); i++)
         delete [] mBoolParamNames[i];
      for (i=0; i<mFloatParamNames.size(); i++)
         delete [] mFloatParamNames[i];
      for (i=0; i<mIntParamNames.size(); i++)
         delete [] mIntParamNames[i];
      for (i=0; i<mStringParamNames.size(); i++)
         delete [] mStringParamNames[i];

      mBoolParamNames.clear();
      mBoolParams.clear();
      mBoolParamBit.clear();
      mFloatParamNames.clear();
      mFloatParams.clear();
      mIntParamNames.clear();
      mIntParams.clear();
      mStringParamNames.clear();
      mStringParams.clear();
      mStringParamMaxLen.clear();
   }

	// progress handling
	void AppConfig::setProgressCallback(progressfnptr callback)
	{
		mProgressCallback = callback;
	}
	
	void AppConfig::setProgress(F32 minor, F32 major, const char* message)
	{
		if (mProgressCallback)
		{
			mProgressCallback(minor, major, message);
		}
	}
}; // namespace DTS


