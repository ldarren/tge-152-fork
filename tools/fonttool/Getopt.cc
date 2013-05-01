/*
** Slab NG - The Next Generation of Slab
** (c) Copyright 2002-2004 Tom Bampton
**     All Rights Reserved.
**
** $Id: Getopt.cpp,v 1.1 2003/10/30 23:55:29 tom Exp $
**
** Filename:    Getopt.cpp
** Author:      Tom Bampton
** Created:     30/10/2003
** Purpose:
**   Command Line Parser
**
*/

/*
 * Based on getopt.c from FreeBSD, bearing the following copyright message:
 *
 * Copyright (c) 1987, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "platform/platform.h"
#include "Getopt.h"


#define EMSG	""

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Getopt::Getopt(char *sOptions)
{
	m_sOptions = sOptions;
	m_sPlace = EMSG;
	m_nOptInd = 1;
}

Getopt::~Getopt()
{

}

int Getopt::getopt(int nargc, char **nargv)
{
	char *oli;				/* option letter list index */

	m_nargc = nargc;
	m_nargv = nargv;

	if (!*m_sPlace) {		/* update scanning pointer */
		if (m_nOptInd >= nargc || *(m_sPlace = nargv[m_nOptInd]) != '-') {
			m_sPlace = EMSG;
			return -1;
		}
		if (m_sPlace[1] && *++m_sPlace == '-') {	/* found "--" */
			++m_nOptInd;
			m_sPlace = EMSG;
			return -1;
		}
	} /* option letter okay? */
	if ((m_nOptOpt = (int)*m_sPlace++) == (int)':' ||
	    !(oli = dStrchr(m_sOptions, m_nOptOpt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means -1.
		 */
		if (m_nOptOpt == (int)'-')
			return (-1);
		if (!*m_sPlace)
			++m_nOptInd;
		if (*m_sOptions != ':' && m_nOptOpt != GO_BAD_CHAR)
			return GO_INVALID_CHAR;
		return (GO_BAD_CHAR);
	}
	if (*++oli != ':') {			/* don't need argument */
		m_sOptArg = NULL;
		if (!*m_sPlace)
			++m_nOptInd;
	}
	else {					/* need an argument */
		if (*m_sPlace)			/* no white space */
			m_sOptArg = m_sPlace;
		else if (nargc <= ++m_nOptInd) {	/* no arg */
			m_sPlace = EMSG;
			//if (*m_sOptions == ':')
			return (GO_BAD_ARG);
			//return (GO_BAD_CHAR);
		}
	 	else				/* white space */
			m_sOptArg = nargv[m_nOptInd];
		m_sPlace = EMSG;
		++m_nOptInd;
	}
	return (m_nOptOpt);			/* dump back option letter */
}

void Getopt::Reset(void)
{
	if (!*m_sPlace) {		/* update scanning pointer */
		if (m_nOptInd >= m_nargc || *(m_sPlace = m_nargv[m_nOptInd]) != '-') {
			m_sPlace = EMSG;
			return;
		}
		if (m_sPlace[1] && *++m_sPlace == '-') {	/* found "--" */
			++m_nOptInd;
			m_sPlace = EMSG;
			return;
		}
	}
}
