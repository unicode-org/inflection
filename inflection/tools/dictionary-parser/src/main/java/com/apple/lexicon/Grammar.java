/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
package com.apple.lexicon;

import java.util.Arrays;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.Locale;

public final class Grammar {
    enum PartOfSpeech {
        ABBREVIATION,
        ADJECTIVE,
        ADVERB,
        CLASSIFIER,
        CONJUNCTION,
        DETERMINER,
        IGNORABLE,
        INTERJECTION,
        NOUN,
        NUMBER,
        //PARTICIPLE,
        PARTICLE,
        PREPOSITION,
        ADPOSITION,
        PROPER_NOUN,
        PRONOUN,
        VERB;

        private final String printableValue;

        PartOfSpeech() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum LemmaCategory {
        FAMILYNAME,
        APPLEPRODUCT;

        private final String printableValue;
        LemmaCategory() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }
        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Miscellaneous {
        LEMMA_WORD;

        private final String printableValue;
        Miscellaneous() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }
        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Count {
        SINGULAR,
        PLURAL,
        DUAL,
        COLLECTIVE;
        //FIXED//; // Both

        private final String printableValue;
        Count() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Emphasis {
        STRESSED,
        UNSTRESSED;

        private final String printableValue;
        Emphasis() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Gender {
        MASCULINE,
        FEMININE,
        NEUTER,
        COMMON; // Typical in Scandinavian languages. This is in contrast to neuter.

        private final String printableValue;
        Gender() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Tense {
        PAST,
        PRESENT,
        FUTURE;

        private final String printableValue;
        Tense() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Person {
        FIRST,
        SECOND,
        THIRD;

        private final String printableValue;
        Person() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Voice {
        ACTIVE,
        PASSIVE;

        private final String printableValue;
        Voice() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum VerbType {
        INFINITIVE,
        GERUND,
        PARTICIPLE;

        private final String printableValue;
        VerbType() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum AdjectiveType {
        PREDICATIVE,
        POSSESSIVE;

        private final String printableValue;
        AdjectiveType() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum DerivationType {
        PATRONYMIC;

        private final String printableValue;
        DerivationType() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum FormType {
        SHORT_FORM,
        IRREGULAR;

        private final String printableValue;
        FormType() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Polarity {
        NEGATIVE;

        private final String printableValue;
        Polarity() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum ComparisonDegree {
        PLAIN,
        COMPARATIVE,
        ABSOLUTE_SUPERLATIVE,
        SUPERLATIVE;
        private final String printableValue;
        ComparisonDegree() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum DeclensionClass {
        WEAK,
        MIXED,
        STRONG;
        private final String printableValue;
        DeclensionClass() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Mood {
        CONDITIONAL,
        INDICATIVE,
        IMPERATIVE,
        SUBJUNCTIVE,
        EMPHATIC;

        private final String printableValue;
        Mood() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Ignorable {
        IGNORABLE_LEMMA,
        IGNORABLE_INFLECTION,
        IGNORABLE_PROPERTY;

        private final String printableValue;
        Ignorable() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Aspect {
        IMPERFECTIVE,
        PERFECTIVE,
        PLUPERFECT;

        private final String printableValue;
        Aspect() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Alternate {
        SPELLING;

        private final String printableValue;
        Alternate() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Case {
        ABESSIVE,
        ABLATIVE,
        ACCUSATIVE,
        ADESSIVE,
        ALLATIVE,
        CAUSAL,
        CAUSATIVE,
        COMITATIVE,
        DATIVE,
        DELATIVE,
        DIRECT,
        ELATIVE,
        ERGATIVE,
        ESSIVE,
        GENITIVE,
        ILLATIVE,
        INESSIVE,
        INSTRUCTIVE,
        INSTRUMENTAL,
        LOCATIVE,
        NOMINATIVE,
        OBLIQUE,
        PARTITIVE,
        PREPOSITIONAL,
        SUBLATIVE,
        SEMBLATIVE,
        SUPERESSIVE,
        TERMINATIVE,
        TRANSLATIVE,
        VOCATIVE
        ;

        private final String printableValue;
        Case() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Animacy {
        ANIMATE,
        HUMAN,
        INANIMATE
        ;

        private final String printableValue;
        Animacy() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Definiteness {
        DEFINITE,
        INDEFINITE,
        DEMONSTRATIVE,
        CONSTRUCT,
        ;

        private final String printableValue;
        Definiteness() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Objectivity {
        DIRECT,
        INDIRECT,
        ;

        private final String printableValue;
        Objectivity() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Sizeness {
        AUGMENTATIVE,
        DIMINUTIVE,
        DOUBLE_DIMINUTIVE,
        ;

        private final String printableValue;
        Sizeness() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Sound {
        CONSONANT_START,
        CONSONANT_END,
        RIEUL_END,
        VOWEL_START,
        VOWEL_END,
        ;

        private final String printableValue;
        Sound() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Register {
        INFORMAL,
        INTIMATE,
        CONVERSATIONAL,
        HIGH,
        FORMAL,
        LITERARY;
        private final String printableValue;
        Register() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    public static TreeSet<String> asTreeSet(String... terms) {
        return new TreeSet<>(Arrays.asList(terms));
    }

    static final Map<TreeSet<String>, Set<? extends Enum<?>>> TYPEMAP = new HashMap<>(1021);
    static final Map<String, String> REMAP = new HashMap<>(1021);

    static {

        TYPEMAP.put(asTreeSet("abbreviation"), EnumSet.of(PartOfSpeech.ABBREVIATION));
        TYPEMAP.put(asTreeSet("adposition"), EnumSet.of(PartOfSpeech.ADPOSITION));
        TYPEMAP.put(asTreeSet("adjective"), EnumSet.of(PartOfSpeech.ADJECTIVE));
        TYPEMAP.put(asTreeSet("adverb"), EnumSet.of(PartOfSpeech.ADVERB));
        TYPEMAP.put(asTreeSet("classifier"), EnumSet.of(PartOfSpeech.CLASSIFIER));
        TYPEMAP.put(asTreeSet("conjunction"), EnumSet.of(PartOfSpeech.CONJUNCTION));
        TYPEMAP.put(asTreeSet("determiner"), EnumSet.of(PartOfSpeech.DETERMINER));
        TYPEMAP.put(asTreeSet("interjection"), EnumSet.of(PartOfSpeech.INTERJECTION));
        TYPEMAP.put(asTreeSet("letter"), EnumSet.of(Ignorable.IGNORABLE_LEMMA));
        TYPEMAP.put(asTreeSet("noun"), EnumSet.of(PartOfSpeech.NOUN));
        TYPEMAP.put(asTreeSet("numeral"), EnumSet.of(PartOfSpeech.NUMBER));
        TYPEMAP.put(asTreeSet("particle"), EnumSet.of(PartOfSpeech.PARTICLE));
        TYPEMAP.put(asTreeSet("preposition"), EnumSet.of(PartOfSpeech.PREPOSITION));
        TYPEMAP.put(asTreeSet("pronoun"), EnumSet.of(PartOfSpeech.PRONOUN));
        TYPEMAP.put(asTreeSet("proper-noun"), EnumSet.of(PartOfSpeech.PROPER_NOUN)); //Listing all the categories available in the lexicon as proper-noun
        TYPEMAP.put(asTreeSet("verb"), EnumSet.of(PartOfSpeech.VERB));

        TYPEMAP.put(asTreeSet("short-form"), EnumSet.of(FormType.SHORT_FORM));
        TYPEMAP.put(asTreeSet("irregular"), EnumSet.of(FormType.IRREGULAR));
        TYPEMAP.put(asTreeSet("negative"), EnumSet.of(Polarity.NEGATIVE));

        TYPEMAP.put(asTreeSet("plain"), EnumSet.of(ComparisonDegree.PLAIN));
        TYPEMAP.put(asTreeSet("comparative"), EnumSet.of(ComparisonDegree.COMPARATIVE));
        TYPEMAP.put(asTreeSet("superlative"), EnumSet.of(ComparisonDegree.SUPERLATIVE));
        TYPEMAP.put(asTreeSet("absolute-superlative"), EnumSet.of(ComparisonDegree.ABSOLUTE_SUPERLATIVE));

        TYPEMAP.put(asTreeSet("declension-weak"), EnumSet.of(DeclensionClass.WEAK));
        TYPEMAP.put(asTreeSet("declension-mixed"), EnumSet.of(DeclensionClass.MIXED));
        TYPEMAP.put(asTreeSet("declension-strong"), EnumSet.of(DeclensionClass.STRONG));

        TYPEMAP.put(asTreeSet("familyname"), EnumSet.of(LemmaCategory.FAMILYNAME));
        TYPEMAP.put(asTreeSet("appleproduct"), EnumSet.of(LemmaCategory.APPLEPRODUCT));

        TYPEMAP.put(asTreeSet("lemma_word"), EnumSet.of(Miscellaneous.LEMMA_WORD));

        TYPEMAP.put(asTreeSet("singular"), EnumSet.of(Count.SINGULAR));
        TYPEMAP.put(asTreeSet("plural"), EnumSet.of(Count.PLURAL));
        TYPEMAP.put(asTreeSet("dual"), EnumSet.of(Count.DUAL));
        TYPEMAP.put(asTreeSet("collective"), EnumSet.of(Count.COLLECTIVE));

        TYPEMAP.put(asTreeSet("stressed"), EnumSet.of(Emphasis.STRESSED));
        TYPEMAP.put(asTreeSet("unstressed"), EnumSet.of(Emphasis.UNSTRESSED));

        TYPEMAP.put(asTreeSet("any-gender"), EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put(asTreeSet("any-gender", "masculine"), EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put(asTreeSet("masculine"), EnumSet.of(Gender.MASCULINE));
        TYPEMAP.put(asTreeSet("feminine"), EnumSet.of(Gender.FEMININE));
        TYPEMAP.put(asTreeSet("neuter"), EnumSet.of(Gender.NEUTER));
        TYPEMAP.put(asTreeSet("common"), EnumSet.of(Gender.COMMON));
        TYPEMAP.put(asTreeSet("common", "neuter"), EnumSet.of(Gender.COMMON, Gender.NEUTER));

        TYPEMAP.put(asTreeSet("feminine", "masculine"), EnumSet.of(Gender.FEMININE, Gender.MASCULINE));
        TYPEMAP.put(asTreeSet("feminine", "neuter"), EnumSet.of(Gender.FEMININE, Gender.NEUTER));
        TYPEMAP.put(asTreeSet("masculine", "neuter"), EnumSet.of(Gender.MASCULINE, Gender.NEUTER));
        TYPEMAP.put(asTreeSet("feminine", "masculine", "neuter"), EnumSet.of(Gender.MASCULINE, Gender.FEMININE, Gender.NEUTER));

        TYPEMAP.put(asTreeSet("common", "feminine"), EnumSet.of(Gender.FEMININE, Gender.COMMON));
        TYPEMAP.put(asTreeSet("common", "masculine"), EnumSet.of(Gender.MASCULINE, Gender.COMMON));
        TYPEMAP.put(asTreeSet("common", "feminine", "masculine"), EnumSet.of(Gender.MASCULINE, Gender.FEMININE, Gender.COMMON));
        TYPEMAP.put(asTreeSet("common", "feminine", "neuter"), EnumSet.of(Gender.COMMON, Gender.FEMININE, Gender.NEUTER));
        TYPEMAP.put(asTreeSet("common", "masculine", "neuter"), EnumSet.of(Gender.COMMON, Gender.MASCULINE, Gender.NEUTER));
        TYPEMAP.put(asTreeSet("common", "feminine", "masculine", "neuter"), EnumSet.of(Gender.COMMON, Gender.FEMININE, Gender.MASCULINE, Gender.NEUTER));

        TYPEMAP.put(asTreeSet("abessive"), EnumSet.of(Case.ABESSIVE));
        TYPEMAP.put(asTreeSet("ablative"), EnumSet.of(Case.ABLATIVE));
        TYPEMAP.put(asTreeSet("accusative"), EnumSet.of(Case.ACCUSATIVE));
        TYPEMAP.put(asTreeSet("adessive"), EnumSet.of(Case.ADESSIVE));
        TYPEMAP.put(asTreeSet("allative"), EnumSet.of(Case.ALLATIVE));
        TYPEMAP.put(asTreeSet("causal-final"), EnumSet.of(Case.CAUSAL));
        TYPEMAP.put(asTreeSet("causative"), EnumSet.of(Case.CAUSATIVE));
        TYPEMAP.put(asTreeSet("comitative"), EnumSet.of(Case.COMITATIVE));
        TYPEMAP.put(asTreeSet("dative"), EnumSet.of(Case.DATIVE));
        TYPEMAP.put(asTreeSet("delative"), EnumSet.of(Case.DELATIVE));
        TYPEMAP.put(asTreeSet("direct"), EnumSet.of(Case.DIRECT));
        TYPEMAP.put(asTreeSet("elative"), EnumSet.of(Case.ELATIVE));
        TYPEMAP.put(asTreeSet("ergative"), EnumSet.of(Case.ERGATIVE));
        TYPEMAP.put(asTreeSet("essive"), EnumSet.of(Case.ESSIVE));
        TYPEMAP.put(asTreeSet("essive-formal"), EnumSet.of(Case.ESSIVE));
        TYPEMAP.put(asTreeSet("genitive"), EnumSet.of(Case.GENITIVE));
        TYPEMAP.put(asTreeSet("illative"), EnumSet.of(Case.ILLATIVE));
        TYPEMAP.put(asTreeSet("inessive"), EnumSet.of(Case.INESSIVE));
        TYPEMAP.put(asTreeSet("instructive"), EnumSet.of(Case.INSTRUCTIVE));
        TYPEMAP.put(asTreeSet("instrumental"), EnumSet.of(Case.INSTRUMENTAL));
        TYPEMAP.put(asTreeSet("locative"), EnumSet.of(Case.LOCATIVE));
        TYPEMAP.put(asTreeSet("nominative"), EnumSet.of(Case.NOMINATIVE));
        TYPEMAP.put(asTreeSet("object"), EnumSet.of(Case.OBLIQUE)); // AKA objective case for pronouns
        TYPEMAP.put(asTreeSet("oblique"), EnumSet.of(Case.OBLIQUE));
        TYPEMAP.put(asTreeSet("partitive"), EnumSet.of(Case.PARTITIVE));
        TYPEMAP.put(asTreeSet("prepositional"), EnumSet.of(Case.PREPOSITIONAL));
        TYPEMAP.put(asTreeSet("superessive"), EnumSet.of(Case.SUPERESSIVE));
        TYPEMAP.put(asTreeSet("semblative"), EnumSet.of(Case.SEMBLATIVE));
        TYPEMAP.put(asTreeSet("sublative"), EnumSet.of(Case.SUBLATIVE));
        TYPEMAP.put(asTreeSet("terminative"), EnumSet.of(Case.TERMINATIVE));
        TYPEMAP.put(asTreeSet("translative"), EnumSet.of(Case.TRANSLATIVE));
        TYPEMAP.put(asTreeSet("vocative"), EnumSet.of(Case.VOCATIVE));

        TYPEMAP.put(asTreeSet("animate"), EnumSet.of(Animacy.ANIMATE));
        TYPEMAP.put(asTreeSet("animate-human"), EnumSet.of(Animacy.HUMAN));
        TYPEMAP.put(asTreeSet("animate-non-human"), EnumSet.of(Animacy.ANIMATE));
        TYPEMAP.put(asTreeSet("inanimate"), EnumSet.of(Animacy.INANIMATE));
        TYPEMAP.put(asTreeSet("animate", "inanimate"), EnumSet.of(Animacy.ANIMATE, Animacy.INANIMATE));

        TYPEMAP.put(asTreeSet("definite"), EnumSet.of(Definiteness.DEFINITE));
        TYPEMAP.put(asTreeSet("compounding"), EnumSet.of(Definiteness.CONSTRUCT));
        TYPEMAP.put(asTreeSet("indefinite"), EnumSet.of(Definiteness.INDEFINITE));
        TYPEMAP.put(asTreeSet("demonstrative"), EnumSet.of(Definiteness.DEMONSTRATIVE));
        TYPEMAP.put(asTreeSet("determiner-interrogative"), EnumSet.of(Ignorable.IGNORABLE_PROPERTY));

        TYPEMAP.put(asTreeSet("present"), EnumSet.of(Tense.PRESENT)); // and usually future
        TYPEMAP.put(asTreeSet("past"), EnumSet.of(Tense.PAST));
        TYPEMAP.put(asTreeSet("future"), EnumSet.of(Tense.FUTURE));

        TYPEMAP.put(asTreeSet("first"), EnumSet.of(Person.FIRST));
        TYPEMAP.put(asTreeSet("second"), EnumSet.of(Person.SECOND));
        TYPEMAP.put(asTreeSet("third"), EnumSet.of(Person.THIRD));

        TYPEMAP.put(asTreeSet("infinitive"), EnumSet.of(VerbType.INFINITIVE));
        TYPEMAP.put(asTreeSet("gerund"), EnumSet.of(VerbType.GERUND));
        TYPEMAP.put(asTreeSet("participle"), EnumSet.of(VerbType.PARTICIPLE));

        TYPEMAP.put(asTreeSet("predicative"), EnumSet.of(AdjectiveType.PREDICATIVE));
        TYPEMAP.put(asTreeSet("possessive"), EnumSet.of(AdjectiveType.POSSESSIVE));

        TYPEMAP.put(asTreeSet("active"), EnumSet.of(Voice.ACTIVE));
        TYPEMAP.put(asTreeSet("passive"), EnumSet.of(Voice.PASSIVE));

        TYPEMAP.put(asTreeSet("direct-object-first"), new HashSet<>(Arrays.asList(Person.FIRST, Objectivity.DIRECT)));
        TYPEMAP.put(asTreeSet("direct-Object-second"), new HashSet<>(Arrays.asList(Person.SECOND, Objectivity.DIRECT)));
        TYPEMAP.put(asTreeSet("direct-object-third"), new HashSet<>(Arrays.asList(Person.THIRD, Objectivity.DIRECT)));

        TYPEMAP.put(asTreeSet("indirect-object"), EnumSet.of(Objectivity.INDIRECT));
        TYPEMAP.put(asTreeSet("direct-object"), EnumSet.of(Objectivity.DIRECT));

        TYPEMAP.put(asTreeSet("conditional"), EnumSet.of(Mood.CONDITIONAL));
        TYPEMAP.put(asTreeSet("indicative"), EnumSet.of(Mood.INDICATIVE));
        TYPEMAP.put(asTreeSet("imperative"), EnumSet.of(Mood.IMPERATIVE));
        TYPEMAP.put(asTreeSet("subjunctive"), EnumSet.of(Mood.SUBJUNCTIVE));
        TYPEMAP.put(asTreeSet("emphatic"), EnumSet.of(Mood.EMPHATIC));

        TYPEMAP.put(asTreeSet("perfect"), EnumSet.of(Aspect.PERFECTIVE));
        TYPEMAP.put(asTreeSet("imperfect"), EnumSet.of(Aspect.IMPERFECTIVE));
        TYPEMAP.put(asTreeSet("pluperfect"), EnumSet.of(Aspect.PLUPERFECT));

        TYPEMAP.put(asTreeSet("patronymic"), EnumSet.of(DerivationType.PATRONYMIC));

        TYPEMAP.put(asTreeSet("informal"), EnumSet.of(Register.INFORMAL));
        TYPEMAP.put(asTreeSet("formal"), EnumSet.of(Register.FORMAL));
        TYPEMAP.put(asTreeSet("intimate"), EnumSet.of(Register.INTIMATE));
        TYPEMAP.put(asTreeSet("high"), EnumSet.of(Register.HIGH));
        TYPEMAP.put(asTreeSet("conversational"), EnumSet.of(Register.CONVERSATIONAL));
        TYPEMAP.put(asTreeSet("literary"), EnumSet.of(Register.LITERARY));

        //TYPEMAP.put(asTreeSet(""), EnumSet.of(Alternate.SPELLING)); // It's like a versus an in English depending on the pronunciation properties of the word after the preposition.

        TYPEMAP.put(asTreeSet("foreign"), EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put(asTreeSet("Lemma"), EnumSet.of(Ignorable.IGNORABLE_LEMMA));

        TYPEMAP.put(asTreeSet("augmentative"), EnumSet.of(Sizeness.AUGMENTATIVE));
        TYPEMAP.put(asTreeSet("diminutive"), EnumSet.of(Sizeness.DIMINUTIVE));
        TYPEMAP.put(asTreeSet("double-diminutive"), EnumSet.of(Sizeness.DOUBLE_DIMINUTIVE));

        TYPEMAP.put(asTreeSet("consonant-end"), EnumSet.of(Sound.CONSONANT_END));
        TYPEMAP.put(asTreeSet("consonant-start"), EnumSet.of(Sound.CONSONANT_START));
        TYPEMAP.put(asTreeSet("rieul-end"), EnumSet.of(Sound.RIEUL_END));
        TYPEMAP.put(asTreeSet("vowel-end"), EnumSet.of(Sound.VOWEL_END));
        TYPEMAP.put(asTreeSet("vowel-start"), EnumSet.of(Sound.VOWEL_START));
    }

    static Set<? extends Enum<?>> getMappedGrammemes(String... grammemes) {
        TreeSet<String> remappedGrammemes = new TreeSet<>();
        for (String grammeme : grammemes) {
            grammeme = grammeme.toLowerCase(Locale.ENGLISH);
            remappedGrammemes.add(REMAP.getOrDefault(grammeme, grammeme));
        }

        return TYPEMAP.get(remappedGrammemes);
    }

}
