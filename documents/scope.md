# Scope and planning

## Stage 1

Estimated completion 2024.

Based on discussion from [2024-04-02](https://docs.google.com/document/d/1a-777RetR4k2gZKHJ5pG3apWKt86jV7-vU-QChX7lv4/edit?usp=sharing) our stage 1 plan is:

1. Focus on a small set of nouns, sourced from CLDR, e.g. month names, units etc to facilitate faster prototyping.
2. Gender and plural inflection can be handled by current message formatting logic, we can tackle them in later stages.
3. Adjectives are rarely used in UI strings, we can tackle them in later stages.
4. Wikidata can be pulled in parallel. We need to resolve licensing with Unicode legal council before committing any data to the repository.
5. We need to come up with a good lexicon format that is compact and fast to use.
6. Alignment between sentence parts, e.g. pronouns and other parts, is not in scope.

We do need to keep in mind wider requirements that would come in later stages, so lexicon structure and API surface should be expandable. For example:

1. Adding custom data and slicing existing data to preserve space.
2. Adding grammatical elements and information, e.g. adjectives, verbs.
3. Passing additional grammatical information into API methods.