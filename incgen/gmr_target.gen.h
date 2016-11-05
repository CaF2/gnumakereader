/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
	Struct containing information about a target.
*/
struct GmrTarget
{
	char *name; ///< name of the target
	//GList *dependancies; ///< extract all dependancies (if useful) type (char*)
	//GList *orderonlydep; ///< extract all order-only-dependancies (if useful) type (char*)
	
	GList *files; ///< list files of type (GmrFile*)
	GmrMakefile *parent; ///< parent structure
	
	//Flags
	//(0=not likely,1=kinda likely (found run executable in recipie),2=very likely (target make run, make test),3=is make run (is both make run, make test and found executable))
	uint8_t runnable : 2; ///< look if its a runnable target like; make run, make test etc... 
	uint8_t debug : 2; ///< if this target is used for debugging in some way, like; make gdb, make ddd, make debug etc...
	
};
