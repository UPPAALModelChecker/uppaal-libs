#include "csvtable.hpp"
#include "errors.hpp"
#include <fstream>
#include <cmath> // nan

#include <unistd.h> // getcwd

using namespace std::string_literals;

static const auto current_dir = [] {
  	char buffer[512];
  	getcwd(buffer, 512);
  	return std::string{buffer};
}();
static const auto err = current_dir + "/libtable.log";

static auto table = table_t{};

/**
 * The exported C library functions must be marked as `extern "C"`.
 * Other (internally linked) functions can be C, C++ -- anything (decided at compile-time).
 */

/** Internal: loads the table from CSV file */
static void read_table(const char* csv_path)
{
	auto is = std::ifstream{csv_path};
	is.peek();
	if (!is || is.eof())
		log_error(err, "failed to open: "s + csv_path);
	table = read_table(is); // empty table in case of errors
}

/** Internal: writes the table to CSV file */
static void write_table(const char* csv_path)
{
	auto os = std::ofstream{csv_path};
	if (!os)
		log_error(err, "failed to open: "s + csv_path);
	write_csv(os, table, ',');
}

/** Called when the library is loaded */
extern "C" void __ON_CONSTRUCT__()
{
	auto default_csv_path = getenv("UPPAAL_TABLE_INPUT");
	auto csv_path = default_csv_path ? default_csv_path : current_dir+"/table_input.csv";
	read_table(csv_path.c_str());
}

/** Called before the library is unloaded */
extern "C" void __ON_DESTRUCT__()
{
	table.clear();
}

/*** Called before simulation start */
extern "C" void __ON_BEGIN__()
{}

/** Called when simulation is finished */
extern "C" void __ON_END__()
{
	auto csv_path = getenv("UPPAAL_TABLE_EXPORT");
	if (csv_path)
		write_table(csv_path);
}

/** User function: get the number of rows in the table */
extern "C" int get_rows()
{
	return table.size();
}

/** User function: get the number of columns in the first table row.
 * Note that some rows may have fewer or more columns (depends on the source of data). */
extern "C" int get_cols()
{
	if (table.empty()) {
		log_error(err, "table is empty");
		return 0;
	}
	return table.front().size();
}

/**
 * Internal function wrapping all the table accesses with range checks.
 * @param row the row number
 * @param col the column number
 * @return the element reference at row:col
 */
static elem_t& access(int row, int col)
{
	using namespace std::string_literals;
	if (row < 0)
		throw std::runtime_error("negative row: "s + std::to_string(row));
	if (table.size() <= (size_t)row)
		throw std::runtime_error("row overflow: "s + std::to_string(row));
	if (col < 0)
		throw std::runtime_error("negative column: "s + std::to_string(col));
	auto& table_row = table[row];
	if (table_row.size() <= (size_t)col)
		throw std::runtime_error("column overflow: "s + std::to_string(col));
	return table_row[col];
}

/** User function: read a floating point number at row:col in the table. */
extern "C" double read_double(int row, int col)
{
	try {
		return access(row, col);
	} catch (std::runtime_error& e) {
		log_error(err, e.what());
	}
	return std::nan("");
}

extern "C" int read_int(int row, int col)
{
	return (int)read_double(row, col);
}

/** User function: resize the entire table to a given rectangular size. */
extern "C" void resize(int rows, int cols, elem_t value)
{
	if (rows < 0) {
		log_error(err, "number of rows cannot be negative");
		return;
	}
	if (cols < 0) {
		log_error(err, "number of columns cannot be negative");
		return;
	}
	table.resize(rows);
	for (auto& row: table)
		row.resize(cols, value);
}

extern "C" void write_double(int row, int col, double value)
{
	try {
		access(row, col) = value;
	} catch (std::runtime_error& e) {
		log_error(err, e.what());
	}
}

extern "C" void write_int(int row, int col, int value)
{
	write_double(row, col, value);
}

extern "C" void export_table()
{
	auto default_csv_path = getenv("UPPAAL_TABLE_OUTPUT");
	auto csv_path = default_csv_path ? default_csv_path : current_dir+"/table_output.csv";
	write_table(csv_path.c_str());
}
