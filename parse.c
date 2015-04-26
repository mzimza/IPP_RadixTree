/*
 * parse.c
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include "defines.h"
#include "parse.h"

/* CODES FOR PARSER */
#define MAX_MSG	512
#define VALID_OPTION	"v"
#define VALID_OPT	'v'
#define NUMBER_SET	"0123456789"
#define CHAR_SET	"abcdefghijklmnopqrstuvwxyz"
#define WHITESPACE	"\t "
#define LAST_INSERT	't'
#define LAST_FIND	'd'
#define BASE	10
#define N_MATCHES	1

/* REGEX MATCH STRINGS */
#define NUMBER	"(0?|([1-9][0-9]*))"
#define BLANKS_IN	"([[:blank:]]+)"
#define BLANK_NUM "([[:blank:]]+)(0?|([1-9][0-9]*))"
#define BLANKS	"([[:blank:]]*)"
#define PREV	"^" BLANKS "(prev)" BLANK_NUM BLANK_NUM BLANK_NUM BLANKS "\n$"
#define PATTERN "([[:lower:]]+)"
#define INSERT	"^" BLANKS "(insert)" BLANKS_IN PATTERN BLANKS "\n$"
#define FIND	"^" BLANKS "(find)" BLANKS_IN PATTERN BLANKS "\n$"
#define DELETE	"^" BLANKS "(delete)" BLANKS_IN NUMBER BLANKS "\n$"
#define CLEAR	"^"	BLANKS "(clear)" BLANKS "\n$"


int compile_regex(regex_t *r, const char *regexText)
{
	int err = regcomp(r, regexText, REG_EXTENDED);
	if (err != 0) {
		char errorMessage[MAX_MSG];
		regerror(err, r, errorMessage, MAX_MSG);
		printf("Regex error compiling '%s': %s\n", regexText, errorMessage);
		return ERROR;
	}
	return 0;
}

int compile_all(regex_t *i, regex_t *p, regex_t *f, regex_t *d, regex_t *c)
{
	if (compile_regex(i, INSERT) || compile_regex(p, PREV)
			|| compile_regex(f, FIND) || compile_regex(d, DELETE)
			|| compile_regex(c, CLEAR))
		return ERROR;
	return 0;
}

void free_all(regex_t *i, regex_t *p, regex_t *f, regex_t *d, regex_t *c)
{
	regfree(i);
	regfree(p);
	regfree(f);
	regfree(d);
	regfree(c);
}

int match_regex(regex_t *r, const char *line)
{
	/* m contains the matches found.
	 * N_MATCHES is the max number of matches allowed */
	regmatch_t m[N_MATCHES];
	return regexec(r, line, N_MATCHES, m, 0);
}

int match(regex_t *i, regex_t *p, regex_t *f, regex_t *d, regex_t *c,
		const char *line)
{
	if (!match_regex(i, line))
		return INSERT_CORRECT;
	if (!match_regex(p, line))
		return PREV_CORRECT;
	if (!match_regex(f, line))
		return FIND_CORRECT;
	if (!match_regex(d, line))
		return DELETE_CORRECT;
	if (!match_regex(c, line))
		return CLEAR_CORRECT;
	return ERROR;
}

int read_line(char **line)
{
	int bytesRead;
	size_t nBytes = MAX_MSG;

	*line = (char *) malloc(MAX_MSG + 1);
	bytesRead = getline(line, &nBytes, stdin);
	return bytesRead;
}

int check_for_options(int argc, char * const argv[])
{
	int flags, opt;
	flags = 0;
	while ((opt = getopt(argc, argv, VALID_OPTION)) != ERROR) {
		if (opt == VALID_OPT)
			flags = 1;
		else {
			fprintf(stderr, "Usage: %s [-v] number of nodes in tree\n",
					argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if (!flags && argc > 1) {
		fprintf(stderr, "Usage: %s [-v] number of nodes in tree\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	return flags;
}

char *get_word(int command, char *line, int bytesRead)
{
	/* change '\n' into '\0' */
	line[bytesRead - 1] = '\0';
	char *pattern = line;
	switch (command) {
		case INSERT_CORRECT:
			pattern = index(line, LAST_INSERT);
			break;
		case FIND_CORRECT:
			pattern = index(line, LAST_FIND);
			break;
	}
	pattern = strpbrk(pattern + 1, CHAR_SET);
	/* delete unnecessary whitespaces at the end */
	char *end = strpbrk(pattern, WHITESPACE);
	int l_p = strlen(pattern);
	int l_e = (end == NULL ? 0 : strlen(end));
	pattern[l_p - l_e] = '\0';
	return pattern;
}

static int valid_int(long long int check)
{
	if ((check == LONG_MIN) || (check == LONG_MAX))
		return NO;
	else if ((check <= INT_MAX) && (check >= INT_MIN))
		return YES;
	return NO;
}

void get_number_prev(int *number, int *start, int *end, char *line)
{
	char *endptr;
	char *num = strpbrk(line, NUMBER_SET);
	long int check = strtol(num, &endptr, BASE);
	if (!valid_int(check))
		*number = ERROR;
	else {
		*number = (int) check;
		num = strchr(num, ' ');
		num = strpbrk(num, NUMBER_SET);
		check = strtol(num, &endptr, BASE);
		if (!valid_int(check))
			*start = ERROR;
		else {
			*start = (int) check;
			num = strchr(num, ' ');
			num = strpbrk(num, NUMBER_SET);
			check = strtol(num, &endptr, BASE);
			if (!valid_int(check))
				*end = ERROR;
			else
				*end = (int) check;
		}
	}
}

void get_number_delete(int *number, char *line)
{
	char *endptr;
	char *num = strpbrk(line, NUMBER_SET);
	long int check = strtol(num, &endptr, BASE);
	if (!valid_int(check))
		*number = ERROR;
	else
		*number = (int) check;
}

void ignore()
{
	fprintf(stdout, "ignored\n");
}

void on_success(int number)
{
	fprintf(stdout, "word number: %d\n", number);
}

void print_find(int succ)
{
	if ((succ == YES) || (succ == LEAF))
		fprintf(stdout, "YES\n");
	else if (succ == NO)
		fprintf(stdout, "NO\n");
}

void print_delete(int number)
{
	fprintf(stdout, "deleted: %d\n", number);
}

void print_clear()
{
	fprintf(stdout, "cleared\n");
}

void print_nodes(int nodes)
{
	fprintf(stderr, "nodes: %d\n", nodes);
}
