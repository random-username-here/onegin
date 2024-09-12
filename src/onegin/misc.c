#include "onegin.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define ESC_PANIC_LABEL   "\x1b[91;1m" // bold red
#define ESC_RESET "\x1b[0m"

__attribute__ ((format (printf, 1, 2)))
__attribute__((noreturn))
void panic(const char* fmt, ...) {
  
  assert(fmt);

  va_list args;
  va_start(args, fmt);


  fprintf(stderr, ESC_PANIC_LABEL "\nProgram panicked: " ESC_RESET);
  vfprintf(stderr, fmt, args);

  exit(EXIT_FAILURE);
}
