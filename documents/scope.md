# Scope and planning

## Stage 1

Estimated completion 2024.

Deliverable (based on points below):
1. FST rules for a set of languages are specified, using e.g. Pynini framwork.
2. C++ API for noun inflection is proposed and implemented using OpenFST library and above mentioned rules.
3. Lexicon is not optimized in any way, e.g. for size or lookup speed, and is based on CLDR date/time data. Further expansion would happen from Wikidata lexicon.

NOTE: The API could become part of ICU library. This is part of further discussion and it can impact language features or dependancies we are allowed to use, e.g. std library or exceptions.

Based on discussion from [2024-04-02](https://docs.google.com/document/d/1a-777RetR4k2gZKHJ5pG3apWKt86jV7-vU-QChX7lv4/edit?usp=sharing) our stage 1 plan is:

1. Focus on a small set of nouns, sourced from CLDR, e.g. month names, units etc to facilitate faster prototyping. See "Noun cases" section for details.
2. Gender and plural inflection can be handled by current message formatting logic, we can tackle them in later stages.
3. Adjectives are rarely used in UI strings, we can tackle them in later stages.
4. Wikidata can be pulled in parallel. We need to resolve licensing with Unicode legal council before committing any data to the repository.
5. We need to come up with a good lexicon format that is compact and fast to use.
6. Alignment between sentence parts, e.g. pronouns and other parts, is not in scope.

### Noun cases

Properly casing a noun includes handling its plural form. It also requires additional data, like gender, is noun animate/inanimate etc. We need to provide that data in the lexicon.

We can focus on smaller set on languages in the initial round. Those languages should have "modern/comprehensive" CLDR support and should present varius challenges to cover as many cases.
Given the languages we can handle in the current group, we could start with this list:
1. English
2. Spanish
3. Serbian
4. German
5. French

### Future needs

We do need to keep in mind wider requirements that would come in later stages, so lexicon structure and API surface should be expandable. For example:

1. Adding custom data and slicing existing data to preserve space.
2. Adding grammatical elements and information, e.g. adjectives, verbs.
3. Passing additional grammatical information into API methods.