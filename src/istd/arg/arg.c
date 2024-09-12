#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "istd/arg.h"
#include "istd/_impl/arg.h"
#include "istd/except.h"

/// Check if character is end of argument in `arg_names`.
static bool is_arg_end(char c) {
  return c == '\0' || c == ',';
}


/// Check if argument from `arg_names` matches given `argv` value
/// \param spec - Argument as defined in `arg_names`.
/// \param arg  - Value from `argv`
static const char* match(const char* spec, const char* arg) {
  while (!is_arg_end(*spec) && *spec == *arg)
    ++spec, ++arg;
  if(is_arg_end(*spec) && (*arg == '=' || *arg == '\0'))
    return arg;
  else 
    return NULL;
}


static const char* mode_to_str(iarg_type type) {
  switch (type) {
    case IARG_FLAG: return "flag (like --help)";
    case IARG_VALUE: return "value (like --output=foobar)";
    case IARG_BOTH: return "flag or value";
    default: assert(0);
  }
}

static void print_until(const char* string, char ch) {
  while (*string && *string != ch)
    fputc(*(string++), stderr);
}


#define IARG_MAX_SUBCMD_DEPTH 128


static const char* match_flag(const char* arg, const char* name) {
  while (*name) {
    // Skip whitespaces
    while(*name && isspace(*name))
      ++name;
    if (!*name)
      return NULL;

    // Test argument
    const char* val_begin = match(name, arg); 
    if (val_begin)
      return val_begin;

    // Skip to comma
    name = strchrnul(name, ',');
    if (*name == ',') ++name;
  }
  return NULL;
}


static iarg_spec* find_flag(const iarg_cmd* module, const char* arg, const char** val_begin_out) {

  assert(module);
  assert(arg);

  for (size_t i = 0; i < module->num_kwargs; ++i) {
    const char* val_begin = match_flag(arg, module->kwargs[i].arg_names);
    if (val_begin) {
      *val_begin_out = val_begin;
      return &module->kwargs[i];
    }
  }

  return NULL;
}


void* iarg_parse(int argc, const char** argv,
               const iarg_cmd* root_module) {

  assert(argv);
  assert(argc > 0);
  assert(root_module);


  const iarg_cmd* tree[IARG_MAX_SUBCMD_DEPTH] = { root_module };
  size_t depth = 1;

  for (size_t i = 1; i < (size_t) argc; ++i) {

    const char* arg = argv[i];
    const iarg_cmd* cur_mod = tree[depth-1];

    if (arg[0] == '-') // that's a flag, ignore it for now
      continue;

    for (size_t i = 0; i < tree[depth-1]->num_subcommands; ++i) {
      if (!strcmp(cur_mod->name, arg)) {
        // We went into that module
        if (depth == IARG_MAX_SUBCMD_DEPTH)
          panic("Too many subcommands : reached %d levels of them", IARG_MAX_SUBCMD_DEPTH);
      }
    }


    // Check if it is -h / --help
    const char* help_val;
    if ((help_val = match_flag(argv[i], _IARG_HELP_FLAG))) {
      // --help was called   
    }

    if (!strcmp("-h", argv[i]) || !strcmp("--help")
    // Check if it is a flag
    const char* arg_val;
    iarg_spec* flag = find_flag(tree[depth-1], argv[i], &arg_val);
    if (flag) {
      
    }
  }

  while (tree[depth-1]->num_subcommands) {
    
  }

  _iarg_generate_help(tree, 1);
  return NULL;
}

void iarg_handler_set_flag(void* state, void* user_param, const char* value) {

}

void iarg_handler_reset_flag(void* state, void* user_param, const char* value) {
  
}



/*
int iarg_parse(int argc, char** argv,
                iarg_spec* spec, size_t num_args,
                void* state) {

  assert(argc > 0);
  assert(argv);

  for (int i = 1; i < argc; ++i) {
    assert(argv[i]);

    bool matched = false;
    for (size_t arg = 0; !matched && arg < num_args; ++arg) {
    assert(spec[arg].callback);
      assert(spec[arg].arg_names);

      bool prev_space = true;

      for (const char* c = spec[arg].arg_names; *c != '\0'; ++c) {
        if (prev_space && *c != ' ') {
          // We are at beginning of a flag defenition
          char* value_begin = match(c, argv[i]);
          
          if (value_begin) {
            // It matched
            iarg_type type = *value_begin == '=' ? IARG_VALUE : IARG_FLAG;
            
            if (! (spec[arg].type & type)) {
              // But user used it wrong...
              *value_begin = '\0';
              fprintf(stderr, "Wrong usage of argument `%s`\n", value_begin);
              fprintf(stderr, "It must be used as %s, but you used it as %s\n",
                      mode_to_str(spec[arg].type), mode_to_str(type));
              return IARG_WRONG_ARG;
            }

            // Call the callback
            spec[arg].callback(state, spec[arg].user_param, *value_begin == '=' ? value_begin+1 : NULL);
            matched = true;
            break;
          }
        }
        prev_space = *c == ' ';
      }
    }

    if (!matched) {
      fprintf(stderr, "Unknown argument `%s`\n", argv[i]);
      return IARG_WRONG_ARG;
    }
  }

  return 0;
}*/
