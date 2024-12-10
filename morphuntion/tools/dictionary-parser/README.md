<!--
 Copyright 2021-2024 Apple Inc. All rights reserved.
-->
# Dictionary Parsers

These tools generate files that describes the grammatical properties of words from the various sources.

## Usage for Wiktionary

1) Download a copy of Wiktionary data from http://dumps.wikimedia.org/ (e.g. http://dumps.wikimedia.org/enwiktionary/20130825/enwiktionary-20130825-pages-articles.xml.bz2)
2) Decompress dump
3) Run `./ParseWiktionary ~/Downloads/enwiktionary-20130825-pages-articles.xml > ../../resources/com/apple/morphuntion/dictionary/dictionary_en.lst`

## Usage for Apple's Lexical Resources (ALR)

1) Get the latest lexicon for your language.
2) Check what options were used for your language. They are at the end of the generated dictionary_XX.lst, look for "generated with options"
   - Run `grep 'generated with options' ../../resources/com/apple/morphuntion/dictionary/dictionary_XX.lst | cut -d':' -f2`
   - If the above command prints nothing, no additional options were used to generate the file, or it was generated with a different tool.
   - To see what options are available run `./ParseLexicon`
3) Run
```
    ./ParseLexicon <THE_OPTIONS_FROM_STEP_2> \
    --inflections ../../resources/com/apple/morphuntion/dictionary/inflectional_XX.xml \
    <THE_LEXICON_FILE> \
    > ../../resources/com/apple/morphuntion/dictionary/dictionary_XX.lst
```
