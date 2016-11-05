/**
	Copyright (c) 2016 Florian Evaldsson (florian.evaldsson@telia.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

int main(int argc,char *argv[])
{
	char *EXE_PATH=NULL;
	GmrMakefile makefileTest;

	if(argc<=1)
		EXE_PATH=(char*)".";
	else
		EXE_PATH=argv[1];
	
	printf("RUNNING FROM PATH: %s\n",EXE_PATH);
	
	GMR_VERBOSE=0;
	
	gmr_makefile_init(&makefileTest,EXE_PATH);
	
	gmr_makefile_dump(&makefileTest,0);
	
	gmr_makefile_finalize(&makefileTest);

	return 0;
}
