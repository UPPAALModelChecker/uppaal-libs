#include "errors.hpp"

#include <algorithm> // hack to fix doctest for MSVC
#include <doctest/doctest.h>

#include <string>
#include <fstream>
#include <iostream>

TEST_CASE("Error message")
{
	set_error_path("test_errors.log");
	log_err("Testing: %s %d %f", "errors", 42, 3.141);
	const auto error_path = get_error_path();
	REQUIRE(error_path != nullptr);
	auto is = std::ifstream{error_path};
    REQUIRE(static_cast<bool>(is));
	const auto buffer = std::string{std::istreambuf_iterator<char>{is},std::istreambuf_iterator<char>{}};
	auto content = std::string_view{buffer};
	REQUIRE(static_cast<bool>(is));
	const auto message_pos = content.find(" ");
	REQUIRE(message_pos != std::string_view::npos);
	const auto at_pos = content.find(" at ", message_pos+1);
	const auto message = content.substr(message_pos+1, at_pos-message_pos-1);
	CHECK(message == "Testing: errors 42 3.141000 in DOCTEST_ANON_FUNC_14");
	const auto test_errors_pos = content.find("test_errors.cpp", at_pos+4);
	REQUIRE(test_errors_pos != std::string_view::npos);
	const auto location = content.substr(test_errors_pos);
	CHECK(location == "test_errors.cpp:12\n");
}