#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include "istd/arg.h"
#include "istd/util/err.h"
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
/*
typedef struct {

  const char* filename;

  bool fancy;

} program_cfg;

void init_cfg(void** cfg) {
  assert(cfg);

  *cfg = calloc(1, sizeof(program_cfg));
  if (!*cfg)
    panic("Failed to allocate memory for command-line options");
}*/

int main (int argc, const char** argv) {

  /*iarg_cmd sorter = {

    .name = "sort",
    .brief = "Sort lines in alphabetical order",
    .arg_names = "FILE, foo, bar, baz",
    .min_num_args = 2,
    .max_num_args = 4
  };

  iarg_cmd poem_sorter = {

    .name = "poem-sort",
    .brief = "Sort lines, comparing them backwards",
    .long_info = "This way those lines are ordered in rythmical order,\n"
                 "so long suffixes of two ajacent lines are same.",

    .arg_names = "FILE",
    .min_num_args = 1,
    .max_num_args = 1
  };

  iarg_cmd root_cmds[] = { sorter, poem_sorter };

  iarg_spec root_args[] = {
    
    {
      .arg_names = "-f, --fancy",
      .brief = "Enable fancy colored output, default in terminals",
      .type = IARG_FLAG,
      .user_param = (void*) offsetof(program_cfg, fancy),
      .callback = iarg_handler_set_flag
    },

    {
      .arg_names = "-p, --plain",
      .brief = "Disable all fancyness of output, use only plain text",
      .type = IARG_FLAG,
      .user_param = (void*) offsetof(program_cfg, fancy),
      .callback = iarg_handler_reset_flag
    }
    
  };

  iarg_cmd root_mod = {

    .name      = "onegin",
    .brief     = "A program to do some stuff with poems",

    .long_info = "This program is designed to make weird stuff out of given poem.\n"
                 "For now this means sorting its lines in diffrent ways.\n"
                 "Later I will add random dummy poem generator, which uses lines from given poem\n"
                 "\n"
                 "I am not responsible for any damage done to you/your property by\n"
                 "angry fans of classical literature. Be warned.",

    .footer    = "(c) Ivan Didyk, 2024",

    .subcommands = root_cmds,
    .num_subcommands = sizeof(root_cmds) / sizeof(iarg_cmd),

    .kwargs = root_args,
    .num_kwargs = sizeof(root_args) / sizeof(iarg_spec),

    .on_module_begin = init_cfg
    
  };

  iarg_parse(argc, argv, &root_mod);

  return 0;*/

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
  if (!f)
    panic$("Failed to open given file\n"
          "  errno says   : %s\n"
          "  file name is : `%s`\n",
          strerror(errno), filename);

  struct stat info;
  fstat(fileno(f), &info);
  size_t len = info.st_size;

  char* buf = calloc(len+1, sizeof(char));
  if (!buf)
    panic$("Failed to allocate buffer of length %zu for poem text\n", len+1);

  len = fread(buf, 1, len, f);
  buf[len] = 0;

  if (fclose(f))
    panic$("Failed to close input file for some reason");

  sort_mode_t sort_mode = !strcmp("sort", mode) ? SORT_FORWARDS : SORT_BACKWARDS;

  const char** lines = NULL;
  size_t num_lines = parse_poem(buf, &lines);
  sort_lines(lines, num_lines, sort_mode);

  if (!strcmp("combine", mode)) {
    printf("TODO\n"); 
  } else {
    for (size_t i = 0; i < num_lines; ++i) {
      print_line(lines[i]);
      printf("\n");
    }
  }

  free(buf);
  free(lines);

  return 0;
}
