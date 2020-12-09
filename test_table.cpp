#include "library.hpp"
#include <iostream>

/** Test for libtable */
int main()
{
	using fn_void = void (*)();
	using fn_void_to_int = int (*)();
	using fn_int_int_to_double = double (*)(int, int);
	using fn_int_int_double = void (*)(int, int, double);
	try {
		auto lib = UppaalLibrary{"./libtable.so"};
		auto get_rows = lib.lookup<fn_void_to_int>("get_rows");
		auto get_cols = lib.lookup<fn_void_to_int>("get_cols");
		auto read_double = lib.lookup<fn_int_int_to_double>("read_double");
		auto write_double = lib.lookup<fn_int_int_double>("write_double");
		auto resize = lib.lookup<fn_int_int_double>("resize");
		auto export_table = lib.lookup<fn_void>("export_table");
		lib.on_begin();
		auto rows = get_rows();
		auto cols = get_cols();
		if (rows == 0 || cols == 0)
			throw std::runtime_error("empty table");
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j)
				std::cout << read_double(i, j) << " ";
			std::cout << '\n';
		}
		if (6 != read_double(1,1))
			throw std::runtime_error("expected 6 at 1:1");
		write_double(1, 1, 3.141);
		if (3.141 != read_double(1,1))
			throw std::runtime_error("expected 3.141 at 1:1");
		lib.on_end();
		resize(rows+1, cols+1, 2.7);
		if (2.7 != read_double(rows,cols))
			throw std::runtime_error("expected 2.7 at new corner");
		export_table();
		lib.~UppaalLibrary();
		std::cout << "Test passed." << std::endl;
		return 0;
	} catch (std::runtime_error& err) {
		std::cerr << "Test failed: " << err.what() << std::endl;
		return -1;
	}
}