# Introduction
The st_tree library implements a C++ template tree container class for storing data in arbitrary tree structures. st_tree is short for “standard template tree,” and supports STL-compatible interfaces including iterators, allocators and standard STL container methods.

The st_tree library allows the programmer to easily declare and manipulate data in a tree:

```cpp
// declare a tree of integers, where children are stored using a vector storage model
tree<int> t1;  

// declare a tree of strings, where node children are maintained in sorted order
tree<string, ordered<> > t2;

// declare a tree of floating point, where children are indexed by a string label
tree<float, keyed<string> > t3;
```

# Features
The st_tree template container class provides the following features

* Fully featured standard STL container interface, including standard container methods, iterators, typedefs and allocators
* Common tree-related methods, including `ply()`, `depth()`, `subtree_size()`, `is_root()` and `parent()`
* Computational efficiency — all operations execute in logarithmic time, except those involving deep copy
* Configurable storage models for child nodes — children of a node may be managed using a `vector<>` (“raw”), `multiset<>` (“ordered”) or `map<>` (“keyed”) container model.
* Compiles under standard ANSI C++
* Unit tested
* Memory tested with valgrind
* Open sourced under the Apache 2.0 License


# Documentation
Documentation for the st_tree template container class and its components can be found here:
https://github.com/erikerlandson/st_tree/wiki/st_tree-documentation

The st_tree wiki is here:
https://github.com/erikerlandson/st_tree/wiki

# Install
The st_tree project home is here:
https://github.com/erikerlandson/st_tree

Instructions for download and install are here:
https://github.com/erikerlandson/st_tree/wiki/st_tree-installation-and-building

Quick start:
```sh
# generate makefiles from cmake
$ cd /path/to/st_tree
$ cmake .

# make examples and tests:
$ make

# run unit tests
$ ./tests/unit_tests

# installs header files and cmake integrations to /usr/local
$ make install
```

To build `st_tree` with a particular c++ standard (e.g. c++ 11, 14, 17, etc):
```sh
$ cmake -D CMAKE_CXX_STANDARD=14 .
```

# Examples
Examples reside in the .../st_tree/examples subdirectory.   You can also view an annotated list here:
https://github.com/erikerlandson/st_tree/wiki/st_tree-examples

# License
The st_tree repository is published under the Apache License, Version 2.0:
http://www.apache.org/licenses/LICENSE-2.0
