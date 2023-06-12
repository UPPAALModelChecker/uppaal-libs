#include "errors.hpp"

#include <string>
#include <fstream>
#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>

int main()
{
	log_err("Testing: %s %d %f", "errors", 42, 3.141);
	const auto error_path = get_error_path();
	assert(error_path != nullptr);
	auto is = std::ifstream{error_path};
    assert(static_cast<bool>(is));
	const auto buffer = std::string{std::istreambuf_iterator<char>{is},std::istreambuf_iterator<char>{}};
	auto content = std::string_view{buffer};
	assert(static_cast<bool>(is));
	const auto message_pos = content.find(" ");
	assert(message_pos != std::string_view::npos);
	const auto at_pos = content.find(" at ", message_pos+1);
	const auto message = content.substr(message_pos+1, at_pos-message_pos-1);
	//std::cout << "Message: " << message << std::endl;
	assert(message == "Testing: errors 42 3.141000 in main");
	const auto test_errors_pos = content.find("test_errors.cpp", at_pos+4);
	assert(test_errors_pos != std::string_view::npos);
	const auto location = content.substr(test_errors_pos);
	//std::cout << "Location: " << location << std::endl;
	assert(location == "test_errors.cpp:15\n");
}