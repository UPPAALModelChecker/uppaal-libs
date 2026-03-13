/**
 * Error logging
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#ifndef INCLUDE_LOGGING_H
#define INCLUDE_LOGGING_H

#include "dynlib.h"

#ifndef NDEBUG
#ifdef _WIN32
#define log_err(format, ...) log_error(__FUNCTION__, __FILE__, __LINE__, format, __VA_ARGS__)
#else  // not _WIN32
#define log_err(format, ...) \
	log_error(__FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__)
#endif	// _WIN32
#else	// with NDEBUG
#define log_err(format, ...)
#endif	// NDEBUG

/// Set the file path for errors, returns 0 always
C_PUBLIC int set_error_path(const char* err_path);
/// Returns the path to the log file, "error.log" by default
C_PUBLIC const char* get_error_path();

void log_error(const char* function, const char* path, int line, const char* format, ...);

#endif	// INCLUDE_LOGGING_H