#include "chunk.h"

/**
 * Initializes the count and capacity of given chunk to 0 and initializes the code and lines
 * of a given chunk to NULL. Also initializes the chunks value array(constants).
 * @param chunk the chunk to initialize
 */
void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

/**
 * Adds a byte to the end of a given chunk.
 * @param chunk pointer to chunk to append byte to
 * @param byte the byte to add to the end of the chunk
 * @param line the line of code that the given bytecode corresponds to
 */
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if(chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

/**
 * Frees all memory held by the chunk*.
 * @param chunk the chunk pointer who's memory is to be freed
 */
void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

/**
 * Adds a value to the end of the chunk's value array.
 * @param chunk the chunk which owns the value array
 * @param value the value to append to the end of the chunks value array
 * @return the index in the value array where the value was added
 */
int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}