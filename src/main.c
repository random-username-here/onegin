#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "onegin.h"
#include <sys/stat.h>

#define ESC_MODE "\x1b[94m"
#define ESC_TITLE "\x1b[4m"
#define ESC_ARG "\x1b[3m"
#define ESC_RESET "\x1b[0m"

void print_help () {
  printf("onegin - A program to do stuff with poems\n");
  printf("\n");
  printf(ESC_TITLE "Usage" ESC_RESET "\n");
  printf("\n");
  printf(ESC_MODE " sort      " ESC_RESET ESC_ARG " FILE     " ESC_RESET " - Sort given poem file in normal order\n");
  printf(ESC_MODE " poem-sort " ESC_RESET ESC_ARG " FILE     " ESC_RESET " - Sort given poem file, comparing lines backwards\n");
  printf(ESC_MODE " combine   " ESC_RESET ESC_ARG " FILE NUM " ESC_RESET " - Make some amalgamation of NUM lines of that file, which looks like a poem\n");
  printf("\n");
}

int main (int argc, const char** argv) {

  if (argc == 2 && !strcmp(argv[1], "--help")) {
    print_help();
    return 0;
  }

  if (argc < 3) {
    fprintf(stderr, "This program expects at least two arguments - mode and file to read. Go read --help\n");
    return -1;
  }

  const char* mode     = argv[1];
  const char* filename = argv[2];
  
  if (strcmp("sort", mode) && strcmp("poem-sort", mode) && strcmp("combine", mode)) {
    fprintf(stderr, "Supported modes are `sort`, `poem-sort` and `combine`\n");
    return -1; 
  }

  FILE* f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "Failed to open poem file : %s", strerror(errno));
    return 0;
  }

  struct stat info;
  fstat(fileno(f), &info);
  size_t len = info.st_size;

  char* buf = calloc(len+1, sizeof(char));
  fseek(f, 0, SEEK_SET);
  len = fread(buf, 1, len, f);
  buf[len] = 0;
  fclose(f);

  sort_mode_t sort_mode = !strcmp("sort", mode) ? SORT_FORWARDS : SORT_BACKWARDS;

  const char** lines = NULL;
  size_t num_lines = parse_poem(buf, &lines);
  sort_lines(lines, num_lines, sort_mode);

  if (!strcmp("combine", mode)) {
    printf("TODO\n"); 
  } else {
    for (size_t i = 0; i < num_lines; ++i) {
      printf("\x1b[90m %3zu | \x1b[0m", i+1);
      print_line(lines[i]);
      printf("\n");
    }
  }

  free(buf);
  free(lines);

  return 0;
}
