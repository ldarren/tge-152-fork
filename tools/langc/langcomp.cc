//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/event.h"
#include "platform/platformAssert.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "math/mathTypes.h"
#include "langc/langc.h"

#include "core/fileStream.h"

#include "i18n/lang.h"

#include <stdlib.h>

#include "langcomp.h"


//////////////////////////////////////////////////////////////////////////
// Simple hash function for ID lookups in translations
//////////////////////////////////////////////////////////////////////////

static U32 hashString(const UTF8 *str)
{
	S32 i;
	U32 h = 0;
	
	for(i = 0;str[i];i++)
	{
		h += (str[i] * i);
	}
	return h;
}

//////////////////////////////////////////////////////////////////////////
// LFileWriter Class
//////////////////////////////////////////////////////////////////////////

LFileWriter::LFileWriter(LangFile *langFile) : mLangFile(langFile)
{
}

bool LFileWriter::Open(const UTF8 *basename)
{
	UTF8 filename[256];
	
	GetFilename(basename, filename, sizeof(filename));
	return mStream.open((const char*)filename, FileStream::Write);
}

void LFileWriter::Close()
{
	mStream.close();
}

//////////////////////////////////////////////////////////////////////////
// Writer Classes
//////////////////////////////////////////////////////////////////////////

class LHeaderWriter : public LFileWriter
{
public:
	LHeaderWriter(LangFile *l) : LFileWriter(l)
	{
	}

	virtual void GetFilename(const UTF8 *basename, UTF8 *buf, U32 bufsize)
	{
		dSprintf(buf, bufsize, "%s.h", basename);
	}

	virtual void WriteHeader()
	{
		mStream.writeLine((U8 *)"// Automatically generated. DO NOT EDIT\n");
		mStream.writeLine((U8 *)"#ifndef _TGE_I18N_AUTOGEN_H_");
		mStream.writeLine((U8 *)"#define _TGE_I18N_AUTOGEN_H_\n");
	}

	virtual void WriteFooter()
	{
		UTF8 buf[LCL_MAXSTRINGLENGTH + 4];
		
		dSprintf(buf, sizeof(buf), "\n#define I18N_NUM_STRINGS\t%d", mLangFile->getNumStrings());
		mStream.writeLine((U8 *)&buf);
		
		mStream.writeLine((U8 *)"\n#endif // _TGE_I18N_AUTOGEN_H_");
	}
	
	virtual void WriteString(UTF8 *idstr, U32 idnum, UTF8 *str)
	{
		UTF8 buf[LCL_MAXSTRINGLENGTH + 4];
		
		dSprintf(buf, sizeof(buf), "// %s", str);
		mStream.writeLine((U8 *)&buf);
		dSprintf(buf, sizeof(buf), "#define %s\t%d", idstr, idnum);
		mStream.writeLine((U8 *)&buf);
	}
};

class LScriptWriter : public LFileWriter
{
public:
	LScriptWriter(LangFile *l) : LFileWriter(l)
	{
	}

	virtual void GetFilename(const UTF8 *basename, UTF8 *buf, U32 bufsize)
	{
		dSprintf(buf, bufsize, "%s.cs", basename);
	}

	virtual void WriteHeader()
	{
		mStream.writeLine((U8 *)"// Automatically generated. DO NOT EDIT\n");
	}
	
	virtual void WriteFooter()
	{
	}
	
	virtual void WriteString(UTF8 *idstr, U32 idnum, UTF8 *str)
	{
		UTF8 buf[LCL_MAXSTRINGLENGTH + 4];
		
		dSprintf(buf, sizeof(buf), "// %s", str);
		mStream.writeLine((U8 *)&buf);
		dSprintf(buf, sizeof(buf), "$%s = %d;", idstr, idnum);
		mStream.writeLine((U8 *)&buf);
	}
};

class LDefaultsWriter : public LFileWriter
{
public:
	LDefaultsWriter(LangFile *l) : LFileWriter(l)
	{
	}

	virtual void GetFilename(const UTF8 *basename, UTF8 *buf, U32 bufsize)
	{
		dSprintf(buf, bufsize, "%sDefaults.cc", basename);
	}

	virtual void WriteHeader()
	{
		mStream.writeLine((U8 *)"// Automatically generated. DO NOT EDIT\n");
		mStream.writeLine((U8 *)"const UTF8 *gI18NDefaultStrings[] =\n{");
	}
	
	virtual void WriteFooter()
	{
		mStream.writeLine((U8 *)"};");
	}
	
	virtual void WriteString(UTF8 *idstr, U32 idnum, UTF8 *str)
	{
		UTF8 buf[LCL_MAXSTRINGLENGTH + 4];
	
		dSprintf(buf, sizeof(buf), "\t// %s = %d", idstr, idnum);
		mStream.writeLine((U8 *)&buf);
		dSprintf(buf, sizeof(buf), "\t(const UTF8*)\"%s\",", str);
		mStream.writeLine((U8 *)&buf);
	}
};

class LTransWriter : public LFileWriter
{
public:
	LTransWriter(LangFile *l) : LFileWriter(l)
	{
	}
	
	virtual void GetFilename(const UTF8 *basename, UTF8 *buf, U32 bufsize)
	{
		dSprintf(buf, bufsize, "%s.tran", basename);
	}
	
	virtual void WriteHeader()
	{
		mStream.writeLine((U8 *)"# Automatically generated.\n");
	}
	
	virtual void WriteFooter()
	{
	}
	
	virtual void WriteString(UTF8 *idstr, U32 idnum, UTF8 *str)
	{
		UTF8 buf[LCL_MAXSTRINGLENGTH + 4];
		
		dSprintf(buf, sizeof(buf), "# [%s:%d] %s", idstr, idnum, str);
		mStream.writeLine((U8 *)&buf);
		dSprintf(buf, sizeof(buf), "%s=\n", idstr);
		mStream.writeLine((U8 *)&buf);
	}
};

//////////////////////////////////////////////////////////////////////////
// LString Class
//////////////////////////////////////////////////////////////////////////

LString::LString(UTF8 *i /* = NULL */, UTF8 *s /* = NULL */, UTF8 *sclean /* = NULL */) : str(NULL), strclean(NULL), id(NULL)
{
	if(s)
	{
		str = new UTF8 [dStrlen(s) + 1];
		dStrcpy(str, s);
	}

	if(sclean)
	{
		strclean = new UTF8 [dStrlen(sclean) + 1];
		dStrcpy(strclean, sclean);
	}
	
	if(i)
	{
		id = new UTF8 [dStrlen(i) + 1];
		dStrcpy(id, i);
	}
}

LString::~LString()
{
	if(str)
		delete [] str;
	if(id)
		delete [] id;
}

//////////////////////////////////////////////////////////////////////////
// LangComp Class
//////////////////////////////////////////////////////////////////////////

LangComp::LangComp(U32 options /* = 0L */) : mOptions(options), mLangFile(NULL),
	mCurrentFilename(NULL), mCurrentLine(0), mNumErrors(0), mNumWarn(0),
	mTransLookup(LC_ID_LOOKUP_SIZE)
{
}

LangComp::~LangComp()
{
	Free();
}

//////////////////////////////////////////////////////////////////////////
// Protected Methods
//////////////////////////////////////////////////////////////////////////

void LangComp::Free()
{
	if(mLangFile)
	{
		delete mLangFile;
		mLangFile = NULL;
	}
	
	if(mCurrentFilename)
	{
		delete [] mCurrentFilename;
		mCurrentFilename = NULL;
	}
	mCurrentLine = 0;

	S32 i;
	for(i = 0;i < mFileWriters.size();i++)
	{
		delete mFileWriters[i];
	}
	mFileWriters.empty();
}

void LangComp::Error(const UTF8 *msg, ...)
{
	UTF8 buf[512];
	va_list va;

	va_start(va, msg);
	dVsprintf(buf, sizeof(buf), msg, va);
	va_end(va);

	if(mCurrentFilename)
		dPrintf("error at %s:%d: %s\n", mCurrentFilename, mCurrentLine, buf);
	else
		dPrintf("error: %s\n", buf);
	
	mNumErrors++;
}

void LangComp::Warn(const UTF8 *msg, ...)
{
	UTF8 buf[512];
	va_list va;
	
	if(mOptions & LCO_NOWARNINGS)
		return;

	va_start(va, msg);
	dVsprintf(buf, sizeof(buf), msg, va);
	va_end(va);
	
	if(mCurrentFilename)
		dPrintf("warning at %s:%d: %s\n", mCurrentFilename, mCurrentLine, buf);
	else
		dPrintf("warning: %s\n", buf);

	mNumWarn++;
}

LString * LangComp::ParseLine(UTF8 *line)
{
	UTF8 *p, id[LCL_MAXIDLENGTH], str[LCL_MAXSTRINGLENGTH];
	S32 i, warnCount;

	i = dStrlen(line)-1;
	if(line[i] == '\n') line[i] = 0;
	i = dStrlen(line)-1;
	if(line[i] == '\r') line[i] = 0;
	
	p = line;
	
	// Allowable comment delimiters: # ; //
	if(*p == '#' || *p == ';' || (*p == '/' && *(p+1) == '/') || *p == 0)
		return new LString();

	i = 0;
	while(*p)
	{
		if(*p == '=')
			break;

		if(i < (sizeof(id)-1))
		{
			if(dIsalnum(*p) || *p == '_')
			{
				id[i] = *p;
				i++;
			}
			else if(! dIsspace(*p) && ! (mOptions & LCO_DONTWARNINVALIDCHAR))
				Warn("invalid character ('%c') in identifier ignored", *p);
		}
		
		p++;
	}
	id[i] = 0;
	p++;

	// Identifiers cannot start with a number
	if(dIsdigit(id[0]))
	{
		Error("identifiers cannot start with a number");
		return NULL;
	}
	
	// Identifiers must be there
	if(id[0] == 0)
	{
		Error("no identifier");
		return NULL;
	}

	i = 0;
	bool foundStart = (mOptions & LCO_DONTSTRIPSPACES);
	while(*p)
	{
		if(i < (sizeof(str)-1))
		{
			if(!foundStart && ! dIsspace(*p))
				foundStart = true;

			if(foundStart && i < (sizeof(str)-1))
			{
				str[i] = *p;
				i++;
			}
		}
		
		p++;
	}
	str[i] = 0;

	if(mOptions & LCO_STRIPTRAILINGSPACE)
	{
		p = dStrchr(str, 0);
		while(dIsspace(*(--p)))
			*p = 0;
	}

	if(mOptions & LCO_WARNNOSTRING && str[0] == 0)
	{
		Warn("identifier '%s' is empty", id);
	}

	UTF8 strbuf[LCL_MAXSTRINGLENGTH];
	processSlashes(str, strbuf);

	return new LString(id, strbuf, str);
}

void LangComp::WriteString(UTF8 *idstr, U32 idnum, UTF8 *str)
{
	S32 i;

	for(i = 0;i < mFileWriters.size();i++)
	{
		mFileWriters[i]->WriteString(idstr, idnum, str);
	}
}

void LangComp::WriteFileHeader()
{
	S32 i;
	
	for(i = 0;i < mFileWriters.size();i++)
	{
		mFileWriters[i]->WriteHeader();
	}
}

void LangComp::WriteFileFooter()
{
	S32 i;
	
	for(i = 0;i < mFileWriters.size();i++)
	{
		mFileWriters[i]->WriteFooter();
	}
}


// [tom, 3/7/2005] There is no buffer size limit here as the size of the buffer
// in ParseLine() is the same size as the buffer the string is in. If this function
// is ripped for use elsewhere, I suggest adding a buffer size check.
UTF8 *LangComp::processSlashes(const UTF8 *string, UTF8 *buffer)
{
	const UTF8 *s = string;
	UTF8 *d = buffer;

	while(*s)
	{
		if(*s == '\\')
		{
			s++;
			switch(*s)
			{
				case 'n':
					*d++ = '\n';
					s++;
					break;

				case 'r':
					*d++ = '\r';
					s++;
					break;

				case 't':
					*d++ = '\t';
					s++;
					break;

				default:
					*d++ = *s++;
					break;
			}
		}
		else
		{
			*d++ = *s++;
		}
	}
	*d = 0;

	return buffer;
}

// [tom, 3/17/2005] No buffer check here either, this time out of lazyness.
// [tom, 3/17/2005] This function isnt actually used anymore. I'm keeping it
// here in case its useful in the future.
UTF8 *LangComp::quoteString(const UTF8 *string, UTF8 *buffer)
{
	static struct {
		unsigned char c;
		unsigned char q;
	} quoteTab[]=
	{
		{ '\n', 'n' },
		{ '\r', 'r' },
		{ '\t', 't' },
		{ '"',	'"' },
		{ '\'',	'\'' },
		{ '\\',	'\\' },

		{ 0, 0 }
	};
	const UTF8 *s = string;
	UTF8 *d = buffer;

	while(*s)
	{
		int i;
		bool rep = false;

		for(i = 0;quoteTab[i].c;i++)
		{
			if(*s == quoteTab[i].c)
			{
				*d++ = '\\';
				*d++ = quoteTab[i].q;
				rep = true;
				break;
			}
		}

		if(!rep)
			*d++ = *s;

		s++;
	}
	*d = 0;

	return buffer;
}

bool LangComp::AddFileWriter(LFileWriter *lfw, const UTF8 *basename)
{
	if(lfw->Open(basename))
	{
		mFileWriters.push_back(lfw);
		return true;
	}

	UTF8 buf[256];
	lfw->GetFilename(basename, buf, sizeof(buf));
	Warn("Could not open file \"%s\"", buf);
	
	delete lfw;

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

bool LangComp::Compile(const UTF8 *filename, const UTF8 *outbasename, const UTF8 *englishTable /* = NULL */)
{
	bool ret = true;
	UTF8 lsoname[256];
	
	Free();

	if((mOptions & LCO_COMPILETRANSLATION) && englishTable == NULL)
	{
		Error("you must specify the english language file when compiling translations.");
		return false;
	}

	if(mOptions & LCO_COMPILETRANSLATION)
	{
		if(! LoadLangForTranslation(englishTable))
		{
			Error("could not load %s", englishTable);
			return false;
		}
	}

	dSprintf(lsoname, sizeof(lsoname), "%s.lso", outbasename);

	mCurrentFilename = new UTF8 [dStrlen(filename) + 1];
	dStrcpy(mCurrentFilename, filename);

	mLangFile = new LangFile;
	
	FileStream fs;
	if(fs.open(filename, FileStream::Read))
	{
		if(mOptions & LCO_WRITEHEADER)
			AddFileWriter(new LHeaderWriter(mLangFile), outbasename);

		if(mOptions & LCO_WRITESCRIPT)
			AddFileWriter(new LScriptWriter(mLangFile), outbasename);

		if(mOptions & LCO_WRITECDEFAULTS)
			AddFileWriter(new LDefaultsWriter(mLangFile), outbasename);

		if(mOptions & LCO_WRITETRANSLATION)
			AddFileWriter(new LTransWriter(mLangFile), outbasename);

		WriteFileHeader();

		// This buffer must be able to hold the max lengths, the equals, end of line chars and a bit of leeway for spaces
		UTF8 buf[LCL_MAXIDLENGTH + LCL_MAXSTRINGLENGTH + 28];
		
		while(fs.getStatus() != Stream::EOS)
		{
			mCurrentLine++;
			fs.readLine((U8 *)&buf[0], sizeof(buf));

			LString *ls;
			if((ls = ParseLine(buf)) == NULL)
				break;

			if(ls->id)
			{
				U32 idnum;
				if(mOptions & LCO_COMPILETRANSLATION)
				{
					idnum = (U32)mTransLookup.retreive(hashString(ls->id));
					if(idnum == 0)
						Warn("id %s does not exist in the english table", ls->id);
					else
						idnum--;
					mLangFile->setString(idnum, ls->str);
				}
				else
					idnum = mLangFile->addString(ls->str);
				
				WriteString(ls->id, idnum, ls->strclean);
			}
			
			delete ls;
		}

		WriteFileFooter();

		S32 i;
		for(i = 0;i < mFileWriters.size();i++)
		{
			mFileWriters[i]->Close();
		}

		fs.close();
	}

	if(mOptions & LCO_WRITELANGTABLE)
	{
		if(ret && fs.open(lsoname, FileStream::Write))
		{
			mLangFile->save(&fs);
			fs.close();
		}
	}

	
	dPrintf("%s - ", lsoname);
	if(ret)
		dPrintf("%d string(s), ", mLangFile->getNumStrings());
	dPrintf("%d error(s), %d warning(s)\n", mNumErrors, mNumWarn);

	return ret;
}

bool LangComp::LoadLangForTranslation(const UTF8 *filename)
{
	bool ret = true;
	FileStream fs;

	UTF8 *fnBak = mCurrentFilename;
	U32 lineBak = mCurrentLine;

	mCurrentFilename = new UTF8 [dStrlen(filename) + 1];
	dStrcpy(mCurrentFilename, filename);
	mCurrentLine = 0;

	if(fs.open(filename, FileStream::Read))
	{
		// This buffer must be able to hold the max lengths, the equals, end of line chars and a bit of leeway for spaces
		UTF8 buf[LCL_MAXIDLENGTH + LCL_MAXSTRINGLENGTH + 28];
		U32 lastID = 1;	// [tom, 4/25/2005] 0 is used as an error indicator
		
		while(fs.getStatus() != Stream::EOS)
		{
			mCurrentLine++;
			fs.readLine((U8 *)&buf[0], sizeof(buf));
			
			LString *ls;
			if((ls = ParseLine(buf)) == NULL)
				break;
			
			if(ls->id)
			{
				U32 h = hashString(ls->id);
				mTransLookup.insert((U32 *)lastID, h);
				lastID++;
			}
			
			delete ls;
		}

		fs.close();
	}

	delete [] mCurrentFilename;
	mCurrentFilename = fnBak;
	mCurrentLine = lineBak;

	return ret;
}
