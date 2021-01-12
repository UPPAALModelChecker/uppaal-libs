/**
 * CSV table representation and its input and output.
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#ifndef CSVTABLE_HPP
#define CSVTABLE_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using elem_t = double;
using row_t = std::vector<elem_t>;
using table_t = std::vector<row_t>;
using dictionary_t = std::unordered_map<elem_t,std::vector<elem_t>>;

inline
void skip_line(std::istream& is)
{
	char c;
	while (is.get(c) && c != '\n')
		;
}

inline
void skip_comments(std::istream& is)
{
	while (is.peek() == '#')
		skip_line(is);
}

table_t table_read_csv(std::istream& is, int skip_lines)
{
	auto sep = char{};
	while (is && skip_lines-->0)
		skip_line(is);
	skip_comments(is);
	auto table = table_t{};
	auto elem = elem_t{};
	while (is >> elem) {
		auto& row = table.emplace_back();
		row.push_back(elem);
		while (is.get(sep) && sep != '\n')
			if (is >> elem)
				row.push_back(elem);
		while (is.peek() == '#')
			skip_line(is);
		skip_comments(is);
	}
	return table;
}

dictionary_t dictionary_read_csv(std::istream& is)
{
	auto dictionary = dictionary_t{};
	auto elem = elem_t{};
	auto sep = char{};
	while (is >> elem) {
		auto& row = dictionary[elem];
		while (is.get(sep) && sep != '\n')
			if (is >> elem)
				row.push_back(elem);
	}
	return dictionary;
}

double interpolate(const table_t& table, const elem_t key, int column)
{
	using namespace std::string_literals;
	if (column < 0)
		throw std::runtime_error("negative column");
	auto it = std::lower_bound(std::begin(table), std::end(table), key,
								   [](const row_t& row, const elem_t& key){
									   return row.front() < key;
								   });
	if (it == std::end(table))
		throw std::runtime_error("key is outside the table range");
	if (std::next(it) == std::end(table)) {
		auto& row = *it;
		if (row.size() <= (size_t)column)
			throw std::runtime_error("column overflow: "s+
									 std::to_string(column)+" at row "+
									 std::to_string(std::distance(std::begin(table), it)+1));
		return row[column]; // extrapolation assumes constant value
	}
	auto& row1 = (*it);
	if (row1.size() <= (size_t)column)
		throw std::runtime_error("column overflow: "s+
								 std::to_string(column)+" at row "+
								 std::to_string(std::distance(std::begin(table), it)+1));
	auto& row2 = *std::next(it);
	if (row2.size() <= (size_t)column)
		throw std::runtime_error("column overflow: "s+
								 std::to_string(column)+" at row "+
								 std::to_string(std::distance(std::begin(table), it)+1));
	auto x1 = row1[0];
	auto x2 = row2[0];
	auto y1 = row1[column];
	if (x2 == x1) // protect against div-by-zero
		return y1; // don't interpolate: pick the first
	auto y2 = row2[column];
	return y1 + (y2-y1)/(x2-x1)*(key-x1); // linear interpolation
}

std::ostream& table_write_csv(std::ostream& os, const table_t& table, const char sep= ',')
{
	for (auto& row : table) {
		auto b = std::begin(row), e = std::end(row);
		if (b != e) {
			os << *b;
			while (++b != e)
				os << sep << *b;
		}
		os << '\n';
	}
	return os;
}

std::ostream& dictionary_write_csv(std::ostream& os, const dictionary_t& dictionary, const char sep= ',')
{
	for (auto& row : dictionary) {
		os << row.first;
		for (auto& e : row.second)
			os << sep << e;
	}
	return os;
}

#endif /* TABLE_HPP */
