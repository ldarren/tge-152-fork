/**********************************************************************
 *<
   FILE: max2dtsExporterPlus.cpp

   DESCRIPTION:   Appwizard generated plugin

   CREATED BY: 

   HISTORY: 

 *>   Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "max2dtsExporterPro.h"
#include "maxSceneEnum.h"
#include "appConfig.h"
#include "resource.h"
#include <fstream>


#define MAX2DTSEXPORTERPLUS_CLASS_ID   Class_ID(0x612c5c7, 0x5924d160)
#define MAX2DSQEXPORTERPLUS_CLASS_ID	Class_ID(0x39ec6494, 0x2ed1496b)

HWND hProgressDialog;

void SetProgress(F32 minor, F32 major, const char* message)
{
	SendDlgItemMessage(hProgressDialog,IDC_MINOR_BAR,PBM_SETPOS, (int)(minor * 100.0f), 0 );
	SendDlgItemMessage(hProgressDialog,IDC_MAJOR_BAR,PBM_SETPOS, (int)(major * 100.0f), 0 );
	HWND hctl = GetDlgItem(hProgressDialog,IDC_MESSAGE);
	SetWindowText(hctl,message);
	InvalidateRect(hProgressDialog,NULL, TRUE);
	UpdateWindow(hProgressDialog);
}

void SetupProgressDialog()
{
	hProgressDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PROGRESS),GetActiveWindow(),NULL);
	CenterWindow(hProgressDialog,GetParent(hProgressDialog));
	ShowWindow(hProgressDialog, SW_SHOW);
}

void ShutdownProgressDialog()
{
	if(hProgressDialog)
	{
		DestroyWindow(hProgressDialog);
	}
}

class Max2dtsExporterPlus : public SceneExport {
   public:
      static HWND hParams;

      int            ExtCount();               // Number of extensions supported
      const TCHAR *   Ext(int n);               // Extension #n (i.e. "3DS")
      const TCHAR *   LongDesc();               // Long ASCII description (i.e. "Autodesk 3D Studio File")
      const TCHAR *   ShortDesc();            // Short ASCII description (i.e. "3D Studio")
      const TCHAR *   AuthorName();            // ASCII Author name
      const TCHAR *   CopyrightMessage();         // ASCII Copyright message
      const TCHAR *   OtherMessage1();         // Other message #1
      const TCHAR *   OtherMessage2();         // Other message #2
      unsigned int   Version();               // Version number * 100 (i.e. v3.01 = 301)
      void         ShowAbout(HWND hWnd);      // Show DLL's "About..." box

      BOOL SupportsOptions(int ext, DWORD options);
      int DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

      void alert(const char * title, const char * message, BOOL suppressPrompts);
      
      //Constructor/Destructor
      Max2dtsExporterPlus();
      ~Max2dtsExporterPlus();      
};

class Max2dsqExporterPlus : public SceneExport {
public:
	static HWND hParams;

	int            ExtCount();               // Number of extensions supported
	const TCHAR *   Ext(int n);               // Extension #n (i.e. "3DS")
	const TCHAR *   LongDesc();               // Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *   ShortDesc();            // Short ASCII description (i.e. "3D Studio")
	const TCHAR *   AuthorName();            // ASCII Author name
	const TCHAR *   CopyrightMessage();         // ASCII Copyright message
	const TCHAR *   OtherMessage1();         // Other message #1
	const TCHAR *   OtherMessage2();         // Other message #2
	unsigned int   Version();               // Version number * 100 (i.e. v3.01 = 301)
	void ShowAbout(HWND hWnd);      // Show DLL's "About..." box

	BOOL SupportsOptions(int ext, DWORD options);
	int            DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

	void alert(const char * title, const char * message, BOOL suppressPrompts);

	//Constructor/Destructor
	Max2dsqExporterPlus();
	~Max2dsqExporterPlus();      
};

class Max2dtsExporterPlusClassDesc:public ClassDesc2 {
   public:
   int          IsPublic() { return TRUE; }
   void *         Create(BOOL loading = FALSE) { return new Max2dtsExporterPlus(); }
   const TCHAR *   ClassName() { return GetString(IDS_CLASS_NAME); }
   SClass_ID      SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
   Class_ID      ClassID() { return MAX2DTSEXPORTERPLUS_CLASS_ID; }
   const TCHAR*    Category() { return GetString(IDS_CATEGORY); }

   const TCHAR*   InternalName() { return _T("Max2dtsExporterPlus"); }   // returns fixed parsable name (scripter-visible name)
   HINSTANCE      HInstance() { return hInstance; }            // returns owning module handle
};

class Max2dsqExporterPlusClassDesc:public ClassDesc2 {
public:
	int          IsPublic() { return TRUE; }
	void *         Create(BOOL loading = FALSE) { return new Max2dsqExporterPlus(); }
	const TCHAR *   ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID      SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID      ClassID() { return MAX2DSQEXPORTERPLUS_CLASS_ID; }
	const TCHAR*    Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*   InternalName() { return _T("Max2dsqExporterPlus"); }   // returns fixed parsable name (scripter-visible name)
	HINSTANCE      HInstance() { return hInstance; }            // returns owning module handle
};



static Max2dtsExporterPlusClassDesc Max2dtsExporterPlusDesc;
ClassDesc2* GetMax2dtsExporterPlusDesc() { return &Max2dtsExporterPlusDesc; }

static Max2dsqExporterPlusClassDesc Max2dsqExporterPlusDesc;
ClassDesc2* GetMax2dsqExporterPlusDesc() { return &Max2dsqExporterPlusDesc; }


BOOL CALLBACK Max2dtsExporterPlusOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
   static Max2dtsExporterPlus *imp = NULL;

   switch(message) {
      case WM_INITDIALOG:
         imp = (Max2dtsExporterPlus *)lParam;
         CenterWindow(hWnd,GetParent(hWnd));
         return TRUE;

      case WM_CLOSE:
         EndDialog(hWnd, 0);
         return TRUE;
   }
   return FALSE;
}


//--- Max2dtsExporterPlus -------------------------------------------------------
Max2dtsExporterPlus::Max2dtsExporterPlus()
{
	hProgressDialog = NULL;
}

Max2dtsExporterPlus::~Max2dtsExporterPlus() 
{

}

int Max2dtsExporterPlus::ExtCount()
{
   // Returns the number of file name extensions supported by the plug-in.
   return 1;
}

const TCHAR *Max2dtsExporterPlus::Ext(int n)
{      
   // Return the 'i-th' file name extension
	return _T("DTS");
}

const TCHAR *Max2dtsExporterPlus::LongDesc()
{
   //Return long ASCII description
   return _T("Torque game engine dts shape");
}
   
const TCHAR *Max2dtsExporterPlus::ShortDesc() 
{         
   //Return short ASCII description
   return _T("Torque DTS Shape");
}

const TCHAR *Max2dtsExporterPlus::AuthorName()
{         
   //Return ASCII Author name
   return _T("Dark Industries");
}

const TCHAR *Max2dtsExporterPlus::CopyrightMessage() 
{   
   // Return ASCII Copyright message
   return _T("Copyright Dark Industries");
}

const TCHAR *Max2dtsExporterPlus::OtherMessage1() 
{      
   //Return Other message #1 if any
   return _T("");
}

const TCHAR *Max2dtsExporterPlus::OtherMessage2() 
{      
   //Return other message #2 in any
   return _T("");
}

unsigned int Max2dtsExporterPlus::Version()
{            
   //Return Version number * 100 (i.e. v3.01 = 301)
   return 100;
}

void Max2dtsExporterPlus::ShowAbout(HWND hWnd)
{
   // Optional
}

BOOL Max2dtsExporterPlus::SupportsOptions(int ext, DWORD options)
{
   // Decide which options to support.  Simply return
   // true for each option supported by each Extension 
   // the exporter supports.

   return TRUE;
}

void Max2dtsExporterPlus::alert(const char * title, const char * message, BOOL suppressPrompts)
{
   if (suppressPrompts)
      DTS::AppConfig::PrintDump(-1,message);
   else
   {
      TSTR str1(message);
      TSTR str2(title);
      MessageBox(GetActiveWindow(), str1, str2, MB_OK);
   }
}

int Max2dtsExporterPlus::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
   // Implement the actual file Export here and 
   // return TRUE If the file is exported properly
	DTS::AppConfig::SetAppFramesPerSec(GetFrameRate());

   if (!DTS::AppConfig::SetDumpFile(name))
      alert("Dump file error",DTS::avar("Unable to create dumpfile for shape \"%s\".",name),suppressPrompts);

	DTS::AppConfig::SetExportOptimized(true);
	//DTS::AppConfig::PrintDump(-1,"Setting up for full export (dts) -- turning on optimized meshes\r\n");

   // Get config file if it exists...
   const char * configBase = i->GetCurFilePath();
   DTS::AppConfig::SearchConfigFile(configBase);

	SetupProgressDialog();
	DTS::AppConfig::SetProgressCallback(SetProgress);
	DTS::AppConfig::SetProgress(0.0f,0.0f,"Exploring scene...");

   DTS::MaxSceneEnum maxSceneEnum(ei->theScene);
   DTS::Shape * shape = maxSceneEnum.processScene();
   if (!DTS::AppConfig::IsExportError())
   {
      std::ofstream os;
      os.open(name,std::ofstream::binary);
      shape->save(os);
      os.close();
   }
   DTS::AppConfig::CloseDumpFile();
   delete shape;

	ShutdownProgressDialog();

   if (DTS::AppConfig::IsExportError())
      alert("Export error",DTS::AppConfig::GetExportError(),suppressPrompts);

   return !DTS::AppConfig::IsExportError();
}


//--- Max2dsqExporterPlus -------------------------------------------------------
Max2dsqExporterPlus::Max2dsqExporterPlus()
{

}

Max2dsqExporterPlus::~Max2dsqExporterPlus() 
{

}

int Max2dsqExporterPlus::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *Max2dsqExporterPlus::Ext(int n)
{      
	// Return the 'i-th' file name extension
	return _T("DSQ");
}

const TCHAR *Max2dsqExporterPlus::LongDesc()
{
	//Return long ASCII description
	return _T("Torque game engine dsq files");
}

const TCHAR *Max2dsqExporterPlus::ShortDesc() 
{         
	//Return short ASCII description
	return _T("Torque DSQ File");
}

const TCHAR *Max2dsqExporterPlus::AuthorName()
{         
	//Return ASCII Author name
	return _T("Dark Industries");
}

const TCHAR *Max2dsqExporterPlus::CopyrightMessage() 
{   
	// Return ASCII Copyright message
	return _T("Copyright Dark Industries");
}

const TCHAR *Max2dsqExporterPlus::OtherMessage1() 
{      
	//Return Other message #1 if any
	return _T("");
}

const TCHAR *Max2dsqExporterPlus::OtherMessage2() 
{      
	//Return other message #2 in any
	return _T("");
}

unsigned int Max2dsqExporterPlus::Version()
{            
	//Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void Max2dsqExporterPlus::ShowAbout(HWND hWnd)
{
	// Optional
}

BOOL Max2dsqExporterPlus::SupportsOptions(int ext, DWORD options)
{
	// Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

void Max2dsqExporterPlus::alert(const char * title, const char * message, BOOL suppressPrompts)
{
	if (suppressPrompts)
		DTS::AppConfig::PrintDump(-1,message);
	else
	{
		TSTR str1(message);
		TSTR str2(title);
		MessageBox(GetActiveWindow(), str1, str2, MB_OK);
	}
}

int Max2dsqExporterPlus::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	// Implement the actual file Export here and 
	// return TRUE If the file is exported properly
	DTS::AppConfig::SetAppFramesPerSec(GetFrameRate());

	if (!DTS::AppConfig::SetDumpFile(name))
		alert("Dump file error",DTS::avar("Unable to create dumpfile for shape \"%s\".",name),suppressPrompts);

	DTS::AppConfig::SetExportOptimized(false);
	//DTS::AppConfig::PrintDump(-1,"Setting up for sequence export only (dsq) -- turning off optimized meshes\r\n");

	// Get config file if it exists...
	const char * configBase = i->GetCurFilePath();
	DTS::AppConfig::SearchConfigFile(configBase);

	SetupProgressDialog();
	DTS::AppConfig::SetProgressCallback(SetProgress);

	DTS::MaxSceneEnum maxSceneEnum(ei->theScene);
	DTS::Shape * shape = maxSceneEnum.processScene();
	if (!DTS::AppConfig::IsExportError())
	{
		std::ofstream os;
		os.open(name,std::ofstream::binary);
		shape->saveSequences(os);
		os.close();
	}
	DTS::AppConfig::CloseDumpFile();
	delete shape;

	ShutdownProgressDialog();

	if (DTS::AppConfig::IsExportError())
		alert("Export error",DTS::AppConfig::GetExportError(),suppressPrompts);

	return !DTS::AppConfig::IsExportError();
}


