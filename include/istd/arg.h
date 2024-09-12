/**
 * \file
 * \brief A little more advanced command line argument parser
 */

#ifndef ISTD_ARG
#define ISTD_ARG
#include <stddef.h>

/// Type of the argument,
/// used when defining list of args
///
/// You may OR them to combine.
typedef enum {
  
  /// This argument must be a flag (like `--help`)
  IARG_FLAG = 1,

  /// This argument must be a value (like `--output=foo.bar`) 
  IARG_VALUE = 2,

  /// This argument can be both
  IARG_BOTH = 3,

} iarg_type;


/// Description of one argument
typedef struct {

  /// Names of this argument, as space separated string.
  /// For example `-f --format --fmt`
  const char* arg_names;

  /// Brief description of the parameter
  const char* brief;

  /// Name of the value (so in help it will be
  /// shown like `--foo <YOUR PLACEHOLDER>`)
  const char* value_placeholder;
  
  /// Type of the argument
  iarg_type type;

  /// Param to pass to the function
  void* user_param;

  /// Callback
  /// Gets `NULL` as value when this argument is a flag.
  void (*callback)(void* state, void* user_param, const char* value);

} iarg_spec;


/// Description of one program module or command
/// For example, to achive `myprogram compile [FILE]`,
/// you must define main module and submodule `compile for it`.
///
typedef struct iarg_cmd {
  
  /// Name of this module/command, e.g `compile` or `sort`,
  /// or program name if module is top level.
  const char* name;

  /// Brief description of that module.
  /// Like `this module sorts given strings`
  const char* brief;

  /// Long, detailed description of the module
  const char* long_info;

  /// Footer to print after all --help.
  const char* footer;

  /// List of submodules of this module
  struct iarg_cmd* subcommands;

  /// Number of submodule entries pointed by `submodules`
  size_t num_subcommands;

  /// Specs of keyword arguments
  iarg_spec* kwargs;

  /// Number of keyword arguments
  size_t num_kwargs;

  /// Names of positional arguments, separated by `,`.
  /// This can be `NULL` if `num_args` is `0`
  ///
  /// Leading whitespaces will be trimmed, trailing will be not,
  /// so write them as "FOO, BAR, BAZ"
  ///
  const char* arg_names;

  /// Number of positional arguments required for this module
  /// For this module to have submodules it must be `0`
  size_t min_num_args;

  /// Maximum number of positional arguments for this module.
  size_t max_num_args;

  /// We entered this module
  /// This function can reallocate state if it wants
  /// to expand state to acommodate settings for this submodule.
  void (*on_module_begin)(void** state);

} iarg_cmd;


/// \brief Parse command line arguments
///
/// \param argc        Number of arguments, as passed to `main()`
/// \param argv        Values of arguments, again, as passed to `main`
/// \param root_module Root module of argument spec.
///
/// \returns State, which resulted from parsing all arguments,
///          or `NULL` if something went wrong.
void* iarg_parse(int argc, const char** argv,
               const iarg_cmd* root_module);

void iarg_handler_set_flag(void* state, void* user_param, const char* value);
void iarg_handler_reset_flag(void* state, void* user_param, const char* value);

#endif
