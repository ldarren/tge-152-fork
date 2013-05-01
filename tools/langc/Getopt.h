/*
** Slab NG - The Next Generation of Slab
** (c) Copyright 2002-2004 Tom Bampton
**     All Rights Reserved.
**
** $Id: Getopt.h,v 1.1 2003/10/30 23:55:29 tom Exp $
**
** Filename:    Getopt.h
** Author:      Tom Bampton
** Created:     30/10/2003
** Purpose:
**   Command Line Parser
**
*/

//////////////////////////////////////////////////////////////////////////
/// \file Getopt.h
/// \brief Header for Getopt
//////////////////////////////////////////////////////////////////////////

#ifndef SLAB_GETOPT_H
#define SLAB_GETOPT_H

#define GO_BAD_CHAR		'?'
#define GO_BAD_ARG		':'
#define GO_INVALID_CHAR	'!'

//////////////////////////////////////////////////////////////////////
/// \brief Command Line Parser
///
/// Getopt provides a command line parser similar to Unix's getopt()
///
/// Note that this version of getopt() will not print any messages to
/// the terminal, you will need to manage this yourself.
///
/// Parts of this section have been lifted from the getopt() manual page.
//////////////////////////////////////////////////////////////////////
class Getopt
{
private:
	char *m_sOptions;
	char *m_sPlace;
	int m_nargc;
	char **m_nargv;

public:
	/*! \brief Current argv Index
	 */
	int m_nOptInd;
	/*! \brief Current Option
	 */
	int m_nOptOpt;
	/*! \brief Argument to current option if applicable
	 */
	char *m_sOptArg;

	/*! \brief Construct a Getopt

		The string tells Getopt what arguments this program takes. It may
		contain the following elements: individual characters, and characters
		followed by a colon to indicate an option argument is to follow.  For
		example, an option string "x" recognizes an option ``-x'', and an
		option string "x:" recognizes an option and argu- ment ``-x argument''.
		It does not matter to Getopt if a following argument has leading
		white space.

		\param sOptions Option string
		\sa getopt(), Reset()
	 */
	Getopt(char *sOptions);
	virtual ~Getopt();

	/*! \brief Parse arguments

		On return from getopt(), m_sOptArg points to an option argument, if it
		is anticipated, and the variable m_nOptInd contains the index to the
		next argv argument for a subsequent call to getopt().  The variable
		m_nOptOpt saves the last known option character returned by getopt().

		The m_nOptInd variable is set to 1, but may be set to another value
		before a set of calls to getopt() in order to skip over more or less
		argv entries.

		In order to use getopt() to evaluate multiple sets of arguments, or to
		evaluate a single set of arguments multiple times, call Reset() before
		the second and each additional set of calls to getopt()

		The getopt() function returns -1 when the argument list is exhausted,
		or GO_INVALID_CHAR if a non-recognized option is encountered. You may
		use m_nOptOpt to find the invalid character, and display a warning. If
		an option takes an argument, but the user did not supply one on the
		command line, getopt() returns GO_BAD_ARG. m_nOptOpt will then contain
		the option, for printing of warning messages. The interpretation of
		options in the argument list may be cancelled by the option `--'
		(double dash) which causes getopt() to signal the end of argument
		processing and return -1.  When all options have been processed (i.e.,
		up to the first non-option argument), getopt() returns -1.

		\param nargc The argc from your main() function
		\param nargv The argv from your main() function
		\return -1 on end of processing, the character of the current option
				or one of the error values as described above.
		\sa Reset(), Getopt(char *sOptions)
	 */
	int getopt(int nargc, char **nargv);
	/*! \brief Reset getopt() for subsequent calls

		See the description of getopt() for information on Reset()

		\sa getopt()
	 */
	void Reset(void);
};

#endif // SLAB_GETOPT_H
