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
	printf("<vector push_back>: adding node nr: %d\n", vec->size - 1);
//	return (vec->size - 1);
}

void *at(vector *vec, int pos)
{
	if ((pos >= 0) && (pos < vec->limit))
		return vec->tab[pos];
	return (void *) ERROR;
}

void set(vector *vec, int pos, void *node)
{
	if ((pos >= 0) && (pos < vec->size))
		vec->tab[pos] = node;
}

int delete_at(vector *vec, int pos)
{
	int shift = 0;
	if ((pos >= 0) && (pos < vec->limit)) {
		vec->tab[pos] = NULL;
		vec->deleted++;
	}
	else
		shift = ERROR;
	if (vec->deleted == vec->size) {
		shift = vec->deleted;
		delete_all(vec);
		init(vec);
	}
	return shift;
}

void delete_all(vector *vec)
{
	free(vec->tab);
}



int size(vector *vec)
{
	return vec->size;
}

int number_of_elements(vector *vec)
{
	return (vec->size - vec->deleted);
}

void init(vector *vec)
{
	(vec)->tab = malloc(sizeof(void *) * BASE_SIZE);
	(vec)->size = 0;
	(vec)->limit = BASE_SIZE;
	(vec)->deleted = 0;
}
