/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#include <string>
#include <chrono>
#include <cstdio>	   // fopen, fprintf
#include <cstdarg>	   // va_list

void log_error(const char* function, const char* path, int line, const char* format, ...)
{
	static FILE* file = std::fopen("error.log", "w");
	auto time = std::chrono::system_clock::now().time_since_epoch();
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(time);
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(time-sec);
#if defined(__APPLE__) && defined(__MACH__)
	fprintf(file, "%ld.%06d ", sec.count(), usec.count());
#else
	fprintf(file, "%d.%06d ", sec.count(), usec.count());
#endif
	va_list argp;
	va_start(argp, format);
	vfprintf(file, format, argp);
	va_end(argp);
	fprintf(file, " in %s at %s:%d\n", function, path, line);
	fflush(file);
}
