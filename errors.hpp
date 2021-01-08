/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#ifndef ERRORS_HPP
#define ERRORS_HPP

#define log_err(format, ...) log_error(format, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

void log_error(const char* format, const char* function, const char* path, int line, ...);

#endif /* ERRORS_HPP */
