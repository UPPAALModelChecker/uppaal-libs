#include "array.hpp"
#include <vector>

static auto data = std::vector<double>{};

C_PUBLIC uint32_t get_size() { return data.size(); }

C_PUBLIC void set_size(uint32_t size) { data.resize(size); }

C_PUBLIC void load_data(double values[], uint32_t size)
{
	auto sz = std::min(static_cast<std::size_t>(size), data.size());
	std::copy(data.data(), data.data() + sz, values);
}

C_PUBLIC void store_data(const double values[], uint32_t size)
{
	data.assign(values, values + size);
}

static auto data2 = std::vector<double>{};

C_PUBLIC uint32_t get_size2() { return data2.size(); }

C_PUBLIC void set_size2(uint32_t size) { data2.resize(size); }

C_PUBLIC void load_data2(double values[], uint32_t size)
{
	auto sz = std::min(static_cast<std::size_t>(size), data2.size());
	std::copy(data2.data(), data2.data() + sz, values);
}

C_PUBLIC void store_data2(const double values[], uint32_t size)
{
	data2.assign(values, values + size);
}

C_PUBLIC void load_both(double values[], uint32_t size, double values2[], uint32_t size2)
{
	auto sz1 = std::min(static_cast<std::size_t>(size), data.size());
	auto sz2 = std::min(static_cast<std::size_t>(size2), data2.size());
	std::copy(data.data(), data.data() + sz1, values);
	std::copy(data2.data(), data2.data() + sz2, values2);
}

C_PUBLIC void store_both(const double values[], uint32_t size, const double values2[],
						 uint32_t size2)
{
	data.assign(values, values + size);
	data2.assign(values2, values2 + size2);
}
