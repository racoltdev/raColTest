<h1>raColTest</h1>
raColTest is a unit testing library made for and in C.
It is intended to be an extension of existing build systems, providing some CICD capabilities such as automated logging and notifying the build system of pass/fail. raColTest is independent of any specific build system and can be integrated into any existing C project.
A limitation of the tool is it's dependence on \*nix programs, including bash, awk, and nm. Furthermore, the actions of this library are hardcoded. There is no future intent to make compatible with non-\*nix systems or to implement friendlier configuration.
