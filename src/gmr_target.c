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
	
	this->runnable=0;
	this->debug=0;
	
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
	g_list_free(this->files);
}

/**
	heap version of finalize
	
	@param this
		data to finalize
*/
void gmr_target_free(GmrTarget *this)
{
	gmr_target_finalize(this);
	free(this);
}

/**
	Used with gmr_makefile_finalize (g_list_free_full)
	
	@param data
		current data
*/
void gmr_target_free_wrapper(gpointer data)
{
	gmr_target_free(data);
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
	/**** FINDING FILES ****/
	GMatchInfo *matchInfo;
	
	//only check for filenames, or something that looks like a filename
	GRegex *regex=g_regex_new("[0-9a-zA-Z_/\\.\\-]*\\.[a-zA-Z]{1,4}(?=(\\ |\\(|\\)))",0,0,NULL);

	g_regex_match(regex, input, 0, &matchInfo);
	
	while(g_match_info_matches(matchInfo))
	{
		gchar *word = g_match_info_fetch(matchInfo, 0);
		
		gmr_file_add(word,input,this);
		
		g_free(word);
		
		g_match_info_next(matchInfo, NULL);
	}
	
	g_match_info_free(matchInfo);
	g_regex_unref(regex);
	
	/**** FINDING OTHER ****/
	/*
		Tries to find information from a compiler such as gcc
	*/
	regex=g_regex_new("(?<=(-))[0-9a-zA-Z_/\\.\\-]+(?=(\\ |\\(|\\)))",0,0,NULL);

	g_regex_match(regex, input, 0, &matchInfo);
	
	while(g_match_info_matches(matchInfo))
	{
		gchar *word = g_match_info_fetch(matchInfo, 0);
		
		GMR_DEBUG("found other :: %s\n",word);
		
		g_free(word);
		
		g_match_info_next(matchInfo, NULL);
	}
	
	g_match_info_free(matchInfo);
	g_regex_unref(regex);
	
	/**** TEST ****/
	
	//regex=g_regex_new("(((?!\\|\\|)(?!&&)(?!\\$\\())[^;\\)])+",0,0,NULL);
	//check if it is a runnable file
	regex=g_regex_new("^\\.{0,2}(\\/[0-9a-zA-Z_/.\\-]+)?\\/[0-9a-zA-Z_/\\-]+(.exe)?(?=(([\\ \\)\\(;&\\|><]|$)))",0,0,NULL);

	g_regex_match(regex, input, 0, &matchInfo);
	
	while(g_match_info_matches(matchInfo))
	{
		gchar *word = g_match_info_fetch(matchInfo, 0);
		
		GMR_DEBUG("Found executable (maybe make run?) :: %s\n",word);
		
		this->runnable++;
		
		g_free(word);
		
		g_match_info_next(matchInfo, NULL);
	}
	
	g_match_info_free(matchInfo);
	g_regex_unref(regex);
	
	/**** SUB-MAKEFILE ****/

/*	regex=g_regex_new("(?<=([mM][aA][kK][eE] .* -C ))[a-zA-Z0-9./]+",0,0,NULL);*/

/*	g_regex_match(regex, input, 0, &matchInfo);*/
/*	*/
/*	while(g_match_info_matches(matchInfo))*/
/*	{*/
/*		gchar *word = g_match_info_fetch(matchInfo, 0);*/
/*		*/
/*		GMR_DEBUG("changing directory to ... :: %s\n",word);*/
/*		*/
/*		g_free(word);*/
/*		*/
/*		g_match_info_next(matchInfo, NULL);*/
/*	}*/
/*	*/
/*	g_match_info_free(matchInfo);*/
/*	g_regex_unref(regex);*/
}

/**
	Check if file is in the list
	
	@param this
		your target
	@param file
		a file to compare with
*/
GmrFile *gmr_target_check_file(GmrTarget *this, GmrFile *file)
{
	GList *l;
	
	for (l = this->files; l != NULL; l = l->next)
  	{
  		GmrFile *lFile=l->data;
  		
  		if(lFile==file)
		{
			return lFile;
		}
  	
		if(strcmp(lFile->name,file->name)==0)
		{
			return lFile;
		}
	}
	
	return NULL;
}

/**
	Dumps the contents from the target
	
	@param this
		input GmrTarget to dump
*/
void gmr_target_dump(GmrTarget *this, int config)
{
	printf("t:\t%s\n",this->name);
	
	for (GList *l = this->files; l != NULL; l = l->next)
  	{
  		GmrFile *file=l->data;
  		
  		char *filename=file->name;
  		
  		if((!(config&GMR_DUMP_PRINT_DEPS)) || file->isDependency==0)
			printf("f:\t\t%s\n",filename);
		
		if(config&GMR_DUMP_PRINT_DEPS)
			for (GList *dep = file->deps; dep != NULL; dep = dep->next)
	 	 	{
	 	 		GmrFile *currdep=dep->data;
	 	 		
	 	 		char *depname=currdep->name;
	  		
				printf("d:\t\t\t%s\n",depname);
	 	 	}
	}
}
