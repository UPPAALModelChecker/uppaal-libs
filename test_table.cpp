#include "library.hpp"
#include <iostream>

/** Test for libtable */
int main()
{
	using fn_str_to_int = int (*)(const char*);
	using fn_int_str_to_int = int (*)(int, const char*);
	using fn_int_to_int = int (*)(int);
	using fn_int_int_to_int = int (*)(int, int);
	using fn_int_int_int_to_double = double (*)(int, int, int);
	using fn_int_int_int_int = int (*)(int, int, int, int);
	using fn_int_int_int_double = int (*)(int, int, int, double);
	try {
		auto lib = Library{"./libtable.so"};
		auto table_new = lib.lookup<fn_int_int_to_int>("table_new");
		auto table_read_csv = lib.lookup<fn_str_to_int>("table_read_csv");
		auto table_write_csv = lib.lookup<fn_int_str_to_int>("table_write_csv");
		auto table_copy = lib.lookup<fn_int_to_int>("table_copy");
		auto table_clear = lib.lookup<fn_int_to_int>("table_clear");
		auto table_rows = lib.lookup<fn_int_to_int>("table_rows");
		auto table_cols = lib.lookup<fn_int_to_int>("table_cols");
		auto read_double = lib.lookup<fn_int_int_int_to_double>("read_double");
		auto write_double = lib.lookup<fn_int_int_int_double>("write_double");
		auto table_resize_int = lib.lookup<fn_int_int_int_int>("table_resize_int");
		auto table_resize_double = lib.lookup<fn_int_int_int_double>("table_resize_double");
		auto id = table_read_csv("table_input.csv");
		auto rows = table_rows(id);
		auto cols = table_cols(id);
		if (rows == 0 || cols == 0)
			throw std::runtime_error("empty table");
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j)
				std::cout << read_double(id, i, j) << " ";
			std::cout << '\n';
		}
		if (6 != read_double(id, 1, 1))
			throw std::runtime_error("expected 6 at 1:1");
		write_double(id, 1, 1, 3.141);
		if (3.141 != read_double(id, 1, 1))
			throw std::runtime_error("expected 3.141 at 1:1");
		table_resize_double(id, rows+1, cols+1, 2.7);
		if (2.7 != read_double(id, rows,cols))
			throw std::runtime_error("expected 2.7 at new corner");
		table_write_csv(id, "table_output.csv");
		auto id2 = table_copy(id);
		if (table_rows(id) != table_rows(id))
			throw std::runtime_error("expected identical number of rows");
		if (table_cols(id) != table_cols(id))
			throw std::runtime_error("expected identical number of cols");
		table_clear(id2);
		if (table_rows(id2) != 0)
			throw std::runtime_error("expected 0 rows");
		auto id3 = table_new(3,4);
		if (table_rows(id3) != 3)
			throw std::runtime_error("expected 3 rows");
		if (table_cols(id3) != 4)
			throw std::runtime_error("expected 4 columns");
		if (read_double(id3, 2, 2) != 0.0)
			throw std::runtime_error("expected 0 at 2:2");
		lib.~Library();
		std::cout << "Test passed." << std::endl;
		return 0;
	} catch (std::runtime_error& err) {
		std::cerr << "Test failed: " << err.what() << std::endl;
		return -1;
	}
}