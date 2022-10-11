/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#include <string>
#include <cassert>
#include <cstdio>	   // fopen, fprintf
#include <cstdarg>	   // va_list
#include <unistd.h>	   // getcwd
#include <sys/time.h>  // gettimeofday

void log_error(const char* function, const char* path, int line, const char* format, ...)
{
	static FILE* file = std::fopen("error.log", "w");
	timeval tv;
	gettimeofday(&tv, nullptr);
#if defined(__APPLE__) && defined(__MACH__)
	fprintf(file, "%ld.%06d ", tv.tv_sec, tv.tv_usec);
#else
	fprintf(file, "%d.%06d ", tv.tv_sec, tv.tv_usec);
#endif
	va_list argp;
	va_start(argp, format);
	vfprintf(file, format, argp);
	va_end(argp);
	fprintf(file, " in %s at %s:%d\n", function, path, line);
	fflush(file);
}
