<!--
 Copyright 2016-2024 Apple Inc. All rights reserved.
-->
# Morphuntion

## About Morphuntion

Morphuntion is a C/C++ library that provides support for the following tasks.

- Word inflection of a word into another [surface form](https://en.wikipedia.org/wiki/Surface_form) of another word.
- Grammatical agreement between words.
- Querying grammatical properties as [grammemes](https://en.wiktionary.org/wiki/grammeme) (the values of [grammatical categories](https://en.wikipedia.org/wiki/Grammatical_category)) 

It uses C++20, [ICU4C](https://icu.unicode.org/), UTF-16 strings (just like Java) and a data source of lexical
dictionaries that contain relationships between inflections of a word. Just like ICU, it is thread safe between service
objects, but mutable objects are not necessarily thread safe between threads.

By making this implementation open sourced, various software frameworks can generate grammatically correct messages
and to lower the barriers to correctly localizing software.

### Platforms
Morphuntion is currently supported on these operating systems:

* iOS
* iPadOS
* macOS
* tvOS
* watchOS
* visionOS
* UBI Linux 9
* Ubuntu Linux 22

## How Morphuntion works
The following sections delve a bit deeper into the low-level functionality of Morphuntion, such as how caching,
multi-threading, work with Morphuntion. These sections are meant as a guide to utilizing Morphuntion in a
safe manner while also squeezing the most potential out of the library as possible.

### Caching
At the time of writing, caching is a one-way street. Once an object that utilizes caching functionality with some data,
it remains in-memory until the process has terminated. Reloading of such caches are not supported, since that involves
ensuring that all dependencies in the process space sharing the same resources have also stopped and released the same
resources.

The caching being done by Morphuntion lowers the lookup time for many portions of the
`morphuntion::dialog::CommonConceptFactory`
operations. It is for this reason that it may be a good idea to initialize these constructs before lookup time, so
that Morphuntion is in a "warmed up" state.

It is important to note that many of these cached data structures have ties to specific references in Morphuntion's
memory-mapped dictionaries. This makes reloading dictionaries difficult.

#### Grammar synthesizer caching
Grammar synthesizers memory map lexical dictionaries and cache various grammatical structures depending on the language.
Synthesized words are not cached.

### Multi-threading
Morphuntion is multi-thread friendly. It has <code>std::mutex</code> in places where deadlocks could occur, and
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
| [CoreFoundation](https://www.swift.org/)          |    ✅    |     ✅      |     ✅     | automatically downloaded |
| [ICU4C](https://icu.unicode.org/)                 |    ✅    |     ✅      |     ✅     |                          |
| [marisa](https://github.com/s-yata/marisa-trie)   |    ✅    |     ✅      |     ✅     | statically linked        |
| [cmake](https://cmake.org/)                       |         |     ✅      |           |                          |
| [libxml2](https://gitlab.gnome.org/GNOME/libxml2) |         |     ✅      |     ✅     |                          |
| [Catch2](https://github.com/catchorg/Catch2/)     |         |            |     ✅     | automatically downloaded |

### Building

Before building this project, you must have a distribution of ICU4C available. The path to the ICU distribution must be
set as ICU_ROOT in either options.mk or as a command line argument to cmake. The path should be the same as the
--prefix value used when ICU was configured, built and installed.

If you want to build this project faster, you can adjust the number of concurrent build jobs used when compiling.

```
cd morphuntion
mkdir build
cd build
CC=clang CXX=clang++ cmake -DICU_ROOT=<PATH_TO_ICU> ..
make -j 8 check
```

Optionally, ICU_ROOT can be specified in the file options.mk with the following type of syntax.
```
ICU_ROOT=<PATH_TO_ICU>
```