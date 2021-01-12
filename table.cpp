#include "csvtable.hpp"
#include "errors.hpp"
#include <fstream>
#include <cmath> // nan

#include <unistd.h> // getcwd

extern "C" int table_new(int rows, int cols);
extern "C" int table_read_csv(const char* csv_path, int skip_lines);
extern "C" int table_write_csv(int id, const char* csv_path);
extern "C" int table_copy(int id);
extern "C" int table_clear(int id);
extern "C" int table_rows(int id);
extern "C" int table_cols(int id);
extern "C" double read_double(int id, int row, int col);
extern "C" int read_int(int id, int row, int col);
extern "C" int table_resize_double(int id, int rows, int cols, double value);
extern "C" int table_resize_int(int id, int rows, int cols, int value);
extern "C" void write_double(int id, int row, int col, double value);
extern "C" void write_int(int id, int row, int col, int value);

using namespace std::string_literals;

static std::vector<table_t> tables{};

extern "C" int table_new(int rows, int cols)
{
	log_err("table_new(%d, %d)", rows, cols);
	auto& t = tables.emplace_back();
	t.resize(rows);
	for (auto& row : t)
		row.resize(cols, 0);
	auto res = tables.size()-1;
	log_err("table_new: ", res);
	return res;
}

/**
 * The exported C library functions must be marked as `extern "C"`.
 * Other (internally linked) functions can be C, C++ -- anything (decided at compile-time).
 */

/** loads the table from CSV file, returns the table id, or -1 on error */
extern "C" int table_read_csv(const char* csv_path, int skip_lines)
{
	log_err("table_read_csv(%s)", csv_path);
	auto is = std::ifstream{csv_path};
	is.peek();
	if (!is || is.eof()) {
		log_err("failed to read: %s", csv_path);
		return -1;
	}
	tables.push_back(table_read_csv(is, skip_lines)); // empty table in case of errors
	auto res = tables.size()-1;
	log_err("table_read_csv: %d (id)", res);
	return res;
}

/** writes the table to CSV file, returns the number of rows, or -1 on error */
extern "C" int table_write_csv(int id, const char* csv_path)
{
	log_err("table_write_csv(%d, %s)", id, csv_path);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	auto os = std::ofstream{csv_path};
	if (!os) {
		log_err("failed to write: %s", csv_path);
		return -1;
	}
	table_write_csv(os, tables[id], ',');
	auto res = tables[id].size();
	log_err("table_write_csv: %d (rows)", res);
	return res;
}

extern "C" int table_copy(int id)
{
	log_err("table_copy(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	tables.push_back(tables[id]);
	auto res = tables.size()-1;
	log_err("table_copy: %d (id)", res);
	return res;
}

extern "C" int table_clear(int id)
{
	log_err("table_clear(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	tables[id].clear();
	tables[id].shrink_to_fit();
	log_err("table_clear: %d (id)", id);
	return id;
}


/** User function: get the number of rows in the table */
extern "C" int table_rows(int id)
{
	log_err("table_rows(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	auto res = tables[id].size();
	log_err("table_rows: %d (rows)", res);
	return res;
}

/** User function: get the number of columns in the first table row.
 * Note that some rows may have fewer or more columns (depends on the source of data). */
extern "C" int table_cols(int id)
{
	log_err("table_cols(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	if (tables[id].empty()) {
		log_err("%s", "table is empty");
		return 0;
	}
	auto res = tables[id].front().size();
	log_err("table_rows: %d (cols)", res);
	return res;
}

/**
 * Internal function wrapping all the table accesses with range checks.
 * @param row the row number
 * @param col the column number
 * @return the element reference at row:col
 */
static elem_t& access(int id, int row, int col)
{
	using namespace std::string_literals;
	if (id < 0)
		throw std::runtime_error("table id too low: "s + std::to_string(id));
	if ((size_t)id >= tables.size())
		throw std::runtime_error("table id too high: "s + std::to_string(id));
	if (row < 0)
		throw std::runtime_error("negative row: "s + std::to_string(row));
	if (tables[id].size() <= (size_t)row)
		throw std::runtime_error("row overflow: "s + std::to_string(row));
	if (col < 0)
		throw std::runtime_error("negative column: "s + std::to_string(col));
	auto& table_row = tables[id][row];
	if (table_row.size() <= (size_t)col)
		throw std::runtime_error("column overflow: "s + std::to_string(col));
	return table_row[col];
}

/** User function: read a floating point number at row:col in the table. */
extern "C" double read_double(int id, int row, int col)
{
	try {
		return access(id, row, col);
	} catch (std::runtime_error& e) {
		log_err("%s", e.what());
	}
	return std::nan("");
}

extern "C" int read_int(int id, int row, int col)
{
	return (int)read_double(id, row, col);
}

/** User function: resize the entire table to a given rectangular size. Return id on success */
extern "C" int table_resize_double(int id, int rows, int cols, double value)
{
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	if (rows < 0) {
		log_err("negative row number: %d", rows);
		return -1;
	}
	if (cols < 0) {
		log_err("negative column number: %d", cols);
		return -1;
	}
	tables[id].resize(rows);
	for (auto& row: tables[id])
		row.resize(cols, value);
	return id;
}

/** User function: resize the entire table to a given rectangular size. Return id on success */
extern "C" int table_resize_int(int id, int rows, int cols, int value)
{
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if ((size_t)id >= tables.size()) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	if (rows < 0) {
		log_err("negative row number: %d", rows);
		return -1;
	}
	if (cols < 0) {
		log_err("negative column number: %d", cols);
		return -1;
	}
	tables[id].resize(rows);
	for (auto& row: tables[id])
		row.resize(cols, value);
	return id;
}


extern "C" void write_double(int id, int row, int col, double value)
{
	try {
		access(id, row, col) = value;
	} catch (std::runtime_error& e) {
		log_err("%s", e.what());
	}
}

extern "C" void write_int(int id, int row, int col, int value)
{
	write_double(id, row, col, value);
}
