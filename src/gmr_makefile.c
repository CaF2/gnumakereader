/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of self software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and self permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

/**
	self is probably the only real function you will use, it will initialize everything :)
	
	@param self
		current makefile
	@param path
		a path to the makefile
	@param use_target
		NULL if all, or specify a target
*/
GmrMakefile *gmr_makefile_init(GmrMakefile *self,const char *path, const char *use_target)
{
	self->path=g_strdup(path);
	self->files=NULL;
	self->current_directory=g_string_sized_new(100);
	
	gmr_makefile_init_targets(self);
	
	if(self->targets)
		gmr_makefile_evaluate_targets(self,use_target);
	
	return self;
}

/**
	Run self at the end, when you want to "finish the object"
	
	@param self
		current makefile structure
*/
void gmr_makefile_finalize(GmrMakefile *self)
{
	free(self->path);
	
	g_list_free_full(self->files,gmr_file_free_wrapper);

	g_list_free_full(self->targets,gmr_target_free_wrapper);
	
	g_string_free(self->current_directory,TRUE);
}

/**
	Evaluate the targets, eg its recipies, makes a couple of dry runs, and extracts what it can from there.
	
	@param self
		current makefile structure
*/
void gmr_makefile_evaluate_targets(GmrMakefile *self, const char *use_target)
{
	GList *list=self->targets;

	for (GList *l = list; l != NULL; l = l->next)
  	{
		GmrTarget *target = l->data;

		const char *targetName=target->name;
		
		if(use_target==NULL || (use_target && targetName && strcmp(use_target,targetName)==0))
		{
			const char *exepath=self->path;

			GMR_DEBUG("======== Running target: %s ========\n",targetName);
			
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
		
			sprintf(exestring,"LANG=c; make -C %s -n -B %s",exepath,targetName);
			//sprintf(exestring,"cd %s && make %s -n -B | grep -oh '[0-9a-zA-Z_/\\.]*\\.\\(c\\|o\\|cpp\\)'",exepath,targetName);
			
			GMR_DEBUG("RUNNING TARGET: %s -> %s\n",targetName,exestring);
			
			fp=shell_execute_from_buffer(exestring);

			//Read the output a line at a time - output it.
			while(fgets(path, sizeof(path), fp) != NULL)
			{
				GMR_DEBUG("got recepie: %s\n",path);
				gmr_target_parse_recipie(target,path);
			}

			pclose(fp);
		}
	}
}

/**
	Extracts the recepies from the makefile, and inserts them into the structure. 
	
	@param self
		input gmrmakefile
*/
void gmr_makefile_init_targets(GmrMakefile *self)
{
	const char *path=self->path;
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
/*		GMR_DEBUG("OUTPUT [%s]\n",output);*/
	
		size_t targetlen=strlen(output);
		
		if(targetlen>1)
		{
			GMatchInfo *matchInfo;

			GRegex *regex=g_regex_new("^[a-zA-Z0-9][^$#\\/\\t=]*(?=(:([^=]|$)))",0,0,NULL);

			g_regex_match(regex, output, 0, &matchInfo);
	
			while(g_match_info_matches(matchInfo))
			{
				gchar *word = g_match_info_fetch(matchInfo, 0);
				
				GMR_DEBUG("Target found: [%s]\n", word);
				
				GmrTarget *newtarget=gmr_target_new(word,self);
				
				if(strcmp(word,"run")==0 || strcmp(word,"test")==0)
					newtarget->runnable=2;
					
				if(strcmp(word,"debug")==0 || strcmp(word,"ddd")==0 || strcmp(word,"gdb")==0)
					newtarget->debug=2;
				
				retTargets=g_list_append(retTargets,newtarget);
				
				g_free(word);
		
				g_match_info_next(matchInfo, NULL);
			}
			
			g_match_info_free(matchInfo);
			g_regex_unref(regex);
		}
	}
	
	pclose(fp);
	
	self->targets=retTargets;
}

/**
	Check if file is in the list
	
	@param self
		your makefile
	@param file
		a file to compare with
*/
GmrFile *gmr_makefile_check_file(GmrMakefile *self, GmrFile *file)
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
	Dumps the contents from the makefile
	
	@param self
		the makefile to dump
	@param target_name
		NULL if all, or specify a target name
*/
void gmr_makefile_dump(GmrMakefile *self, const char *target_name, int config)
{
	if(config==DUMP_ALL)
	{
		printf("=== DUMPING INFORMATION ===\n");
		printf("-path = '%s'\n\n",self->path);
	}
	
	GList *list=self->targets;

	for (GList *l = list; l != NULL; l = l->next)
	{
		GmrTarget *target=l->data;

		if(target_name==NULL || (target && target->name && strcmp(target->name,target_name)==0))
		{
			gmr_target_dump(l->data,config);
		}
	}
}
