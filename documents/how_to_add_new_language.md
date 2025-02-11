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
* UPDATE: inflection/src/inflection/util/LocaleUtils.hpp
* UPDATE: inflection/src/inflection/util/LocaleUtils.cpp
* UPDATE: inflection/test/src/inflection/util/LocaleUtilsTest.cpp
* UPDATE: inflection/resources/org/unicode/inflection/locale/supported-locales.properties
* ADD: inflection/resources/org/unicode/inflection/tokenizer/config_*XX*.properties

## Define language grammar
* UPDATE: inflection/resources/org/unicode/inflection/features/grammar.xml

## Add pronouns if applicable
* ADD: inflection/resources/org/unicode/inflection/inflection/pronoun_*XX*.csv

## Implement grammar synthesizer for inflecting words and retrieving grammatical properties
TODO: We need to expand what each of these do.
* ADD: inflection/src/inflection/grammar/synthesis/*Xx*GrammarSynthesizer.hpp
* ADD: inflection/src/inflection/grammar/synthesis/*Xx*GrammarSynthesizer.cpp
* ADD: inflection/src/inflection/grammar/synthesis/*Xx*GrammarSynthesizer_*Xx*DisplayFunction.hpp
* ADD: inflection/src/inflection/grammar/synthesis/*Xx*GrammarSynthesizer_*Xx*DisplayFunction.hpp
* UPDATE: inflection/src/inflection/grammar/synthesis/GrammarSynthesizerFactory.cpp
* UPDATE: inflection/src/inflection/grammar/synthesis/fwd.hpp

## Add language specific properties for lists, quantities and related topics
* ADD: inflection/src/inflection/dialog/language/*Xx*CommonConceptFactory.hpp
* ADD: inflection/src/inflection/dialog/language/*Xx*CommonConceptFactory.hpp
* UPDATE: inflection/src/inflection/dialog/language/fwd.hpp

## Define and create lexion
There are a couple of ways of adding data, please follow [this](./add_lexicon_data.md) guide.

### Add data to test lexicon
Use our data driven testing approach to test your logic. You'll need to define a small lexicon that covers all test cases for inflection and pronouns (if applicable for the language).

* ADD: inflection/test/resources/inflection/dialog/inflection/Xx.xml
* ADD: inflection/test/resources/inflection/dialog/pronoun/Xx.xml
