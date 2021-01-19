# uppaal-libs

Generic examples of dynamically loaded libraries for Uppaal:

* `libtable` read, manipulate and write table data via CSV files.

## Requirements
* cmake [>=3.15]
* C++ compiler supporting C++17 (e.g. g++-9)

## Compile

Run `cmake-linux64.sh` to compile debug and release builds of the library:
* Debug build produces `error.log` in the current directory for debugging.
* Release build is optimized for speed and does not produce logs.

## Usage

* Put `libtable.so` next to your project files.
* Import the library into Uppaal model:
```c
import "path/to/libtable.so" {
    /** create a new table, fill it with integer value and return its id: */
    int table_new_int(int rows, int cols, int value);
    /** create a new table, fill it with double value and return its id: */
    int table_new_double(int rows, int cols, double value);
    /** read the table from the csv file and return its id: */
    int table_read_csv(const string& filename, int skip_lines);
    /** write the table to csv file and return the number of rows: */
    int table_write_csv(int id, const string& filename);
    /** create a new table copy and return its id: */
    int table_copy(int id);
    /** release the resources associated with the table and return its id: */
    int table_clear(int id);
    /** resize the table with the given dimensions: */
    int table_resize_int(int id, int rows, int cols, int value);
    /** return the number of rows in the table: */
    int table_rows(int id);
    /** return the number of columns in the table: */
    int table_cols(int id);
    /** read an integer value at row:col, counted from 0: */
    int read_int(int id, int row, int col);
    /** read a double value at row:col, counted from 0: */
    double read_double(int id, int row, int col);
    /** return interpolated look up value from row with key in key_column (sorted in ascending order) from value_column */
    double interpolate(int id, double key, int key_column, int value_column);
    /** write an integer value at row:col */
    void write_int(int id, int row, int col, int value);
    /** write a double value at row:col */
    void write_double(int id, int row, int col, double value);
};
```
* Call the library to load the CSV file into a table, read the size and entries:
```c
const int id = table_read_csv("path/to/table.csv");
const int rows = table_rows(id);
const int cols = table_cols(id);
int value_at_row0_col0 = read_int(id, 0, 0);
int value_at_row1_col2 = read_int(id, 1, 2);
```
* As the API implies, it is also possible to create, copy, resize, modify and write table data, but the modifications must be used with extreme care as they are **not side-effect-free**.

* A correct use is to not modify the table at all (**read-only** access is **side-effect-free**).

* Consider the following **bad modification** use case with two edges emanating form the initial location: the first edge modifies the table and the second just reads -- the model-checker may execute the first (and modify the table) and come back to explore the second location, however the table modification is visible for the second edge because engine could not reset the data in the external library.

* A possibly correct, but very tedious and error prone scenario with modification is to create a separate data for each new state and then refer back to the same data when the state changes back, i.e. maintain one-to-one correspondence between system state and the data in the external library. For example, an new edge update may create/update a new table/row in the table identified by some variable value and then the same table/row should be used when the system returns to the exact same state (which can be indexed by that variable value).
