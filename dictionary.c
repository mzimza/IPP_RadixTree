/*
 * dictionary.c
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <signal.h>
#include "defines.h"
#include "trie.h"
#include "parse.h"

char *line;
char *pattern;
int readBytes;
int matchType;
int number, start, end;
int counter, nodes, option_set;
int checkNodes;
int ignored;
regex_t insert_r, prev_r, find_r, delete_r, clear_r;

void exit_dictionary(int sig)
{
	free_all(&insert_r, &prev_r, &find_r, &delete_r, &clear_r);
	clear(&counter, &nodes);
	free(line);
	exit(0);
}

int main(int argc, char *argv[])
{
	/* clean everything nicely up, if SIGINT or SIGSEGV*/
	if (signal(SIGINT, exit_dictionary) == SIG_ERR)
		fprintf(stderr, "Signal handling error\n");
	if (signal(SIGSEGV, exit_dictionary) == SIG_ERR)
		fprintf(stderr, "Signal handling error\n");

	/* set options if valid */
	if ((option_set = check_for_options(argc, argv))) {
		nodes = 0;
		checkNodes = 0;
		ignored = NO;
	}

	/* prepare regexes for different commands */
	compile_all(&insert_r, &prev_r, &find_r, &delete_r, &clear_r);

	counter = 0;

	/* read from input and check for regexes */
	while (!feof(stdin)) {
		readBytes = read_line(&line);
		if (readBytes != ERROR) {
			matchType = match(&insert_r, &prev_r, &find_r, &delete_r, &clear_r,
					line);
			/* handle commands */
			switch (matchType) {
				case INSERT_CORRECT:
					pattern = get_word(INSERT_CORRECT, line, readBytes);
					if ((checkNodes = insert(pattern, NO_NUMBER, NO_NUMBER)) == ERROR) {
						ignore();
					}
					else {
						nodes = checkNodes;
						on_success(counter);
						counter++;
						if (option_set)
							print_nodes(nodes);
					}
					break;
				case PREV_CORRECT:
					get_number_prev(&number, &start, &end, line);
					if ((number == ERROR) || (start == ERROR) || (end == ERROR)
							|| ((checkNodes = prev(number, start, end)) == ERROR)) {
						ignore();
					}
					else {
						nodes = checkNodes;
						on_success(counter);
						counter++;
						if (option_set)
							print_nodes(nodes);
					}
					break;
				case FIND_CORRECT:
					pattern = get_word(FIND_CORRECT, line, readBytes);
					print_find(find(pattern));
					break;
				case DELETE_CORRECT:
					get_number_delete(&number, line);
					if ((number == ERROR) || ((checkNodes = delete(number)) == ERROR)) {
						ignore();
					}
					else {
						nodes = checkNodes;
						print_delete(number);
						if (option_set)
							print_nodes(nodes);
					}
					break;
				case CLEAR_CORRECT:
					clear(&counter, &nodes);
					print_clear();
					if (option_set)
						print_nodes(nodes);
					break;
				default:
					ignore();
			}
		}
		free(line);
		line = 0;
	}
	/* tidy up */
	free_all(&insert_r, &prev_r, &find_r, &delete_r, &clear_r);
	clear(&counter, &nodes);
	return 0;
}
