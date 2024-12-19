# How to add a new language support

Adding a new language requires multiple steps:
1. Adding support for a new locale
2. Adding inflection rules for the language
3. Adding lexicon for the language

The following steps with help you identify files that need to be added or changed, and will guide you in rule writing and lexicon preparation.

NOTE: Take a look at [PR #40](https://github.com/unicode-org/inflection/pull/40)) for example on how to add initial language support.
In general, to bootstrap your progress look for grammatically similar language that's already supported, e.g. if you are adding Serbian look for existing Russian or Croatian implementation.
This will help you find most of the files you need to add/change and will speed up implementation of the rules and lexicons.

## Mark your language as supported
* UPDATE: morphuntion/src/morphuntion/util/LocaleUtils.hpp
* UPDATE: morphuntion/src/morphuntion/util/LocaleUtils.cpp
* UPDATE: morphuntion/test/src/morphuntion/util/LocaleUtilsTest.cpp
* UPDATE: morphuntion/resources/org/unicode/morphuntion/locale/supported-locales.properties
* ADD: morphuntion/resources/org/unicode/morphuntion/tokenizer/config_XX.properties

## Define language grammar
* UPDATE: morphuntion/resources/org/unicode/morphuntion/features/grammar.xml

## Add pronouns if applicable
* UPDATE: morphuntion/resources/org/unicode/morphuntion/features/grammar.xml

## Implament basic handlers
TODO: We need to expand what each of these do.
* ADD: morphuntion/src/morphuntion/grammar/synthesis/XxGrammarSynthesizer.hpp
* ADD: morphuntion/src/morphuntion/grammar/synthesis/XxGrammarSynthesizer.cpp
* ADD: morphuntion/src/morphuntion/grammar/synthesis/XxGrammarSynthesizer_XxDisplayFunction.hpp
* ADD: morphuntion/src/morphuntion/grammar/synthesis/XxGrammarSynthesizer_XxDisplayFunction.hpp
* UPDATE: morphuntion/src/morphuntion/grammar/synthesis/GrammarSynthesizerFactory.cpp
* UPDATE: morphuntion/src/morphuntion/grammar/synthesis/fwd.hpp
* UPDATE: morphuntion/src/morphuntion/grammar/synthesis/fwd.cpp
* ADD: morphuntion/src/morphuntion/dialog/language/XxCommonConceptFactory.hpp
* ADD: morphuntion/src/morphuntion/dialog/language/XxCommonConceptFactory.hpp

## Define and create lexion
TODO: We need to describe how to get Wikidata lexicon and generate the files.