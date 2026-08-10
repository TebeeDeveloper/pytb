#ifndef VECTOR_INT_H
#define VECTOR_INT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int32_t *ptr;
    size_t size;
    size_t capacity;
} VectorInt_t;

VectorInt_t* vector_int_init(size_t capacity);
VectorInt_t* vector_int_from_array(int32_t *restrict arr, size_t n);
void vector_int_free(VectorInt_t *restrict v);

void vector_int_pushback(VectorInt_t *restrict v, int32_t val);
void vector_int_extend(VectorInt_t *restrict v, int32_t *src, size_t n);
void vector_int_emplaceback(VectorInt_t *restrict v, int32_t val);
void vector_int_emplace(VectorInt_t *restrict v, int32_t *src, size_t n);
void vector_int_copy(VectorInt_t *restrict v0, VectorInt_t *restrict v1);

void vector_int_sort(VectorInt_t *restrict v);
int32_t vector_int_find(VectorInt_t *v, int32_t key);
int32_t vector_int_count(VectorInt_t *v, int32_t key);
VectorInt_t* vector_int_findall(VectorInt_t *v, int32_t key);

void vector_int_iadd(VectorInt_t *restrict v0, VectorInt_t *restrict v1);
VectorInt_t* vector_int_add(VectorInt_t *restrict v0, VectorInt_t *restrict v1);

void vector_int_isub(VectorInt_t *restrict v0, VectorInt_t *restrict v1);
VectorInt_t* vector_int_sub(VectorInt_t *restrict v0, VectorInt_t *restrict v1);

void vector_int_imul(VectorInt_t *restrict v0, VectorInt_t *restrict v1);
VectorInt_t* vector_int_mul(VectorInt_t *restrict v0, VectorInt_t *restrict v1);

int64_t vector_int_dotproduct(VectorInt_t *restrict v0, VectorInt_t *restrict v1);
int64_t vector_int_sum(VectorInt_t *restrict v);
void vector_int_axpy(VectorInt_t *restrict v0, VectorInt_t *restrict v1, int32_t a);
#endif