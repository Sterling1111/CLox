#include <stdio.h>

#include "memory.h"
#include "value.h"

/**
 * Initializes the double pointer array to NULL and sets capacity and count of valueArray to 0.
 * @param array the value array* to be initialized
 */
void initValueArray(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

/**
 * Adds a double to the end of a value array.
 * @param array the array to append to value to
 * @param value the double added to the end of the array.
 */
void writeValueArray(ValueArray* array, Value value) {
    if(array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

/**
 * Frees the memory held by the ValueArray*
 * @param array pointer to the ValueArray who's memory is to be freed
 */
void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

/**
 * Prints out a value with precision corresponding to the precision of the value
 * @param value the double to be printed
 */
void printValue(Value value) {
    printf("%g", value);
}