#include "generator.hpp"

#include <vector>
#include <fstream>
#include <random>
#include <cassert>

static std::istream& skip_comments(std::istream& is)
{
	auto line = std::string{};
	while (is) {
		if (is.peek() == '#')
			std::getline(is, line);
		else if (std::isspace(is.peek()))
			is.get();
		else
			break;
	}
	return is;
}

static std::istream& skip_separator(std::istream& is)
{
	while (is && std::isspace(is.peek()))
		is.get();
	if (is) {
		switch (is.peek()) {
		case ',':
		case ';':
			is.get(); break;
		default: is.setstate(std::ios::badbit);
		}
	} else {
		is.setstate(std::ios::badbit);
	}
	return is;
}


struct Bar
{
	double offset;
	double width; // remove if distribution is discrete
	double height;
};

static std::istream& operator>>(std::istream& is, Bar& bar)
{
	is >> skip_comments >> bar.offset >> skip_separator >> bar.width >> skip_separator >> bar.height;
	if (bar.width < 0 || bar.height < 0)
		is.setstate(std::ios::badbit);
	return is;
}

static double sum(const std::vector<Bar>& bars)
{
	auto res = 0.0;
	for (const auto& bar: bars)
		res += bar.width * bar.height;
	return res;
}

static std::mt19937& generator()
{
	static auto gen = std::mt19937{std::random_device{}()};
	return gen;
}

class Histogram
{
	std::vector<Bar> bars;
	double total;
public:
	Histogram(std::vector<Bar> bars): bars{std::move(bars)}, total{sum(this->bars)} {}
	Histogram(): Histogram{{}} {}
	void clear() { bars.clear(); total = 0; }
	void add(double offset, double width, double height) {
		if (not bars.empty() and offset < bars.back().offset + bars.back().width)
			throw std::logic_error{"Offsets must be increasing monotonically and cannot overlap"};
		bars.emplace_back(offset, width, height);
		total += width * height;
	}
	unsigned int size() const { return bars.size(); }
	double sample() {
		if (bars.empty())
			return 0;
		auto dist = std::uniform_real_distribution<double>{0, total};
		auto pick = dist(generator());
		auto i = 0u;
		auto weight = bars[i].width * bars[i].height;
		while (weight < pick) {
			pick -= weight;
			++i;
			assert(i < bars.size());
			weight = bars[i].width * bars[i].height;
		}
		return bars[i].offset + pick / bars[i].height;
	}
};

static Histogram& histogram()
{
	static auto hist = Histogram{};
	return hist;
}

C_PUBLIC int init_generator(unsigned int seed)
{
	generator() = std::mt19937{seed};
	return 1;
}

/// Loads a histogram from a CSV file
C_PUBLIC int load_histogram(const char* path)
{
	auto last_offset = -std::numeric_limits<double>::infinity();
	auto bars = std::vector<Bar>{};
	auto is = std::ifstream{path};
	auto bar = Bar{};
	while (is >> bar) {
		if (last_offset <= bar.offset) {
			bars.push_back(bar);
			last_offset = bar.offset + bar.width;
		} else
			is.setstate(std::ios::badbit);
	}
	histogram() = Histogram{std::move(bars)};
	return histogram().size();
}

/// Clears the histogram
C_PUBLIC int clear_histogram()
{
	histogram().clear();
	return 1;
}

/// Adds a histogram bar
C_PUBLIC int add_bar(double offset, double width, double height)
{
	try {
		histogram().add(offset, width, height);
		return 1;
	} catch (std::exception& e) {
		return 0;
	}
}


/// generates random values from bars
C_PUBLIC double generate()
{
	return histogram().sample();
}
