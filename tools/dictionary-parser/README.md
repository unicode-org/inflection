<!--
 Copyright 2025 Unicode Incorporated and others. All rights reserved.
 Copyright 2021-2024 Apple Inc. All rights reserved.
-->
# Dictionary Parsers

These tools generate files that describes the grammatical properties of words from the various sources.

## Usage for Wikidata

1) Download a copy of Wikidata from https://dumps.wikimedia.org/wikidatawiki/entities/ (e.g. https://dumps.wikimedia.org/wikidatawiki/entities/20250115/wikidata-20250115-lexemes.json.bz2)
2) Optionally decompress the file. If this tool will be run more than once, decompressing the file will make the tool run faster.
3) Check what options were used for your language. They are at the end of the generated dictionary_XX.lst, look for "generated with options"
   - Run `grep 'generated with options' ../../resources/org/unicode/inflection/dictionary/dictionary_XX.lst | cut -d':' -f2`
   - If the above command prints nothing, no additional options were used to generate the file, or it was generated with a different tool.
   - To see what options are available run `./ParseWikidata`
   - At minimum use the `--language` option to specify the ISO-639 code for the language to extract.
4) Run
```
    ./ParseWikidata <THE_OPTIONS_FROM_STEP_3> \
    --inflections ../../resources/org/unicode/inflection/dictionary/inflectional_XX.xml \
    --dictionary ../../resources/org/unicode/inflection/dictionary/dictionary_XX.lst \
    <wikidata-NNNNNNNN-lexemes.json[.bz2]>
```
