#ifndef UPPAAL_LIBS_ARRAY_HPP
#define UPPAAL_LIBS_ARRAY_HPP

#include "dynlib.h"

/// Demonstrates data exchange via arrays

/// Returns the last saved data array size
C_PUBLIC int get_size();
/// Resized the stored data array
C_PUBLIC void set_size(int size);
/// Reads the stored data into a given array
C_PUBLIC void load_data(double values[], int size);
/// Writes data from a given array to storage
C_PUBLIC void store_data(const double values[], int size);

#endif	// UPPAAL_LIBS_ARRAY_HPP
