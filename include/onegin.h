#ifndef ONEGIN_H
#define ONEGIN_H

#include <stddef.h>

typedef enum sort_mode_t {
  SORT_FORWARDS,
  SORT_BACKWARDS
} sort_mode_t;

typedef struct {
  const char* begin;
  size_t len;
} string_view_t;

/// Sort given list of line beginnings
///
void sort_lines(string_view_t* lines, size_t n, sort_mode_t mode);

/// Print line starting from that pointer
///
void print_sview(const string_view_t str);

/// Parse poem from given null-terminated buffer
/// and store resulting list of pointers to line beginnings
/// into `lines_output`.
///
size_t parse_poem(const char* buffer, string_view_t** lines_output);

/// Type of the generated rythm. 
typedef enum {
  GENERATED_RYTHM_SEQUENTIAL, // a a b b
  GENERATED_RYTHM_CROSSOVER,  // a b a b
  GENERATED_RYTHM_WRAPPED     // a b b a
} generated_rythm_type;

/// Generate a poem
/// Generates in blocks of four lines, returns owned array of
/// non-owned pointers to lines.
string_view_t* generate_something_weird(
    const string_view_t* poem, size_t poem_len,
    size_t generated_len, generated_rythm_type rythm_type);

char* read_file(const char* name);

/// A custom qsort implementation
void my_qsort(void* buf, size_t len, size_t item_size,
              int (*cmp)(const void* a, const void* b, void* arg),
              void* arg);

#endif
