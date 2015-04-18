/*
 * trie.c
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "trie.h"
#include "vector.h"

#define INDEXES	26
#define ASCII_SHIFT	97

struct node;

typedef struct {
	struct node *child;
	struct node *parent;
	char *label;
} edge;

typedef struct node {
	edge *edges[INDEXES];
	edge *parentEdge;
	int wordNum;
	int children;
} node;

typedef struct {
	node *node;
	int length;
} nodeInfo;

vector *words = NULL;
node *root = NULL;
int nodes = 0;	// number of nodes in the dictionary
int shift = 0;	// shift in numeration after deletion

int is_leaf(node *node)
{
	if (node->children == 0)
		return YES;
	else
		return NO;
}

// concatenate word from bottom to top
char *concatenate(int number)
{
	char *word = NULL;
	char *suffix = "\0";
	node *currentNode = NULL;
	int size;
	*currentNode = *((node *) (at(words, number)));
	while (currentNode->parentEdge != NULL) {
		//z węzła w którym jestem chcę wziąc label z krawedzi do ojca
		word = malloc(
				sizeof(char)
						* (strlen(currentNode->parentEdge->label)
								+ strlen(suffix)) + 1);
		size = sprintf(word, "%s%s", currentNode->parentEdge->label, suffix);
		if (size == ERROR) {
			printf("cus nie pyka\n");
			return NULL;
		}
		suffix = word;
		*currentNode = *(currentNode->parentEdge->parent);
	}
	return word;
}

char *substring(char *pattern, int start, int end)
{
	int length = end - start + 1;
	char *new = malloc(sizeof(char) * length + 1);
	strncpy(new, pattern + start, length);
	new[length] = '\0';
	return new;
}

//TODO returns number of nodes on success, else ERROR
int insert(char *word)
{
	//word = substring(word, )
	if (words == NULL)
		init(words);
	/* when it's either not in the dictionary, or just the prefix */
	if (find(word) != LEAF) {

	}
	return ERROR;
}

int prev(int number, int start, int end)
{
	/* is the dictionary not empty?
	 * is the word number valid?
	 * is the length > start & >end?
	 * are start & end >= 0?
	 * is start < end?
	 */
	int length;
	char *pattern;
	if (words != NULL) {
		if (at(words, number) != NULL) {
			length = ((nodeInfo *) at(words, number))->length;
			if ((start < end) && (start < length) && (end < length)) {
				// go up and get the string
				pattern = substring(concatenate(number), start, end);
				// go down the tree and see if such a pattern exists
				if (find(pattern) == NO)
					return insert(pattern);
				else
					return ERROR;
			}
			else
				return ERROR;
		}
		else
			return ERROR;
	}
	else
		return ERROR;
}

//TODO
int delete(int number)
{
	if (at(words, number) != NULL)
		return delete_at(words, number);
	else
		return ERROR;
}

int get_index(char c)
{
	return (c - ASCII_SHIFT);
}

char *suffix(char *pattern, int elementsFound)
{
	if (pattern != NULL) {
		int length = strlen(pattern);
		return substring(pattern, elementsFound, length);
	}
	return NULL;
}

int check_coincident(char *pattern, int *elementsFound, char *label)
{
	int i = 0;
	int length_p, length_l;
	length_p = strlen(pattern);
	length_l = strlen(label);
	while ((i < length_p) && (i < length_l) && (pattern[i] == label[i])) {
		i++;
		(*elementsFound)++;
	}
	/* either found pattern or the label ended before pattern */
	if ((i == length_p) || (i == length_l))
		return YES;
	else /*pattern differs from label */
		return NO;
}

//TODO
int find(char *pattern)
{
	if (number_of_elements(words) == 0)
		return ERROR;
	node *traverseNode = root;
	int elementsFound = 0;
	int length = strlen(pattern);
	int i;
	int foundNext = YES;
	while ((traverseNode != NULL) && (!is_leaf(traverseNode))
			&& (elementsFound < length) && (foundNext)) {
		i = 0;
		i = get_index(pattern[elementsFound]);
		if (traverseNode->edges[i] != NULL) {
			// the pattern exists, chceck how much of it
			edge nextEdge = *(traverseNode->edges[i]);
			foundNext = check_coincident(suffix(pattern, elementsFound),
					&elementsFound, nextEdge.label);
			traverseNode = nextEdge.child;
		}
		else
			return NO;
	}
	if (elementsFound == length) {
		if (traverseNode != NULL)
			return YES;
		else
			return LEAF;
	}
	else
		return NO;
}

//TODO
void clear(int *counter, int *nodesNum)
{
	delete_all(words);
	free(words);
	words = NULL;
	nodes = 0;
	*counter = 0;
	*nodesNum = nodes;
}
