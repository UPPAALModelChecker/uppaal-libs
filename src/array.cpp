#include "array.hpp"
#include "errors.hpp"
#include <vector>

static auto data = std::vector<double>{};

C_PUBLIC int get_size() { return data.size(); }

C_PUBLIC void set_size(int size)
{
	if (size < 0)
		log_err("size cannot be negative");
	data.resize(size);
}

C_PUBLIC void load_data(double values[], int size)
{
	if (size < 0)
		log_err("size cannot be negative");
	auto sz = std::min(static_cast<std::size_t>(size), data.size());
	std::copy(data.data(), data.data() + sz, values);
}

C_PUBLIC void store_data(const double values[], int size)
{
	if (size < 0)
		log_err("size cannot be negative");
	data.resize(size);
	data.assign(values, values + size);
}
