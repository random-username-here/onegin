#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "istd/_impl/arg.h"
#include "istd/arg.h"

#define ESC_TITLE "\x1b[1;4m" // for titles like `Usage`: bold, underlined
#define ESC_ITEM  "\x1b[96m"   // for items like `subcommand`: bold
#define ESC_ARG   "\x1b[92m"   // for positional arguments
#define ESC_RESET "\x1b[0m"

static void generate_usage_help(const iarg_cmd** tree, size_t tree_size) {
  
  assert(tree);
  const iarg_cmd* module = tree[tree_size-1];
  assert(module);

  fprintf(stderr, ESC_TITLE "\nUsage:" ESC_RESET " " ESC_ITEM);

  for (size_t i = 0; i < tree_size; ++i)
    fprintf(stderr, "%s ", tree[i]->name);

  fprintf(stderr, ESC_RESET ESC_ARG);

  if (module->num_kwargs)
    fprintf(stderr, "[OPTIONS] ");
  
  if (module->num_subcommands) {
    fprintf(stderr, "COMMAND ");

  } else if (module->max_num_args) {
    assert(module->min_num_args <= module->max_num_args);
    assert(module->arg_names);

    const char* name = module->arg_names;
    size_t arg_idx = 0;
    
    while (*name) {
      // Skip whitespaces
      while (*name && isspace(*name))
        ++name;
      if (!*name)
        break;

      // Print name
      fputc(arg_idx < module->min_num_args ? '<' : '[', stderr);
      while (*name && *name != ',')
        fputc(*(name++), stderr);
      fputc(arg_idx < module->min_num_args ? '>' : ']', stderr);
      fputc(' ', stderr);

      // Skip comma, if there is one
      if (*name == ',')
        ++name;
      ++arg_idx;
    }
  }
  fprintf(stderr, ESC_RESET "\n");
}


static void generate_subcommand_help(const iarg_cmd** tree, size_t tree_size) {

  assert(tree);
  const iarg_cmd* module = tree[tree_size-1];
  assert(module);

  if (module->num_subcommands) {
    fprintf(stderr, ESC_TITLE "\nCommands:" ESC_RESET "\n");
    fprintf(stderr, "\n");

    size_t max_cmd_len = 0;

    for (size_t i = 0; i < module->num_subcommands; ++i) {
      size_t len = strlen(module->subcommands[i].name);
      if (len > max_cmd_len)
        max_cmd_len = len;
    }

    for (size_t i = 0; i < module->num_subcommands; ++i) {
      fprintf(stderr, ESC_ITEM "  %-*s  " ESC_RESET "%s\n",
              (int) max_cmd_len, module->subcommands[i].name, module->subcommands[i].brief);
    }
  }
}

static void generate_options_help(const iarg_cmd** tree, size_t tree_size) {
  
  assert(tree);
  const iarg_cmd* module = tree[tree_size-1];
  assert(module);

  if (module->num_kwargs) {
    fprintf(stderr, ESC_TITLE "\nOptions:" ESC_RESET "\n");
    fprintf(stderr, "\n");

    size_t max_arg_len = sizeof(_IARG_HELP_FLAG);

    for (size_t i = 0; i < module->num_kwargs; ++i) {
      size_t len = strlen(module->kwargs[i].arg_names);
      if (len > max_arg_len)
        max_arg_len = len;
    }

    for (size_t i = 0; i < module->num_kwargs; ++i) {
      fprintf(stderr, ESC_ITEM "  %-*s  " ESC_RESET "%s\n",
              (int) max_arg_len, module->kwargs[i].arg_names, module->kwargs[i].brief);
    }
    fprintf(stderr, ESC_ITEM "  %-*s  " ESC_RESET "%s\n",
            (int) max_arg_len, _IARG_HELP_FLAG, "Print this message");

  }
}

/// Print `--help` of given module
void _iarg_generate_help (const iarg_cmd** tree,
                           size_t tree_size) {

  assert(tree);
  assert(tree_size > 0);

  const iarg_cmd* module = tree[tree_size-1];
  assert(module);

  if (module->brief)
    fprintf(stderr, "\n%s\n", module->brief);

  if (module->long_info)
    fprintf(stderr, "\n%s\n", module->long_info);

  generate_usage_help(tree, tree_size);
  generate_subcommand_help(tree, tree_size);
  generate_options_help(tree, tree_size);

  if (module->footer)
    fprintf(stderr, "\n%s\n", module->footer);
}


