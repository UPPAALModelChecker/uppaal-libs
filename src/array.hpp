#ifndef UPPAAL_LIBS_ARRAY_HPP
#define UPPAAL_LIBS_ARRAY_HPP

#include "dynlib.h"
#include <cinttypes>

/// Demonstrates data exchange via arrays

/// Returns the last saved data array size
C_PUBLIC uint32_t get_size();
/// Resized the stored data array
C_PUBLIC void set_size(uint32_t size);
/// Reads the stored data into a given array
C_PUBLIC void load_data(double values[], uint32_t size);
/// Writes data from a given array to storage
C_PUBLIC void store_data(const double values[], uint32_t size);

/// Returns the last saved data2 array size
C_PUBLIC uint32_t get_size2();
/// Resized the stored data2 array
C_PUBLIC void set_size2(uint32_t size);
/// Reads the stored data2 into a given array
C_PUBLIC void load_data2(double values[], uint32_t size);
/// Writes a given array into data2
C_PUBLIC void store_data2(const double values[], uint32_t size);

/// Reads the stored data2 into a given array
C_PUBLIC void load_both(double values1[], uint32_t size1, double values2[], uint32_t size2);
/// Writes a given array into data2
C_PUBLIC void store_both(const double values1[], uint32_t size1, const double values2[],
						 uint32_t size2);

#endif	// UPPAAL_LIBS_ARRAY_HPP
