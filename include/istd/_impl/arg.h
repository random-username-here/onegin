/**
 * \file
 * \brief Internals of implemenetation of `arg.h`.
 */
#ifndef ISTD__IMPL_ARG
#define ISTD__IMPL_ARG

#include "istd/arg.h"

#define _IARG_HELP_FLAG "-h, --help"

/// Print `--help` of given module
/// 
/// Considering module tree: if we have command
/// like this:
///
///   ./foo --nocolor bar baz something.txt
///
/// We will get in module tree:
///
///  0. [root module `foo`]
///  1. [module `bar`]
///  2. [module `baz`]
///
/// \param tree Array of pointers to module tree.
/// \param tree_size Number of modules in the tree
///
void _iarg_generate_help (const iarg_cmd** tree, size_t tree_size);

#endif
