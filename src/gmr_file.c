/**
	Copyright (c) 2016 Florian Evaldsson

	Permission is hereby granted, free of charge, to any person obtaining a copy of self software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and self permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "includes.h"

GmrFile *gmr_file_init(GmrFile *self,const char *filename,const char *source,GmrTarget *target)
{
	self->name=g_strdup(filename);
	self->source=g_strdup(source);
	
	//initiate the first target
	self->targets=g_list_append(NULL,target);
	
	self->deps=NULL;
	
	//assume self first
	self->isDependency=0;
	self->failedDepCheck=0;
	
	//FOR DEPS
	gmr_file_check_for_dependency(self,filename,source,target);
	
	return self;
}

void gmr_file_finalize(GmrFile *self)
{
	g_free(self->name);
	g_free(self->source);
	
	g_list_free(self->deps);
	g_list_free(self->targets);
}

void gmr_file_free(GmrFile *self)
{
	gmr_file_finalize(self);
	free(self);
}

void gmr_file_free_wrapper(gpointer data)
{
	gmr_file_free(data);
}

/**
	
*/
void gmr_file_check_for_dependency(GmrFile *self,const char *filename,const char *source,GmrTarget *target)
{
	/**
		TODO make self better, with regex
	*/
	if((g_str_has_suffix(filename,".c") || g_str_has_suffix(filename,".cpp")))
	{
		g_autofree char *exestring=NULL;
		char path[1035];
		FILE *fp;
		
		GMR_DEBUG("found more... %s\n",source);
		
		g_autofree char *completeidirsstr=gmr_get_include_strings(source);
		
		if(completeidirsstr)
		{
			asprintf(&exestring,"cd %s && cpp -M %s %s",target->parent->path,filename,completeidirsstr);
		}
		else
		{
			asprintf(&exestring,"LANG=c; cd %s && cpp -M %s 2>&1",target->parent->path,filename);
		}
		
		GMR_DEBUG("exestr:: %s\n",exestring);
				
		fp=shell_execute_from_buffer(exestring);
		
		/* Read the output a line at a time - output it. */
		while(fgets(path, sizeof(path), fp) != NULL)
		{
			/**
				TODO better regex for finding errors
			*/
			if(g_strrstr(path,"fatal error"))
			{
				GMR_DEBUG("GOT ERROR ON COMMAND %s!\n",path);
				self->failedDepCheck=1;
				break;
			}
		
			GMR_DEBUG("ADDING DEP :: %s\n",path);
			//gmr_target_parse_recipie(target,path);
			gmr_file_add_header_dependency(self,target,path);
		}

		/* close */
		pclose(fp);
	}
}

int gmr_file_target_check(GmrFile *self,GmrTarget *target)
{
	GList *l;

	for (l = self->targets; l != NULL; l = l->next)
  	{
  		GmrTarget *currtarget=l->data;
  		
  		if(currtarget==target)
  			return 1;
  		
		if(strcmp(currtarget->name,target->name)==0)
		{
			return 1;
		}
	}
	
	return 0;
}

void gmr_file_add_header_dependency(GmrFile *self, GmrTarget *target, const char *source)
{
	g_autoptr(GMatchInfo) matchInfo=NULL;
	
	GMR_DEBUG("adding dep\n");
	
	//only check for filenames, or something that looks like a filename
	g_autoptr(GRegex) regex=g_regex_new("[0-9a-zA-Z_/\\.\\-]*\\.[hHpP]*(?=(\\ |\\(|\\)))",0,0,NULL);

	g_regex_match(regex, source, 0, &matchInfo);
	
	while(g_match_info_matches(matchInfo))
	{
		g_autofree gchar *word = g_match_info_fetch(matchInfo, 0);
		
		GmrFile *depfile=gmr_file_add(word,source,target);
		depfile->isDependency=1;
		self->deps=g_list_append(self->deps,depfile);
		
		g_match_info_next(matchInfo, NULL);
	}
}

/**
	Function for adding files, self is very generic, and will add for both targets and makefile.
	
	@param filename
		filename to add
	@param source
		recipie or something where you found the file
	@param target
		the recipie target
*/
GmrFile *gmr_file_add(const char *filename, const char *source, GmrTarget *target)
{
	g_autofree char *actual_filename;
	
	g_autofree char *directory_path=gmr_makefile_get_current_directory(target->parent);
	
	if(target->parent->current_directory->len>0 && !is_root_path(filename))
	{
		asprintf(&actual_filename,"%s/%s",directory_path,filename);
	}
	else
	{
		actual_filename=g_strdup(filename);
	}
	
	GMR_DEBUG("actual filepath :: %s\n",actual_filename);

	GmrFile check={.name=(char*)actual_filename};

	GmrMakefile *makefile=target->parent;

	GmrFile *qfoundfile=gmr_makefile_check_file(makefile,&check);
	
	if(!qfoundfile)
	{
		GMR_DEBUG("added :: %s\n",actual_filename);

		GmrFile *self=gmr_file_init(malloc(sizeof(GmrFile)),actual_filename,source,target);
	
		makefile->files=g_list_append(makefile->files,self);
		
		if(!gmr_target_check_file(target,self))
			target->files=g_list_append(target->files,self);
			
		return self;
	}
	else if(!gmr_target_check_file(target,qfoundfile))
	{
		target->files=g_list_append(target->files,qfoundfile);
		
		//also add deps
		for (GList *l = qfoundfile->deps; l != NULL; l = l->next)
 	 	{
 	 		GmrFile *currdep=l->data;
 	 		
 	 		if(!gmr_target_check_file(target,currdep))
 	 			target->files=g_list_append(target->files,currdep);
 	 	}
	}
	else
	{
		GMR_DEBUG("skipping :: %s\n",actual_filename);
	}
	
	//previous dependency check failed, lets retry (could not find on a target like 'echo "compiling ... hello.c"') --> waiting for like gcc -I./something hello.c
	if(qfoundfile && qfoundfile->failedDepCheck)
	{
		GMR_DEBUG("Retry dep check on %s ...\n",actual_filename);
	
		qfoundfile->failedDepCheck=0;
		free(qfoundfile->source);
		
		qfoundfile->source=g_strdup(source);
		gmr_file_check_for_dependency(qfoundfile,actual_filename,source,target);
	}
	
	return qfoundfile;
}
