#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <argp.h>

struct arguments {
	char* url;
	char* delete;
	char* get;
	char* help;
	char* post;
	char* put;
	char* message;
};

int parse_arguments(int argc, char* argv[], struct arguments* args, int* message_index);

#endif /* ifndef ARGPARSER_H */
