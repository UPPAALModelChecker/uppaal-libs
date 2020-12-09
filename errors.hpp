/**
 * Error reporting
 * Author: Marius Mikucionis <marius@cs.aau.dk>
 */

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <fstream>

void log_error(const std::string& fn, const std::string& message)
{
	auto log = std::ofstream{fn, std::ios::ate | std::ios::app};
	log << message << '\n';
}


#endif /* ERRORS_HPP */
