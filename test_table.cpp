#include "library.hpp"
#include <iostream>

/** Test for libtable */
int main()
{
	using fn_str_int_to_int = int (*)(const char*, int);
	using fn_int_str_to_int = int (*)(int, const char*);
	using fn_int_to_int = int (*)(int);
	using fn_int_int_int_to_int = int (*)(int, int, int);
	using fn_int_int_double_to_int = int (*)(int, int, double);
	using fn_int_int_int_to_double = double (*)(int, int, int);
	using fn_int_int_int_int = int (*)(int, int, int, int);
	using fn_int_int_int_double = int (*)(int, int, int, double);
	using fn_int_int_int_intp_int_int = void(*)(int, int, int, int*, int, int);


	try {
		auto lib = Library{"./libtable.so"};
		auto table_new_int = lib.lookup<fn_int_int_int_to_int>("table_new_int");
		auto table_new_double = lib.lookup<fn_int_int_double_to_int>("table_new_double");
		auto table_resize_int = lib.lookup<fn_int_int_int_int>("table_resize_int");
		auto table_resize_double = lib.lookup<fn_int_int_int_double>("table_resize_double");
		auto table_read_csv = lib.lookup<fn_str_int_to_int>("table_read_csv");
		auto table_write_csv = lib.lookup<fn_int_str_to_int>("table_write_csv");
		auto table_copy = lib.lookup<fn_int_to_int>("table_copy");
		auto table_clear = lib.lookup<fn_int_to_int>("table_clear");
		auto table_rows = lib.lookup<fn_int_to_int>("table_rows");
		auto table_cols = lib.lookup<fn_int_to_int>("table_cols");
		auto read_int = lib.lookup<fn_int_int_int_to_int>("read_int");
		auto write_int = lib.lookup<fn_int_int_int_int>("write_int");
		auto read_double = lib.lookup<fn_int_int_int_to_double>("read_double");
		auto write_double = lib.lookup<fn_int_int_int_double>("write_double");
		auto read_int_col = lib.lookup<fn_int_int_int_intp_int_int>("read_int_col");
		auto read_int_row = lib.lookup<fn_int_int_int_intp_int_int>("read_int_row");

		// read from file:
		const auto id = table_read_csv("table_input.csv", 0);
		auto rows = table_rows(id);
		auto cols = table_cols(id);
		if (rows == 0 || cols == 0)
			throw std::runtime_error("empty table");

		// read access:
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j)
				std::cout << read_double(id, i, j) << " ";
			std::cout << '\n';
		}
		if (6 != read_double(id, 1, 1))
			throw std::runtime_error("expected 6 at 1:1");

		// read in bulk:
		int column1[rows];
		read_int_col(id, 0, 1, column1, 0, rows);
		if (column1[0] != 5 || column1[1] != 6 || column1[2] != 7 || column1[3] != 8)
			throw std::runtime_error("wrong column1 values");

		int row1[cols];
		read_int_row(id, 1, 0, row1, 0, cols);
		if (row1[0] != 2 || row1[1] != 6 || row1[2] != 10)
			throw std::runtime_error("wrong row1 values");

		// write access:
		write_double(id, 1, 1, 3.141);
		if (3.141 != read_double(id, 1, 1))
			throw std::runtime_error("expected 3.141 at 1:1");

		table_resize_double(id, rows+1, cols+1, 2.7);
		if (2.7 != read_double(id, rows,cols))
			throw std::runtime_error("expected 2.7 at new corner");
		table_write_csv(id, "table_output.csv");


		const auto id2 = table_copy(id);
		if (table_rows(id) != table_rows(id))
			throw std::runtime_error("expected identical number of rows");
		if (table_cols(id) != table_cols(id))
			throw std::runtime_error("expected identical number of cols");
		table_clear(id2);
		if (table_rows(id2) != 0)
			throw std::runtime_error("expected 0 rows");
		auto id3 = table_new_double(3,4, 3.14);
		if (table_rows(id3) != 3)
			throw std::runtime_error("expected 3 rows");
		if (table_cols(id3) != 4)
			throw std::runtime_error("expected 4 columns");
		if (read_double(id3, 2, 2) != 3.14)
			throw std::runtime_error("expected 3.14 at 2:2");
		lib.~Library();
		std::cout << "Test passed." << std::endl;
		return 0;
	} catch (std::runtime_error& err) {
		std::cerr << "Test failed: " << err.what() << std::endl;
		return -1;
	}
}