/**
	Copyright (c) 2016 Florian Evaldsson (florian.evaldsson@telia.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <getopt.h>

#include "includes.h"

void print_help(void)
{
	printf("GNU-Make reader printing tool\n");
	printf("%-3s %-20s %s\n","-h","--help","Print this text");
	printf("%-3s %-20s %s\n","-p","--path","Specify the path to the makefile");
	printf("%-3s %-20s %s\n","-v","--verbose","Print more information");
	printf("%-3s %-20s %s\n","-g","--gtags","Dump the content in a gtags format");
	printf("%-3s %-20s %s\n","-t","--target","Dump only the specified target");
}

int main(int argc,char *argv[])
{
	char *EXE_PATH=".";
	char *dump_target=NULL;
	int dump_option=DUMP_ALL;

	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"verbose", no_argument, 0, 'v'},
			/* These options don’t set a flag.
			We distinguish them by their indices. */
			{"help", no_argument, 0, 'h'},
			{"verbose", no_argument, 0, 'v'},
			{"gtags", no_argument, 0, 'g'},
			{"target", no_argument, 0, 't'},
			{"path", required_argument, 0, 'p'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		int c = getopt_long (argc, argv, "hvp:gt:",long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
			break;

			case 'h':
				print_help();
			return 0;
			
			case 'v':
				GMR_VERBOSE=1;
			break;
			
			case 'p':
/*				printf ("Vill parse the makefile at the directory:: `%s`\n", optarg);*/
				EXE_PATH=optarg;
			break;
			
			case 't':
				dump_target=optarg;
			break;
			
			case 'g':
				dump_option=DUMP_GTAGS;
			break;

			case '?':
				/* getopt_long already printed an error message. */
			break;

			default:
			abort ();
		}
	}

	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
		printf ("%s ", argv[optind++]);
		putchar ('\n');
	}
	
	/////////////////////

	GmrMakefile makefileTest;
	
	printf("RUNNING FROM PATH: %s\n",EXE_PATH);
	
	gmr_makefile_init(&makefileTest,EXE_PATH);
	
	gmr_makefile_dump(&makefileTest,dump_target,dump_option);
	
	gmr_makefile_finalize(&makefileTest);

	return 0;
}
