#include "logging.h"

#include <algorithm>  // hack to fix doctest for MSVC
#include <doctest/doctest.h>

#include <string>
#include <fstream>

TEST_CASE("Error message")
{
	CHECK(get_error_path() == std::string_view{"error.log"});
	set_error_path("logging_test.log");
	log_err("Testing: %s %d %f", "errors", 42, 3.141);
	const auto error_path = get_error_path();
	REQUIRE(error_path != nullptr);
	auto is = std::ifstream{error_path};
	REQUIRE(static_cast<bool>(is));
	using isbit = std::istreambuf_iterator<char>;
	const auto buffer = std::string{isbit{is}, isbit{}};
	const auto content = std::string_view{buffer};
	REQUIRE(static_cast<bool>(is));
	const auto message_pos = content.find(" ");
	REQUIRE(message_pos != std::string_view::npos);
	const auto in_pos = content.find(" in ", message_pos + 1);
	REQUIRE(in_pos != std::string_view::npos);
	const auto message = content.substr(message_pos + 1, in_pos - message_pos - 1);
	CHECK(message == "Testing: errors 42 3.141000");
	const auto at_pos = content.find(" at ", in_pos + 1);
	REQUIRE(at_pos != std::string_view::npos);
	const auto test_errors_pos = content.find("logging_test.cpp", at_pos + 4);
	REQUIRE(test_errors_pos != std::string_view::npos);
	const auto location = content.substr(test_errors_pos);
	CHECK(location == "logging_test.cpp:13\n");
}