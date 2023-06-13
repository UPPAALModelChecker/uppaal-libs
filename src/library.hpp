/**
 * C++ wrapper for opening dynamically linked libraries
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#ifndef _LIBRARY_HPP_
#define _LIBRARY_HPP_

#include <string> // to_string
#include <stdexcept> // runtime_error

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h> // dlopen, dlsym, dlerror

/** Wrapper for opening Library files.
 * Methods may throw runtime_error upon errors. */
class Library
{
	void* handle; // library handle

public:
	Library(const char* filepath):
		handle{dlopen(filepath, RTLD_LAZY | RTLD_LOCAL)}
	{
		if (!handle)
			throw std::runtime_error{dlerror()};
	}
	~Library() noexcept
	{
		if (handle) {
			dlclose(handle);
			handle = nullptr;
		}
	}
	/** Link the symbol from a library to a function pointer.
	 * Usage:
	 *   auto fn = lib.lookup<fn_type>(fn_name);
	 *   fn(arg1, arg2);
	 * Where fn_type ::= return_type (*)(arg1_type, arg2_type);
	 * */
	template <typename FnType>
	FnType lookup(const char* fn_name)
	{
		auto res = (FnType)(dlsym(handle, fn_name));
		if (res == nullptr)
			throw std::runtime_error(dlerror());
		return res;
	}
};

#elif defined(_WIN32) || defined(__MINGW32__)
#include <windows.h>

class Library
{
	HMODULE handle; // library handle

public:
	Library(const char* filepath):
		handle{LoadLibrary(TEXT(filepath))}
	{
		if (!handle)
			throw std::runtime_error{"Failed loading library with error "+std::to_string(GetLastError())};
	}
	~Library() noexcept
	{
		if (handle) {
			FreeLibrary(handle);
			handle = nullptr;
		}
	}
	/** Link the symbol from a library to a function pointer.
	 * Usage:
	 *   auto fn = lib.lookup<fn_type>(fn_name);
	 *   fn(arg1, arg2);
	 * Where fn_type ::= return_type (*)(arg1_type, arg2_type);
	 * */
	template <typename FnType>
	FnType lookup(const char* fn_name)
	{
		auto res = (FnType)(GetProcAddress(handle, fn_name));
		if (res == nullptr)
			throw std::runtime_error{"Failed symbol lookup with error "+std::to_string(GetLastError())};
		return res;
	}
};


#else
#error "unsupported platform"
#endif

#endif /* _LIBRARY_HPP_ */
