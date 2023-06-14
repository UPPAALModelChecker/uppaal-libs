/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#include "errors.hpp"

#include <string>
#include <chrono>
#include <cstdio>	   // fopen, fprintf
#include <cstdarg>	   // va_list
#include <cerrno>      // errno

static auto error_own = false; // do we own the error file?
static auto error_path = std::string{"error.log"};

C_PUBLIC void set_error_path(const char* path)
{
	error_path = path;
	error_own = false;
}

C_PUBLIC const char* get_error_path()
{
    return error_path.c_str();
}

FILE* open_error_file()
{
	const auto path = get_error_path();
	FILE* file = nullptr;
#ifdef __STDC_LIB_EXT1__
	auto err = errno_t{};
	if (error_own) {
		err = fopen_s(&file, path, "a");
	} else {
		err = fopen_s(&file, path, "w");
		error_own = true;
	}
	if (err != 0) {
		fprintf(stderr, "error while opening %s: %d\n", path, err);
		file = nullptr;
	}
#else
	if (error_own) {
		file = std::fopen(get_error_path(), "a");
	} else {
		file = std::fopen(get_error_path(), "w");
		error_own = true;
	}
	if (file == nullptr) {
		fprintf(stderr, "error while opening %s: %d\n", path, errno);
	}
#endif
	return file;
}

void log_error(const char* function, const char* path, int line, const char* format, ...)
{
	auto file = open_error_file();
	const auto time = std::chrono::system_clock::now().time_since_epoch();
	const auto sec = std::chrono::duration_cast<std::chrono::seconds>(time);
	const auto usec = std::chrono::duration_cast<std::chrono::microseconds>(time-sec);
#if (defined(__APPLE__) && defined(__MACH__)) || defined(_WIN32)
	fprintf(file, "%lld.%06lld ", sec.count(), usec.count());
#else
	fprintf(file, "%ld.%06ld ", sec.count(), usec.count());
#endif
	va_list argp;
	va_start(argp, format);
	vfprintf(file, format, argp);
	va_end(argp);
	fprintf(file, " in %s at %s:%d\n", function, path, line);
	fclose(file);
}
