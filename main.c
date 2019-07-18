#include <stdio.h>
#include <libgen.h>
#include <errno.h>
#include "bstr.h"
#include "blog.h"
#include "pattern.h"


int
main(int argc, char **argv)
{
	char	*execn;
	bstr_t	*seq;
	int	err;
	int	ret;
	barr_t	*lines;
	bstr_t	*line;
	int	linenr;

	err = 0;
	seq = NULL;
	lines = NULL;

	if(xstrempty(argv[0])) {
		fprintf(stderr, "Invalid argv[0]\n");
		err = -1;
		goto end_label;
	}

	execn = basename(argv[0]);
	if(xstrempty(execn)) {
		fprintf(stderr, "Can't get executable name\n");
		err = -1;
		goto end_label;
	}

	if(argc < 2 || xstrempty(argv[1])) {
		fprintf(stderr, "No sequence file specified\n");
		err = -1;
		goto end_label;
	}


	ret = blog_init(execn);
	if(ret != 0) {
		fprintf(stderr, "Could not initialize logging: %s\n",
		    strerror(ret));
		err = -1;
		goto end_label;
	}

	seq = binit();
	if(seq == NULL) {
		blogf("Couldn't allocate seq");
		goto end_label;
	}

	ret = bfromfile(seq, argv[1]);
	if(ret != 0) {
		fprintf(stderr, "Couldn't load sequence file\n");
		err = -1;
		goto end_label;
	}

	ret = bstrsplit(seq, "\n", 0, &lines);

	if(ret != 0 || barr_cnt(lines) == 0) {
		fprintf(stderr, "Input file empty\n");
		err = -1;
		goto end_label;
	}

	ret = pattern_init();
	if(ret != 0) {
		fprintf(stderr, "Couldn't initialize parser\n");
		err = -1;
		goto end_label;
	}


	linenr = 1;
	for(line = (bstr_t *) barr_begin(lines);
	    line < (bstr_t *) barr_end(lines); ++line) {
		if(bstrempty(line) || bstrbeginswith(line, "#")) {
			++linenr;
			continue;	
		}

		ret = pattern_parse_line(line);
		if(ret != 0) {
			fprintf(stderr, "Couldn't parse line %d.\n", linenr);
			err = -1;
			goto end_label;
		}
		++linenr;
	}

	ret = pattern_process();
	if(ret != 0) {
		fprintf(stderr, "Error while processing pattern\n");
		err = -1;
		goto end_label;
	}
	
end_label:

	blog_uninit();

	buninit(&seq);

	if(lines != NULL) {
		for(line = (bstr_t *) barr_begin(lines);
		    line < (bstr_t *) barr_end(lines); ++line) {
			buninit_(line);
		}
		barr_uninit(&lines);
	}

	return err;
}
