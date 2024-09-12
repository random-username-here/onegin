#ifndef ISTD_EXCEPT
#define ISTD_EXCEPT

/// Fail due to some bad event.
/// Will print given message and terminate the program.
///
/// This should be later depreacted in favour of exception
/// handling, but for now this will suffice.
///
__attribute__ ((format (printf, 1, 2)))
__attribute__((noreturn))
void panic(const char* fmt, ...);

#endif
