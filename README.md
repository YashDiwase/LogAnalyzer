# Enterprise Log Analyzer

A desktop **Log Analyzer & Visualization Tool** built using a hybrid
**C++ + C# architecture**.

The application parses and analyzes log files using a native C++ engine,
exposes the engine through a C++/CLI bridge, and provides a WPF
dashboard for visualization.

## Architecture

``` text
WPF C# UI
    |
    v
C++/CLI Bridge
    |
    v
Native C++ CoreEngine
    |
    v
ParserFactory
    |
    +--> TextLogParser (.txt / .log)
    |
    +--> JsonLogParser (.json)
              |
              v
          LogEntry[]
              |
              v
         LogAnalyzer
              |
       +------+------+------+
       |      |      |
      INFO   WARN  ERROR
                     |
                     v
             Error by Module
```

## Features

-   Parse `.txt`, `.log`, and `.json` log files
-   Common `ILogParser` interface for multiple formats
-   `ParserFactory` for parser selection
-   Multithreaded text-log parsing
-   Thread-safe result merging
-   Common `LogEntry` data model
-   INFO / WARN / ERROR statistics
-   Error counts grouped by module
-   Malformed record/line detection
-   Invalid JSON detection
-   Unknown log-level handling
-   Prevent duplicate analysis while analysis is running
-   Dark-themed WPF dashboard
-   Log Distribution pie chart
-   Error-by-module DataGrid
-   Analysis status bar

## Supported Text Format

``` text
YYYY-MM-DD HH:MM:SS,ms LEVEL Module - Message
```

Example:

``` text
2025-01-20 10:15:32,123 ERROR OrderService - Order failed for ID=1023
2025-01-20 10:15:33,210 INFO PaymentService - Payment successful
2025-01-20 10:15:35,900 WARN AuthService - Token expired
```

Supported extensions:

``` text
.txt
.log
```

## Supported JSON Format

``` json
[
    {
        "timestamp": "2025-01-20 10:15:32,123",
        "level": "ERROR",
        "module": "OrderService",
        "message": "Order failed for ID=1023"
    },
    {
        "timestamp": "2025-01-20 10:15:33,210",
        "level": "INFO",
        "module": "PaymentService",
        "message": "Payment successful"
    },
    {
        "timestamp": "2025-01-20 10:15:35,900",
        "level": "WARN",
        "module": "AuthService",
        "message": "Token expired"
    }
]
```

Supported extension:

``` text
.json
```

## Parser Design

Both parsers convert their input into the same `LogEntry`
representation:

``` text
TextLogParser ----+
                  |
                  v
              LogEntry[]
                  ^
                  |
JsonLogParser ----+
```

The analyzer is therefore independent of the input format.

### ILogParser

``` cpp
class ILogParser
{
public:
    virtual ~ILogParser() = default;

    virtual bool LoadFile(const std::string& path) = 0;

    virtual const std::vector<LogEntry>&
        GetLogs() const = 0;

    virtual size_t GetMalformedRecordCount() const = 0;
};
```

### ParserFactory

The factory selects a parser from the file extension:

``` cpp
auto parser = ParserFactory::CreateParser(".json");
```

It returns:

``` cpp
std::unique_ptr<ILogParser>
```

so callers depend on the interface rather than concrete parser classes.

## LogAnalyzer

Parsing and analysis are deliberately separated.

``` text
Parser
  |
  v
LogEntry[]
  |
  v
LogAnalyzer
  |
  +--> INFO count
  +--> WARN count
  +--> ERROR count
  +--> Error count by module
```

The parsers answer **"What log entries are present?"**.

`LogAnalyzer` answers **"What do these entries tell us?"**.

This avoids duplicating aggregation logic between the text and JSON
parsers.

## Multithreading

The text parser splits input lines into chunks and processes chunks
concurrently.

``` text
Input
 |
 +--> Thread 1 --> local LogEntry[]
 +--> Thread 2 --> local LogEntry[]
 +--> Thread 3 --> local LogEntry[]
 +--> Thread 4 --> local LogEntry[]
                    |
                    v
             synchronized merge
```

Each worker maintains local results. Malformed counts are also
maintained locally before being merged, avoiding a data race on the
shared counter.

Original input ordering is not required by the current dashboard because
analysis is based on aggregate statistics.

## Error Handling

The application distinguishes between:

### Invalid JSON syntax

The complete JSON document is syntactically invalid.

`LoadFile()` reports failure.

### Malformed JSON record

The JSON document is valid, but an individual log object is missing
required fields.

That record is counted as malformed while other records can still be
processed.

### Unknown log level

A value such as `DEBUG` or `TRACE` is structurally valid but is not one
of the currently recognized levels.

It is represented as:

``` cpp
LogLevel::Unknown
```

## JSON Library

JSON parsing uses **nlohmann/json**, a header-only C++ JSON library.

It is used to:

-   Parse the JSON document
-   Validate JSON structure
-   Extract fields
-   Convert records into `LogEntry` objects

## WPF Dashboard

The WPF UI provides:

-   File selection
-   Browse button
-   Analyze button
-   INFO card
-   WARN card
-   ERROR card
-   Log Distribution pie chart
-   Error-by-module DataGrid
-   Analysis status bar

### Zero-value chart categories

Zero-count levels are not added as pie-chart slices.

For example:

``` text
INFO  = 1
WARN  = 0
ERROR = 1
```

The chart contains only INFO and ERROR. This avoids misleading
zero-value labels.

## Test Files

  -----------------------------------------------------------------------
  File                                Purpose
  ----------------------------------- -----------------------------------
  `sample_valid.json`                 Completely valid JSON records

  `sample_malformed_records.json`     Valid JSON containing incomplete
                                      records

  `sample_invalid_json.json`          Invalid JSON syntax

  `sample_unknown_level.json`         Unknown levels such as DEBUG and
                                      TRACE

  `sample_empty.json`                 Empty JSON array
  -----------------------------------------------------------------------

## Project Structure

``` text
EnterpriseLogAnalyzer/
|
+-- CoreEngine/
|   |
|   +-- Header Files/
|   |   +-- LogEntry.h
|   |   +-- ILogParser.h
|   |   +-- TextLogParser.h
|   |   +-- JsonLogParser.h
|   |   +-- ParserFactory.h
|   |   +-- LogAnalyzer.h
|   |
|   +-- Source Files/
|       +-- TextLogParser.cpp
|       +-- JsonLogParser.cpp
|       +-- ParserFactory.cpp
|       +-- LogAnalyzer.cpp
|
+-- LogAnalyzerBridge/
|   +-- AnalyzerWrapper.h
|   +-- AnalyzerWrapper.cpp
|
+-- LogAnalyzerApp/
    +-- MainWindow.xaml
    +-- MainWindow.xaml.cs
    +-- MainViewModel.cs
```

## Technology Stack

### Native C++

-   C++17 or later
-   STL
-   `std::thread`
-   `std::async`
-   `std::future`
-   `std::mutex`
-   `std::unique_ptr`
-   `std::filesystem`
-   `std::vector`
-   `std::unordered_map`

### C++/CLI

Used as the interoperability layer between native C++ and managed C#.

### C

-   .NET
-   WPF
-   MVVM-style data binding

### Visualization

-   LiveCharts2

### JSON

-   nlohmann/json

## Build Architecture

``` text
CoreEngine
    |
    v
LogAnalyzerBridge
    |
    v
LogAnalyzerApp
```

`CoreEngine` is a native C++ static library.

`LogAnalyzerBridge` consumes the native engine and exposes managed APIs.

`LogAnalyzerApp` references the C++/CLI bridge.

Recommended platform configuration:

``` text
x64
```

Use a consistent C++ runtime configuration across the native C++
projects for Debug and Release builds.

## Analysis Flow

When the user clicks **Analyze**:

``` text
1. WPF gets the selected file path
2. C++/CLI Bridge receives the path
3. CoreEngine extracts the file extension
4. ParserFactory selects the parser
5. Parser loads and parses the file
6. Parser creates LogEntry objects
7. LogAnalyzer calculates statistics
8. Bridge converts native results to managed types
9. WPF updates cards, chart and DataGrid
```

The WPF UI does not need to know which parser is being used.

## Design Principles Demonstrated

-   Separation of concerns
-   Interface-based design
-   Factory pattern
-   Runtime polymorphism
-   Multithreading
-   Thread-safe aggregation
-   RAII and smart pointers
-   Native/managed interoperability
-   MVVM and data binding
-   JSON processing
-   Data visualization

## Future Improvements

-   Additional formats such as CSV/XML
-   Streaming support for extremely large files
-   Progress reporting
-   Cancellation support
-   Date/time range filtering
-   Search and filtering
-   Log-level filtering
-   Export analysis results
-   Additional visualizations
-   Unit tests for parsers and analyzer
-   Parser performance benchmarking
-   JSON schema validation
-   Structured logging fields
-   Parser plugin architecture

## Why This Project?

This project demonstrates practical experience with:

-   Modern C++
-   Object-oriented design
-   Multithreading
-   STL
-   RAII and smart pointers
-   Design patterns
-   C++/CLI interoperability
-   C#
-   WPF
-   MVVM
-   JSON processing
-   Data visualization

It is designed as an interview and portfolio project where individual
architectural and implementation decisions can be discussed in depth.

## License

This is a personal learning and portfolio project. Add an appropriate
license if the repository is published publicly.

## Author

**Yash Diwase**

Enterprise Log Analyzer --- C++ / C++/CLI / C# / WPF
