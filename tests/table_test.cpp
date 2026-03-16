/**
 * Unit tests for loading libtable library.
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#include "library.hpp"

#include <cmath>
#include <doctest/doctest.h>

#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>

TEST_SUITE_BEGIN("libtable");

#ifndef LIBTABLE_PATH
#error "Please define LIBTABLE_PATH to path to the dynamic library of table"
#endif
#ifndef CSV_PATH
#error "Please define CSV_PATH to path to the example csv file"
#endif

#define STRINGIFY(x) #x
#define TO_STRING(x)  STRINGIFY(x)

using std::filesystem::path;
using std::filesystem::exists; // MSVC fails to do ADL
const path libtable_path = TO_STRING(LIBTABLE_PATH);
const path csv_path = TO_STRING(CSV_PATH);

TEST_CASE("load libtable")
{
	REQUIRE_MESSAGE(exists(libtable_path), ("Failed to find " + libtable_path.string()));
	REQUIRE_MESSAGE(exists(csv_path), ("Failed to find " + csv_path.string()));

	using fn_str_int_to_int = int (*)(const char*, int);
	using fn_int_str_to_int = int (*)(int, const char*);
	using fn_int_to_int = int (*)(int);
	using fn_int_int_int_to_int = int (*)(int, int, int);
	using fn_int_int_double_to_int = int (*)(int, int, double);
	using fn_int_int_int_to_double = double (*)(int, int, int);
	using fn_int_double_int_int_to_double = double (*)(int, double, int, int);
	using fn_int_int_int_int = int (*)(int, int, int, int);
	using fn_int_int_int_double = int (*)(int, int, int, double);
	using fn_int_int_int_intp_int_int = int (*)(int, int, int, int*, int, int);

	auto approx = doctest::Approx{0}.epsilon(0.00001);

	try {
		auto lib_path_str = libtable_path.string();
		auto lib = Library{lib_path_str.c_str()};  // may throw upon errors
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
		const auto id = table_read_csv(csv_path.string().c_str(), 0);
		REQUIRE(id >= 0);  // success with loading table
		const auto rows = table_rows(id);
		REQUIRE(rows >= 0);				  // table should be non-empty
		CHECK(table_rows(id + 1) == -1);  // non-existing table
		const auto cols = table_cols(id);
		REQUIRE(cols >= 0);				  // should be some columns
		CHECK(table_cols(id + 1) == -1);  // non-existing table

		// read access:
		auto os = std::ostringstream{};
		for (int row = 0; row < rows; ++row) {
			os << read_double(id, row, 0);
			for (int col = 1; col < cols; ++col)
				os << " " << read_double(id, row, col);
			os << '\n';
		}
		CHECK(os.str() == "1 5 9 1\n2 6 10 4\n3 7 11 9\n4 8 12 16\n");
		CHECK(6 == read_double(id, 1, 1));
		// bad arguments:
		CHECK(std::isnan(read_double(-1, 1, 1)));	   // negative table id
		CHECK(std::isnan(read_double(id + 1, 1, 1)));  // non-existing table
		CHECK(std::isnan(read_double(id, -1, 1)));	   // negative row
		CHECK(std::isnan(read_double(id, rows, 1)));   // row overflow
		CHECK(std::isnan(read_double(id, 1, -1)));	   // negative column
		CHECK(std::isnan(read_double(id, 1, cols)));   // column overflow
		constexpr auto bad_int = std::numeric_limits<int>::lowest();
		CHECK(read_int(-1, 1, 1) == bad_int);	   // negative table id
		CHECK(read_int(id + 1, 1, 1) == bad_int);  // non-existing table
		CHECK(read_int(id, -1, 1) == bad_int);	   // negative row
		CHECK(read_int(id, rows, 1) == bad_int);   // row overflow
		CHECK(read_int(id, 1, -1) == bad_int);	   // negative column
		CHECK(read_int(id, 1, cols) == bad_int);   // column overflow

		const auto v1_2 = interpolate(id, 1.2, 0, 1);
		CHECK(v1_2 == approx(5.2));
		const auto v0 = interpolate(id, 0.0, 0, 1);
		CHECK(v0 == approx(5.0));
		const auto v5_5 = interpolate(id, 5.5, 0, 1);
		CHECK(8.0 == v5_5);
		// read in bulk:
		auto column1 = std::vector<int>(static_cast<size_t>(rows), 0);
		REQUIRE(read_int_col(id, 0, 1, column1.data(), 0, rows) == 0);
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
	} catch (const std::exception& err) {
		FAIL(err.what());
	} catch (...) {
		FAIL("Failed with unknown exception");
	}
}

TEST_SUITE_END();