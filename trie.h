/*
 * trie.h
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#ifndef TRIE_H_
#define TRIE_H_

/********************************************************************************
*			TRIE OPERATIONS (as specified in the assignment)					*
*********************************************************************************/
int insert(char *, int, int);/* inserts [char*], if [int] != -1 inserts substring */
int prev(int, int, int);
int delete (int);
int find (char *);
void clear(int *, int *);

#endif /* TRIE_H_ */
