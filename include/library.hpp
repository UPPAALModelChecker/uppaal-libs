/**
 * C++ wrapper for opening dynamically linked libraries
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#ifndef INCLUDE_LIBRARY_HPP
#define INCLUDE_LIBRARY_HPP

#include <string>	  // to_string
#include <stdexcept>  // runtime_error

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>	// dlopen, dlsym, dlerror

/** Wrapper for opening Library files.
 * Methods may throw runtime_error upon errors. */
struct Library
{
	Library(const char* filepath): handle{dlopen(filepath, RTLD_LAZY | RTLD_LOCAL)}
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
	 */
	template <typename FnType>
	FnType lookup(const char* fn_name)
	{
		if (auto res = reinterpret_cast<FnType>(dlsym(handle, fn_name)); res)
			return res;
		throw std::runtime_error{dlerror()};
	}

private:
	void* handle{};	 ///< library handle
};

#elif defined(_WIN32) || defined(__MINGW32__)
#include <system_error>
#include <windows.h>

struct Library
{
	Library(const char* filepath): handle{LoadLibrary(TEXT(filepath))}
	{
		if (!handle) {
			auto err_no = static_cast<int>(::GetLastError());
			throw std::runtime_error{std::system_category().message(err_no)};
		}
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
		if (auto res = reinterpret_cast<FnType>(GetProcAddress(handle, fn_name)); res != nullptr)
			return res;
		throw std::runtime_error{"Failed symbol lookup: " + std::to_string(GetLastError())};
	}

private:
	HMODULE handle{};  ///< library handle
};

#else
#error "unsupported platform"
#endif

#endif /* INCLUDE_LIBRARY_HPP */
