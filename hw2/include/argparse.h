#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <argp.h>

struct arguments {
	char* url;
	char* delete;
	char* get;
	char* help;
	char* post;
	char* put;
	char** message;
};

int parse_arguments(int argc, char* argv[], struct arguments* args);

#endif /* ifndef ARGPARSER_H */
