<!--
 Copyright 2016-2024 Apple Inc. All rights reserved.
-->
# Inflection  {#mainpage}

## About Unicode Inflection

Unicode Inflection is a C/C++ library that provides support for the following tasks.

- Word inflection of a word into another [surface form](https://en.wikipedia.org/wiki/Surface_form) of another word.
- Grammatical agreement between words.
- Querying grammatical properties as [grammemes](https://en.wiktionary.org/wiki/grammeme) (the values of [grammatical categories](https://en.wikipedia.org/wiki/Grammatical_category)) 

It uses C++20, [ICU4C](https://icu.unicode.org/), UTF-16 strings (just like Java) and a data source of lexical
dictionaries that contain relationships between inflections of a word. Just like ICU, it is thread safe between service
objects, but mutable objects are not necessarily thread safe between threads.

By making this implementation open sourced, various software frameworks can generate grammatically correct messages
and to lower the barriers to correctly localizing software.

### Platforms
Unicode Inflection is currently supported on these operating systems:

* iOS
* iPadOS
* macOS
* tvOS
* watchOS
* visionOS
* UBI Linux 9
* Ubuntu Linux 22

## How Unicode Inflection works
The following sections delve a bit deeper into the low-level functionality of Unicode Inflection, such as how caching,
multi-threading, work with Unicode Inflection. These sections are meant as a guide to utilizing Unicode Inflection in a
safe manner while also squeezing the most potential out of the library as possible.

### Caching
At the time of writing, caching is a one-way street. Once an object that utilizes caching functionality with some data,
it remains in-memory until the process has terminated. Reloading of such caches are not supported, since that involves
ensuring that all dependencies in the process space sharing the same resources have also stopped and released the same
resources.

The caching being done by Unicode Inflection lowers the lookup time for many portions of the
`inflection::dialog::CommonConceptFactory`
operations. It is for this reason that it may be a good idea to initialize these constructs before lookup time, so
that Unicode Inflection is in a "warmed up" state.

It is important to note that many of these cached data structures have ties to specific references in Unicode Inflection's
memory-mapped dictionaries. This makes reloading dictionaries difficult.

#### Grammar synthesizer caching
Grammar synthesizers memory map lexical dictionaries and cache various grammatical structures depending on the language.
Synthesized words are not cached.

### Multi-threading
Unicode Inflection is multi-thread friendly. It has <code>std::mutex</code> in places where deadlocks could occur, and
generally tries to abstract this away from users.

## History

This project was donated to the Unicode consortium from Siri at Apple Inc. These additional
resources may be helpful background information to reference:

* [Automatic Grammar Agreement in Message Formatting](https://www.youtube.com/watch?v=C2e7hYIkqoM) ([2023.11.8](https://www.unicode.org/events/utw/2023/talks/grammar/))
* [Authoring Grammatically Correct Conversational Templates for Siri](https://www.youtube.com/watch?v=emlIWUTaJFM) ([2020.10.16](https://www.unicodeconference.org/iuc44/Conference_Program.pdf))
* [Let's Come To An Agreement About Our Words](https://www.youtube.com/watch?v=KclVxxHX26k) ([2017.02.16](https://www.imug.org/events/imug-2017-events.htm#words))

## Dependencies

The following are the dependencies to use this code.

| Library                                           | runtime | build time | test time | Note                     |
|---------------------------------------------------|:-------:|:----------:|:---------:|--------------------------|
| [ICU4C](https://icu.unicode.org/)                 |    ✅    |     ✅      |     ✅     |                          |
| [marisa](https://github.com/s-yata/marisa-trie)   |    ✅    |     ✅      |     ✅     | statically linked        |
| [cmake](https://cmake.org/)                       |         |     ✅      |           |                          |
| [libxml2](https://gitlab.gnome.org/GNOME/libxml2) |         |     ✅      |     ✅     |                          |
| [Catch2](https://github.com/catchorg/Catch2/)     |         |            |     ✅     | automatically downloaded |

### Building

#### Working with LFS

Additional checkout steps are necessary when working with the repository as it utilizes Git LFS files.

##### Get LFS client and install it, e.g. on Ubuntu

```bash
sudo apt install git-lfs
git lfs install
```

For more details and troubleshooting refer to this [guide](https://git-lfs.com/).

#### Resolving ICU4C dependancy

Before building this project, you must have a distribution of ICU4C available. The path to the ICU distribution must be
set as ICU_ROOT in either options.mk or as a command line argument to cmake. The path should be the same as the
--prefix value used when ICU was configured, built and installed.

Typical ICU installation requires:
* Downloading "Latest" released ICU4C "Source code.[zip|tar.gz]" from https://github.com/unicode-org/icu/releases.
* Following [build and installation instructions](https://unicode-org.github.io/icu/userguide/icu4c/build.html).

##### Linux specific
* ICU4C is installed to /usr/local/lib/*icu* by default.
* ICU_ROOT should point to /usr/local/lib/. Omitting /icu applies to any other installation path.

#### Build instructions

```
cd inflection
mkdir build
cd build
CC=clang CXX=clang++ cmake -DICU_ROOT=<PATH_TO_ICU> ..
make -j 8 check
```

Alternatively, you can use cmake for the building, testing etc.
```
cmake --build . -j8 -t check
```

Optionally, ICU_ROOT can be specified in the file options.mk with the following type of syntax.
```
ICU_ROOT=<PATH_TO_ICU>
```
