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
regex_t insert_r, prev_r, find_r, delete_r, clear_r;

void exit_dictionary(int sig)
{
	printf("Caught interrupt signal. Exiting...\n");
	free_all(&insert_r, &prev_r, &find_r, &delete_r, &clear_r);
	free(line);
	exit(0);
}

int main(int argc, char *argv[])
{
	/* clean everything nicely up, if SIGINT */
	if (signal(SIGINT, exit_dictionary) == SIG_ERR)
		fprintf(stderr, "Signal handling error\n");
	if (signal(SIGSEGV, exit_dictionary) == SIG_ERR)
		fprintf(stderr, "Signal handling error\n");

	if ((option_set = check_for_options(argc, argv))) {
		nodes = 0;
		checkNodes = 0;
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
			switch (matchType) {
				case INSERT_CORRECT:
					pattern = get_word(INSERT_CORRECT, line, readBytes);
					printf("%s", pattern);
					if ((checkNodes = insert(pattern)) == ERROR)
						ignore();
					else {
						nodes = checkNodes;
						on_success(counter);
						counter++;
					}
					break;
				case PREV_CORRECT:
					get_number_prev(&number, &start, &end, line);
					printf("prev: %d %d %d\n", number, start, end);
					if ((checkNodes = prev(number, start, end)) == ERROR)
						ignore();
					else {
						nodes = checkNodes;
						on_success(counter);
						counter++;
					}
					break;
				case FIND_CORRECT:
					pattern = get_word(FIND_CORRECT, line, readBytes);
					printf("%s", pattern);
					print_find(find(pattern));
					break;
				case DELETE_CORRECT:
					get_number_delete(&number, line);
					printf("%d \n", number);
					if ((checkNodes = delete(number)) == ERROR)
						ignore();
					else {
						nodes = checkNodes;
						print_delete(number);
						counter++;
					}
					break;
				case CLEAR_CORRECT:
					clear(&counter, &nodes);
					print_clear();
					break;
				default:
					ignore();
			}
			if (option_set)
				print_nodes(nodes);
		}
		free(line);
		line = 0;
	}
	/* tidy up */
	free_all(&insert_r, &prev_r, &find_r, &delete_r, &clear_r);
	return 0;
}
