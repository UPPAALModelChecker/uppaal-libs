# uppaal-libs

## Requirements
* cmake [>=3.15]
* C++ compiler supporting C++17

## Compile

Run `cmake-linux64-debug.sh`

## Usage

* Put `cmake-linux64-debug/libtable.so` next to your project files.
* Put your data into `table_input.csv` (can be customized by `UPPAAL_TABLE_INPUT` environment variable).
* Export path is `table_output.csv` (can be customized by `UPPAAL_TABLE_OUTPUT` environment variable).
* Import the library into Uppaal model:
```c
import "path/to/libtable.so" {
	double read_double(int row, int col);
	void write_double(int row, int col, double value);
	void export_table();
};
```
* After each run, the resulting table is exported to a file with path in `UPPAAL_TABLE_EXPORT`environment variable.
