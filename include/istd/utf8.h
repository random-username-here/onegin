/**
 * \file
 * \brief UTF8 decoder
 */

#ifndef ISTD_UTF8
#define ISTD_UTF8

#include <stdint.h>
#include <stddef.h>

/**
 * \brief A type to represent a UTF8 codepoint.
 *
 * This type represents one **codepoint**, not one visible glyph. 
 * For example, some emojis are composed of multiple separate
 * codepoints. Also there are combining accents, which are also
 * separate codepoints.
 */
typedef uint32_t rune;


/**
 * \brief Extract one codepoint from given string, advance forwards
 *
 * ```
 *         cp = u'б'
 *       ┌───┐
 * d0 b0 d0 b1 d0 b2
 * ----- ----- -----
 * а     б     в     
 *       │     └─ returned
 *       └─ ptr
 * ```
 *
 * This function reads one UTF8 codepoint from the beginning of given
 * `str`, decodes it and puts it into `cp` (if it is not `NULL`). 
 * Value returned is pointer to next character after the codepoint.
 *
 * If end of string is found, then `cp` is set to `0`, and `str` is returned.
 *
 * If there is invalid UTF8 in the string, `NULL` is returned and
 * `errno` is set to `EILSEQ`.
 *
 * \param [in] str String to decode unicode from
 * \param [out] cp Pointer to put decoded codepoint into, or `NULL`
 * \returns Pointer pointing after extracted codepoint
 */
const char* utf8_next(const char* ptr, rune* cp);

/**
 * \brief Advance backwards and extract one codepoint from given string.
 *
 * ```
 *         cp = u'б'
 *       ┌───┐
 * d0 b0 d0 b1 d0 b2
 * ----- ----- -----
 * а     б     в     
 *       │     └─ ptr
 *       └─ returned
 * ```
 *
 * This function moves backwards by some distance to start of
 * previous character, and stores it into `cp` if `cp` is not `NULL`.
 * Value returned is a pointer to the beginning of decoded character.
 *
 * If beginning of the string is reached (provided with `begin`), this function
 * will set `cp` to `0` and return `begin`.
 *
 * If this function failed to decode a character, `NULL` is returned and `errno`
 * is set to `EILSEQ`.
 *
 * \param [in] ptr Pointer, pointing after character you want to extract
 * \param [in] begin Beginning of the string, to which `ptr` is pointing to
 * \param [out] cp Extracted codepoint
 * \returns Pointer to the beginning of extracted codepoint
 */
const char* utf8_prev(const char* ptr, const char* begin, rune* cp);

/// \brief Value returned by `utf8_length()` when it finds broken unicode.
#define UTF8_INVALID ((size_t) -1)

/**
 * \brief Compute length of utf8-encoded `\0`-terminated string.
 *
 * If `str` contains invalid UTF8, `-1` is returned by this function,
 * and `errno` is set to `EILSEQ`.
 *
 * \param [in] str String to find length of
 * \returns Length of the string, or `-1` if an error occured.
 */
size_t utf8_length(const char* str);

/**
 * \brief Encode UTF8 codepoint into given buffer.
 *
 * \todo Add more docs to this
 */
size_t utf8_encode_codepoint(rune cp, char* output);

#endif
