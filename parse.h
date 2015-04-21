/*
 * parse.h
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#ifndef PARSE_H_
#define PARSE_H_

#include <regex.h>

#define INSERT_CORRECT	108
#define PREV_CORRECT	109
#define FIND_CORRECT	110
#define DELETE_CORRECT	111
#define CLEAR_CORRECT	112


int compile_regex(regex_t *, const char *);
int compile_all(regex_t *, regex_t *, regex_t *, regex_t *, regex_t *);
void free_all(regex_t *, regex_t *, regex_t *, regex_t *, regex_t *);
int match(regex_t *, regex_t *, regex_t *, regex_t *, regex_t *, const char *);
int read_line(char **);
int match_regex(regex_t *, const char *);
int check_for_options(int, char * const[]);
char *get_word(int, char *, int);
void get_number_prev(int *, int *, int *, char *);
void get_number_delete(int *, char *);
void ignore();
void on_success(int);
void print_find(int);
void print_delete(int);
void print_clear();
void print_nodes(int);
#endif /* PARSE_H_ */
