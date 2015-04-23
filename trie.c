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
} nodeInfo;

vector *words = NULL;
vector *inserts = NULL;
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

// get the word to be inserted by prev
char *concatenate(int number, int start, int end)
{
	char *word = NULL;
	//char *suffix = "\0";
	node *currentNode = NULL;
	int size = end - start + 1;
	currentNode = ((nodeInfo *) (at(words, number)))->node;
	word = malloc(sizeof(char) * (size + 1));
	strncpy(word, currentNode->parentEdge->label->label + start, size);
	word[size] = '\0';
	return word;

}
/*while (currentNode->parentEdge != NULL) {
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
 */

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

int check_coincident(char *pattern, int *elementsFound, char *label, int start,
		int end)
{
	int i = 0;
	int length_p, length_l;
	length_p = strlen(pattern);
	length_l = end - start + 1;
	fprintf(stderr, "<trie check_coincident> pattern: %s label: %s\n", pattern,
			label);
	while ((i < length_p) && (i < length_l) && (pattern[i] == label[i + start])) {
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

char * store_label(char *word)
{
	int length = strlen(word);
	char *label = malloc(sizeof(char) * strlen(word) + 1);
	strcpy(label, word);
	label[length] = '\0';
	push_back(inserts, label);
	return label;
}

labelInfo *create_labelInfo(char *label, int start, int end)
{
	labelInfo *info = malloc(sizeof(labelInfo));
	info->label = label;
	info->start = start;
	info->end = end;
	return info;
}

void create_edge(edge **edge, char *word, int start, int end, node **parentNode)
{
	*edge = malloc(sizeof(struct edge));
	(*edge)->label = create_labelInfo(word, start, end);
	(*edge)->parent = (parentNode == NULL ? NULL : *parentNode);
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

void create_nodeInfo(nodeInfo **info, node *node, int length)
{
	*info = malloc(sizeof(nodeInfo));
	(*info)->node = node;
	(*info)->length = length;
	push_back(words, (void *) *info);
}

void init_dict(node **node, char *word)
{
	char *label = store_label(word);
	int index = get_index(label[0]);
	create_node(node, NO_NUMBER, NULL);
	(*node)->parentEdge = NULL;
	int length = strlen(label);
	create_edge(&((*node)->edges[index]), label, 0, length - 1, node);
	(*node)->children++;
	fprintf(stderr, "<init_dict> label end: %d, start: %d i label: %s\n",
			(*node)->edges[index]->label->end,
			(*node)->edges[index]->label->start,
			(*node)->edges[index]->label->label);
	create_node(&(((*node)->edges[index])->child), 0, &((*node)->edges[index]));
	/* add leaf to leaves vector */
	nodeInfo *nodeIn = NULL;
	create_nodeInfo(&nodeIn, (*node)->edges[index]->child, length);
	/* debug */
	nodeInfo *nodeIn2 = ((nodeInfo *) at(words, 0));
	int l = nodeIn2->length;
	fprintf(stderr, "<init_dict> correct? length: %d\n", l);
}

void print_tree(node *currentNode)
{
	int i = 0;
	edge *currentEdge = NULL;
	fprintf(stderr, "<print_tree> node_number: %d\n", (currentNode)->wordNum);
	fprintf(stderr, "<print_tree> children: %d\n", (currentNode)->children);
	fprintf(stderr, "---------------------------------\n");
	if (currentNode->parentEdge != NULL) {
		fprintf(stderr, "<print_tree> parent label: %s, od: %d do: %d\n",
				(currentNode)->parentEdge->label->label,
				(currentNode)->parentEdge->label->start,
				(currentNode)->parentEdge->label->end);
	}
	else
		fprintf(stderr, "<print_tree> nie mam parenta\n");
	for (i = 0; i < INDEXES; i++) {
		(currentEdge) = (currentNode)->edges[i];
		if (currentEdge != NULL) {
			assert(currentEdge->label != NULL);
			fprintf(stderr, "<print_tree> label: %s\n",
					(currentEdge)->label->label);
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

node *split_edge(edge *old, int index, int nodeNr)
{
	/* higher label with the beginning of the word */
	int oldEnd = old->label->start + index - 1;
	/* lower label with the suffix */
	int newStart = old->label->start + index;
	int newEnd = old->label->end;
	old->label->end = oldEnd;
	node *midNode;
	create_node(&midNode, nodeNr, &old);
	midNode->children++;
	char first = *(old->label->label + newStart);
	create_edge(&(midNode->edges[get_index(first)]), old->label->label,
			newStart, newEnd, &(midNode));
	midNode->edges[get_index(first)]->child = old->child;
	old->child->parentEdge = midNode->edges[get_index(first)];
	old->child = midNode;
	return midNode;
}

void add_node(node *mid, int index, int nodeNr, char *word)
{
	int next = get_index(word[index]);
	create_edge(&(mid->edges[next]), word, index, strlen(word) - 1, &mid);
	create_node(&(mid->edges[next]->child), nodeNr, &(mid->edges[next]));
	mid->children++;
	nodeInfo *info = NULL;
	create_nodeInfo(&info, mid->edges[next]->child, strlen(word));
}

void add_node_prev(node *mid, int start, int end, int nodeNr, char *word)
{
	int next = get_index(word[start]);
	create_edge(&(mid->edges[next]), word, start, end, &mid);
	create_node(&(mid->edges[next]->child), nodeNr, &(mid->edges[next]));
	mid->children++;
	nodeInfo *info = NULL;
	create_nodeInfo(&info, mid->edges[next]->child, end - start + 1);
}

//TODO push_back nodeInfo not node
//returns number of nodes on success, else ERROR
//if start set to NO_NUMBER add new word to vector
int insert(char *word, int start, int end)
{
	fprintf(stderr, "<insert>: word: %s, start: %d, end: %d\n", word, start, end);
	//easy_insert(&root);
	if (root == NULL) {
		words = malloc(sizeof(vector));
		init(words);
		inserts = malloc(sizeof(vector));
		init(inserts);
		init_dict(&root, word);
		print_tree(root);
		return nodes;
	}
	node *crossNode = root;
	int elementsFound = 0;
	int index;
	int wordLength;
	if (start != -1) {
		wordLength = end - start + 1;
		index = get_index(word[start]);
	}
	else {
		wordLength = strlen(word);
		index = get_index(word[elementsFound]);
	}
	// dopóki istnieje ścieżka zadana tymi literami
	while (crossNode->edges[index] != NULL) { //wyrzuciłam crossNode != NULL
		// nie jest liściem, ide po label, sprawdzam czy sie zawiera, różni itp
		edge **edge = &(crossNode->edges[index]);
		int diff = elementsFound;
		char *suff;
		if (start != -1)
			suff = substring(word, start, end);
		else
			suff = suffix(word, elementsFound);
		int matchFound = check_coincident(suff, &elementsFound,
				(*edge)->label->label, (*edge)->label->start,
				(*edge)->label->end);
		free(suff);
		diff = elementsFound - diff;
		int labelLength = (*edge)->label->end - (*edge)->label->start + 1;
		if (matchFound && (diff < labelLength)) {
			// skonczyl sie pattern, czyli zawiera się -> nowy mid wezel

			node *mid = split_edge(*edge, diff, shift + size(words));
			nodeInfo *info;
			create_nodeInfo(&info, mid, wordLength);
			print_tree(root);
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
		else if (matchFound && (diff == labelLength)
				&& (elementsFound == wordLength)) {
			// takie słowo istnieje
			return ERROR;
		}
		else if (!matchFound) {
			// rózni się -> nowy mid węzeł
			fprintf(stderr, "match not found\n");
			//store_label(word);
			node *mid = split_edge(*edge, diff, NO_NUMBER);
			add_node(mid, elementsFound, shift + size(words),
					store_label(word));
			print_tree(root);
			//		fprintf(stderr, "<insert>2 vector<0> parent children: %d\n", ((nodeInfo *)at(words, 0))->node->parentEdge->parent->children);
			//push_back(words, (*edge)->child->edges[nextIndexWord]->child);
			return nodes;
		}
	}
	if ((elementsFound < wordLength) && (start == NO_NUMBER)) {
		/* normal insert, edge with new first symbol, add to inserts */
		// z buta wstawiam
		fprintf(stderr,
				"<insert> z buta całość dodaje, jestem w wezle nr: %d\n",
				crossNode->wordNum);

		add_node(crossNode, elementsFound, shift + size(words),
				store_label(word));
		print_tree(root);
	}
	else if ((elementsFound < wordLength) && (start != NO_NUMBER)) {
		/* adding word from prev command, does not store label */
		add_node_prev(crossNode, start + elementsFound, end, shift + size(words), word);
		print_tree(root);
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
		nodeInfo *info = (nodeInfo *) at(words, number);
		if (info != NULL) {
			length = info->length;
			fprintf(stderr, "<prev> at(words, number) != NULL i length: %d\n",
					length);
			if ((start <= end) && (start < length) && (end < length)) {
				// go up and get the string
				pattern = concatenate(number, start, end);
				labelInfo *label = info->node->parentEdge->label;
				// go down the tree and see if such a pattern exists
				int result = find(pattern);
				if (result != LEAF) {
					fprintf(stderr, "nie ma takiego slowa: %s\n", pattern);
					free(pattern);
					return insert(label->label, start, end);
				}
				else {
					free(pattern);
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

int find_next_child(node *node, int first)
{
	int i;
	for (i = 0; i < INDEXES; i++)
		if ((i != first) && (node->edges[i] != NULL))
			return i;
	return ERROR;
}

void merge_edges(edge *upper, edge *lower, int nodeNr)
{
	node *toDelete = upper->child;
	int number = toDelete->wordNum;
	upper->child = lower->child;
	upper->label->label = lower->label->label;
	upper->label->end = lower->label->end;
	lower->child->parentEdge = upper;
	free(lower->label);
	free(lower);
	free(toDelete);
	nodes--;
	if ((nodeNr != number) && (number != NO_NUMBER))
		delete_at(words, number - shift);
}

void free_node_edge(node **node)
{
	free((*node)->parentEdge->label);
	free((*node)->parentEdge);
	free(*node);
	nodes--;
}

//TODO roota tez chcemy usunac jezeli bedzie to ostatnie slowo usuwane,
//wtedy tez nulluje wektor, ale shift zostaje
int delete(int number)
{
	if (words != NULL) {
		int vecNumber = number - shift;
		int nextChild = 0;
		node **deleteNode = &((nodeInfo *) at(words, vecNumber))->node;
		if (deleteNode != NULL) {
			//if (shift != ERROR) {
			int parentChildren = (*deleteNode)->parentEdge->parent->children;
			node **parentNode = &(*deleteNode)->parentEdge->parent;
			labelInfo *parentLabel = (*deleteNode)->parentEdge->label;
			if (((*deleteNode)->parentEdge == NULL) && (parentChildren == 1)) {
				//jest tylko ostatnie słowo, trzeba usunąć też roota
				fprintf(stderr, "<delete> usuwam wszystko i roota\n");
				free_node_edge(deleteNode);
				free(*parentNode);
				*parentNode = NULL;
				nodes--;
			}
			//dopóki moim ojcem nie jest root
			else if ((is_leaf(*deleteNode)) && (parentChildren == 2)
					&& ((*parentNode)->wordNum == -1)) {
				//łączę parentNode->parentEdge z drugim dzieckiem parentNode
				fprintf(stderr, "<delete>: przypadek 1\n");
				nextChild = find_next_child((*parentNode),
						get_index(parentLabel->label[parentLabel->start]));
				merge_edges((*parentNode)->parentEdge,
						(*parentNode)->edges[nextChild], number);
				free_node_edge(deleteNode);
				print_tree(root);
			}
			else if (is_leaf(*deleteNode)) {
				//tylko usuwamy deleteNode->parentEdge i deleteNode
				fprintf(stderr, "<delete>: przypadek 2\n");
				(*parentNode)->children--;
				(*parentNode)->edges[get_index(
						parentLabel->label[parentLabel->start])] = NULL;
				free_node_edge(deleteNode);
				print_tree(root);
			}
			else if (!is_leaf(*deleteNode) && ((*deleteNode)->children >= 2)) {
				// easy, just change deleteNode->wordNum to -1
				fprintf(stderr, "<delete>: przypadek 3\n");
				(*deleteNode)->wordNum = NO_NUMBER;
			}
			else if (!is_leaf(*deleteNode) && ((*deleteNode)->children == 1)) {
				//łącze deleteNode->parentEdge z labelem dziecka
				fprintf(stderr, "<delete>: przypadek 4\n");
				nextChild = find_next_child(*deleteNode, NO_NUMBER);
				fprintf(stderr, "<delete>: index edge: %c\n", nextChild + 97);
				merge_edges((*deleteNode)->parentEdge,
						(*deleteNode)->edges[nextChild], number);
				print_tree(root);
			}
			shift = delete_at(words, vecNumber);
			return (shift == ERROR ? shift : nodes);
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

	/*
	 * <parse get_word> aa i size: 2
	 <find> patter to find: aa
	 <trie check_coincident> pattern: aa label: aaaa
	 <trie check_coincident> YES
	 aaNO
	 *
	 */
	if ((words == NULL) || (number_of_elements(words) == 0))
		return ERROR;
	node *traverseNode = root;
	int elementsFound = 0;
	fprintf(stderr, "<find> patter to find: %s\n", pattern);
	int length = strlen(pattern);
	int diff = 0;
	int i = 0;
	int foundNext = YES;
	while ((traverseNode != NULL) && (!is_leaf(traverseNode))
			&& (elementsFound < length) && (foundNext)) {
		i = get_index(pattern[elementsFound]);
		fprintf(stderr, "<find>: elementsFound: %d and node nr %d\n",
				elementsFound, traverseNode->wordNum);
		if (traverseNode->edges[i] != NULL) {
			// the pattern exists, check how much of it
			edge *nextEdge = traverseNode->edges[i];
			diff = elementsFound;
			//TODO czy to tak moze byc
			char *suff = suffix(pattern, elementsFound);
			foundNext = check_coincident(suff, &elementsFound,
					nextEdge->label->label, nextEdge->label->start,
					nextEdge->label->end);
			free(suff);
			diff = elementsFound - diff;
			if (elementsFound == length) {
				if (diff < strlen(nextEdge->label->label))
					return YES;
				else {
					fprintf(stderr, "<find> LEAF\n");
					return LEAF;
				}
			}
			traverseNode = nextEdge->child;
		}
		else
			return NO;
	}
	return NO;
}

//TODO
// możliwe, że trzeba bedzie zmienić ilość gwiazdek
void clear_tree(node **currentNode)
{
	if (currentNode != NULL) {
		int i = 0;
		edge *currentEdge = NULL;
		for (i = 0; i < INDEXES; i++) {
			currentEdge = (*currentNode)->edges[i];
			if (currentEdge != NULL) {
				clear_tree(&((currentEdge)->child));
				//free((currentEdge)->label->label);
				free((currentEdge)->label);
				free(currentEdge);
				currentEdge = NULL;
			}
		}
		free(*currentNode);
		*currentNode = NULL;
	}
}

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
