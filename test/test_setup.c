/**
 * \file
 * \brief Setup the test system. This file contanis `main()` defenition.
 */
#include <stdio.h>
#include <stdlib.h>
#include "onegin.h"

int cmp(const void* a, const void* b, void* v) {
  return *(int*)b - *(int*)a;
}

int main (void) {

  size_t len = 30;

  int *array = calloc(len, sizeof(int));

  for (size_t i = 0; i < len; ++i)
    array[i] = (len - i) % 10;

  my_qsort(array, len, sizeof(int), cmp, NULL);

  for (size_t i = 0; i < len; ++i) {
    printf("%d ", array[i]);
  }
  printf("\n");

  return 0;
}
