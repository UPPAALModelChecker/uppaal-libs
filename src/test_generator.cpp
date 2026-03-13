#include "generator.hpp"

#include <doctest/doctest.h>

#include <vector>
#include <cmath> // round

TEST_CASE("Blank histogram")
{
	for (auto i = 0; i < 10; ++i) {
		const auto n = generate();
		CHECK(n == 0);
	}
}

static const auto approx = doctest::Approx{0}.epsilon(0.05); // within 5% margin

TEST_CASE("Histogram")
{
	const auto init = init_generator(42*42*42); // make a predictable sequence
	REQUIRE(init == 1);
	SUBCASE("From a file")
	{
		const auto rows = load_histogram("histogram.csv");
		REQUIRE(rows == 7);
	}
	SUBCASE("From scratch")
	{   // same as in "histogram_input.csv"
		CHECK(clear_histogram() == 1);
		CHECK(add_bar(2000, 1000,  10) == 1);
		CHECK(add_bar(3000, 1000, 150) == 1);
		CHECK(add_bar(4000, 1000,   0) == 1);
		CHECK(add_bar(5000, 1000, 220) == 1);
		CHECK(add_bar(6000, 1000,  40) == 1);
		CHECK(add_bar(7000, 1000, 300) == 1);
		CHECK(add_bar(9000, 1000, 280) == 1);
	}
	auto bars = std::vector<size_t>(8, 0);
	constexpr auto N = 1000;
	for (auto i = 0; i < 1000*N; ++i) {
		const auto n = generate();
		CHECK(2000 <= n);
		CHECK(n < 10000);
		const auto bar = static_cast<size_t>((n-2000) / 1000);
		REQUIRE(bar < bars.size());
		++bars[bar];
	}
	CHECK(bars[0] == approx(10.*N));
	CHECK(bars[1] == approx(150.*N));
	CHECK(bars[2] == 0);
	CHECK(bars[3] == approx(220.*N));
	CHECK(bars[4] == approx(40.*N));
	CHECK(bars[5] == approx(300.*N));
	CHECK(bars[6] == 0);
	CHECK(bars[7] == approx(280.*N));
}
