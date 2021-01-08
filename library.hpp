/**
 * C++ wrapper for opening dynamically linked libraries
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <stdexcept> // runtime_error
#include <dlfcn.h> // dlopen, dlsym, dlerror

/** Wrapper for opening UppaalLibrary files.
 * Methods may throw runtime_error upon errors. */
class UppaalLibrary
{
	void* handle; // library handle

public:

	UppaalLibrary(const char* filepath):
		handle{dlopen(filepath, RTLD_LAZY | RTLD_LOCAL)}
	{
		if (!handle)
			throw std::runtime_error(dlerror());
	}
	~UppaalLibrary() noexcept
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

#endif /* LIBRARY_HPP */
