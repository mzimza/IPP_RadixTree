/*
 * trie.c
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "defines.h"
#include "trie.h"
#include "vector.h"

#define INDEXES	26
#define ASCII_SHIFT	97

struct node;

typedef struct labelInfo {
	char *label;
	int start;
	int end;
} labelInfo;

typedef struct edge {
	struct node *child;
	struct node *parent;
	labelInfo *label;
} edge;

typedef struct node {
	edge *edges[INDEXES];
	edge *parentEdge;
	int wordNum;
	int children;
} node;

typedef struct nodeInfo {
	node *node;
	int length;
	int start;
} nodeInfo;

vector *words = NULL;
vector *inserts = NULL;
node *root = NULL;
int nodes = 0;	// number of nodes in the dictionary
int shift = 0;	// shift in numeration after deletion

/********************************************************************************
 *					STATIC FUNCTIONS FOR THIS FILE								*
 *********************************************************************************/

/**************************STRING OPERATIONS************************************/
// get the word to be inserted by prev
static char *concatenate(int number, int start, int end)
{
	char *word = NULL;
	node *currentNode = NULL;
	int size = end - start + 1;
	currentNode = ((nodeInfo *) (at(words, number)))->node;
	word = malloc(sizeof(char) * (size + 1));
	strncpy(word, currentNode->parentEdge->label->label + start, size);
	word[size] = '\0';
	return word;

}

static char *substring(char *pattern, int start, int end)
{
	int length = end - start + 1;
	char *new = malloc(sizeof(char) * length + 1);
	strncpy(new, pattern + start, length);
	new[length] = '\0';
	return new;
}

static int get_index(char c)
{
	return (c - ASCII_SHIFT);
}

static char *suffix(char *pattern, int elementsFound)
{
	if (pattern != NULL) {
		int length = strlen(pattern);
		return substring(pattern, elementsFound, length);
	}
	return NULL;
}

static int check_coincident(char *pattern, int *elementsFound, char *label,
		int start, int end)
{
	int i = 0;
	int length_p, length_l;
	length_p = strlen(pattern);
	length_l = end - start + 1;
	/* follow the pattern */
	while ((i < length_p) && (i < length_l) && (pattern[i] == label[i + start])) {
		i++;
		(*elementsFound)++;
	}
	/* either found pattern or the label ended before pattern */
	if ((i == length_p) || (i == length_l)) {
		return YES;
	}
	else
		/*pattern differs from label */
		return NO;
}

static char *store_label(char *word)
{
	int length = strlen(word);
	char *label = malloc(sizeof(char) * strlen(word) + 1);
	strcpy(label, word);
	label[length] = '\0';
	push_back(inserts, label);
	return label;
}

/*********************************TRIE MANIPULATION*****************************/

static int is_leaf(node *node)
{
	if (node->children == 0)
		return YES;
	else
		return NO;
}

static labelInfo *create_labelInfo(char *label, int start, int end)
{
	labelInfo *info = malloc(sizeof(labelInfo));
	info->label = label;
	info->start = start;
	info->end = end;
	return info;
}

static edge *create_edge(char *word, int start, int end, node *parentNode)
{
	edge *edge = malloc(sizeof(struct edge));
	edge->label = create_labelInfo(word, start, end);
	edge->parent = (parentNode == NULL ? NULL : parentNode);
	edge->child = NULL;
	return edge;
}

static node *create_node(int number, edge *parentEdge)
{
	node *node = malloc(sizeof(struct node));
	int i;
	for (i = 0; i < INDEXES; i++)
		node->edges[i] = NULL;
	node->parentEdge = (parentEdge == NULL ? NULL : parentEdge);
	node->wordNum = number;
	node->children = 0;
	nodes++;
	return node;
}

static nodeInfo *create_nodeInfo(node *node, int length, int start)
{
	nodeInfo *info = malloc(sizeof(nodeInfo));
	info->node = node;
	info->length = length;
	info->start = start;
	push_back(words, (void *) info);
	return info;
}

static node *init_dict(char *word)
{
	char *label = store_label(word);
	int index = get_index(label[0]);
	node *node = create_node(NO_NUMBER, NULL);
	node->parentEdge = NULL;
	int length = strlen(label);
	node->edges[index] = create_edge(label, 0, length - 1, node);
	node->children++;
	if (words == NULL)
		node->edges[index]->child = create_node(0, node->edges[index]);
	else
		node->edges[index]->child = create_node(size(words),
				node->edges[index]);
	/* add leaf to leaves vector */
	create_nodeInfo(node->edges[index]->child, length, 0);
	return node;
}

/* splits edge into two and add a node in between */
static node *split_edge(edge *old, int index, int nodeNr)
{
	/* higher label with the beginning of the word */
	int oldEnd = old->label->start + index - 1;
	/* lower label with the suffix */
	int newStart = old->label->start + index;
	int newEnd = old->label->end;
	old->label->end = oldEnd;
	node *midNode = create_node(nodeNr, old);
	midNode->children++;
	char first = *(old->label->label + newStart);
	midNode->edges[get_index(first)] = create_edge(old->label->label, newStart,
			newEnd, midNode);
	midNode->edges[get_index(first)]->child = old->child;
	old->child->parentEdge = midNode->edges[get_index(first)];
	old->child = midNode;
	return midNode;
}

/* add node with parent edge to an existing node */
static void add_node(node *mid, int index, int nodeNr, char *word, int start)
{
	int next = get_index(word[index]);
	mid->edges[next] = create_edge(word, index, strlen(word) - 1, mid);
	mid->edges[next]->child = create_node(nodeNr, mid->edges[next]);
	mid->children++;
	//nodeInfo *info = NULL;
	create_nodeInfo(mid->edges[next]->child, strlen(word), start);
}

/* variation of add_node, but used with prev command */
static void add_node_prev(node *mid, int wordStart, int start, int end, int nodeNr,
		char *word)
{
	int next = get_index(word[start]);
	mid->edges[next] = create_edge(word, start, end, mid);
	mid->edges[next]->child = create_node(nodeNr, mid->edges[next]);
	mid->children++;
	create_nodeInfo(mid->edges[next]->child, end - wordStart + 1, wordStart);
}

/********************************************************************************
*								TRIE OPERATIONS									*
*********************************************************************************/

/* returns number of nodes on success, else ERROR,
 * if start set to NO_NUMBER adds new word to vector */
int insert(char *word, int start, int end)
{
	/* if the dictionary is empty */
	if (root == NULL) {
		if (words == NULL) {
			words = malloc(sizeof(vector));
			init(words);
		}
		if (inserts == NULL) {
			inserts = malloc(sizeof(vector));
			init(inserts);
		}
		root = init_dict(word);
		return nodes;
	}
	node *crossNode = root;
	int elementsFound = 0;
	int index;
	int wordLength;
	/* calculate word's length */
	if (start != -1) {
		wordLength = end - start + 1;
		index = get_index(word[start]);
	}
	else {
		wordLength = strlen(word);
		index = get_index(word[elementsFound]);
	}
	/* while there is a path with given letters */
	while (crossNode->edges[index] != NULL) {
		/* crossNode is not a leaf, check the label if contains the pattern */
		edge *edge = crossNode->edges[index];
		int diff = elementsFound;
		char *suff;
		if (start != -1)
			suff = substring(word, start + elementsFound, end);
		else
			suff = suffix(word, elementsFound);
		int matchFound = check_coincident(suff, &elementsFound,
				edge->label->label, edge->label->start, edge->label->end);
		free(suff);
		suff = NULL;
		diff = elementsFound - diff;
		int labelLength = edge->label->end - edge->label->start + 1;
		if (matchFound && (diff < labelLength)) {
			/* case 1: pattern ended, before label -> just add a mid node
			 * different word's starts for insert and prev operations! */
			node *mid = split_edge(edge, diff, size(words));
			if (start == NO_NUMBER)
				create_nodeInfo(mid, wordLength, 0);
			else {
				create_nodeInfo(mid, wordLength,
						edge->label->start - (elementsFound - diff));
			}
			return nodes;
		}
		else if (matchFound && (elementsFound < wordLength)) {
			/* case 2: got to next node, because label ended
			 * different word's starts for insert and prev operations! */
			crossNode = edge->child;
			if (start == NO_NUMBER)
				index = get_index(word[elementsFound]);
			else
				index = get_index(word[start + elementsFound]);
		}
		else if (matchFound && (diff == labelLength)
				&& (elementsFound == wordLength)
				&& (edge->child->wordNum != -1)) {
			/* case 3: such word already exists! */
			return ERROR;
		}
		else if (matchFound && (diff == labelLength)
				&& (elementsFound == wordLength)
				&& (edge->child->wordNum == -1)) {
			/* case 4: there is a node, but with -1, change wordNum */
			edge->child->wordNum = size(words);
			if (start == -1)
				create_nodeInfo(edge->child, wordLength, 0);
			else
				create_nodeInfo(edge->child, wordLength,
						edge->label->end - elementsFound + 1);
			return nodes;
		}
		else if (!matchFound) {
			/* case 5: label differs from pattern, new mid node
			 * different word's starts for insert and prev operations! */
			node *mid = split_edge(edge, diff, NO_NUMBER);
			if (start == -1) {
				add_node(mid, elementsFound, size(words), store_label(word), 0);
			}
			else {
				add_node_prev(mid, start, start + elementsFound, end,
						size(words), word);
			}
			return nodes;
		}
	}
	if ((elementsFound < wordLength) && (start == NO_NUMBER)) {
		/* case 6: normal insert, edge with new first symbol, add to inserts */
		add_node(crossNode, elementsFound, size(words), store_label(word), 0);
	}
	else if ((elementsFound < wordLength) && (start != NO_NUMBER)) {
		/* case 6: adding word from prev command, does not store label */
		add_node_prev(crossNode, start, start + elementsFound, end, size(words),
				word);
	}
	return nodes;
}

int prev(int number, int start, int end)
{
	int length;
	char *pattern;
	nodeInfo *info = NULL;
	if (words != NULL) {
		void *err = NULL;
		err = at(words, number);
		if (err != NULL) {
			info = (nodeInfo *) err;
			if (info != NULL) {
				length = info->length;
				if ((start <= end) && (start < length) && (end < length)) {
					// go up and get the string
					labelInfo *label = info->node->parentEdge->label;
					int myStart;
					int myEnd;
					myStart = info->start + start;
					myEnd = myStart + (end - start + 1) - 1;
					pattern = concatenate(number, myStart, myEnd);
					// go down the tree and see if such a pattern exists
					int result = find(pattern);
					if (result != LEAF) {
						free(pattern);
						pattern = NULL;
						return insert(label->label, myStart, myEnd);
					}
					else {
						free(pattern);
						pattern = NULL;
						return ERROR;
					}
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
	else
		return ERROR;
}

/*********************************TRIE MANIPULATION*****************************/

/* finds next not NULLed edge */
static int find_next_child(node *node, int first)
{
	int i;
	for (i = 0; i < INDEXES; i++)
		if ((i != first) && (node->edges[i] != NULL))
			return i;
	return ERROR;
}

static void merge_edges(edge *upper, edge *lower, int nodeNr)
{
	node *toDelete = upper->child;
	int number = toDelete->wordNum;
	upper->child = lower->child;
	upper->label->label = lower->label->label;
	upper->label->start = lower->label->start
			- (upper->label->end - upper->label->start + 1);
	upper->label->end = lower->label->end;
	lower->child->parentEdge = upper;
	free(lower->label);
	lower->label = NULL;
	free(lower);
	lower = NULL;
	free(toDelete);
	toDelete = NULL;
	nodes--;
	if ((nodeNr != number) && (number != NO_NUMBER))
		delete_at(words, number);
}

static void free_node_edge(node **node)
{
	free((*node)->parentEdge->label);
	(*node)->parentEdge->label = NULL;
	free((*node)->parentEdge);
	(*node)->parentEdge = NULL;
	free(*node);
	*node = NULL;
	node = NULL;
	nodes--;
}

/********************************************************************************
*								TRIE OPERATIONS									*
*********************************************************************************/

int delete(int number)
{
	if (words != NULL) {
		int vecNumber = number;
		int nextChild = 0;
		void *error = at(words, vecNumber);
		if (error != NULL) {
			node **deleteNode = &((nodeInfo *) at(words, vecNumber))->node;
			if (deleteNode != NULL) {
				assert(*deleteNode != root);
				int parentChildren = (*deleteNode)->parentEdge->parent->children;
				node *parentNode = (*deleteNode)->parentEdge->parent;
				labelInfo *parentLabel = (*deleteNode)->parentEdge->label;
				if ((parentNode == root) && (is_leaf(*deleteNode))
						&& (parentChildren == 1)) {
					/* case 1: only this one node and root - delete both */
					free(root);
					root = NULL;
					free_node_edge(deleteNode);
					nodes--;
				}
				else if ((is_leaf(*deleteNode)) && (parentChildren == 2)
						&& ((parentNode)->wordNum == -1)
						&& (parentNode != root)) {
					/* case 2: merge labels, because there is only one child left */
					nextChild = find_next_child((parentNode),
							get_index(parentLabel->label[parentLabel->start]));
					merge_edges((parentNode)->parentEdge,
							(parentNode)->edges[nextChild], number);
					free_node_edge(deleteNode);
				}
				else if (is_leaf(*deleteNode)) {
					/* case 3: only delete the node and its parentLabel */
					(parentNode)->children--;
					labelInfo *nextLabel = (*deleteNode)->parentEdge->label;
					int next = get_index(nextLabel->label[nextLabel->start]);
					free_node_edge(deleteNode);
					parentNode->edges[next] = NULL;
				}
				else if (!is_leaf(*deleteNode)
						&& ((*deleteNode)->children >= 2)) {
					/* case 4: easy, just change deleteNode->wordNum to -1 */
					(*deleteNode)->wordNum = NO_NUMBER;
				}
				else if (!is_leaf(*deleteNode)
						&& ((*deleteNode)->children == 1)) {
					/* case 5: merge deleteNode->parentEdge with child's edge */
					nextChild = find_next_child(*deleteNode, NO_NUMBER);
					merge_edges((*deleteNode)->parentEdge,
							(*deleteNode)->edges[nextChild], number);
				}
				shift = delete_at(words, vecNumber);
				return (shift == ERROR ? ERROR : nodes);
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

int find(char *pattern)
{
	if ((words == NULL) || (number_of_elements(words) == 0))
		return NO;
	node *traverseNode = root;
	int elementsFound = 0;
	int length = strlen(pattern);
	int diff = 0;
	int i = 0;
	int foundNext = YES;
	while ((traverseNode != NULL) && (!is_leaf(traverseNode))
			&& (elementsFound < length) && (foundNext)) {
		i = get_index(pattern[elementsFound]);
		if (traverseNode->edges[i] != NULL) {
			// the pattern exists, check how much of it
			edge *nextEdge = traverseNode->edges[i];
			diff = elementsFound;
			char *suff = suffix(pattern, elementsFound);
			foundNext = check_coincident(suff, &elementsFound,
					nextEdge->label->label, nextEdge->label->start,
					nextEdge->label->end);
			free(suff);
			suff = NULL;
			diff = elementsFound - diff;
			if (elementsFound == length) {
				/* found pattern */
				if ((diff < strlen(nextEdge->label->label))
						|| ((nextEdge->child != NULL)
								&& (nextEdge->child->wordNum == NO_NUMBER)))
					return YES;
				else {
					/* there is such a word already */
					return LEAF;
				}
			}
			/* continue search */
			traverseNode = nextEdge->child;
		}
		else
			return NO;
	}
	return NO;
}

static void clear_tree(node **currentNode)
{
	if (currentNode != NULL) {
		if (*currentNode != NULL) {
			int i = 0;
			edge *currentEdge = NULL;
			for (i = 0; i < INDEXES; i++) {
				assert(*currentNode != NULL);
				currentEdge = (*currentNode)->edges[i];
				if (currentEdge != NULL) {
					assert(currentEdge != NULL);
					clear_tree(&((currentEdge)->child));
					free((currentEdge)->label);
					currentEdge->label = NULL;
					free(currentEdge);
					currentEdge = NULL;
				}
			}
			free(*currentNode);
			*currentNode = NULL;
			currentNode = NULL;
		}
	}
}

/* clear tree and vectors, zero counter and nodesNum */
void clear(int *counter, int *nodesNum)
{
	if (root != NULL)
		clear_tree(&root);
	if (words != NULL) {
		delete_all(words);
		free(words);
	}
	if (inserts != NULL) {
		delete_all(inserts);
		free(inserts);
	}
	words = NULL;
	inserts = NULL;
	nodes = 0;
	shift = 0;
	*counter = 0;
	*nodesNum = nodes;
}
