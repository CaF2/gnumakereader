/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

/**
	This is probably the only real function you will use, it will initialize everything :)
	
	@param this
		current makefile
*/
GmrMakefile *gmr_makefile_init(GmrMakefile *this,const char *path)
{
	this->path=g_strdup(path);
	
	gmr_makefile_init_targets(this);
	
	if(this->targets)
		gmr_makefile_evaluate_targets(this);
	
	return this;
}

/**
	Run this at the end, when you want to "finish the object"
	
	@param this
		current makefile structure
*/
void gmr_makefile_finalize(GmrMakefile *this)
{
	free(this->path);
}

/**
	Evaluate the targets, eg its recipies, makes a couple of dry runs, and extracts what it can from there.
	
	@param this
		current makefile structure
*/
void gmr_makefile_evaluate_targets(GmrMakefile *this)
{
	GList *list=this->targets;

	for (GList *l = list; l != NULL; l = l->next)
  	{
		GmrTarget *target = l->data;

		const char *targetName=target->name;
		const char *exepath=this->path;

		printf("======== Running target: %s ========\n",targetName);
			
		FILE *fp;
		char path[1035];
	
		char exestring[100];
	
		/*
			#*.h-filer
			make -n -B | grep -oh '\-I[0-9a-zA-Z_/\.]*\ '
						
			#includeträd?

			#*.c-filer
			make -n -B | grep -oh '[0-9a-zA-Z_/\.]*\.\(c\|cpp\)'
		*/
	
		sprintf(exestring,"cd %s && make %s -n -B",exepath,targetName);
		//sprintf(exestring,"cd %s && make %s -n -B | grep -oh '[0-9a-zA-Z_/\\.]*\\.\\(c\\|o\\|cpp\\)'",exepath,targetName);
	
		fp=shell_execute_from_buffer(exestring);

		//Read the output a line at a time - output it.
		while(fgets(path, sizeof(path), fp) != NULL)
		{
			printf("got recepie: %s\n",path);
			gmr_target_parse_recipie(target,path);
		}

		pclose(fp);
		
	}
}

/**
	Extracts the recepies from the makefile, and inserts them into the structure. 
	
	@param this
		input gmrmakefile
*/
void gmr_makefile_init_targets(GmrMakefile *this)
{
	const char *path=this->path;
	GList *retTargets=NULL;

	FILE *fp;
	char output[1035];
	char exestring[150];
	
	// Open the command for reading.
	sprintf(exestring,"cd %s && make -qp",path);
			
	fp=shell_execute_from_buffer(exestring);

	// Read the output a line at a time - output it.
	while(fgets(output, sizeof(output), fp) != NULL)
	{
/*		printf("OUTPUT [%s]\n",output);*/
	
		size_t targetlen=strlen(output);
		
		if(targetlen>1)
		{
			GMatchInfo *matchInfo;

			GRegex *regex=g_regex_new("^[a-zA-Z0-9][^$#\\/\\t=]*(?=(:([^=]|$)))",0,0,NULL);

			g_regex_match(regex, output, 0, &matchInfo);
	
			while(g_match_info_matches(matchInfo))
			{
				gchar *word = g_match_info_fetch(matchInfo, 0);
				
				printf("Target found: [%s]\n", word);
				
				retTargets=g_list_append(retTargets,gmr_target_init(malloc(sizeof(GmrTarget)),word,this));
				
				g_free(word);
		
				g_match_info_next(matchInfo, NULL);
			}
			
			g_match_info_free(matchInfo);
			g_regex_unref(regex);
		}
	}
	
	pclose(fp);
	
	this->targets=retTargets;
}

/**
	Dumps the contents from the makefile
	
	@param this
		the makefile to dump
*/
void gmr_makefile_dump(GmrMakefile *this)
{
	puts("=== DUMPING INFORMATION ===");
	printf("-path = '%s'\n\n",this->path);
	
	GList *list=this->targets;

	for (GList *l = list; l != NULL; l = l->next)
  	{
		gmr_target_dump(l->data);
	}
}
