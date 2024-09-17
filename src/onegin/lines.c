/// Implementation of `sort_lines`
#include "onegin.h"
#define _GNU_SOURCE // for qsort_r
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "istd/util/utf8.h"

#define ANNOT_BEGIN '['
#define ANNOT_END ']'
#define ESC_ANNOT "\x1b[90m"
#define ESC_END   "\x1b[0m"

/// Print given line from start to line break or `\0`
void print_line(const char* begin) {
  if (!begin) {
    printf("(NULL)");
    return;
  }
  for(const char* c = begin; *c && *c != '\n'; ++c) {
    if (*c == ANNOT_BEGIN)
      fputs(ESC_ANNOT, stdout);
    putc(*c, stdout);
    if (*c == ANNOT_END)
      fputs(ESC_END, stdout);
  }
}

size_t line_len(const char* begin) {
  if (!begin) return 0;
  size_t res = 0;
  for (const char* c = begin; *c && *c != '\n'; ++c, ++res);
  return res;
}

/// Should qsort compare that rune, or is it punctuation/whitespace/
/// which is ignored by sorting algorithm.
///
int should_compare(rune rn) {
  return (u'а' <= rn && rn <= u'я')
      || (u'А' <= rn && rn <= u'Я')
      || (u'a' <= rn && rn <= u'z')
      || (u'A' <= rn && rn <= u'Z');
}

/// Should this line be ignored entirely?
///
/// This returns `1` for:
///  - Lines without symbols worth comparing (this includes empty lines)
///  - Lines without `\t` at the start - because all poem lines are tabbed,
///    while annotations are not.
///
bool should_ignore_line(const char* begin) {

  assert(begin);

  rune r = 0;
  bool first_char = true;
  
  do {
    begin = utf8_next(begin, &r);
    
    if (first_char && r != '\t')
      return true;
    first_char = false;

    if (should_compare(r))
      return false;

  } while (begin && r != '\0' && r != '\n');
  
  return true;
}

/// Return line begin with skipped whitespaces at the start
///
const char* trimmed_line_begin(const char* line) {
  while (*line != '\0' && *line != '\n' && isspace(*line))
    ++line;
  return line;
}

/// Find next letter worth comparing in given string
/// using current position `pos`, line beginning `begin` and
/// iterator `next`.
///
const char* find_next_letter(
    const char* pos,
    const char* begin,
    rune* rn,
    const char*(*next)(const char*, const char*, rune*)
  ) {
  
  assert(pos);
  assert(begin);
  assert(rn);
  assert(next);

  pos = next(pos, begin, rn);
  bool in_annotation = false;

  while (*rn != '\0' && *rn != '\n' && (in_annotation || !should_compare(*rn))) {

    if (*rn == ANNOT_BEGIN || *rn == ANNOT_END)
      in_annotation = !in_annotation;
    
    pos = next(pos, begin, rn);
    
    if (!pos) { // Treat error as end of string
      *rn = 0;
      return NULL;
    }
  }

  return pos;
}

int is_end(rune r) {
  return r == 0 || r == u'\n';
}

void* SORT_MODE_TAG_FORWARDS = (void*) 1;
void* SORT_MODE_TAG_BACKWARDS = (void*) 2;

const char* utf8_next_adapter(const char* pos, const char* _, rune* rn) {
  (void) _;

  return utf8_next(pos, rn);
}

int sort_predicate(const void* _a, const void* _b, void* mode) {
  const char* a = *((const char**) _a);
  const char* b = *((const char**) _b);

  assert(a);
  assert(b);

  const char* a_begin = a, *b_begin = b;

  if (mode == SORT_MODE_TAG_BACKWARDS) {
    a += line_len(a);
    b += line_len(b);
  }

  const char* (*next_fn)(const char*, const char*, rune*) =
    mode == SORT_MODE_TAG_FORWARDS ? utf8_next_adapter : utf8_prev;
  rune a_rn = 1, b_rn = 1;

  while (a_rn == b_rn && !is_end(a_rn) && !is_end(b_rn)) {
    a = find_next_letter(a, a_begin, &a_rn, next_fn);
    b = find_next_letter(b, b_begin, &b_rn, next_fn);

    if (!a || !b)
      return 0;
  }

  return (int) a_rn - (int) b_rn;
}

void sort_lines(const char** lines, size_t n, sort_mode_t mode) {
  
  assert(lines);

  qsort_r(
      lines, n, sizeof(char*),
      sort_predicate,
      mode == SORT_BACKWARDS 
       ? SORT_MODE_TAG_BACKWARDS
       : SORT_MODE_TAG_FORWARDS
  );
}

size_t parse_poem(const char* buffer, const char*** lines_output) {

  assert(buffer);
  assert(lines_output);

  size_t num_lines = 0;

  for (const char* c = buffer; *c; ++c) {
    if (*c == '\n' && !should_ignore_line(c+1))
      num_lines++;
  }

  const char** lines = calloc(num_lines, sizeof(char*));
  *lines_output = lines;

  size_t cur_line = 0;
  if (!should_ignore_line(buffer))
    lines[cur_line++] = trimmed_line_begin(buffer);

  for (const char* c = strchrnul(buffer, '\n');
       *c;
       c = strchrnul(c+1, '\n'))
    if (!should_ignore_line(c+1))
      lines[cur_line++] = trimmed_line_begin(c+1);
  
  return num_lines;
}
