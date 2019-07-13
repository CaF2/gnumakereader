/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

//for debugging
uint8_t GMR_VERBOSE=0;

/**
	Extract all the -I commands.
	
	@param command
		bash command (single)
	@returns
		the includes part (you need to free this string urself)
*/
char *gmr_get_include_strings(const char *command)
{
	g_autoptr(GMatchInfo) matchInfo=NULL;
				
	g_autoptr(GString) idirsstr=g_string_sized_new(100);

	g_autoptr(GRegex) regex=g_regex_new("-I[^\\ \\t]*",0,0,NULL);

	g_regex_match (regex, command, 0, &matchInfo);

	while(g_match_info_matches(matchInfo))
	{
		g_autofree gchar *word = g_match_info_fetch(matchInfo, 0);
		
		g_string_append_printf(idirsstr,"%s ",word);
		
		GMR_DEBUG("matchy:: %s\n",word);
		
		g_match_info_next(matchInfo, NULL);
	}
	
	return g_string_free(g_steal_pointer(&idirsstr),FALSE);
}
