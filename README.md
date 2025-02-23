# raColTest        <img src="./logo.svg" width="100" align="center"/>

## Description
raColTest is a unit testing library made for C++. Future updates intend to make it compatible with C projects as well. Currently, it will only run on linux. Future expansion to posix and windows is planned. <br />
<br />
raColTest contains two primary components: the test runner, and the test library: <br />
The test runner can be compiled once and used across a system for any test suites a user may want. This is the `racoltest` executable that is created from building this project. <br />
The test library must be linked to and included in any project you want to use raColTest. It is the set of files that can be found under `src/lib_raColTest/` and is compiled into a dynamically linked library. This must be linked against any test files to produce test binaries. The test runner will then find, execute, log, and produce status reports on the test binaries. <br />
<br />

## Build from source
Prerequisites:
- make
- g++
<!-- end list -->
Run `make all` to build from source. Ensure tests are performing as expected by running `./raColTest`. Expected output: <br /> <br /> <img src="raColTest.png"/> <br /> <br />
If all tests perform as expected, run `make install` to place the raColTest executable in your path, and the library in an accessible location. This is the only time the test runner or library should need to be built unless you would like to upgrade to a newer version. From this point on, if you have a set of tests that are built according to the [Usage/Build](#build) section, running `racoltest` in the project root directory will collect and execute those tests.<br />
<br />
If you would like to install either the test runner or the test library in a different location, change the `INSTALL_BINDIR` and `INSTALL_LIBDIR` variables in the makefile, respectively. The default install locations are `/usr/local/bin/` and `/usr/lib`, which should work fine on most distributions.

## Usage
### Build
A test suite must be built according to a particular schema that the test runner expects to encounter. The makefile in the [example project](https://github.com/racoltdev/raColTest-Example-Project) can be used to build your own projects with raColTest with minimal editing.<br />
To use the premade build system, you will need an `include/` directory in your project root which contains all of the header files located in `src/lib_raColTest/` of this repository. <br />
<br />
To use your own build system, any test code should be put in a directory at project root called `test/`. Each file in `test/` should be compiled into it's own executable, each of which should be placed in a directory at project root called `testbin/`. The header files found within `src/lib_raColTest` must be included while compiling objects. Test binaries must be linked against `lib_raColTest.so`. <br />
<br />

### Writing tests
Each test file should be placed in a directory called `test/` at project root. They should each have a main function with the signature `int main(int argc, char** argv)`. `argv` is used internally to track the name of the test file. <br />
<br />
All test files must `#include macros.h` and every test must call `TEST()`, `ASSERT()`, and `END_TEST` in that order. Each test must be headed with `TEST(test_name)`, which declares needed variables and opens an exception safe testing environment. Next, write any needed test code. Following this, `ASSERT(conditional, description)`, which determines if the test has passed or failed based off of the conditional it is given, and logs test data. Any destructor code you may need should follow. Finally, a test is finalized with `END_TEST`. It is also important to note that all code between `TEST()` and `END_TEST` is within a local scope. Any values you would like to access outside of a test must be declared prior to opening the `TEST()` block. <br />
<br />
The files under `test/` can be used as examples of test files. <br />
<br />
The header files under `src/lib_raColTest` should be included in any project you would like to test, as raColTest requires them to build test executables. `src/lib_raColTest/macros.h` is the only stable interface with raColTest. <br />
<br />

### Configuration
raColTest supports the use of configuration files and a few configuration options. <br />
| Option Name | Data Type | Description |
| --- | --- | --- |
| logfile_name | string | Where test logs should be written to |
| timeout | int | How long a test can run before it is automatically stopped, in seconds |
| test_source_dir | string | Where the library should search for test source files |
| test_bin_dir | string | Where the library should search for test executable files |
| enable_github_status | bool | Whether to automatically update .github/status based on test completion |
| verbosity | int | How much information to print to console. 0 is minimal. 1 prints only failing tests. 2 prints all tests |

raColTest will automatically search 3 different locations for configuration files and choose the first one it finds. It searches in the following order: <br />
| name | default path | environment variable |
| --- | --- | --- |
|1. local | `$(pwd)/raColTest.config` | `$RACOLTEST_LOCAL_CONFIG_PATH` |
|2. global | `$(XDG_CONFIG_HOME)/raColTest/raColTest.config` | `$RACOLTEST_GLOBAL_CONFIG_PATH` |
|3. default | `/etc/raColTest/defaults.config` | `$RACOLTEST_DEFAULT_CONFIG_PATH` |

Note: If `$(XDG_CONFIG_HOME)` is not set, global instead searches `~/raColTest/raColTest.config`. <br />
Each of these locations can be modified by setting the corresponding environment variable on your system. <br />
<br />
For information on the config format used, see https://github.com/WizardCarter/simple-config-library. <br />
Warning: Invalid config files might not produce exceptions. If something isn't working as expected, double check the config file is correct. <br />
<br />

## Output
raColTest prints output in 4 parts: Collection, Excuction, Details, and Status.
### 1. Collection
Headed by "Collecting tests.......", this section lists the full canonical path of all the test source files raColTest was able to find in a project.
### 2. Execution
Headed by "Executing tests........", this section lists the local path of each test executable as it is run, and status "blips" are filled in as each individual test in a file is executed. Blips are a quick indicator of how a test suite is running before it is finished. The kinds of blips are:
- `Pass`: "p" with a green background
- `Fail`: "F" with a red background. Indicates an incorrect assertion
- `Exception`: "e" with a red background. Indicates a test failed because it raised an exception.
- `Error`: "E....." with a yellow background. Indicates an unhandlable error or signal was raised and forced the test file to exit prematurely. Some tests in that file may not have been executed.
<!--end list-->
### 3. Details
This section follows "Finished executing tests". If all tests pass, this section will not be printed. This section displays the name of any failed tests (excluding tests that may have been skipped due to an `Error`) as well as whether it was failed by an `Exception`/`Error` or incorrect assertion. If failed by an `Exception`/`Error`, the explaination of the exception or the signal name is printed. If failed by an incorrect assertion, the assertion and its conditional is printed, as well as any details about the assertion. Finally, any stdout produced by that test is printed.
### 4. Status
This is the final section, and it either shows "All tests passed successfully." or "Some tests failed!" as a final summary of the test suite. <br />
<br />

## Examples
The included `test/` directory can be used as an example of how to write tests with raColTest. For a more comprehensive example, see https://github.com/racoltdev/raColTest-Example-Project, which is a bare bones C++ project that includes tests written for raColTest and the required project setup to get raColTest to function correctly. <br />
<br />

## Uninstallation
Run `make uninstall` from this project's root directory to completely remove the test runner and the test library from your computer. Any tests existing on your computer will no longer run. <br />
<br />

## Acknowledgements
raColTest uses [simple-config-library by WizardCarter](https://github.com/WizardCarter/simple-config-library) to handle configuration file parsing. README and LICENSE for simple-config-library is included under `src/lib_raColTest/config/slc`.
