/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#include <string>
#include <cstdio> // fopen, fprintf
#include <cstdarg> // variadic c function
#include <unistd.h> // getcwd
#include <sys/time.h> // gettimeofday


void log_error(const char* format, const char* function, const char* path, int line, ...)
{
	static auto file = []{
		char buffer[512];
		getcwd(buffer, 512);
		return std::fopen((std::string(buffer)+"/libtable.log").c_str(), "w");
	}();
	timeval tv;
	gettimeofday(&tv, nullptr);
	fprintf(file, "%d.%06d ", tv.tv_sec, tv.tv_usec);
	va_list args;
	va_start(args, line);
	fprintf(file, format, args);
	fprintf(file, " in %s at %s:%d\n", function, path, line);
}
