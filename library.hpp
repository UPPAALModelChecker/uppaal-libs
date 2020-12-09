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
public:
	using fn_ptr = void(*)();
	fn_ptr on_begin;     // mark the beginning of a new simulation
	fn_ptr on_end;       // mark the ending of the simulation

	UppaalLibrary(const char* filepath):
		handle{dlopen(filepath, RTLD_LAZY | RTLD_LOCAL)}
	{
		if (!handle)
			throw std::runtime_error(dlerror());
		on_construct = lookup<fn_ptr>("__ON_CONSTRUCT__");
		on_destruct = lookup<fn_ptr>("__ON_DESTRUCT__");
		on_begin = lookup<fn_ptr>("__ON_BEGIN__");
		on_end = lookup<fn_ptr>("__ON_END__");
		on_construct();
	}
	~UppaalLibrary() noexcept
	{
		if (!handle) {
			on_destruct();
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
private:
	void* handle; // library handle
	fn_ptr on_construct; // allocate resources (prepare)
	fn_ptr on_destruct;  // deallocate resources (cleanup)
};

#endif /* LIBRARY_HPP */
