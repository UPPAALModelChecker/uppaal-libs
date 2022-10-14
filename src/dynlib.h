#ifndef UPPAAL_LIBS_DYNLIB_H
#define UPPAAL_LIBS_DYNLIB_H

#if defined(_WIN32)
#define C_PUBLIC extern "C" __declspec(dllexport)
#elif defined(__linux__)
#define C_PUBLIC extern "C"
#else
#define C_PUBLIC extern "C" __attribute__((visibility("default")))
#endif

#endif	// UPPAAL_LIBS_DYNLIB_H
