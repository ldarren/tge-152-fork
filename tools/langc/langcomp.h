//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "core/tVector.h"
#include "tSimpleHash.h"

#ifndef _LANGCOMP_H_
#define _LANGCOMP_H_

// Limits
#define LCL_MAXIDLENGTH				128			// Maximum ID length
#define LCL_MAXSTRINGLENGTH		512			// Maximum string length

class LFileWriter
{
protected:
	FileStream mStream;
	LangFile *mLangFile;
	
public:
	LFileWriter(LangFile *langFile);

	virtual bool Open(const UTF8 *basename);
	virtual void Close();

	virtual void GetFilename(const UTF8 *basename, UTF8 *buf, U32 bufsize) = 0;
	
	virtual void WriteHeader() = 0;
	virtual void WriteFooter() = 0;

	virtual void WriteString(UTF8 *idstr, U32 idnum, UTF8 *str) = 0;
};

struct LString
{
	UTF8 *str;
	UTF8 *strclean;
	UTF8 *id;
	
	LString(UTF8 *i = NULL, UTF8 *s = NULL, UTF8 *sclean = NULL);
	virtual ~LString();
};

// [tom, 4/25/2005] This is the size of the SimpleHash table. Its created in
// the SimpleHash ctor so there is a trade off here between performance and
// memory usage, as the array will always be created. In practice, the memory
// usage probably isnt really going to be that horrendous for an empty table.
// It works out to 12 bytes multiplied by whatever you set LC_ID_LOOKUP_SIZE to.
//
// Since SimpleHash doesnt rehash, the optimal size depends entirely on the
// amount of strings in the table. starter.fps typically has a few hundred,
// whilst the C++ code typically has a few thousand. Compiling translations is
// still going to be pretty fast regardless of how bad the performance of
// the SparseArry is, so on the whole this comment probably doesnt really matter.
#define LC_ID_LOOKUP_SIZE		128

class LangComp
{
protected:

	U32 mOptions;

	LangFile *mLangFile;
	UTF8 *mCurrentFilename;
	U32 mCurrentLine;

	U32 mNumErrors;
	U32 mNumWarn;

	Vector<LFileWriter *> mFileWriters;
	SimpleHash<U32> mTransLookup;

	void Free(void);
	void Error(const UTF8 *msg, ...);
	void Warn(const UTF8 *msg, ...);

	LString * ParseLine(UTF8 *line);

	void WriteString(UTF8 *idstr, U32 idnum, UTF8 *str);

	void WriteFileHeader();
	void WriteFileFooter();

	UTF8 *processSlashes(const UTF8 *string, UTF8 *buffer);
	UTF8 *quoteString(const UTF8 *string, UTF8 *buffer);

	bool AddFileWriter(LFileWriter *lfw, const UTF8 *basename);

public:
	LangComp(U32 options = 0L);
	virtual ~LangComp();

	bool Compile(const UTF8 *filename, const UTF8 *outbasename, const UTF8 *englishTable = NULL);
	bool LoadLangForTranslation(const UTF8 *filename);
};

// Option Flags
#define LCO_DONTSTRIPSPACES			(1L)		// Don't strip leading spaces in strings
#define LCO_STRIPTRAILINGSPACE		(1L << 1)	// Strip trailing spaces in strings
#define LCO_DONTWARNINVALIDCHAR		(1L << 2)	// Don't warn for invalid characters
#define LCO_WARNNOSTRING			(1L << 3)	// Warn for empty identifiers
#define LCO_NOWARNINGS				(1L << 4)	// Don't warn at all

#define LCO_WRITEHEADER				(1L << 5)	// Write C++ Header file
#define LCO_WRITESCRIPT				(1L << 6)	// Write Script IDs file
#define LCO_WRITECDEFAULTS			(1L << 7)	// Write C++ defaults file
#define LCO_WRITECSDEFAULTS			(1L << 8)	// Write Script defaults file
#define LCO_WRITELANGTABLE			(1L << 9)	// Write Lang Table
#define LCO_WRITETRANSLATION		(1L << 10)	// Write an empty language file for translation

#define LCO_COMPILETRANSLATION		(1L << 11)	// Compile a translation file

#endif // _LANGCOMP_H_
