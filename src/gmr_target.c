/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of self software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and self permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

/**
	Initializes the GmrTarget
	
	@param self
		your GmrTarget
*/
GmrTarget *gmr_target_init(GmrTarget *self, const char *name, GmrMakefile *parent)
{
	if(!self)
	{
		return NULL;
	}

	memset(self,'\0',sizeof(GmrTarget));

	self->name=g_strdup(name);
	self->files=NULL;
	self->parent=parent;
	
	self->runnable=0;
	self->debug=0;
	
	return self;
}

GmrTarget *gmr_target_new(const char *name, GmrMakefile *parent)
{
	GmrTarget *self=malloc(sizeof(GmrTarget));
	return gmr_target_init(self, name, parent);
}

/**
	Run self on GmrTarget in the end
	
	@param self
		your GmrTarget
*/
void gmr_target_finalize(GmrTarget *self)
{
	if(self)
	{
		free(self->name);
		g_list_free(self->files);
	}
}

/**
	heap version of finalize
	
	@param self
		data to finalize
*/
void gmr_target_free(GmrTarget *self)
{
	gmr_target_finalize(self);
	free(self);
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
	
	@param self
		input GmrTarget
	@param input
		bash string
*/
void gmr_target_parse_recipie(GmrTarget *self, const char *input)
{
	{
		/**** FINDING FILES ****/
		g_autoptr(GMatchInfo) matchInfo=NULL;
		
		//only check for filenames, or something that looks like a filename
		g_autoptr(GRegex) regex=g_regex_new("[0-9a-zA-Z_/\\.\\-]*\\.[a-zA-Z]{1,4}(?=(\\ |\\(|\\)))",0,0,NULL);

		g_regex_match(regex, input, 0, &matchInfo);
		
		while(g_match_info_matches(matchInfo))
		{
			g_autofree gchar *word = g_match_info_fetch(matchInfo, 0);
			
			gmr_file_add(word,input,self);
			
			g_match_info_next(matchInfo, NULL);
		}
	}
	
	/**** FINDING OTHER ****/
	/*
		Tries to find information from a compiler such as gcc
	*/
	{
		g_autoptr(GMatchInfo) matchInfo=NULL;
	
		g_autoptr(GRegex) regex=g_regex_new("(?<=(-))[0-9a-zA-Z_/\\.\\-]+(?=(\\ |\\(|\\)))",0,0,NULL);

		g_regex_match(regex, input, 0, &matchInfo);
		
		while(g_match_info_matches(matchInfo))
		{
			g_autofree gchar *word = g_match_info_fetch(matchInfo, 0);
			
			GMR_DEBUG("found other :: %s\n",word);
			
			g_match_info_next(matchInfo, NULL);
		}
	}
	
	/**** TEST ****/
	
	//regex=g_regex_new("(((?!\\|\\|)(?!&&)(?!\\$\\())[^;\\)])+",0,0,NULL);
	//check if it is a runnable file
	{
		g_autoptr(GMatchInfo) matchInfo=NULL;
	
		g_autoptr(GRegex) regex=g_regex_new("^\\.{0,2}(\\/[0-9a-zA-Z_/.\\-]+)?\\/[0-9a-zA-Z_/\\-]+(.exe)?(?=(([\\ \\)\\(;&\\|><]|$)))",0,0,NULL);

		g_regex_match(regex, input, 0, &matchInfo);
		
		while(g_match_info_matches(matchInfo))
		{
			g_autofree gchar *word = g_match_info_fetch(matchInfo, 0);
			
			GMR_DEBUG("Found executable (maybe make run?) :: %s\n",word);
			
			self->runnable++;
			
			g_match_info_next(matchInfo, NULL);
		}
	}
	
	/**** SUB-MAKEFILE ****/

	//looks like self:
	// make[2]: Entering directory '/home/caf2/dev/C/gnumakereader/test'
	{
		g_autoptr(GMatchInfo) matchInfo=NULL;
	
/*		regex=g_regex_new("(?<=([mM][aA][kK][eE] .* -C ))[a-zA-Z0-9./]+",0,0,NULL);*/
		g_autoptr(GRegex) regex=g_regex_new("[mM][aA][kK][eE]\\[[0-9]+\\]:[^']+\\'\\K[^']+",0,0,NULL);

		g_regex_match(regex, input, 0, &matchInfo);

		while(g_match_info_matches(matchInfo))
		{
			g_autofree gchar *word = g_match_info_fetch(matchInfo, 0);

			gsize wordlen = strlen(word);
			
			GmrMakefile *curr_makefile = self->parent;
			
			GString *current_directory = curr_makefile->current_directory;

			GMR_DEBUG("changing directory to ... :: %s\n",word);

			if(current_directory)
			{
				if(current_directory->len<wordlen)
				{
					curr_makefile->current_directory=g_string_append(current_directory,word);
				}
				else
				{
					gsize after_eventual_match_len=current_directory->len-wordlen;
					gchar *eventual_match=current_directory->str+after_eventual_match_len;
				
					if(strcmp(eventual_match,word)==0)
					{
						curr_makefile->current_directory=g_string_truncate(current_directory,after_eventual_match_len);
					}
					else
					{
						curr_makefile->current_directory=g_string_append(current_directory,word);
					}
				}
			}
			else
			{
				curr_makefile->current_directory=g_string_append(current_directory,word);
			}
			
			GMR_DEBUG("curr current_directory:: %s\n",curr_makefile->current_directory->str);

			g_match_info_next(matchInfo, NULL);
		}
	}
}

/**
	Check if file is in the list
	
	@param self
		your target
	@param file
		a file to compare with
*/
GmrFile *gmr_target_check_file(GmrTarget *self, GmrFile *file)
{
	GList *l;
	
	for (l = self->files; l != NULL; l = l->next)
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

	@param self
		input GmrTarget to dump
*/
void gmr_target_dump(GmrTarget *self, int config)
{
	if(config==DUMP_ALL)
	{
		printf("t:\t%s\n",self->name);
		printf("flag:\trunnable=%d\n",self->runnable);
		printf("flag:\tdebug=%d\n",self->debug);
	}

	for (GList *l = self->files; l != NULL; l = l->next)
  	{
  		GmrFile *file=l->data;
  		
  		char *filename=file->name;
  		
  		if((!(config&GMR_DUMP_PRINT_DEPS)) || file->isDependency==0)
  		{
  			if(!(config&DUMP_FILES))
  			{
				printf("f:\t\t%s\n",filename);
			}
			else
			{
				printf("%s\n",filename);
			}
		}
		
		if(config&GMR_DUMP_PRINT_DEPS)
		{
			for (GList *dep = file->deps; dep != NULL; dep = dep->next)
	 	 	{
	 	 		GmrFile *currdep=dep->data;
	 	 		
	 	 		char *depname=currdep->name;
	  		
				printf("d:\t\t\t%s\n",depname);
	 	 	}
	 	 }
	}
}
