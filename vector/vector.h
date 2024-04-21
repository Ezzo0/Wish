#ifndef VECTOR_H
#define VECTOR_H

/********************************INCLUDES**************************************/
#include <stdio.h>
#include <stdlib.h>


/*****************************Macro Declarations*******************************/
#define VECTOR_INIT_CAPACITY 4


/***************************Data Type Declarations*****************************/
typedef struct
{
    int size;
    int capacity;
    void **items;
}vector;


/***************************Function Declarations***************************/
void vector_init(vector *);

int vector_size(vector *);

void vector_add(vector *, void *);

void vector_set(vector *, int, void *);

void *vector_get(vector *, int);

void vector_delete(vector *, int);

void vector_delete_all(vector *);

void vector_free(vector *);

#endif