/**
 * CSV table representation and its input and output.
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */
#ifndef _CSVTABLE_HPP_
#define _CSVTABLE_HPP_

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using elem_t = double;
using row_t = std::vector<elem_t>;
using table_t = std::vector<row_t>;
using dictionary_t = std::unordered_map<elem_t, std::vector<elem_t>>;

inline void skip_line(std::istream& is)
{
	char c;
	while (is.get(c) && c != '\n')
		;
}

inline void skip_comments(std::istream& is)
{
	while (is.peek() == '#')
		skip_line(is);
}

[[nodiscard]] table_t table_read_csv(std::istream& is, int skip_lines)
{
	auto sep = char{};
	while (is && skip_lines-- > 0)
		skip_line(is);
	skip_comments(is);
	auto table = table_t{};
	auto elem = elem_t{};
	while (is >> elem) {
		auto& row = table.emplace_back();
		row.push_back(elem);
		while (is.get(sep) && sep != '\n' && sep != '\r')
			if (is >> elem)
				row.push_back(elem);
		while ((is.peek() == '\r' || is.peek() == '\n') && is.get(sep))
			;
		skip_comments(is);
	}
	return table;
}

[[nodiscard]] dictionary_t dictionary_read_csv(std::istream& is)
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

[[nodiscard]] double interpolate(const table_t& table, const elem_t key, int key_column,
								 int value_column)
{
	using namespace std::string_literals;
	if (key_column < 0)
		throw std::runtime_error("negative key column");
	if (key_column >= static_cast<int>(table.front().size()))
		throw std::runtime_error("key column overflow");
	if (value_column < 0)
		throw std::runtime_error("negative value column");
	if (value_column >= static_cast<int>(table.front().size()))
		throw std::runtime_error("value column overflow");
	auto it2 = std::lower_bound(std::begin(table), std::end(table), key,
								[=](const row_t& row, const elem_t& key) {
									return row[static_cast<size_t>(key_column)] < key;
								});
	if (it2 == std::end(table))
		return table.back()[static_cast<size_t>(value_column)];	 // extrapolate with the last value
	if (it2 == std::begin(table))
		return table
			.front()[static_cast<size_t>(value_column)];  // extrapolate with the first value
	auto it1 = std::prev(it2);
	auto& row1 = (*it1);
	auto& row2 = (*it2);
	auto& x1 = row1[static_cast<size_t>(key_column)];
	auto& x2 = row2[static_cast<size_t>(key_column)];
	auto& y1 = row1[static_cast<size_t>(value_column)];
	if (x2 == x1)	// protect against div-by-zero
		return y1;	// don't interpolate: pick the first
	auto& y2 = row2[static_cast<size_t>(value_column)];
	return y1 + (y2 - y1) / (x2 - x1) * (key - x1);	 // linear interpolation
}

std::ostream& table_write_csv(std::ostream& os, const table_t& table, const char sep = ',')
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

std::ostream& dictionary_write_csv(std::ostream& os, const dictionary_t& dictionary,
								   const char sep = ',')
{
	for (auto& row : dictionary) {
		os << row.first;
		for (auto& e : row.second)
			os << sep << e;
	}
	return os;
}

#endif /* _CSVTABLE_HPP_ */
