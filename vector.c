/*
 * vector.c
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vector.h"

#define ERROR	-1
#define BASE_SIZE	32

void resize(vector *vec, int newLimit)
{
	vec->limit *= 2;
	void **tab = realloc(vec->tab, sizeof(void *) * newLimit);
	if (tab) {
		vec->tab = tab;
		vec->limit = newLimit;
	}
}

void push_back(vector *vec, void *node)
{
	if (vec->size == vec->limit)
		resize(vec, vec->limit * 2);
	vec->tab[vec->size] = node;
	vec->size++;
}

void *at(vector *vec, int pos)
{
	pos = pos - vec->shift;
	if ((pos >= 0) && (pos < vec->size)) {
		return vec->tab[pos];
	}
	return NULL;
}

void set(vector *vec, int pos, void *node)
{
	if ((pos >= 0) && (pos < vec->size))
		vec->tab[pos] = node;
}

int delete_at(vector *vec, int pos)
{
	int vecPos = pos - vec->shift;
	if ((vecPos >= 0) && (vecPos < vec->size)) {
		free(vec->tab[vecPos]);
		vec->tab[vecPos] = NULL;
		vec->deleted++;
	}
	else {
		return ERROR;
	}
	if (vec->deleted == vec->size) {
		int shift = vec->shift + vec->deleted;
		delete_all(vec);
		init(vec);
		vec->shift = shift;
	}
	return 0;
}

void delete_all(vector *vec)
{
	int i = 0;
	for (i = 0; i < vec->size; i++)
		if (vec->tab[i] != NULL) {
			free(vec->tab[i]);
		}
	if (vec->tab != NULL)
		free(vec->tab);
}

int size(vector *vec)
{
	return (vec->size + vec->shift);
}

int number_of_elements(vector *vec)
{
	return (vec->size - vec->deleted);
}

void init(vector *vec)
{
	(vec)->tab = malloc(sizeof(void *) * BASE_SIZE);
	memset(vec->tab, 0, BASE_SIZE);
	(vec)->size = 0;
	(vec)->limit = BASE_SIZE;
	(vec)->deleted = 0;
	(vec)->shift = 0;
}
