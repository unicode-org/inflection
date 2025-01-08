# How to add a new language support

Adding a new language requires multiple steps:
1. Adding support for a new locale
2. Adding inflection rules for the language
3. Adding lexicon for the language

The following steps with help you identify files that need to be added or changed, and will guide you in rule writing and lexicon preparation.

NOTE: Take a look at [PR #40](https://github.com/unicode-org/inflection/pull/40) for example on how to add initial language support.
In general, to bootstrap your progress look for grammatically similar language that's already supported, e.g. if you are adding Serbian look for existing Russian or Croatian implementation.
This will help you find most of the files you need to add/change and will speed up implementation of the rules and lexicons.

## Mark your language as supported
* UPDATE: morphuntion/src/morphuntion/util/LocaleUtils.hpp
* UPDATE: morphuntion/src/morphuntion/util/LocaleUtils.cpp
* UPDATE: morphuntion/test/src/morphuntion/util/LocaleUtilsTest.cpp
* UPDATE: morphuntion/resources/org/unicode/morphuntion/locale/supported-locales.properties
* ADD: morphuntion/resources/org/unicode/morphuntion/tokenizer/config_*XX*.properties

## Define language grammar
* UPDATE: morphuntion/resources/org/unicode/morphuntion/features/grammar.xml

## Add pronouns if applicable
* ADD: morphuntion/resources/org/unicode/morphuntion/inflection/pronoun_*XX*.csv

## Implement grammar synthesizer for inflecting words and retrieving grammatical properties
TODO: We need to expand what each of these do.
* ADD: morphuntion/src/morphuntion/grammar/synthesis/*Xx*GrammarSynthesizer.hpp
* ADD: morphuntion/src/morphuntion/grammar/synthesis/*Xx*GrammarSynthesizer.cpp
* ADD: morphuntion/src/morphuntion/grammar/synthesis/*Xx*GrammarSynthesizer_*Xx*DisplayFunction.hpp
* ADD: morphuntion/src/morphuntion/grammar/synthesis/*Xx*GrammarSynthesizer_*Xx*DisplayFunction.hpp
* UPDATE: morphuntion/src/morphuntion/grammar/synthesis/GrammarSynthesizerFactory.cpp
* UPDATE: morphuntion/src/morphuntion/grammar/synthesis/fwd.hpp

## Add language specific properties for lists, quantities and related topics
* ADD: morphuntion/src/morphuntion/dialog/language/*Xx*CommonConceptFactory.hpp
* ADD: morphuntion/src/morphuntion/dialog/language/*Xx*CommonConceptFactory.hpp
* UPDATE: morphuntion/src/morphuntion/dialog/language/fwd.hpp

## Define and create lexion
TODO: We need to describe how to get Wikidata lexicon and generate the files.

### Add data to test lexicon
Use our data driven testing approach to test your logic. You'll need to define a small lexicon that covers all test cases for inflection and pronouns (if applicable for the language).

* ADD: morphuntion/test/resources/morphuntion/dialog/inflection/Xx.xml
* ADD: morphuntion/test/resources/morphuntion/dialog/pronoun/Xx.xml
