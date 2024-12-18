# How to add a new language support

Adding a new language requires multiple steps:
1. Adding support for a new locale
2. Adding inflection rules for the language
3. Adding lexicon for the language

The following steps with help you identify files that need to be added or changed, and will guide you in rule writing and lexicon preparation.

NOTE: To bootstrap your progress look for grammatically similar language that's already supported, e.g. if you are adding Serbian look for existing Russian implementation.
This will help you find most of the files you need to add/change and will speed up implementation of the rules and lexicons.

## Step 1
Add your language to the supported list in (try to follow alphabetical order):
* morphuntion/src/morphuntion/util/LocaleUtils.hpp
* morphuntion/src/morphuntion/util/LocaleUtils.cpp
