#include "csvtable.hpp"
#include "errors.hpp"
#include <fstream>
#include <cmath> // nan

#include <unistd.h> // getcwd

extern "C" int table_new_int(int rows, int cols, int value);
extern "C" int table_new_double(int rows, int cols, double value);
extern "C" int table_resize_double(int id, int rows, int cols, double value);
extern "C" int table_resize_int(int id, int rows, int cols, int value);
extern "C" int table_read_csv(const char* csv_path, int skip_lines);
extern "C" int table_write_csv(int id, const char* csv_path);
extern "C" int table_copy(int id);
extern "C" int table_clear(int id);
extern "C" int table_rows(int id);
extern "C" int table_cols(int id);
extern "C" int read_int(int id, int row, int col);
extern "C" double read_double(int id, int row, int col);
extern "C" void write_int(int id, int row, int col, int value);
extern "C" void write_double(int id, int row, int col, double value);
extern "C" double interpolate(int id, double key, int key_col, int valu_col);
extern "C" void read_int_col(int id, int row, int col, int* items, int offset, int count);
extern "C" void read_int_row(int id, int row, int col, int* items, int offset, int count);


using namespace std::string_literals;

static std::vector<table_t> tables{};
#ifdef ENABLE_CSV_CACHE
static std::unordered_map<std::string, table_t> cache;
#endif

extern "C" int table_new_int(int rows, int cols, int value)
{
	log_err("table_new(%d, %d, %d)", rows, cols, value);
	auto& t = tables.emplace_back();
	t.resize(rows);
	for (auto& row : t)
		row.resize(cols, value);
	auto res = tables.size()-1;
	log_err("table_new: ", res);
	return res;
}

extern "C" int table_new_double(int rows, int cols, double value)
{
	log_err("table_new(%d, %d, %f)", rows, cols, value);
	auto& t = tables.emplace_back();
	t.resize(rows);
	for (auto& row : t)
		row.resize(cols, value);
	auto res = tables.size()-1;
	log_err("table_new: ", res);
	return res;
}

static table_t load(const std::string& path, int skip_lines)
{
#ifdef ENABLE_CSV_CACHE
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
		log_err("failed to read: %s", path.c_str());
	}
	return table_read_csv(is, skip_lines);
#endif
}

/** loads the table from CSV file, returns the table id, or -1 on error */
extern "C" int table_read_csv(const char* csv_path, int skip_lines)
{
	log_err("table_read_csv(%s, %d)", csv_path, skip_lines);
	tables.push_back(load(csv_path, skip_lines)); // empty table in case of errors
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

static table_t& get_table(int id)
{
	if (id < 0)
		throw std::runtime_error("table id too low: "s + std::to_string(id));
	if ((size_t)id >= tables.size())
		throw std::runtime_error("table id too high: "s + std::to_string(id));
	return tables[id];
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
	auto& table = get_table(id);
	if (rows < 0) {
		log_err("negative row number: %d", rows);
		return -1;
	}
	if (cols < 0) {
		log_err("negative column number: %d", cols);
		return -1;
	}
	table.resize(rows);
	for (auto& row: table)
		row.resize(cols, value);
	return id;
}

/** User function: resize the entire table to a given rectangular size. Return id on success */
extern "C" int table_resize_int(int id, int rows, int cols, int value)
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
	table.resize(rows);
	for (auto& row: table)
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

extern "C" double interpolate(int id, double key, int key_col, int valu_col)
{
	auto res = 0.0;
	try {
		auto& table = get_table(id);
		res = interpolate(table, key, key_col, valu_col);
	} catch (std::runtime_error& e) {
		log_err("%s", e.what());
	}
	return res;
}

extern "C" void read_int_col(int id, int row, int col, int* items, int offset, int count)
{
	try {
		log_err("read_int_col(%d, %d, %d, %p, %d %d)", id, row, col, items, offset, count);
		auto& table = get_table(id);
		if (row < 0)
			throw std::runtime_error("negative row");
		if (col < 0)
			throw std::runtime_error("negative column");
		if ((size_t)row+count > table.size())
			throw std::runtime_error("row range is beyond table size");
		if ((size_t)col >= table[row].size())
			throw std::runtime_error("column is beyond table size");
		auto rb = std::next(std::begin(table), row), re = std::end(table);
		for (auto i = 0; i < count && rb != re; ++i, ++rb)
			items[offset + i] = (*rb)[col];
	} catch(std::runtime_error& e) {
		log_err("%s", e.what());
	}
}

extern "C" void read_int_row(int id, int row, int col, int* items, int offset, int count)
{
	try {
		log_err("read_int_row(%d, %d, %d, %p, %d %d)", id, row, col, items, offset, count);
		auto& table = get_table(id);
		if (row < 0)
			throw std::runtime_error("negative row");
		if (col < 0)
			throw std::runtime_error("negative column");
		if ((size_t)row >= table.size())
			throw std::runtime_error("row is beyond table size");
		if ((size_t)col+count > table[row].size())
			throw std::runtime_error("column range is beyond table size");
		auto rb = std::next(std::begin(table), row);
		for (auto i = 0; i < count; ++i)
			items[offset + i] = (*rb)[col+i];
	} catch(std::runtime_error& e) {
		log_err("%s", e.what());
	}
}
