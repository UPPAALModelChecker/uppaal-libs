#include "library.hpp"

#include <doctest/doctest.h>

#include <filesystem>
#include <iostream>

#if defined(__linux__)
const auto libname = "libtable.so";
#elif defined(__APPLE__)
const auto libname = "libtable.dylib";
#elif defined(__MINGW32__)
const auto libname = "libtable.dll";
#elif defined(_WIN32)
const auto libname = "table.dll";
#else
#error("Unknown platform")
#endif


constexpr auto eps = 0.00001;

TEST_CASE("load libtable")
{
	using fn_str_int_to_int = int (*)(const char*, int);
	using fn_int_str_to_int = int (*)(int, const char*);
	using fn_int_to_int = int (*)(int);
	using fn_int_int_int_to_int = int (*)(int, int, int);
	using fn_int_int_double_to_int = int (*)(int, int, double);
	using fn_int_int_int_to_double = double (*)(int, int, int);
	using fn_int_double_int_int_to_double = double (*)(int, double, int, int);
	using fn_int_int_int_int = int (*)(int, int, int, int);
	using fn_int_int_int_double = int (*)(int, int, int, double);
	using fn_int_int_int_intp_int_int = void (*)(int, int, int, int*, int, int);

	try {
		auto lib_path = std::filesystem::current_path() / libname;
		auto lib_path_str = lib_path.string();
		auto lib = Library{lib_path_str.c_str()};
		auto table_new_int [[maybe_unused]] = lib.lookup<fn_int_int_int_to_int>("table_new_int");
		auto table_new_double = lib.lookup<fn_int_int_double_to_int>("table_new_double");
		auto table_resize_int [[maybe_unused]] = lib.lookup<fn_int_int_int_int>("table_resize_int");
		auto table_resize_double = lib.lookup<fn_int_int_int_double>("table_resize_double");
		auto table_read_csv = lib.lookup<fn_str_int_to_int>("table_read_csv");
		auto table_write_csv = lib.lookup<fn_int_str_to_int>("table_write_csv");
		auto table_copy = lib.lookup<fn_int_to_int>("table_copy");
		auto table_clear = lib.lookup<fn_int_to_int>("table_clear");
		auto table_rows = lib.lookup<fn_int_to_int>("table_rows");
		auto table_cols = lib.lookup<fn_int_to_int>("table_cols");
		auto read_int [[maybe_unused]] = lib.lookup<fn_int_int_int_to_int>("read_int");
		auto write_int [[maybe_unused]] = lib.lookup<fn_int_int_int_int>("write_int");
		auto read_double = lib.lookup<fn_int_int_int_to_double>("read_double");
		auto write_double = lib.lookup<fn_int_int_int_double>("write_double");
		auto read_int_col = lib.lookup<fn_int_int_int_intp_int_int>("read_int_col");
		auto read_int_row = lib.lookup<fn_int_int_int_intp_int_int>("read_int_row");
		auto interpolate = lib.lookup<fn_int_double_int_int_to_double>("interpolate");

		// read from file:
		const auto id = table_read_csv("table_input.csv", 0);
		auto rows = table_rows(id);
		auto cols = table_cols(id);
		REQUIRE(rows != 0); // table should not be empty
		REQUIRE(cols != 0);

		// read access:
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j)
				std::cout << read_double(id, i, j) << " ";
			std::cout << '\n';
		}
		CHECK(6 == read_double(id, 1, 1));
		const auto v1_2 = interpolate(id, 1.2, 0, 1);
		CHECK(v1_2 == doctest::Approx{5.2}.epsilon(eps));
		const auto v0 = interpolate(id, 0.0, 0, 1);
		CHECK(v0 == doctest::Approx{5.0}.epsilon(eps));
		const auto v5_5 = interpolate(id, 5.5, 0, 1);
		CHECK(8.0 == v5_5);
		// read in bulk:
		auto column1 = std::vector<int>(static_cast<size_t>(rows), 0);
		read_int_col(id, 0, 1, column1.data(), 0, rows);
		CHECK(column1[0] == 5);
		CHECK(column1[1] == 6);
		CHECK(column1[2] == 7);
		CHECK(column1[3] == 8);

		auto row1 = std::vector<int>(static_cast<size_t>(cols), 0);
		read_int_row(id, 1, 0, row1.data(), 0, cols);
		CHECK(row1[0] == 2);
		CHECK(row1[1] == 6);
		CHECK(row1[2] == 10);

		// write access:
		write_double(id, 1, 1, 3.141);
		CHECK(3.141 == read_double(id, 1, 1));

		table_resize_double(id, rows + 1, cols + 1, 2.7);
		CHECK(2.7 == read_double(id, rows, cols));
		table_write_csv(id, "table_output.csv");

		const auto id2 = table_copy(id);
		CHECK(table_rows(id) == table_rows(id2));
		CHECK(table_cols(id) == table_cols(id2));
		table_clear(id2);
		CHECK(table_rows(id2) == 0);
		auto id3 = table_new_double(3, 4, 3.14);
		CHECK(table_rows(id3) == 3);
		CHECK(table_cols(id3) == 4);
		CHECK(read_double(id3, 2, 2) == 3.14);
	} catch (std::runtime_error& err) {
		std::cerr << "Failed: " << err.what() << std::endl;
		CHECK(false);
	}
}
