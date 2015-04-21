/*
 * vector.h
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#define ERROR	-1
#define BASE_SIZE	32

typedef struct {
	void **tab;
	int size;
	int limit;
	int deleted;
} vector;

void push_back(vector *, void *);
void *at(vector *, int);
void set(vector *, int, void *);
int delete_at(vector *, int);
void delete_all(vector *);
int size(vector *);
int number_of_elements(vector *);
void init(vector *);

#endif /* VECTOR_H_ */
