#include "csvtable.hpp"
#include "errors.hpp"
#include "dynlib.h"
#include <fstream>
#include <string> // to_string/MSVC
#include <cmath> // nan

C_PUBLIC int table_new_int(int rows, int cols, int value);
C_PUBLIC int table_new_double(int rows, int cols, double value);
C_PUBLIC int table_resize_double(int id, int rows, int cols, double value);
C_PUBLIC int table_resize_int(int id, int rows, int cols, int value);
C_PUBLIC int table_read_csv(const char* csv_path, int skip_lines);
C_PUBLIC int table_write_csv(int id, const char* csv_path);
C_PUBLIC int table_copy(int id);
C_PUBLIC int table_clear(int id);
C_PUBLIC int table_rows(int id);
C_PUBLIC int table_cols(int id);
C_PUBLIC int read_int(int id, int row, int col);
C_PUBLIC double read_double(int id, int row, int col);
C_PUBLIC void write_int(int id, int row, int col, int value);
C_PUBLIC void write_double(int id, int row, int col, double value);
C_PUBLIC double interpolate(int id, double key, int key_col, int valu_col);
C_PUBLIC void read_int_col(int id, int row, int col, int* items, int offset, int count);
C_PUBLIC void read_int_row(int id, int row, int col, int* items, int offset, int count);


using namespace std::string_literals;

static std::vector<table_t> tables{};

C_PUBLIC int table_new_int(int rows, int cols, int value)
{
	log_err("table_new(%d, %d, %d)", rows, cols, value);
	auto& t = tables.emplace_back();
	t.resize(static_cast<size_t>(rows));
	for (auto& row : t)
		row.resize(static_cast<size_t>(cols), value);
	const auto res = static_cast<int>(tables.size())-1;
	log_err("table_new: ", res);
	return res;
}

C_PUBLIC int table_new_double(int rows, int cols, double value)
{
	log_err("table_new(%d, %d, %f)", rows, cols, value);
	auto& t = tables.emplace_back();
	t.resize(static_cast<size_t>(rows));
	for (auto& row : t)
		row.resize(static_cast<size_t>(cols), value);
	const auto res = static_cast<int>(tables.size())-1;
	log_err("table_new: ", res);
	return res;
}

static table_t load(const std::string& path, int skip_lines)
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

/** loads the table from CSV file, returns the table id, or -1 on error */
C_PUBLIC int table_read_csv(const char* csv_path, int skip_lines)
{
	log_err("table_read_csv(%s, %d)", csv_path, skip_lines);
	tables.push_back(load(csv_path, skip_lines)); // empty table in case of errors
	auto res = static_cast<int>(tables.size())-1;
	log_err("table_read_csv: id=%d", res);
	return res;
}

/** writes the table to CSV file, returns the number of rows, or -1 on error */
C_PUBLIC int table_write_csv(const int id, const char* csv_path)
{
	log_err("table_write_csv(%d, %s)", id, csv_path);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if (id >= static_cast<int>(tables.size())) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	auto os = std::ofstream{csv_path};
	if (!os) {
		log_err("failed to write: %s", csv_path);
		return -1;
	}
	table_write_csv(os, tables[static_cast<size_t>(id)], ',');
	auto res = static_cast<int>(tables[static_cast<size_t>(id)].size());
	log_err("table_write_csv: %d (rows)", res);
	return res;
}

C_PUBLIC int table_copy(const int id)
{
	log_err("table_copy(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if (id >= static_cast<int>(tables.size())) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	tables.push_back(tables[static_cast<size_t>(id)]);
	auto res = static_cast<int>(tables.size())-1;
	log_err("table_copy: %d (id)", res);
	return res;
}

C_PUBLIC int table_clear(int id)
{
	log_err("table_clear(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if (id >= static_cast<int>(tables.size())) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	tables[static_cast<size_t>(id)].clear();
	tables[static_cast<size_t>(id)].shrink_to_fit();
	log_err("table_clear: %d (id)", id);
	return id;
}


/** User function: get the number of rows in the table */
C_PUBLIC int table_rows(const int id)
{
	log_err("table_rows(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if (id >= static_cast<int>(tables.size())) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	auto res = static_cast<int>(tables[static_cast<size_t>(id)].size());
	log_err("table_rows: %d (rows)", res);
	return res;
}

/** User function: get the number of columns in the first table row.
 * Note that some rows may have fewer or more columns (depends on the source of data). */
C_PUBLIC int table_cols(const int id)
{
	log_err("table_cols(%d)", id);
	if (id < 0) {
		log_err("table id is too low: %d", id);
		return -1;
	}
	if (id >= static_cast<int>(tables.size())) {
		log_err("table id is too high: %d", id);
		return -1;
	}
	if (tables[static_cast<size_t>(id)].empty()) {
		log_err("%s", "table is empty");
		return 0;
	}
	const auto res = static_cast<int>(tables[static_cast<size_t>(id)].front().size());
	log_err("table_rows: %d (cols)", res);
	return res;
}

static table_t& get_table(int id)
{
	if (id < 0)
		throw std::runtime_error("table id too low: "s + std::to_string(id));
	if (id >= static_cast<int>(tables.size()))
		throw std::runtime_error("table id too high: "s + std::to_string(id));
	return tables[static_cast<size_t>(id)];
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
	auto& table = get_table(id);
	if (row < 0)
		throw std::runtime_error("negative row: "s + std::to_string(row));
	if (static_cast<int>(table.size()) <= row)
		throw std::runtime_error("row overflow: "s + std::to_string(row));
	if (col < 0)
		throw std::runtime_error("negative column: "s + std::to_string(col));
	auto& table_row = table[static_cast<size_t>(row)];
	if (static_cast<int>(table_row.size()) <= col)
		throw std::runtime_error("column overflow: "s + std::to_string(col));
	return table_row[static_cast<size_t>(col)];
}

/** User function: read a floating point number at row:col in the table. */
C_PUBLIC double read_double(int id, int row, int col)
{
	try {
		return access(id, row, col);
	} catch (std::runtime_error& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return std::nan("");
}

C_PUBLIC int read_int(int id, int row, int col)
{
	return (int)read_double(id, row, col);
}

/** User function: resize the entire table to a given rectangular size. Return id on success */
C_PUBLIC int table_resize_double(int id, int rows, int cols, double value)
{
	auto& table = get_table(id);
	if (rows < 0) {
		log_err("negative row number: %d", rows);
		return -1;
	}
	if (cols < 0) {
		log_err("negative column number: %d", cols);
		return -1;
	}
	table.resize(static_cast<size_t>(rows));
	for (auto& row: table)
		row.resize(static_cast<size_t>(cols), value);
	return id;
}

/** User function: resize the entire table to a given rectangular size. Return id on success */
C_PUBLIC int table_resize_int(int id, int rows, int cols, int value)
{
	auto& table = get_table(id);
	if (rows < 0) {
		log_err("negative row number: %d", rows);
		return -1;
	}
	if (cols < 0) {
		log_err("negative column number: %d", cols);
		return -1;
	}
	table.resize(static_cast<size_t>(rows));
	for (auto& row: table)
		row.resize(static_cast<size_t>(cols), value);
	return id;
}


C_PUBLIC void write_double(int id, int row, int col, double value)
{
	try {
		access(id, row, col) = value;
	} catch (std::runtime_error& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
}

C_PUBLIC void write_int(int id, int row, int col, int value)
{
	write_double(id, row, col, value);
}

C_PUBLIC double interpolate(int id, double key, int key_col, int valu_col)
{
	auto res = 0.0;
	try {
		auto& table = get_table(id);
		res = interpolate(table, key, key_col, valu_col);
	} catch (std::runtime_error& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
	return res;
}

C_PUBLIC void read_int_col(int id, int row, int col, int* items, int offset, int count)
{
	try {
		log_err("read_int_col(%d, %d, %d, %p, %d %d)", id, row, col, items, offset, count);
		auto& table = get_table(id);
		if (row < 0)
			throw std::runtime_error("negative row");
		if (col < 0)
			throw std::runtime_error("negative column");
		if (row+count > static_cast<int>(table.size()))
			throw std::runtime_error("row range is beyond table size");
		if (col >= static_cast<int>(table[static_cast<size_t>(row)].size()))
			throw std::runtime_error("column is beyond table size");
		auto rb = std::next(std::begin(table), row), re = std::end(table);
		for (auto i = 0; i < count && rb != re; ++i, ++rb)
			items[offset + i] = static_cast<int>((*rb)[static_cast<size_t>(col)]);
	} catch (std::runtime_error& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
}

C_PUBLIC void read_int_row(int id, int row, int col, int* items, int offset, int count)
{
	try {
		log_err("read_int_row(%d, %d, %d, %p, %d %d)", id, row, col, items, offset, count);
		auto& table = get_table(id);
		if (row < 0)
			throw std::runtime_error("negative row");
		if (col < 0)
			throw std::runtime_error("negative column");
		if (row >= static_cast<int>(table.size()))
			throw std::runtime_error("row is beyond table size");
		if (col+count > static_cast<int>(table[static_cast<size_t>(row)].size()))
			throw std::runtime_error("column range is beyond table size");
		auto rb = std::next(std::begin(table), row);
		for (auto i = 0; i < count; ++i)
			items[offset + i] = static_cast<int>((*rb)[static_cast<size_t>(col+i)]);
	} catch (std::runtime_error& e [[maybe_unused]]) {
		log_err("%s", e.what());
	}
}
