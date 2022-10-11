/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#ifndef ERRORS_HPP
#define ERRORS_HPP

#ifndef NDEBUG
#define log_err(format, ...) log_error(__FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(,) __VA_ARGS__)
#else
#define log_err(format, ...)
#endif

void set_error_file(FILE*);

void log_error(const char* function, const char* path, int line, const char* format, ...);

#endif /* ERRORS_HPP */
