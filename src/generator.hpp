#ifndef UPPAAL_LIBS_GENERATOR_HPP
#define UPPAAL_LIBS_GENERATOR_HPP

#include "dynlib.h"

/** Initializes the pseudo random number generator with specific seed
 * @returns 1 upon success */
C_PUBLIC int init_generator(unsigned int seed);

/** Loads a histogram from a CSV file
 * @returns the number of successfully loaded data rows
 */
C_PUBLIC int load_histogram(const char* path);

/// Clears the histogram
C_PUBLIC int clear_histogram();

/// Adds a histogram bar
C_PUBLIC int add_bar(double offset, double width, double height);

/// Generates random values from a previously loaded histogram
C_PUBLIC double generate();

#endif	// UPPAAL_LIBS_GENERATOR_HPP
