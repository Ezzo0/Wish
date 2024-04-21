#include "vector.h"


/***************************Function Definitions***************************/

/**
 * @Description This function return the minimum number. 
 * @Preconditions None
 * @param num1 - An integer number.
 * @param num2 - An integer number.
 * @return the minimum number of the inputs
 */
static int min(int num1, int num2) 
{
    return (num1 > num2 ) ? num2 : num1;
}

/**
 * @Summary Initialize a vector
 * @Description This function dynamically allocate space in memory for a vector.
 * @Preconditions None
 * @param v - A vector variable.
 * @return void
 */

void vector_init(vector *v) {
    v->capacity = VECTOR_INIT_CAPACITY;
    v->size = 0;
    v->items = (void**) malloc(sizeof(void *) * v->capacity);
}

/**
 * @Description This function give back the size of the vector. 
 * @Preconditions None
 * @param v - A vector variable.
 * @return Number of elements in the vector
 */
 
int vector_size(vector *v) {
    return v->size;
}

/**
 * @Summary Resizing the vector
 * @Description This function dynamically allocate new space in memory for a vector and remove the old one. 
 * @Preconditions None
 * @param v - A vector variable.
 * @param capacity - New desired capacity.
 * @return void.
 */
 
static void vector_resize(vector *v, int capacity) { 
    void **items = (void**)malloc(sizeof(void *) * capacity);
    int n = min(v->size, capacity);
    
    if (items) {
        for (int i = 0; i < n; i++)
            items[i] = v->items[i];
        free(v->items);
        v->items = items;
        v->capacity = capacity;
    }
}

/**
 * @Summary Add item to the back of the vector
 * @Description This function checks if there is a space in the vector for the new or not. If there is a space, it add the new item. 
 *              If there isn't space, it allocate new space and add the new item.
 * @Preconditions None
 * @param v - A vector variable.
 * @param item      - A pointer to the new item to be added.
 * @return void
 */
 
void vector_add(vector *v, void *item) {
    if (v->capacity == v->size)
        vector_resize(v, v->capacity * 2);
    v->items[v->size++] = item;
}

/**
 * @Summary Set the new value of an element in a vector.
 * @Description This function checks if the desired index is in the vector boundaries or not. If index is in the boundaries, it change the value of the element.
 * @Preconditions None
 * @param v         - A vector variable.
 * @param index     - Desired index.
 * @param item      - A pointer to the new value.
 * @return void
 */
 
void vector_set(vector *v, int index, void *item) {
    if (index >= 0 && index < v->size)
        v->items[index] = item;
}

/**
 * @Summary Give back an element in a vector. 
 * @Description This function checks if the desired index is in the vector boundaries or not. If index is in the boundaries, it get the value of the element. 
 * @Preconditions None
 * @param v     - A vector variable.
 * @param index - Desired index.
 * @return Number of commands in the line
 */
 
void *vector_get(vector *v, int index) {
    if (index >= 0 && index < v->size)
        return v->items[index];
    return NULL;
}

/**
 * @Summary Delete an element in a vector.
 * @Description This function checks if the desired index is in the vector boundaries or not. 
 *              If index is in the boundaries, it delete the element and resize the vector if the number of elements is smaller than or equal to 1/4 the capacity.
 * @Preconditions None
 * @param v     - A vector variable.
 * @param index - Desired index.
 * @return Number of commands in the line
 */
 
void vector_delete(vector *v, int index) {
    if (index < 0 || index >= v->size)
        return;
 
    v->items[index] = NULL;
    int i;
    for (i = 0; i < v->size - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }
 
    v->size--;
 
    if (v->size > 0 && v->size == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}

/**
 * @Summary Empty a vector.
 * @Description This function delete all elements of a vector and resize its space to a smaller one. 
 * @Preconditions None
 * @param v - A vector variable.
 * @return void
 */

void vector_delete_all(vector *v){
    v->size = 0;
    vector_resize(v, VECTOR_INIT_CAPACITY);
}

/**
 * @Description This function deallocate space of a vector. 
 * @Preconditions None
 * @param v - A vector variable.
 * @return void
 */
 
void vector_free(vector *v) {
    free(v->items);
}
 
// int main(void) {
//     int i;
 
//     vector v;
//     vector_init(&v);
 
//     vector_add(&v, "Bonjour");
//     vector_add(&v, "tout");
//     vector_add(&v, "le");
//     vector_add(&v, "monde");
//     vector_add(&v, "Bonjour");
//     vector_add(&v, "tout");
//     vector_add(&v, "le");
//     vector_add(&v, "monde");
//     vector_add(&v, "Bonjour");
//     vector_add(&v, "tout");
//     vector_add(&v, "le");
//     vector_add(&v, "monde");
 
//     for (i = 0; i < vector_size(&v); i++)
//         printf("%s\n", (char *) vector_get(&v, i));
 
//     vector_free(&v);
// }