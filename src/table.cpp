/**
 * Implements libtable functions.
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#include "csvtable.hpp"
#include "logging.h"
#include "dynlib.h"
#include <fstream>
#include <string>  // to_string/MSVC
#include <format>

#include <cmath>   // nan

/// creates a new table rows x cols and populates with zeros
C_PUBLIC int table_new(int rows, int cols);
/// creates a new table rows x cols and populates with integer value
C_PUBLIC int table_new_int(int rows, int cols, int value);
/// creates a new table rows x cols and populates with floating point value
C_PUBLIC int table_new_double(int rows, int cols, double value);
/// Resizes the table to a given rows x cols size. Returns id on success
C_PUBLIC int table_resize(int id, int rows, int cols);
/// Resizes the table to a given rows x cols size. Returns id on success
C_PUBLIC int table_resize_double(int id, int rows, int cols, double value);
/// Resizes the table to a given rows x cols size. Returns id on success
C_PUBLIC int table_resize_int(int id, int rows, int cols, int value);
/// Loads the table from CSV file, returns the table id, or -1 on error
C_PUBLIC int table_read_csv(const char* csv_path, int skip_lines);
/// Writes the table to CSV file, returns the number of rows, or -1 on error
C_PUBLIC int table_write_csv(int id, const char* csv_path);
/// Creates an independent copy of table with id and returns the ID of the new table
C_PUBLIC int table_copy(int id);
/// Clears selected table to 0x0 and releases associated resources
C_PUBLIC int table_clear(int id);
/// Returns the number of rows in the table with given id
C_PUBLIC int table_rows(int id);
/// Returns the number of columns in the table with given id
C_PUBLIC int table_cols(int id);
/// Returns an integer value stored at given row and column of table with given id
C_PUBLIC int read_int(int id, int row, int col);
/// Returns a floating point value stored at given row and column of table with given id
C_PUBLIC double read_double(int id, int row, int col);
/// Writes an integer value into given table at given row and column
C_PUBLIC int write_int(int id, int row, int col, int value);
/// Writes a floating point value into given table at given row and column
C_PUBLIC int write_double(int id, int row, int col, double value);
/// Maps key from key column to value column by using interpolation
C_PUBLIC double interpolate(int id, double key, int key_col, int value_col);
/// Copies column of count integers starting with given row and col into items starting with offset.
C_PUBLIC int read_int_col(int id, int row, int col, int* items, int offset, int count);
/// Copies row of count integers starting with given row and col into items starting with offset.
C_PUBLIC int read_int_row(int id, int row, int col, int* items, int offset, int count);

using namespace std::string_literals;

static std::vector<Table> tables{};

C_PUBLIC int table_new_double(int rows, int cols, double value)
{
	log_err("table_new(%d, %d, %f)", rows, cols, value);
	auto& t = tables.emplace_back();
	t.resize(static_cast<size_t>(rows));
	for (auto& row : t)
		row.resize(static_cast<size_t>(cols), value);
	const auto res = static_cast<int>(tables.size()) - 1;
	log_err("table_new: ", res);
	return res;
}

C_PUBLIC int table_new(int rows, int cols)
{
	return table_new_double(rows, cols, 0.0);
}

C_PUBLIC int table_new_int(int rows, int cols, int value)
{
	return table_new_double(rows, cols, value);
}

static Table load(const std::string& path, int skip_lines)
{
#ifdef ENABLE_CSV_CACHE
	static auto cache = std::unordered_map<std::string, table_t>{};
	auto it = cache.find(path);
	if (it == cache.end()) {
		log_err("No table in cache, loading from scratch");
		auto is = std::ifstream{path};
		is.peek();
		if (!is || is.eof()) {
			log_err("failed to read: %s", path.c_str());
		}
		bool res = false;
		std::tie(it, res) = cache.emplace(path, table_read_csv(is, skip_lines));
	} else {
		log_err("Found table in cache");
	}
	return it->second;
#else
	auto is = std::ifstream{path};
	is.peek();
	if (!is || is.eof()) {
		log_err("failed to read \"%s\": ", path.c_str());
	}
	return table_read_csv(is, skip_lines);
#endif
}

C_PUBLIC int table_read_csv(const char* csv_path, int skip_lines)
{
	log_err("table_read_csv(%s, %d)", csv_path, skip_lines);
	tables.push_back(load(csv_path, skip_lines));  // empty table in case of errors
	auto res = static_cast<int>(tables.size()) - 1;
	log_err("table_read_csv: id=%d", res);
	return res;
}

static void validate_table_id(int id)
{
	if (id < 0)
		throw std::underflow_error{std::format("table id is too low: {}", id)};
	if (id >= static_cast<int>(tables.size()))
		throw std::overflow_error{std::format("table id is too high: {}", id)};
}

static Table& get_table(int id)
{
	validate_table_id(id);
	return tables[static_cast<size_t>(id)];
}

C_PUBLIC int table_write_csv(const int id, const char* csv_path)
{
	try {
		log_err("table_write_csv(%d, %s)", id, csv_path);
		const auto& table = get_table(id);
		auto os = std::ofstream{csv_path};
		if (!table_write_csv(os, table, ','))
			throw std::runtime_error{std::format("failed to write file: {}", csv_path)};
		const auto rows = table.size();
		log_err("table_write_csv: %zu rows", rows);
		return static_cast<int>(rows);
	} catch (const std::exception& e) {
		log_err("%s", e.what());
	}
	return -1;
}

C_PUBLIC int table_copy(const int id)
{
	try {
		log_err("table_copy(%d)", id);
		const auto& table = get_table(id);
		const auto res = static_cast<int>(tables.size());
		tables.push_back(table);
		log_err("table_copy: %d (id)", res);
		return res;
	} catch (const std::exception& e) {
		log_err("%s", e.what());
	}
	return -1;
}

C_PUBLIC int table_clear(int id)
{
	try {
		log_err("table_clear(%d)", id);
		auto& table = get_table(id);
		table.clear();
		table.shrink_to_fit();
		log_err("table_clear: %d (id)", id);
		return id;
	} catch (const std::exception& e) {
		log_err("%s", e.what());
	}
	return -1;
}

/** User function: get the number of rows in the table */
C_PUBLIC int table_rows(const int id)
{
	try {
		log_err("table_rows(%d)", id);
		const auto& table = get_table(id);
		const auto res = table.size();
		log_err("table_rows: %zu (rows)", res);
		return static_cast<int>(res);
	} catch (const std::exception& e) {
		log_err("%s", e.what());
	}
	return -1;
}

/** User function: get the number of columns in the first table row.
 * Note that some rows may have fewer or more columns (depends on the source of data). */
C_PUBLIC int table_cols(const int id)
{
	try {
		log_err("table_cols(%d)", id);
		const auto& table = get_table(id);
		if (table.empty()) {
			log_err("%s", "table is empty");
			return 0;
		}
		const auto res = table.front().size();
		log_err("table_rows: %zu (cols)", res);
		return static_cast<int>(res);
	} catch (const std::exception& e) {
		log_err("%s", e.what());
	}
	return -1;
}

static auto& get_table_row(int id, int row)
{
	auto& table = get_table(id);
	if (row < 0)
		throw std::underflow_error{std::format("negative row: {}",row)};
	if (static_cast<int>(table.size()) <= row)
		throw std::overflow_error{std::format("row overflow: {}", row)};
	return table[static_cast<size_t>(row)];
}

/**
 * Internal function wrapping all the table accesses with range checks.
 * @param row the row number
 * @param col the column number
 * @return the element reference at row:col
 */
static Elem& access(int id, int row, int col)
{
	using namespace std::string_literals;
	auto& table_row = get_table_row(id, row);
	if (col < 0)
		throw std::underflow_error{std::format("negative column: {}", col)};
	if (static_cast<int>(table_row.size()) <= col)
		throw std::overflow_error{std::format("column overflow: {}", col)};
	return table_row[static_cast<size_t>(col)];
}

/** User function: read a floating point number at row:col in the table. */
C_PUBLIC double read_double(int id, int row, int col)
{
	try {
		return access(id, row, col);
	} catch (const std::exception& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return std::nan("");
}

C_PUBLIC int read_int(int id, int row, int col)
{
	const auto res = read_double(id, row, col);
	return static_cast<int>(res);
}

C_PUBLIC int table_resize_double(int id, int rows, int cols, double value)
{
	try {
		auto& table = get_table(id);
		if (rows < 0)
			throw std::underflow_error{std::format("negative number of rows: {}", rows)};
		if (cols < 0)
			throw std::underflow_error{std::format("negative number of columns: {}", cols)};
		table.resize(static_cast<size_t>(rows));
		for (auto& row : table)
			row.resize(static_cast<size_t>(cols), value);
		return id;
	} catch (const std::exception& e) {
		log_err("%s", e.what());
	}
	return -1;
}

C_PUBLIC int table_resize_int(int id, int rows, int cols, int value)
{
	return table_resize_double(id, rows, cols, value);
}

C_PUBLIC int table_resize(int id, int rows, int cols)
{
	return table_resize_double(id, rows, cols, 0);
}

C_PUBLIC int write_double(int id, int row, int col, double value)
{
	try {
		access(id, row, col) = value;
		return 0;
	} catch (const std::exception& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return -1;
}

C_PUBLIC int write_int(int id, int row, int col, int value) { return write_double(id, row, col, value); }

C_PUBLIC double interpolate(int id, double key, int key_col, int value_col)
{
	try {
		const auto& table = get_table(id);
		const auto res = interpolate(table, key, key_col, value_col);
		return res;
	} catch (const std::exception& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return std::nan("");
}

C_PUBLIC int read_int_col(int id, int row, int col, int* items, int offset, int count)
{
	try {
		log_err("read_int_col(%d, %d, %d, %p, %d %d)", id, row, col, items, offset, count);
		const auto& table = get_table(id);
		if (row < 0)
			throw std::underflow_error{std::format("negative row: {}", row)};
		if (row + count > static_cast<int>(table.size()))
			throw std::overflow_error{std::format("row+count {} is beyond number of rows", row+count)};
		if (col < 0)
			throw std::underflow_error{std::format("negative column {}", col)};
		if (col >= static_cast<int>(table[static_cast<size_t>(row)].size()))
			throw std::overflow_error{std::format("column {} is beyond table size", col)};
		const auto re = std::end(table);
		auto rb = std::next(std::begin(table), row);
		for (auto i = 0; i < count && rb != re; ++i, ++rb)
			items[offset + i] = static_cast<int>((*rb)[static_cast<size_t>(col)]);
		return 0;
	} catch (const std::exception& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return -1;
}

C_PUBLIC int read_int_row(int id, int row, int col, int* items, int offset, int count)
{
	try {
		log_err("read_int_row(%d, %d, %d, %p, %d %d)", id, row, col, items, offset, count);
		const auto& table = get_table(id);
		if (row < 0)
			throw std::underflow_error{std::format("negative row {}", row)};
		if (row >= static_cast<int>(table.size()))
			throw std::overflow_error{std::format("row {} is beyond table size", row)};
		if (offset < 0)
			throw std::underflow_error{std::format("negative offset {}", offset)};
		if (count < 0)
			throw std::underflow_error{std::format("negative count {}", count)};
		if (col < 0)
			throw std::underflow_error{std::format("negative column {}", col)};
		if (col + count > static_cast<int>(table[static_cast<size_t>(row)].size()))
			throw std::overflow_error{std::format("col+count {} is beyond table size", col+count)};
		auto rb = std::next(std::begin(table), row);
		for (auto i = size_t{0}; i < static_cast<size_t>(count); ++i)
			items[static_cast<size_t>(offset) + i] = static_cast<int>((*rb)[static_cast<size_t>(col) + i]);
		return 0;
	} catch (const std::exception& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return -1;
}
