#include "library.hpp"

#include <doctest/doctest.h>

#include <filesystem>
#include <vector>
#include <iostream>

#if defined(__linux__)
const auto array_path = std::filesystem::current_path() / "libarray.so";
#elif defined(__APPLE__)
const auto array_path = std::filesystem::current_path() / "libarray.dylib";
#elif defined(__MINGW32__)
const auto array_path = std::filesystem::current_path() / "libarray.dll";
#elif defined(_WIN32)
const auto array_path = [] {
	// CMake on Windows puts Release binaries into CMAKE_CURRENT_BINARY_DIR/Release
	// otherwise binaries are in CMAKE_CURRENT_BINARY_DIR
	auto buffer = std::string(1024, '\0');
	auto size = GetModuleFileNameA(
		NULL, buffer.data(), static_cast<DWORD>(buffer.size()));  // path to current executable
	while (size >= buffer.size()) {
		buffer.resize(buffer.size() * 2, '\0');
		size = GetModuleFileNameA(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
	}
	buffer.resize(size);  // truncate the path
	return std::filesystem::path{buffer}.parent_path() / "array.dll";
}();
#else
#error("Unknown platform")
#endif

TEST_CASE("Array storage")
{
	using get_size_fn = int(*)();
	using set_size_fn = void(*)(int);
	using load_data_fn = void(*)(double[], int);
	using store_data_fn = void(*)(const double[], int);
	try {
		auto lib = Library(array_path);
		auto get_size = lib.lookup<get_size_fn>("get_size");
		auto set_size = lib.lookup<set_size_fn>("set_size");
		auto load_data = lib.lookup<load_data_fn>("load_data");
		auto store_data = lib.lookup<store_data_fn>("store_data");
		CHECK(get_size() == 0);
		set_size(2);
		CHECK(get_size() == 2);
		const auto input = std::vector{2.1718, 3.1415, 42.0};
		auto output = std::vector(3, 0.0);
		load_data(output.data(), output.size());
		CHECK(get_size() == 2);
		CHECK(output[0] == 0);
		CHECK(output[1] == 0);
		store_data(input.data(), input.size());
		CHECK(get_size() == 3);
		load_data(output.data(), output.size());
		CHECK(get_size() == 3);
		CHECK(output[0] == 2.1718);
		CHECK(output[1] == 3.1415);
		CHECK(output[2] == 42.0);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}