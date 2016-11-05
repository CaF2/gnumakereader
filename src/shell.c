/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

FILE *shell_execute_from_buffer(const char *str)
{
	FILE *fp;
	
	/* Open the command for reading. */
	fp = popen(str, "r");

	if (fp == NULL)
	{
		GMR_DEBUG("Failed to run command ...\n" );
		exit(1);
	}
	
	return fp;
}

/*
gchar **shell_execute_from_buffer2(const char *command)
{
	GMR_DEBUG("got command %s\n",command);

	gchar *stdoutstr = NULL;
	GError *error = NULL;
	GCancellable *cancellable = NULL;

	GSubprocess *subprocess=g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "sh", "-c",command,NULL);

	if(!subprocess)
	{
		g_assert(error != NULL);
		return NULL;
	}

	if(!g_subprocess_communicate_utf8 (subprocess, NULL, cancellable, &stdoutstr, NULL, &error))
	{
		g_assert(error != NULL);
		return NULL;
	}
	
	gchar **splitted=g_strsplit(stdoutstr, "\n", 0);
	
	g_free(stdoutstr);
	
	return splitted;
}

gchar *shell_execute_from_buffer3(const char *command)
{
	gchar *stdoutstr = NULL;
	GError *error = NULL;
	GCancellable *cancellable = NULL;

	GSubprocess *subprocess=g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "sh", "-c",command,NULL);

	if(!subprocess)
	{
		g_assert(error != NULL);
		return NULL;
	}

	if(!g_subprocess_communicate_utf8 (subprocess, NULL, cancellable, &stdoutstr, NULL, &error))
	{
		g_assert(error != NULL);
		return NULL;
	}
	
	return stdoutstr;
}

gchar *shell_execute_from_buffer4(const char *command)
{
	gchar *stdoutstr = NULL;
	GError *error = NULL;
	GCancellable *cancellable = NULL;

	GSubprocess *subprocess=g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "make", "-nB",command,NULL);

	if(!subprocess)
	{
		g_assert(error != NULL);
		return NULL;
	}

	if(!g_subprocess_communicate_utf8 (subprocess, NULL, cancellable, &stdoutstr, NULL, &error))
	{
		g_assert(error != NULL);
		return NULL;
	}
	
	return stdoutstr;
}
*/
