#ifndef ONEGIN_H
#define ONEGIN_H

#include <stddef.h>

typedef enum sort_mode_t {
  SORT_FORWARDS,
  SORT_BACKWARDS
} sort_mode_t;

/// Sort given list of line beginnings
///
void sort_lines(const char** lines, size_t n, sort_mode_t mode);

/// Print line starting from that pointer
///
void print_line(const char* begin);

/// Parse poem from given null-terminated buffer
/// and store resulting list of pointers to line beginnings
/// into `lines_output`.
///
size_t parse_poem(const char* buffer, const char*** lines_output);




#endif
