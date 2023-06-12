/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#ifndef _ERRORS_HPP_
#define _ERRORS_HPP_

#ifndef NDEBUG
#define log_err(format, ...) log_error(__FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(,) __VA_ARGS__)
#else
#define log_err(format, ...)
#endif

#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	void set_error_path(const char* err_path);
	const char* get_error_path();

	void log_error(const char* function, const char* path, int line, const char* format, ...);
#ifdef __cplusplus
}
#endif	// __cplusplus

#endif /* _ERRORS_HPP_ */
