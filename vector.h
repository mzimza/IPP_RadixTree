/*
 * vector.h
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#ifndef VECTOR_H_
#define VECTOR_H_

typedef struct {
	void **tab;
	int size;
	int limit;
	int deleted;
	int shift;
} vector;

/* shift stores the difference in numeration of indexes and elements at each index.
 * e.g. if 64 elements are added and then deleted, for memory optimization
 * vector resizes to smaller one and the next push_back adds the element at 0, not
 * 64
 */

void push_back(vector *, void *);
void *at(vector *, int);
void set(vector *, int, void *);
int delete_at(vector *, int);
void delete_all(vector *);
int size(vector *);
int number_of_elements(vector *);
void init(vector *);

#endif /* VECTOR_H_ */
