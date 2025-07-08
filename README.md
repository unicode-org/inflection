# Inflection

Code, data and documentation solving language inflection problems.

## Introduction

[Inflection](https://en.wikipedia.org/wiki/Inflection) is the process of changing the form of a word to express different grammatical features, such as tense, number, gender, or case. In many languages, inflection is a complex and nuanced process, and it can be difficult to implement inflection correctly in software. This can lead to an inability to express native sounding sentences or to errors in text processing, such as incorrect word forms or incorrect grammatical agreement.

For speakers of non-inflected languages, it may be hard to see the importance of inflections. Here is an illustration from Serbian:

① Velike<sup>(The big)</sup> ② crvene<sup>(red)</sup> ③ jabuke<sup>(apples)</sup> ④ su pale<sup>(have fallen)</sup> sa<sup>(from)</sup> ⑤ drveta<sup>(the/a tree)</sup>.

* Adjectives ① and ② have to agree in number, gender and case with the noun ③
* Verb ④ (in the past tense) has to agree with the noun ③ in number and gender.
* Noun ⑤ needs the locative case; the nominative would be drvo.

The noun cases often correspond to using prepositions in English, so one way to appreciate how bad messages can appear to users in other languages is to consider English sentences with the wrong prepositions and incorrect agreement in number: “There is 3 item of your inbox” vs correct form “There are 3 items in your inbox”. 

## Problem Statement

The problem of inflection is particularly acute in languages that have a large number of inflectional forms, such as all Slavic and Indic languages, Arabic, Korean and Finnish, as well as many other languages, thus affecting a large number of users. In some languages (like Romance languages), inflection affects mostly common words - adjectives, nouns, verbs, but in many languages, proper nouns (like Geo-location names, Brand, People names) can also inflect. Industry so far has either avoided solving this problem or tried solving it for narrow use cases/language combinations. While it is particularly acute in those languages, languages such as French also need to inflect according to gender and number.

LLMs can craft natural-sounding sentences in many languages, including those mentioned above. They have the ability to generate and fill message formats for supported world languages, such as English. Additionally, LLMs can be used to create lexicons and inflection rules. However, they do have some drawbacks that won’t be solved soon - they are large and expensive to train and run, they induce latency for online services and often can’t fit on smaller devices without significant quality loss (if at all). They also depend on the quality of data they are trained on, so languages with less (or lower quality) available data may have insufficient quality.

## References

For more details refer to the [Proposal for a Unicode Language Inflection Work Group](https://docs.google.com/document/d/1YWGnMxnJWUC9MFl4j4l8fHeh83ikbpG9_1jCiDCrvIg/edit?usp=sharing) document.

See more details about how to use this library in [API Document](https://unicode-org.github.io/inflection).
### Copyright & Licenses

Copyright © 2024 Unicode, Inc. Unicode and the Unicode Logo are registered trademarks of Unicode, Inc. in the United States and other countries.

A CLA is required to contribute to this project - please refer to the [CONTRIBUTING.md](https://github.com/unicode-org/.github/blob/main/.github/CONTRIBUTING.md) file (or start a Pull Request) for more information.

The contents of this repository are governed by the Unicode [Terms of Use](https://www.unicode.org/copyright.html) and are released under [LICENSE](./LICENSE).
