//This file was generated! Dont touch!
#pragma once

#ifndef inc_gen_gmr_makefile_h
#define inc_gen_gmr_makefile_h

/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
	Struct for handeling files
*/
typedef struct GmrFile
{
	char *name; ///< name of the file
	GList *deps; ///< dependancies, eg header files etc
}GmrFile;

/**
	Main struct for handeling the makefile
*/
typedef struct GmrMakefile
{
	char *path; ///< path to this makefile
	GList *targets; ///< a list with all targets (GmrTarget*)
}GmrMakefile;

GmrMakefile *gmr_makefile_init(GmrMakefile *this,const char *path);
void gmr_makefile_finalize(GmrMakefile *this);
void gmr_makefile_evaluate_targets(GmrMakefile *this);
void gmr_makefile_init_targets(GmrMakefile *this);
void gmr_makefile_dump(GmrMakefile *this);

#endif
