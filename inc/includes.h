/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <glib.h>
#include <gio/gio.h>

typedef struct GmrFile GmrFile;
typedef struct GmrMakefile GmrMakefile;
typedef struct GmrTarget GmrTarget;

#include "gmr_file.h"
#include "gmr_makefile.h"
#include "gmr_target.h"
#include "shell.h"
#include "gmr_misc.h"

#define GMR_VERBOSE gmr_verbose_get()

#define GMR_DEBUG(...) if(GMR_VERBOSE) printf(__VA_ARGS__)
#define GMR_ERROR(...) printf(__VA_ARGS__)

#define GMR_FILE_N(filename_in,directory_in) memcpy(malloc(sizeof(GmrFile)),&(GmrFile){.filename=strdup(filename_in),.directory=strdup(directory_in)},sizeof(GmrFile))
