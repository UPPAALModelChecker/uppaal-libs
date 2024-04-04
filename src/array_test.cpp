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
	using get_size_fn = uint32_t (*)();
	using set_size_fn = void (*)(uint32_t);
	using load_data_fn = void (*)(double[], uint32_t);
	using store_data_fn = void (*)(const double[], uint32_t);
	using load_both_fn = void (*)(double[], uint32_t, double[], uint32_t);
	using store_both_fn = void (*)(const double[], uint32_t, const double[], uint32_t);
	try {
		auto lib = Library{array_path.string()};
		auto get_size = lib.lookup<get_size_fn>("get_size");
		auto set_size = lib.lookup<set_size_fn>("set_size");
		auto load_data = lib.lookup<load_data_fn>("load_data");
		auto store_data = lib.lookup<store_data_fn>("store_data");

		auto get_size2 = lib.lookup<get_size_fn>("get_size2");
		auto set_size2 = lib.lookup<set_size_fn>("set_size2");
		auto load_data2 = lib.lookup<load_data_fn>("load_data2");
		auto store_data2 = lib.lookup<store_data_fn>("store_data2");

		auto load_both = lib.lookup<load_both_fn>("load_both");
		auto store_both = lib.lookup<store_both_fn>("store_both");

		/// Test access to the first array:
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
		REQUIRE(get_size() == 3);
		CHECK(output[0] == 2.1718);
		CHECK(output[1] == 3.1415);
		CHECK(output[2] == 42.0);

		/// Test access to the second array:
		CHECK(get_size2() == 0);
		set_size2(2);
		CHECK(get_size2() == 2);
		load_data2(output.data(), output.size());
		REQUIRE(get_size2() == 2);
		CHECK(output[0] == 0);
		CHECK(output[1] == 0);
		store_data2(input.data(), input.size());
		REQUIRE(get_size2() == 3);
		load_data2(output.data(), output.size());
		CHECK(get_size2() == 3);
		CHECK(output[0] == 2.1718);
		CHECK(output[1] == 3.1415);
		CHECK(output[2] == 42.0);

		/// Test access to both arrays at once:
		const auto input2 = std::vector{42.0, 2.1718, 3.1415};
		auto output2 = std::vector<double>(3, 0.0);
		load_both(output.data(), output.size(), output2.data(), output2.size());
		CHECK(output == input);
		CHECK(output2 == input);
		set_size(2);
		set_size2(2);
		CHECK(get_size() == 2);
		CHECK(get_size2() == 2);
		store_both(input2.data(), input2.size(), input2.data(), input2.size());
		REQUIRE(get_size() == 3);
		REQUIRE(get_size2() == 3);
		load_both(output.data(), output.size(), output2.data(), output2.size());
		CHECK(output == input2);
		CHECK(output2 == input2);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		REQUIRE(false);
	}
}