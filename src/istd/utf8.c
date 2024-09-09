/**
 * \brief Implementation of UTF8 decoder.
 *
 * | NOTE
 * |
 * | This file uses `goto`-s. And has a perfect reason to
 * | do so - because it simplifies the code. I will not
 * | aplogize to fans of `GOTO-s considered harmful`, who
 * | think what all `goto`-s must be eradicated.
 * |
 * | Every instrument should be used when it does the
 * | job best.
 *
 * Anyway, here is a reference for decoding UTF8:
 * 
 * -------------------------------------------------------------------------------
 * | First code point 	Last code point 	Byte 1 	  Byte 2    Byte 3    Byte 4   |
 * |-----------------------------------------------------------------------------|
 * | U+0000             U+007F            0xxxxxxx 	-         -         -        |
 * | U+0080             U+07FF            110xxxxx  10xxxxxx 	-         -        |
 * | U+0800             U+FFFF            1110xxxx  10xxxxxx  10xxxxxx 	-        |
 * | U+010000           U+10FFFF          11110xxx  10xxxxxx  10xxxxxx  10xxxxxx |
 * -------------------------------------------------------------------------------
 * 
 * Utf8 characters are sort of big endian - most significant
 * bits are stored in byte 1, while least significant are in
 * byte 4.
 *
 */

#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include "istd/utf8.h"

/// UTF8 continuation byte flag
#define BAD_CODEPOINT 0


/// Get length of utf8 codepoint starting from
/// that byte or `BAD_CODEPOINT` if this is cont. byte
static inline size_t cp_len(uint8_t byte) {

  if ((byte >> 6) == 2) // 0b10xxxxxx
    return BAD_CODEPOINT;
  else if ((byte >> 5) == 6) // 0b110xxxxx
    return 2;
  else if ((byte >> 4) == 14) // 0b1110xxxx
    return 3;
  else if ((byte >> 3) == 30) // 0b11110xxx
    return 4;
  else
    return 1;
}


/// Check if given byte is continuation byte
static inline int is_continuation(uint8_t ch) {
  // in form of 0b10xxxxxx
  return (ch >> 6) == 2;
}


/// Trim top `nb` bits of given number.
static inline uint8_t trim_top_bits(uint8_t num, size_t nb) {
  return (uint8_t) (num << nb) >> nb;
}


/// Decode UTF8 codepoint starting from that position
static inline rune decode(const uint8_t* ptr, uint8_t len) {
  rune res = trim_top_bits(ptr[0], len);
  for (size_t i = 1; i < len; ++i)
    res = (res << 6) + trim_top_bits(ptr[i], 1);
  return res;
}


/// Decode codepoint at `ptr`, store it in `cp` and return advanced position.
/// I wrote a long text in header, so read it for more details.
const char* utf8_next(const char* ptr, rune* cp) {

  assert(ptr);

  // End of the string reached (`\0` was found)
  if (*ptr == '\0') {
    // '\0'
    if (cp)
      *cp = 0;
    return ptr;
  }

  // Decode codepoint length
  size_t len = cp_len((uint8_t) ptr[0]);
  if (len == BAD_CODEPOINT)
    goto decoding_failed;

  // Check continuation bytes
  for (size_t i = 1; i < len; ++i)
    if (!is_continuation(ptr[i]))
      goto decoding_failed;

  // Decode codepoint if user wants it.
  if (cp) 
    *cp = decode((const uint8_t*) ptr, len);

  // Return advanced location
  return ptr + len;

 decoding_failed:
  // Set errno and fail
  errno = EILSEQ;
  return NULL; 
}


/// Move baсkwards one codepoint, and decode codepoint we are now at.
/// Long description is also in the header, look there for full docs.
const char* utf8_prev(const char* ptr, const char* begin, rune* cp) {

  assert(ptr);
  assert(begin);
  assert(ptr >= begin);

  // Beginning of the string reached
  if (ptr == begin) {
    if (cp)
      *cp = 0;
    return begin;
  }

  // Find start and length of the codepoint
  size_t len = 1;
  while (len < 4 && // Codepoint must up to 4 bytes in length 
         ptr >= begin + len && // It must be contained after string beginning
         is_continuation((uint8_t) *(ptr - len))) // 
    ++len;

  const char* codepoint_begin = ptr - len;

  // We checked four bytes (or up to beginning of the string) before
  // given position, but there was no sign of
  // start of the character.
  if (is_continuation((uint8_t) codepoint_begin[0]))
    goto decoding_failed; // So, fail.

  // Decode codepoint if needed
  if (cp)
    *cp = decode((const uint8_t*) codepoint_begin, len);

  return codepoint_begin;

 decoding_failed:
  // Set errno and fail
  errno = EILSEQ;
  return NULL; 
}

/// Compute length of UTF8 encoded string
size_t utf8_length(const char* str) {

  assert(str);

  size_t len = 0;
  rune r = 0; 

  do {
    str = utf8_next(str, &r);
    if (str == NULL)
      return UTF8_INVALID;
    ++len;
  } while (r != 0);

  return len;
}

/// Write given codepoint into given buffer, returning number of bytes used.
/// Buffer must be at least 5 bytes long.
size_t utf8_encode_codepoint(rune cp, char* output) {

  size_t len = cp < 0x80    ? 1 : 
               cp < 0x800   ? 2 :
               cp < 0x10000 ? 3 :
                              4 ;

  // Make byte with LEN top bits set.
  uint8_t first_top_bits = ((1 << len) - 1) << (8 - len);
  uint8_t cont_top_bits = 1 << 7;

  // Encode first byte.
  // If `len` = 1, then it is just `cp`.
  // If `len` > 1, then we take top bits of cp and combine them
  //               with bits representing the size of the run.
  output[0] = (len == 1 ? 0 : first_top_bits) | (cp >> (len-1)*6);

  for (size_t i = 1; i < len; ++i)
    output[i] = cont_top_bits | trim_top_bits(cp >> (len-1-i)*6, 2);

  output[len] = '\0';

  return len;
}
