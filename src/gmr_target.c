/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

/**
	Initializes the GmrTarget
	
	@param this
		your GmrTarget
*/
GmrTarget *gmr_target_init(GmrTarget *this, const char *name, GmrMakefile *parent)
{
	this->name=g_strdup(name);
	this->files=NULL;
	this->parent=parent;
	
	return this;
}

/**
	Run this on GmrTarget in the end
	
	@param this
		your GmrTarget
*/
void gmr_target_finalize(GmrTarget *this)
{
	free(this->name);
}

/**
	Check if file is in the target
	
	@param this
		your target
	@param comparestr
		a string to compare with
*/
int gmr_target_file_check(GmrTarget *this, const char *comparestr)
{
	GList *l;

	for (l = this->files; l != NULL; l = l->next)
  	{
		if(strcmp(l->data,comparestr)==0)
		{
			return 1;
		}
	}
	
	return 0;
}

/**
	Parses all targets and extract files from the shell commands (recepies)
	
	@param this
		input GmrTarget
	@param input
		bash string
*/
void gmr_target_parse_recipie(GmrTarget *this, const char *input)
{
	GMatchInfo *matchInfo;
	
	//only check for filenames, or something that looks like a filename
	GRegex *regex=g_regex_new("[0-9a-zA-Z_/\\.\\-]*\\.[a-zA-Z]*(?=(\\ |\\(|\\)))",0,0,NULL);

	g_regex_match(regex, input, 0, &matchInfo);
	
	while(g_match_info_matches(matchInfo))
	{
		gchar *word = g_match_info_fetch(matchInfo, 0);
		
		if(!gmr_target_file_check(this,word))
		{
			printf("added:: %s\n",word);
			//add file to list, dont free it!
			this->files=g_list_append(this->files,word);
		
			size_t wordlen=strlen(word);
		
			//if c-file
			if((g_str_has_suffix(word,".c") || g_str_has_suffix(word,".cpp")))
			{
				char exestring[100];
				char path[1035];
				FILE *fp;
				
				printf("found more...\n");
				
				char *completeidirsstr=gmr_get_include_strings(input);
				
				sprintf(exestring,"cd %s && cpp -M %s %s",this->parent->path,word,completeidirsstr);
			
				free(completeidirsstr);
			
				fp=shell_execute_from_buffer(exestring);

				/* Read the output a line at a time - output it. */
				while(fgets(path, sizeof(path), fp) != NULL)
				{
					gmr_target_parse_recipie(this,path);
				}

				/* close */
				pclose(fp);
			}
		}
		else
		{
			//printf("no match! %s\n",word);
			g_free(word);
		}
		
		g_match_info_next(matchInfo, NULL);
	}
	
	g_match_info_free(matchInfo);
	g_regex_unref(regex);
}

void gmr_target_add_file(GmrTarget *this,const char *filename)
{
	GmrMakefile *makefile=this->parent;
	
	
}

/**
	Dumps the contents from the target
*/
void gmr_target_dump(GmrTarget *this)
{
	printf("t:\t%s\n",this->name);
	
	for (GList *l = this->files; l != NULL; l = l->next)
  	{
  		char *filename=l->data;
  	
		printf("f:\t\t%s\n",filename);
	}
}
