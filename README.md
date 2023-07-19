<h1>raColTest</h1>
<h2>Description</h2>
raColTest is a unit testing library made for C and C++. Most of the code is valid in both languages.
It is intended to be an extension of existing build systems, providing some CICD capabilities such as automated logging and notifying the build system of pass/fail. raColTest is independent of any specific build system and can be integrated into any existing C or C++ project. <br /> <br />

<h2>Build from source</h2>
Run <code>make all</code> to build from source. <br />
This will also run the test suite. Ensure tests are running as expected. <br />
<br />
<h2>Usage</h2>
The included build system is capable of functioning with other projects and running tests as expected. <br /> <br />

All test files must include raColTest_macros.h and every test must call TEST(), ASSERT(), and END_TEST in that order. Each test must be headed with <code>TEST(test_name)</code>, which declares needed variables and opens an exception safe testing environment. Next, write any needed test code. Following this, <code>ASSERT(conditional, description)</code>, which determines if the test has passed or failed based off of the conditional it is given, and logs test data. Any destructor code you may need should follow. Finally, a test is finalized with <code>END_TEST</code>. <br />
It is also important to note that all code between TEST() and END_TEST is within a local scope. Any values you would like to access outside of a test must be declared prior to opening the TEST() block. <br /> <br />

test/raColTest_macros.cpp can be used as an example of a test file. 
