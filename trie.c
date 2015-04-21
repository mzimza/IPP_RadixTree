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
#define NO_NUMBER	-1

struct node;

typedef struct edge {
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
	currentNode = ((nodeInfo *) (at(words, number)))->node;
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
		currentNode = currentNode->parentEdge->parent;
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
	fprintf(stderr, "<trie check_coincident> pattern: %s label: %s\n", pattern,
			label);
	while ((i < length_p) && (i < length_l) && (pattern[i] == label[i])) {
		i++;
		(*elementsFound)++;
	}
	/* either found pattern or the label ended before pattern */
	if ((i == length_p) || (i == length_l)) {
		fprintf(stderr, "<trie check_coincident> YES\n");
		return YES;
	}
	else
		/*pattern differs from label */
		return NO;
}

void create_edge(edge **edge, char *word, node **parentNode)
{
	*edge = malloc(sizeof(struct edge));
	(*edge)->label = malloc(sizeof(char) * strlen(word) + 1);
	strcpy((*edge)->label, word);
	(*edge)->parent = (parentNode == NULL ? NULL : *parentNode); //(parentNode == NULL ? NULL : *parentNode);
	//(*edge)->parent = NULL;
	(*edge)->child = NULL;
}

void create_node(node **node, int number, edge **parentEdge)
{
	*node = malloc(sizeof(struct node));
	int i;
	for (i = 0; i < INDEXES; i++)
		(*node)->edges[i] = NULL;
	(*node)->parentEdge = (parentEdge == NULL ? NULL : *parentEdge);
	(*node)->wordNum = number;
	(*node)->children = 0;
	nodes++;
}

void init_dict(node **node, char *word)
{
	int index = get_index(word[0]);
	create_node(node, NO_NUMBER, NULL);
	(*node)->parentEdge = NULL;

	create_edge(&((*node)->edges[index]), word, node);
	(*node)->children++;
	//((*node)->edges[index])->parent = *node;

	create_node(&(((*node)->edges[index])->child), 0, &((*node)->edges[index]));
	//(((*node)->edges[index])->child)->parentEdge = (*node)->edges[index];
	nodeInfo *nodeIn = malloc(sizeof(nodeInfo));
	nodeIn->node = (*node)->edges[index]->child;
	nodeIn->length = strlen(word);
	push_back(words, (void *) nodeIn);
	nodeIn = ((nodeInfo *) at(words, 0));
	fprintf(stderr, "<init_dict> correct? length: %d\n", nodeIn->length);
}

void print_tree(node *currentNode)
{
	int i = 0;
	edge *currentEdge = NULL;
	fprintf(stderr, "<print_tree> node_number: %d\n", (currentNode)->wordNum);
	fprintf(stderr, "<print_tree> children: %d\n", (currentNode)->children);
	fprintf(stderr, "---------------------------------\n");
	if (currentNode->parentEdge != NULL) {
		fprintf(stderr, "<print_tree> parent label: %s\n",
				(currentNode)->parentEdge->label);
	}
	for (i = 0; i < INDEXES; i++) {
		(currentEdge) = (currentNode)->edges[i];
		if (currentEdge != NULL) {
			fprintf(stderr, "<print_tree> label: %s\n", (currentEdge)->label);
			fprintf(stderr,
					"<print_tree> parent node_number: %d and my number: %d\n",
					(currentEdge)->parent->wordNum,
					currentEdge->child->wordNum);
			fprintf(stderr, "<print_tree> parent children: %d\n",
					(currentEdge)->parent->children);
			print_tree(((currentEdge)->child));
		}
	}
}

void easy_insert(node **rootNode)
{
	fprintf(stderr, "easy\n");
	if (words == NULL) {
		words = malloc(sizeof(vector));
		init(words);
		init_dict(rootNode, "aaaa");
		print_tree(*rootNode);
	}	// dodaje "aa"
	node *node = *rootNode;
	int elementsFound = 0;
	if (node->edges['a' - 97] != NULL) {
		// nie jest liściem, ide po label, sprawdzam czy sie zawiera, różni itp
		edge **edge = &(node->edges[0]); /* to nie działa!! trzeba inaczej to zapisywać */
		int diff = elementsFound;
		int matchFound = check_coincident(suffix("aa", elementsFound),
				&elementsFound, (*edge)->label);
		diff = elementsFound - diff;
		if (matchFound && (diff < strlen((*edge)->label))) {
			// zawiera się -> nowy mid wezel
			struct node *oldNode = NULL; //tu też w ten sposób nie działa :/
			fprintf(stderr, "<easy_insert> edge->child->nr: %d\n",
					(*edge)->child->wordNum);
			oldNode = ((*edge)->child);	//zapamietuje stary węzeł
			create_node(&((*edge)->child), 1, edge);	//tworze nowy
			//((*edge)->child)->parentEdge = (*edge);
			((*edge)->child)->children++;
			((*edge))->label = "aa";
			create_edge(&(((*edge)->child)->edges[0]), "aa", &((*edge)->child)); //tworzę nową krawedź
			//((*edge)->child)->edges[0]->parent =(*edge)->child;
			((*edge)->child->edges[0]->child) = (oldNode);
			print_tree(root);
		}
		else if (matchFound && (diff == strlen((*edge)->label))) {
			// takie słowo istnieje
			fprintf(stderr, "ignored\n");
		}
		else if (!matchFound) {
			// rózni się -> nowy mid węzeł
			struct node *oldNode = NULL;
			oldNode = ((node->edges[0])->child);	//zapamietuje stary węzeł
			create_node(&(node->edges[0]->child), NO_NUMBER, &(node->edges[0]));//tworze nowy
			(node->edges[0]->child)->parentEdge = (node->edges[0]);
			(node->edges[0]->child)->children++;
			(node->edges[0])->label = "aa";
			create_edge(&((node->edges[0]->child)->edges[0]), "aa",
					&(node->edges[0]->child)); //tworzę nową krawedź z "aa"
			//(node->edges[0]->child)->edges[0]->parent = node->edges[0]->child;
			(node->edges[0]->child->edges[0]->child) = oldNode;
			create_edge(&((node->edges[0]->child)->edges[1]), "b",
					&(node->edges[0]->child));
			//(node->edges[0]->child)->edges[1]->parent = node->edges[0]->child;
			create_node(&((node->edges[0]->child)->edges[1]->child), 1,
					&(node->edges[0]->child)->edges[1]);
			(node->edges[0]->child)->edges[1]->child->parentEdge =
					(node->edges[0]->child)->edges[1];
			(node->edges[0]->child)->children++;
			print_tree(root);
		}

	}
	else {
		// z buta wstawiam całość, czyli create_node i create_edge
		fprintf(stderr, "z buta całość dodaje\n");
		create_edge(&(node->edges[1]), "b", &node);
		node->children++;
		create_node(&(node->edges[1]->child), 1, &(node->edges[1]));
		print_tree(root);
	}
}

//TODO returns number of nodes on success, else ERROR
int insert(char *word)
{
	//easy_insert(&root);
	if (root == NULL) {
		words = malloc(sizeof(vector));
		init(words);
		init_dict(&root, word);
		print_tree(root);
		return nodes;
	}
	node *crossNode = root;
	int elementsFound = 0;
	int wordLength = strlen(word);
	int index = get_index(word[elementsFound]);
	// dopóki istnieje ścieżka zadana tymi literami
	while (crossNode->edges[index] != NULL) { //wyrzuciłam crossNode != NULL
		// nie jest liściem, ide po label, sprawdzam czy sie zawiera, różni itp
		edge **edge = &(crossNode->edges[index]);
		int diff = elementsFound;
		int matchFound = check_coincident(suffix(word, elementsFound),
				&elementsFound, (*edge)->label);
		diff = elementsFound - diff;
		if (matchFound && (diff < strlen((*edge)->label))) {
			// skonczyl sie pattern, czyli zawiera się -> nowy mid wezel
			struct node *oldNode = NULL; //tu też w ten sposób nie działa :/
			oldNode = ((*edge)->child);	//zapamietuje stary węzeł
			create_node(&((*edge)->child), shift + size(words), edge);//tworze nowy
			(*edge)->child->children++;
			char *oldLabel = suffix((*edge)->label, diff);
			(*edge)->label = substring((*edge)->label, 0, diff - 1);
			create_edge(&((*edge)->child->edges[get_index(oldLabel[0])]),
					oldLabel, &((*edge)->child)); //tworzę nową krawedź
			((*edge)->child->edges[get_index((*edge)->label[0])]->child) =
					oldNode;
			oldNode->parentEdge = (*edge)->child->edges[get_index(oldLabel[0])];
			print_tree(root);
			push_back(words, (*edge)->child);
			return nodes;
		}
		else if (matchFound && (elementsFound < wordLength)) {
			// ide dalej, bo nie skończył mi się pattern, a przeszłam po labelu
			fprintf(stderr,
					"<insert> skończyła się krwedz, ide dalej do wezla nr: %d\n",
					(*edge)->child->wordNum);
			crossNode = (*edge)->child;
			index = get_index(word[elementsFound]);
		}
		else if (matchFound && (diff == strlen((*edge)->label))
				&& (elementsFound == wordLength)) {
			// takie słowo istnieje
			return ERROR;
		}
		else if (!matchFound) {
			// rózni się -> nowy mid węzeł
			fprintf(stderr, "match not found\n");
			struct node *oldNode = NULL;
			oldNode = (*edge)->child;	//zapamietuje stary węzeł
			create_node(&((*edge)->child), NO_NUMBER, edge);	//tworze nowy
			(*edge)->child->children++;
			(*edge)->label = substring((*edge)->label, 0, diff - 1); //poczatek labela
			int nextIndex = get_index((*edge)->label[diff]);
			create_edge(&(((*edge)->child)->edges[nextIndex]),
					suffix((*edge)->label, diff), &((*edge)->child)); //tworzę nową krawedź z "aa"
			((*edge)->child->edges[nextIndex]->child) = oldNode;
			int nextIndexWord = get_index(word[elementsFound]);
			create_edge(&((*edge)->child->edges[nextIndexWord]),
					suffix(word, elementsFound), &((*edge)->child));
			create_node(&((*edge)->child->edges[nextIndexWord]->child),
					shift + size(words),
					&((*edge)->child)->edges[nextIndexWord]);
			//(node->edges[0]->child)->edges[1]->child->parentEdge = (node->edges[0]->child)->edges[1];
			(*edge)->child->children++;
			print_tree(root);
			push_back(words, (*edge)->child->edges[nextIndexWord]->child);
			return nodes;
		}
	}
	if (elementsFound < wordLength) {
		// z buta wstawiam
		fprintf(stderr,
				"<insert> z buta całość dodaje, jestem w wezle nr: %d\n",
				crossNode->wordNum);
		create_edge(&(crossNode->edges[get_index(word[elementsFound])]),
				suffix(word, elementsFound), &crossNode);
		crossNode->children++;
		create_node(&(crossNode->edges[get_index(word[elementsFound])]->child),
				shift + size(words),
				&(crossNode->edges[get_index(word[elementsFound])]));
		print_tree(root);
		push_back(words,
				crossNode->edges[get_index(word[elementsFound])]->child);

	}
	return nodes;
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
			fprintf(stderr, "<prev> at(words, number) != NULL i length: %d\n",
					length);
			if ((start <= end) && (start < length) && (end < length)) {
				// go up and get the string
				pattern = substring(concatenate(number), start, end);
				// go down the tree and see if such a pattern exists
				if (find(pattern) != LEAF) {
					fprintf(stderr, "nie ma takiego slowa: %s\n", pattern);
					return insert(pattern);
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

int find_next_child(node *node, int first)
{
	int i;
	for (i = 0; i < INDEXES; i++)
		if ((i != first) && (node->edges[i] != NULL))
			return i;
	return ERROR;
}

char *concatenate_strings(char *one, char *two)
{
	char *word = malloc(sizeof(char) * (strlen(one) + strlen(two)) + 1);
	int size = sprintf(word, "%s%s", one, two);
	if (size == ERROR) {
		printf("cus nie pyka male\n");
		return NULL;
	}
	return word;
}

//TODO roota tez chcemy usunac jezeli bedzie to ostatnie slowo usuwane,
//wtedy tez nulluje wektor, ale shift zostaje
int delete(int number)
{
	int vecNumber = number - shift;
	int nextChildIndex = 0;
	char *newLabel;
	node **deleteNode = &((nodeInfo *) at(words, vecNumber))->node;
	if (deleteNode != NULL) {
		shift = delete_at(words, vecNumber);
		if (shift != ERROR) {
			int parentChildren = (*deleteNode)->parentEdge->parent->children;
			node **parentNode = &(*deleteNode)->parentEdge->parent;
			node **secondChild;
			if (((*deleteNode)->parentEdge == NULL) && (parentChildren == 1)) {
				//jest tylko ostatnie słowo, trzeba usunąć też roota
				fprintf(stderr, "<delete> usuwam wszystko i roota\n");
				free((*deleteNode)->parentEdge->label);
				free((*deleteNode)->parentEdge);
				free(*deleteNode);
				free(*parentNode);
				*parentNode = NULL;
			}
			//dopóki moim ojcem nie jest root
			else if ((is_leaf(*deleteNode)) && (parentChildren == 2) && ((*parentNode)->wordNum == -1)) {
				//łączę parentNode->parentEdge z drugim dzieckiem parentNode
				nextChildIndex = find_next_child((*parentNode), get_index((*deleteNode)->parentEdge->label[0]));
				newLabel = concatenate_strings((*parentNode)->parentEdge->label, (*parentNode)->edges[nextChildIndex]->label);
				free((*parentNode)->parentEdge->label);
				(*parentNode)->parentEdge->label = newLabel;
fprintf(stderr, "<delete> new label: %s\n", (*parentNode)->parentEdge->label);
				secondChild = &((*parentNode)->edges[nextChildIndex]->child);
				(*parentNode)->parentEdge->child = *secondChild;
				(*secondChild)->parentEdge = (*parentNode)->parentEdge;
				free((*deleteNode)->parentEdge->label);
				free((*deleteNode)->parentEdge);
				free(*parentNode);
				free(*deleteNode);
		}
		else if (is_leaf(*deleteNode) && (parentChildren == 2)
				&& ((*parentNode)->wordNum != -1)) {
			//tylko usuwamy deleteNode->parentEdge i deleteNode
		}
		else if (is_leaf(*deleteNode)) {
			//tylko usuwamy deleteNode->parentEdge i deleteNode, bo nie moze byc 1 syn i -1word num
		}
		else if (!is_leaf(*deleteNode) && ((*deleteNode)->children >= 2)) {
			// easy, just change deleteNode->wordNum to -1
			(*deleteNode)->wordNum = NO_NUMBER;
		}
		else if (!is_leaf(*deleteNode) && ((*deleteNode)->children == 1)
				&& (parentChildren >= 2)) {
			//łącze deleteNode->parentEdge z labelem dziecka

		}

		return shift;
	}
	else
		return ERROR;
}
else
	return ERROR;
}

//TODO check if it works
int find(char *pattern)
{
if (number_of_elements(words) == 0)
	return ERROR;
node *traverseNode = root;
int elementsFound = 0;
int length = strlen(pattern);
int diff = 0;
int i = 0;
int foundNext = YES;
while ((traverseNode != NULL) && (!is_leaf(traverseNode))
		&& (elementsFound < length) && (foundNext)) {
	i = 0;
	i = get_index(pattern[elementsFound]);
	if (traverseNode->edges[i] != NULL) {
		// the pattern exists, check how much of it
		edge *nextEdge = traverseNode->edges[i];
		diff = elementsFound;
		foundNext = check_coincident(suffix(pattern, elementsFound),
				&elementsFound, nextEdge->label);
		diff = elementsFound - diff;
		if (elementsFound == length) {
			if (diff < strlen(nextEdge->label))
				return YES;
			else {
				fprintf(stderr, "<find> LEAF\n");
				return LEAF;
			}
		}
		traverseNode = nextEdge->child;
	}
	return NO;
}
return NO;
}

//TODO
// możliwe, że trzeba bedzie zmienić ilość gwiazdek
void clear_tree(node **currentNode)
{
int i = 0;
edge **currentEdge = NULL;
for (i = 0; i < INDEXES; i++) {
	(*currentEdge) = (*currentNode)->edges[i];
	if (*currentEdge != NULL) {
		clear_tree(&((*currentEdge)->child));
		free((*currentEdge)->label);
		free(*currentEdge);
		*currentEdge = NULL;
	}
}
free(*currentNode);
*currentNode = NULL;
}

void clear(int *counter, int *nodesNum)
{
clear_tree(&root);
delete_all(words);
free(words);
words = NULL;
nodes = 0;
shift = 0;
*counter = 0;
*nodesNum = nodes;
}
