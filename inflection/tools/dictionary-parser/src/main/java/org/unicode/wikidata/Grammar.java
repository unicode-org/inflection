/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.Arrays;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

/**
 * Contains the set of known grammemes (grammatical category values).
 * Typically, they are mapped from Wikidata Q (query) and P (property) entries to more readable wording.
 * Some of the Q entries can contain multiple grammemes, which is why a Q entry can have a set of grammemes to consider.
 * Some of these grammemes affect sorting or if we care to include them into the lexical dictionary.
 */
public final class Grammar {
    public enum PartOfSpeech {
        ARTICLE,
        ABBREVIATION,
        ADJECTIVE,
        ADVERB,
        CLASSIFIER,
        CONJUNCTION,
        DETERMINER,
        IGNORABLE,
        INTERJECTION,
        INTERROGATIVE,
        NOUN,
        NUMERAL,
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

    enum Count {
        COUNTABLE,
        UNCOUNTABLE,
        ;

        private final String printableValue;
        Count() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum Number {
        SINGULAR,
        PLURAL,
        DUAL,
        UNNUMBERED;
        //FIXED//; // Both

        private final String printableValue;
        Number() {
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
        COMMON, // Typical in Scandinavian languages. This is in contrast to neuter.
        NONVIRILE; // All genders except masculine personal or masculine human

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
        DISTANT_PAST,
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

    enum Transitivity {
        INTRANSITIVE,
        TRANSITIVE,
        ;

        private final String printableValue;
        Transitivity() {
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
        NONFINITE,
        GERUND,
        GERUNDIVE, // a verb form that functions as a verbal adjective
        PARTICIPLE,
        SUPINE,
        TRANSGRESSIVE,
        ;

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
        ATTRIBUTIVE,
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
        LONG_FORM;

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
        AFFIRMATIVE,
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
        POSITIVE,
        PLAIN,
        COMPARATIVE,
        SUPERLATIVE,
        EQUATIVE,
        ;
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
        EMPHATIC,
        IMPERATIVE,
        INDICATIVE,
        JUSSIVE,
        QUOTATIVE,
        SUBJUNCTIVE,
        VOLITIVE,
        ;

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
        HABITUAL,
        IMPERFECTIVE,
        IMPERFECT,
        PERFECT, // Not to be confused with Perfective aspect. See https://en.wikipedia.org/wiki/Perfect_(grammar)
        PERFECTIVE,
        PLUPERFECT,
        SIMPLE, // verb form which is underspecified for grammatical aspect
        ;

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
        ABSOLUTIVE,
        ACCUSATIVE,
        ADESSIVE,
        ALLATIVE,
        BENEFACTIVE,
        CAUSATIVE,
        COMITATIVE,
        DATIVE,
        DELATIVE,
        DESTINATIVE,
        DIRECT,
        DIRECTIVE,
        ELATIVE,
        ERGATIVE,
        ESSIVE,
        GENITIVE,
        ILLATIVE,
        INESSIVE,
        INSTRUCTIVE,
        INSTRUMENTAL,
        LOCATIVE,
        MOTIVATIVE,
        NOMINATIVE,
        OBLIQUE,
        PARTITIVE,
        PREPOSITIONAL,
        PROLATIVE,
        SOCIATIVE,
        SUBLATIVE,
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
        NONHUMAN,
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

    enum PronounType {
        PERSONAL,
        REFLEXIVE,
        ;

        private final String printableValue;
        PronounType() {
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
        BACK_ROUND,
        BACK_UNROUND,
        FRONT_ROUND,
        FRONT_UNROUND,
        HARD_CONSONANT,
        SOFT_CONSONANT,
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
        CONVERSATIONAL,
        FAMILIAR,
        FORMAL,
        HIGH,
        PEJORATIVE, // pejorative (or derogatory) A word form expressing a negative or belittling attitude towards the person or thing referred to
        INFORMAL,
        INTIMATE,
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

    enum Usage {
        STANDARD,
        RARE,
        ;
        private final String printableValue;
        Usage() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    enum WordOrder {
        SUBJECT_VERB,
        VERB_SUBJECT,
        ;
        private final String printableValue;
        WordOrder() {
            printableValue = super.toString().toLowerCase().replace('_', '-');
        }

        @Override
        public String toString() {
            return printableValue;
        }
    }

    static final Map<String, Enum<?>> DEFAULTMAP = new HashMap<>(1021);

    static {
        for (var enumClass : Grammar.class.getDeclaredClasses()) {
            for (var enumValue : enumClass.getEnumConstants()) {
                DEFAULTMAP.put(enumValue.toString(), (Enum<?>)enumValue);
            }
        }
    }

    static final Map<String, Set<? extends Enum<?>>> TYPEMAP = new HashMap<>(1021);

    static {

        // Consider the following for reference:
        // https://www.wikidata.org/wiki/Wikidata:Lexicographical_data/Statistics/Count_of_lexemes_by_lexical_category
        // https://www.wikidata.org/wiki/Wikidata:Lexicographical_data/Statistics/Count_of_forms_by_grammatical_feature

        TYPEMAP.put("Q103184", EnumSet.of(PartOfSpeech.ARTICLE));
        TYPEMAP.put("Q102786", EnumSet.of(PartOfSpeech.ABBREVIATION)); // abbreviation
        TYPEMAP.put("Q101244", EnumSet.of(PartOfSpeech.ABBREVIATION)); // acronym
        TYPEMAP.put("Q918270", EnumSet.of(PartOfSpeech.ABBREVIATION)); // initalism
        TYPEMAP.put("Q30619513", EnumSet.of(PartOfSpeech.ABBREVIATION)); // USPS abbreviation
        TYPEMAP.put("Q126473", EnumSet.of(PartOfSpeech.ABBREVIATION)); // contraction
        TYPEMAP.put("Q1130279", EnumSet.of(PartOfSpeech.ABBREVIATION)); // hypocorism, short nickname
        TYPEMAP.put("Q34698", EnumSet.of(PartOfSpeech.ADJECTIVE));
        TYPEMAP.put("Q12259986", EnumSet.of(PartOfSpeech.ADJECTIVE)); // prenominal adjective
        TYPEMAP.put("Q7233569", EnumSet.of(PartOfSpeech.ADJECTIVE)); // postpositive adjective
        TYPEMAP.put("Q11639843", EnumSet.of(PartOfSpeech.ADJECTIVE)); // adnominal adjective
        TYPEMAP.put("Q1091269", EnumSet.of(PartOfSpeech.ADJECTIVE)); // na-adjective in Japanese
        TYPEMAP.put("Q7250170", EnumSet.of(PartOfSpeech.ADJECTIVE)); // proper adjective, the adjective form of a proper noun
        TYPEMAP.put("Q332375", EnumSet.of(PartOfSpeech.ADJECTIVE)); // absolute adjective (uncomparable adjective)
        TYPEMAP.put("Q3618903", EnumSet.of(PartOfSpeech.ADJECTIVE)); // indefinite adjective
        TYPEMAP.put("Q380057", EnumSet.of(PartOfSpeech.ADVERB));
        TYPEMAP.put("Q1668170", EnumSet.of(PartOfSpeech.INTERROGATIVE, PartOfSpeech.ADVERB));
        TYPEMAP.put("Q1522423", EnumSet.of(PartOfSpeech.ADVERB)); // locative adverb, but we don't need that much precision about the type.
        TYPEMAP.put("Q1941737", EnumSet.of(PartOfSpeech.ADVERB)); // modal adverb, but we don't need that much precision about the type.
        TYPEMAP.put("Q1930668", EnumSet.of(PartOfSpeech.ADVERB)); // parenthesis, explanatory or qualifying word, clause, or sentence inserted into a passage
        TYPEMAP.put("Q63153", EnumSet.of(PartOfSpeech.CLASSIFIER)); // classifier, count word, measure word
        TYPEMAP.put("Q36484", EnumSet.of(PartOfSpeech.CONJUNCTION));
        TYPEMAP.put("Q12262560", EnumSet.of(PartOfSpeech.CONJUNCTION)); // adversative linker
        TYPEMAP.put("Q12564489", EnumSet.of(PartOfSpeech.CONJUNCTION)); // disjunctive conjunction, which we don't need to differentiate.
        TYPEMAP.put("Q55965516", EnumSet.of(PartOfSpeech.CONJUNCTION)); // alias of disjunctive conjunction, which we don't need to differentiate.
        TYPEMAP.put("Q11655558", EnumSet.of(PartOfSpeech.CONJUNCTION)); // subordinating conjunction
        TYPEMAP.put("Q28833099", EnumSet.of(PartOfSpeech.CONJUNCTION)); // coordinating conjunction
        TYPEMAP.put("Q576271", EnumSet.of(PartOfSpeech.DETERMINER));
        TYPEMAP.put("Q5051", new HashSet<>(Arrays.asList(Case.GENITIVE, PartOfSpeech.DETERMINER))); // possessive determiner
        TYPEMAP.put("Q2824480", EnumSet.of(PartOfSpeech.DETERMINER)); // demonstrative adjective, but it's really a determiner.
        TYPEMAP.put("Q2112896", EnumSet.of(PartOfSpeech.DETERMINER)); // pronominal adverb
        TYPEMAP.put("Q83034", EnumSet.of(PartOfSpeech.INTERJECTION));
        TYPEMAP.put("Q2304610", EnumSet.of(PartOfSpeech.INTERROGATIVE));
        TYPEMAP.put("Q12021746", EnumSet.of(PartOfSpeech.INTERROGATIVE));
        TYPEMAP.put("Q54310231", EnumSet.of(PartOfSpeech.INTERROGATIVE, PartOfSpeech.PRONOUN));
        TYPEMAP.put("Q60798917", EnumSet.of(PartOfSpeech.INTERROGATIVE)); // question tag
        TYPEMAP.put("Q9788", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // letter
        TYPEMAP.put("Q3241972", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // character
        TYPEMAP.put("Q1084", EnumSet.of(PartOfSpeech.NOUN));
        TYPEMAP.put("Q1787727", EnumSet.of(PartOfSpeech.NOUN)); // agent noun
        TYPEMAP.put("Q2428747", EnumSet.of(PartOfSpeech.NOUN)); // common noun
        TYPEMAP.put("Q63116", EnumSet.of(PartOfSpeech.NUMERAL));
        TYPEMAP.put("Q1329258", EnumSet.of(PartOfSpeech.NUMERAL)); // cardinal numeral
        TYPEMAP.put("Q191780", EnumSet.of(PartOfSpeech.NUMERAL)); // ordinal
        TYPEMAP.put("Q55951821", EnumSet.of(PartOfSpeech.NUMERAL)); // numeral adjective
        TYPEMAP.put("Q11563", EnumSet.of(PartOfSpeech.NUMERAL)); // number
        TYPEMAP.put("Q25098893", EnumSet.of(PartOfSpeech.NUMERAL)); // distributive numeral
        TYPEMAP.put("Q115526488", EnumSet.of(PartOfSpeech.NUMERAL)); // singulative numeral
        TYPEMAP.put("Q38918", EnumSet.of(PartOfSpeech.NUMERAL)); // Roman numerals
        TYPEMAP.put("Q55093533", EnumSet.of(PartOfSpeech.NUMERAL)); // collective numeral
        TYPEMAP.put("Q184943", EnumSet.of(PartOfSpeech.PARTICLE));
        TYPEMAP.put("Q1480213", EnumSet.of(PartOfSpeech.PARTICLE)); // Japanese particle
        TYPEMAP.put("Q10535365", EnumSet.of(PartOfSpeech.PARTICLE)); // infinitive marker, infinitive participle, infinitive particle
        TYPEMAP.put("Q113198319", new HashSet<>(Arrays.asList(PartOfSpeech.ADVERB, PartOfSpeech.PARTICLE))); // adverbial particle
        TYPEMAP.put("Q115762248", EnumSet.of(PartOfSpeech.PARTICLE)); // vocative particle
        TYPEMAP.put("Q115475265", EnumSet.of(PartOfSpeech.PARTICLE)); // honorific particle
        TYPEMAP.put("Q113076880", EnumSet.of(PartOfSpeech.ADVERB)); // postpositive adverb
        TYPEMAP.put("Q65807752", EnumSet.of(PartOfSpeech.ADVERB)); // demonstrative adverb
        TYPEMAP.put("Q117321826", EnumSet.of(PartOfSpeech.ADVERB)); // localiser, similar to an adverb
        TYPEMAP.put("Q134316", EnumSet.of(PartOfSpeech.ADPOSITION)); // adposition
        TYPEMAP.put("Q161873", EnumSet.of(PartOfSpeech.ADPOSITION)); // postposition
        TYPEMAP.put("Q4833830", EnumSet.of(PartOfSpeech.ADPOSITION)); // preposition
        TYPEMAP.put("Q36224", EnumSet.of(PartOfSpeech.PRONOUN));
        TYPEMAP.put("Q2006180", EnumSet.of(PartOfSpeech.PRONOUN)); // pro-form, word that substitutes for another word, broader scope than pronoun
        TYPEMAP.put("Q115272253", EnumSet.of(PartOfSpeech.PRONOUN)); // possessive adjective, like "your"
        TYPEMAP.put("Q2824485", EnumSet.of(PartOfSpeech.PRONOUN)); // pronominal adjective
        TYPEMAP.put("Q115272205", EnumSet.of(PartOfSpeech.PRONOUN)); // reflexive adjective
        TYPEMAP.put("Q79377411", EnumSet.of(PartOfSpeech.PRONOUN)); // demonstrative pronoun
        TYPEMAP.put("Q147276", EnumSet.of(PartOfSpeech.PROPER_NOUN)); // proper noun
        TYPEMAP.put("Q7884789", EnumSet.of(PartOfSpeech.PROPER_NOUN)); // toponym
        TYPEMAP.put("Q43229", EnumSet.of(PartOfSpeech.PROPER_NOUN)); // organization
        TYPEMAP.put("Q217438", EnumSet.of(PartOfSpeech.PROPER_NOUN)); // demonym
        TYPEMAP.put("Q81058955", EnumSet.of(PartOfSpeech.PROPER_NOUN)); // national demonym
        TYPEMAP.put("Q125546349", EnumSet.of(PartOfSpeech.PROPER_NOUN)); // some sort of name
        TYPEMAP.put("Q24905", EnumSet.of(PartOfSpeech.VERB));
//         TYPEMAP.put("Q1350145", EnumSet.of(PartOfSpeech.VERB, PartOfSpeech.NOUN)); // verbal noun, like boxing
        TYPEMAP.put("Q11399805", EnumSet.of(PartOfSpeech.VERB)); // auxiliary verb
        TYPEMAP.put("Q131431824", EnumSet.of(PartOfSpeech.VERB)); // proper verb where you use a proper noun as a verb
        TYPEMAP.put("Q3254028", EnumSet.of(PartOfSpeech.VERB)); // separable verb, verb with a prefix which separates from the core verb in certain positions in a sentence

        TYPEMAP.put("Q4239848", new HashSet<>(Arrays.asList(FormType.SHORT_FORM, PartOfSpeech.ADJECTIVE))); // short form of an adjective
        TYPEMAP.put("Q96406487", EnumSet.of(FormType.SHORT_FORM)); // short form
        TYPEMAP.put("Q112154", EnumSet.of(FormType.SHORT_FORM)); // apocope, loss of word-final sounds
        TYPEMAP.put("Q650250", EnumSet.of(FormType.SHORT_FORM)); // elision, omission of one or more sounds in a word
        TYPEMAP.put("Q114092330", EnumSet.of(FormType.SHORT_FORM)); // prevocalic form, linguistic feature marking a linguistic unit as appearing only before vowels
        TYPEMAP.put("Q96406455", EnumSet.of(FormType.LONG_FORM)); // long form

        TYPEMAP.put("Q109267112", EnumSet.of(Polarity.AFFIRMATIVE));
        TYPEMAP.put("Q1478451", EnumSet.of(Polarity.NEGATIVE));
        TYPEMAP.put("Q3745428", EnumSet.of(Polarity.AFFIRMATIVE, Polarity.NEGATIVE));
        TYPEMAP.put("Q64012400", new HashSet<>(Arrays.asList(Mood.IMPERATIVE, Polarity.AFFIRMATIVE))); // imperative affirmative
        TYPEMAP.put("Q64004115", new HashSet<>(Arrays.asList(Mood.IMPERATIVE, Polarity.NEGATIVE))); // negative imperative

        TYPEMAP.put("Q3482678", EnumSet.of(ComparisonDegree.POSITIVE));
        TYPEMAP.put("Q5384239", EnumSet.of(ComparisonDegree.EQUATIVE));
        TYPEMAP.put("plain", EnumSet.of(ComparisonDegree.PLAIN));
//        TYPEMAP.put(asTreeSet("Q15737187"), EnumSet.of(ComparisonDegree.NEGATIVE));
        TYPEMAP.put("Q14169499", EnumSet.of(ComparisonDegree.COMPARATIVE));
        TYPEMAP.put("Q65248385", new HashSet<>(Arrays.asList(PartOfSpeech.ADVERB, ComparisonDegree.COMPARATIVE))); // comparative adverb
        TYPEMAP.put("Q1817208", EnumSet.of(ComparisonDegree.SUPERLATIVE));
        TYPEMAP.put("Q65248545", new HashSet<>(Arrays.asList(PartOfSpeech.ADVERB, ComparisonDegree.SUPERLATIVE))); // superlative adverb

        TYPEMAP.put("Q7977953", EnumSet.of(DeclensionClass.WEAK));
        TYPEMAP.put("Q96407524", EnumSet.of(DeclensionClass.MIXED));
        TYPEMAP.put("Q3481903", EnumSet.of(DeclensionClass.STRONG));

        TYPEMAP.put("Q110786", EnumSet.of(Number.SINGULAR));
        TYPEMAP.put("Q604984", EnumSet.of(Number.SINGULAR)); // singulare tantum, the lack of a plural form in the inflection table conveys the same information.
        TYPEMAP.put("Q146786", EnumSet.of(Number.PLURAL));
        TYPEMAP.put("Q138246", EnumSet.of(Number.PLURAL)); // plurale tantum, the lack of a singular form in the inflection table conveys the same information.
        TYPEMAP.put("Q118533730", EnumSet.of(Number.PLURAL)); // substantive plural
        TYPEMAP.put("Q20386151", EnumSet.of(Number.PLURAL)); // sound plural, whole plural, or regular plural, which is in contrast to a broken plural or loan plural.
        TYPEMAP.put("Q1098772", new HashSet<>(Arrays.asList(Usage.RARE, Number.PLURAL))); // broken plural
        TYPEMAP.put("Q126002589", new HashSet<>(Arrays.asList(Usage.RARE, Number.PLURAL))); // loan plural
        TYPEMAP.put("Q110022", EnumSet.of(Number.DUAL));

        TYPEMAP.put("Q1520033", EnumSet.of(Count.COUNTABLE)); // count noun
        TYPEMAP.put("Q489168", EnumSet.of(Count.UNCOUNTABLE)); // mass noun
        TYPEMAP.put("Q53998049", EnumSet.of(Count.UNCOUNTABLE)); // indefinite number, neither singular nor plural, uncountable. Unmarked appears in declension when it is not necessary to specify singular or plural, such as because it is a proper name or is next to a determiner or a quantifier.

        TYPEMAP.put("stressed", EnumSet.of(Emphasis.STRESSED));
        TYPEMAP.put("Q55464002", EnumSet.of(Emphasis.STRESSED)); // strong form
        TYPEMAP.put("unstressed", EnumSet.of(Emphasis.UNSTRESSED));
        TYPEMAP.put("Q55464014", EnumSet.of(Emphasis.UNSTRESSED)); // weak form

        TYPEMAP.put("Q499327", EnumSet.of(Gender.MASCULINE));
        TYPEMAP.put("Q54020116", new HashSet<>(Arrays.asList(Gender.MASCULINE, Animacy.ANIMATE)));
        TYPEMAP.put("Q52943434", new HashSet<>(Arrays.asList(Gender.MASCULINE, Animacy.INANIMATE)));
        TYPEMAP.put("Q27918551", new HashSet<>(Arrays.asList(Gender.MASCULINE, Animacy.HUMAN))); // masculine personal
        TYPEMAP.put("Q52943193", new HashSet<>(Arrays.asList(Gender.MASCULINE, Animacy.ANIMATE))); // masculine animate non-personal
        TYPEMAP.put("Q18478758", new HashSet<>(Arrays.asList(Gender.MASCULINE, Gender.FEMININE))); // common of two genders
        TYPEMAP.put("Q100919075", new HashSet<>(Arrays.asList(Gender.MASCULINE, Gender.FEMININE))); // ambiguous gender
        TYPEMAP.put("Q132672407", new HashSet<>(Arrays.asList(Gender.MASCULINE, Gender.NEUTER))); // Dutch m/n gender
        TYPEMAP.put("Q133259189", new HashSet<>(Arrays.asList(Gender.MASCULINE, Gender.FEMININE, Gender.NEUTER))); // Dutch 'm/f/n'
        TYPEMAP.put("Q1775415", EnumSet.of(Gender.FEMININE));
        TYPEMAP.put("Q1775461", EnumSet.of(Gender.NEUTER));
        TYPEMAP.put("Q1305037", EnumSet.of(Gender.COMMON));
        TYPEMAP.put("Q54152717", EnumSet.of(Gender.NONVIRILE));

        TYPEMAP.put("Q319822", EnumSet.of(Case.ABESSIVE));
        TYPEMAP.put("Q156986", EnumSet.of(Case.ABLATIVE));
        TYPEMAP.put("Q332734", EnumSet.of(Case.ABSOLUTIVE));
        TYPEMAP.put("Q146078", EnumSet.of(Case.ACCUSATIVE));
        TYPEMAP.put("Q420020", EnumSet.of(Case.ACCUSATIVE)); // object case, but it's a synonym of the formal accusative name.
        TYPEMAP.put("Q281954", EnumSet.of(Case.ADESSIVE));
        TYPEMAP.put("Q655020", EnumSet.of(Case.ALLATIVE));
        TYPEMAP.put("Q664905", EnumSet.of(Case.BENEFACTIVE));
        TYPEMAP.put("Q113330743", EnumSet.of(Case.CAUSATIVE));
        TYPEMAP.put("Q838581", EnumSet.of(Case.COMITATIVE));
        TYPEMAP.put("Q145599", EnumSet.of(Case.DATIVE));
        TYPEMAP.put("Q1183901", EnumSet.of(Case.DELATIVE));
        TYPEMAP.put("Q55862897", EnumSet.of(Case.DESTINATIVE, Case.ALLATIVE));
        TYPEMAP.put("Q1751855", EnumSet.of(Case.DIRECT));
        TYPEMAP.put("Q55862884", EnumSet.of(Case.DIRECTIVE, Case.ALLATIVE));
        TYPEMAP.put("Q394253", EnumSet.of(Case.ELATIVE));
        TYPEMAP.put("Q324305", EnumSet.of(Case.ERGATIVE));
        TYPEMAP.put("Q148465", EnumSet.of(Case.ESSIVE));
        TYPEMAP.put("Q146233", EnumSet.of(Case.GENITIVE));
        TYPEMAP.put("Q474668", EnumSet.of(Case.ILLATIVE));
        TYPEMAP.put("Q65262551", new HashSet<>(Arrays.asList(FormType.SHORT_FORM, Case.ILLATIVE))); // short illative or perhaps aditive
        TYPEMAP.put("Q282031", EnumSet.of(Case.INESSIVE));
        TYPEMAP.put("Q1665275", EnumSet.of(Case.INSTRUCTIVE));
        TYPEMAP.put("Q192997", EnumSet.of(Case.INSTRUMENTAL));
        TYPEMAP.put("Q202142", EnumSet.of(Case.LOCATIVE));
        TYPEMAP.put("Q1949687", EnumSet.of(Case.MOTIVATIVE));
        TYPEMAP.put("Q131105", EnumSet.of(Case.NOMINATIVE));
        TYPEMAP.put("Q98946930", EnumSet.of(Case.NOMINATIVE)); // non-genitive
        TYPEMAP.put("Q1233197", EnumSet.of(Case.OBLIQUE));
        TYPEMAP.put("Q857325", EnumSet.of(Case.PARTITIVE));
        TYPEMAP.put("Q2114906", EnumSet.of(Case.PREPOSITIONAL));
        TYPEMAP.put("Q952933", EnumSet.of(Case.PROLATIVE));
        TYPEMAP.put("Q3773161", EnumSet.of(Case.SOCIATIVE));
        TYPEMAP.put("Q222355", EnumSet.of(Case.SUPERESSIVE));
        TYPEMAP.put("Q2120615", EnumSet.of(Case.SUBLATIVE));
        TYPEMAP.put("Q747019", EnumSet.of(Case.TERMINATIVE));
        TYPEMAP.put("Q55862800", EnumSet.of(Case.TERMINATIVE, Case.ALLATIVE));
        TYPEMAP.put("Q950170", EnumSet.of(Case.TRANSLATIVE));
        TYPEMAP.put("Q185077", EnumSet.of(Case.VOCATIVE));

        TYPEMAP.put("Q51927507", EnumSet.of(Animacy.ANIMATE));
        TYPEMAP.put("Q67372736", EnumSet.of(Animacy.HUMAN));
        TYPEMAP.put("Q67372837", EnumSet.of(Animacy.NONHUMAN));
        TYPEMAP.put("Q51927539", EnumSet.of(Animacy.INANIMATE));

        TYPEMAP.put("Q53997851", EnumSet.of(Definiteness.DEFINITE));
        TYPEMAP.put("Q2865743", new HashSet<>(Arrays.asList(Definiteness.DEFINITE, PartOfSpeech.ARTICLE)));
        TYPEMAP.put("Q1641446", EnumSet.of(Definiteness.CONSTRUCT));
        TYPEMAP.put("Q53997857", EnumSet.of(Definiteness.INDEFINITE)); // indefinite
        TYPEMAP.put("Q3813849", new HashSet<>(Arrays.asList(Definiteness.INDEFINITE, PartOfSpeech.ARTICLE)));
        TYPEMAP.put("Q956030", new HashSet<>(Arrays.asList(Definiteness.INDEFINITE, PartOfSpeech.PRONOUN)));
//        TYPEMAP.put(asTreeSet("Q53998049"), EnumSet.of(Definiteness.INDEFINITE)); // indefinite number
        TYPEMAP.put("Q10265745", new HashSet<>(Arrays.asList(Definiteness.DEMONSTRATIVE, PartOfSpeech.DETERMINER))); // demonstrative determiner
        TYPEMAP.put("Q79377486", new HashSet<>(Arrays.asList(Definiteness.DEMONSTRATIVE, PartOfSpeech.DETERMINER))); // distal, demonstrative

        TYPEMAP.put("Q10345583", new HashSet<>(Arrays.asList(Tense.PRESENT, VerbType.PARTICIPLE)));
        TYPEMAP.put("Q1230649", new HashSet<>(Arrays.asList(Tense.PAST, VerbType.PARTICIPLE)));
        TYPEMAP.put("Q72249355", new HashSet<>(Arrays.asList(Voice.ACTIVE, VerbType.PARTICIPLE)));
        TYPEMAP.put("Q72249544", new HashSet<>(Arrays.asList(Voice.PASSIVE, VerbType.PARTICIPLE)));
        TYPEMAP.put("Q430255", new HashSet<>(Arrays.asList(Tense.PRESENT, Voice.ACTIVE, VerbType.PARTICIPLE))); // present active participle
        TYPEMAP.put("Q117824585", new HashSet<>(Arrays.asList(Tense.PRESENT, Voice.PASSIVE, VerbType.PARTICIPLE))); // present active participle
        TYPEMAP.put("Q16086106", new HashSet<>(Arrays.asList(Tense.PAST, Voice.PASSIVE, VerbType.PARTICIPLE))); // past active participle
        TYPEMAP.put("Q113133303", EnumSet.of(VerbType.PARTICIPLE)); // conjunctive participle
        TYPEMAP.put("Q192613", EnumSet.of(Tense.PRESENT)); // present tense
        TYPEMAP.put("Q12230930", EnumSet.of(Tense.PRESENT)); // fi'il muḍāri', which is a non-past verb feature, but the future tense is usually formed by adding a prefix or word before the present tense verb.
        TYPEMAP.put("Q3910936", new HashSet<>(Arrays.asList(Aspect.SIMPLE, Tense.PRESENT))); // simple present and usually future
        TYPEMAP.put("Q1994301", EnumSet.of(Tense.PAST)); // past tense
        TYPEMAP.put("Q1392475", new HashSet<>(Arrays.asList(Aspect.SIMPLE, Tense.PAST))); // simple past
        TYPEMAP.put("Q113326559", EnumSet.of(Tense.PAST)); // non-remote tense
        TYPEMAP.put("Q113326099", EnumSet.of(Tense.DISTANT_PAST)); // remote tense
        TYPEMAP.put("Q501405", EnumSet.of(Tense.FUTURE)); // future tense
        TYPEMAP.put("Q344", EnumSet.of(Tense.FUTURE)); // future
        TYPEMAP.put("Q1475560", new HashSet<>(Arrays.asList(Aspect.SIMPLE, Tense.FUTURE))); // simple future
        TYPEMAP.put("Q96323395", new HashSet<>(Arrays.asList(Aspect.SIMPLE, Tense.FUTURE))); // simple future

        TYPEMAP.put("Q21714344", EnumSet.of(Person.FIRST));
        TYPEMAP.put("Q51929049", EnumSet.of(Person.SECOND));
        TYPEMAP.put("Q51929074", EnumSet.of(Person.THIRD));
        TYPEMAP.put("Q88778575", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // zero person

        TYPEMAP.put("Q179230", EnumSet.of(VerbType.INFINITIVE));
        TYPEMAP.put("Q106236131", EnumSet.of(VerbType.INFINITIVE)); // personal infinitive. It could be first or third person.
        TYPEMAP.put("Q52434245", new HashSet<>(Arrays.asList(Tense.PRESENT, VerbType.INFINITIVE))); // present infinitive
        TYPEMAP.put("Q52434302", new HashSet<>(Arrays.asList(Tense.PAST, VerbType.INFINITIVE))); // past infinitive
        TYPEMAP.put("Q115223950", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // infinitive form in Norwegian Nynorsk that ends in 'a'
        TYPEMAP.put("Q115223951", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // infinitive form in Norwegian Nynorsk that ends in 'e'
        TYPEMAP.put("Q1923028", EnumSet.of(VerbType.GERUND));
        TYPEMAP.put("Q380012", EnumSet.of(VerbType.GERUND)); // adverbial
        TYPEMAP.put("Q904896", EnumSet.of(VerbType.GERUND)); // transgressive, adverbial participle
        TYPEMAP.put("Q52434511", new HashSet<>(Arrays.asList(Tense.PRESENT, VerbType.GERUND)));
        TYPEMAP.put("Q52434598", new HashSet<>(Arrays.asList(Tense.PAST, VerbType.GERUND)));
        TYPEMAP.put("Q1050494", EnumSet.of(VerbType.NONFINITE));
        TYPEMAP.put("Q731298", EnumSet.of(VerbType.GERUNDIVE));
        TYPEMAP.put("Q65540125", new HashSet<>(Arrays.asList(Tense.PRESENT, VerbType.TRANSGRESSIVE))); // present infinitive, or participle
        TYPEMAP.put("Q12750232", new HashSet<>(Arrays.asList(Tense.PAST, VerbType.TRANSGRESSIVE))); // past infinitive, or participle
        TYPEMAP.put("Q814722", EnumSet.of(VerbType.PARTICIPLE));
        TYPEMAP.put("Q548470", EnumSet.of(VerbType.SUPINE));
        TYPEMAP.put("Q12717679", new HashSet<>(Arrays.asList(Tense.PAST, VerbType.PARTICIPLE)));

        TYPEMAP.put("Q4818723", EnumSet.of(AdjectiveType.ATTRIBUTIVE));
        TYPEMAP.put("Q1931259", EnumSet.of(AdjectiveType.PREDICATIVE));

        TYPEMAP.put("Q1774805", EnumSet.of(Transitivity.TRANSITIVE)); // transitive verb
        TYPEMAP.put("Q113330733", EnumSet.of(Transitivity.TRANSITIVE)); // transitive phase
        TYPEMAP.put("Q113330736", EnumSet.of(Transitivity.INTRANSITIVE));

        TYPEMAP.put("Q1317831", EnumSet.of(Voice.ACTIVE));
        TYPEMAP.put("Q1194697", EnumSet.of(Voice.PASSIVE));
        TYPEMAP.put("Q1509829", EnumSet.of(Voice.PASSIVE)); // impersonal verb, verb that has no determinate subject
        TYPEMAP.put("Q64003131", new HashSet<>(Arrays.asList(Voice.PASSIVE, VerbType.INFINITIVE)));

        TYPEMAP.put("Q468801", EnumSet.of(PartOfSpeech.PRONOUN)); // personal pronoun
        TYPEMAP.put("Q1502460", EnumSet.of(PartOfSpeech.PRONOUN)); // possessive pronoun
        TYPEMAP.put("Q34793275", EnumSet.of(PartOfSpeech.PRONOUN)); // demonstrative pronoun
        TYPEMAP.put("Q953129", EnumSet.of(PartOfSpeech.PRONOUN)); // reflexive pronoun
        TYPEMAP.put("Q130266209", EnumSet.of(PartOfSpeech.PRONOUN)); // reflexive personal pronoun
        TYPEMAP.put("Q1050744", EnumSet.of(PartOfSpeech.PRONOUN)); // relative pronoun
        TYPEMAP.put("Q1462657", EnumSet.of(PartOfSpeech.PRONOUN)); // reciprocal pronoun

        TYPEMAP.put("Q625581", EnumSet.of(Mood.CONDITIONAL));
        TYPEMAP.put("Q3686414", new HashSet<>(Arrays.asList(Tense.PRESENT, Mood.CONDITIONAL))); // conditional present
        TYPEMAP.put("Q65211247", new HashSet<>(Arrays.asList(Aspect.SIMPLE, Mood.CONDITIONAL))); // simple conditional
        TYPEMAP.put("Q7272884", EnumSet.of(Mood.QUOTATIVE));
        TYPEMAP.put("Q22716", EnumSet.of(Mood.IMPERATIVE));
        TYPEMAP.put("Q4348304", EnumSet.of(Mood.IMPERATIVE)); // imperative form for Japanese verb
        TYPEMAP.put("Q1665268", EnumSet.of(Mood.IMPERATIVE)); // command
        TYPEMAP.put("Q52434162", new HashSet<>(Arrays.asList(Tense.PRESENT, Mood.IMPERATIVE))); // present imperative
        TYPEMAP.put("Q682111", EnumSet.of(Mood.INDICATIVE));
        TYPEMAP.put("Q63997439", new HashSet<>(Arrays.asList(Tense.FUTURE, Mood.INDICATIVE)));
        TYPEMAP.put("Q56682909", new HashSet<>(Arrays.asList(Tense.PRESENT, Mood.INDICATIVE)));
        TYPEMAP.put("Q125286251", new HashSet<>(Arrays.asList(Tense.PAST, Mood.INDICATIVE)));
        TYPEMAP.put("Q462367", EnumSet.of(Mood.JUSSIVE));
        TYPEMAP.put("Q473746", EnumSet.of(Mood.SUBJUNCTIVE));
        TYPEMAP.put("Q3502553", new HashSet<>(Arrays.asList(Tense.PRESENT, Mood.SUBJUNCTIVE))); // present subjunctive
        TYPEMAP.put("Q55685962", new HashSet<>(Arrays.asList(Tense.PRESENT, Mood.SUBJUNCTIVE))); // subjunctive I, present subjunctive
        TYPEMAP.put("Q54671845", new HashSet<>(Arrays.asList(Tense.PAST, Mood.SUBJUNCTIVE))); // subjunctive II, past subjunctive
        TYPEMAP.put("Q3502544", new HashSet<>(Arrays.asList(Tense.PAST, Mood.SUBJUNCTIVE))); // past subjunctive
        TYPEMAP.put("Q3502541", new HashSet<>(Arrays.asList(Aspect.IMPERFECT, Tense.PAST, Mood.SUBJUNCTIVE))); // imperfect subjunctive
        TYPEMAP.put("Q113289507", EnumSet.of(Mood.EMPHATIC));
        TYPEMAP.put("Q113959607", EnumSet.of(Mood.EMPHATIC)); // emphatic particle
        TYPEMAP.put("Q2532941", EnumSet.of(Mood.VOLITIVE));

        TYPEMAP.put("Q134489509", EnumSet.of(WordOrder.SUBJECT_VERB)); // verb position A, the Dutch main clause verb position
        TYPEMAP.put("Q134489487", EnumSet.of(WordOrder.VERB_SUBJECT)); // verb position B, the Dutch subordinate clause verb position

        TYPEMAP.put("Q5636904", EnumSet.of(Aspect.HABITUAL));
        TYPEMAP.put("Q75243920", new HashSet<>(Arrays.asList(Aspect.HABITUAL, Tense.PAST))); // habitual past
        TYPEMAP.put("Q3517796", EnumSet.of(Aspect.SIMPLE)); // simple tense
        TYPEMAP.put("Q121765742", EnumSet.of(Aspect.SIMPLE));
        TYPEMAP.put("Q1424306", EnumSet.of(Aspect.PERFECTIVE));
        TYPEMAP.put("Q442485", new HashSet<>(Arrays.asList(Aspect.PERFECTIVE, Tense.PAST))); // Preterite or perfective with past tense
        TYPEMAP.put("Q216497", new HashSet<>(Arrays.asList(Aspect.PERFECTIVE, Tense.PAST))); // aorist, verb forms usually express perfective aspect and refer to past events
        TYPEMAP.put("Q12547192", new HashSet<>(Arrays.asList(Aspect.IMPERFECT, Tense.PAST, Mood.INDICATIVE))); // past imperfect, verb form in several languages indicating past tense, imperfective aspect, and indicative mood
        TYPEMAP.put("Q625420", EnumSet.of(Aspect.PERFECT));
        TYPEMAP.put("Q1240211", new HashSet<>(Arrays.asList(Tense.PRESENT, Aspect.PERFECT)));
        TYPEMAP.put("Q64005357", new HashSet<>(Arrays.asList(Tense.PAST, Aspect.PERFECT)));
        TYPEMAP.put("Q23663136", new HashSet<>(Arrays.asList(Tense.PAST, Aspect.PERFECT)));
        TYPEMAP.put("Q124351233", new HashSet<>(Arrays.asList(Tense.PAST, Aspect.PERFECT))); // performative aspect
        TYPEMAP.put("Q371427", EnumSet.of(Aspect.IMPERFECTIVE));
        TYPEMAP.put("Q54556033", EnumSet.of(Aspect.IMPERFECTIVE)); // imperfective verb
        TYPEMAP.put("Q2898727", EnumSet.of(Aspect.IMPERFECTIVE)); // imperfective form for Japanese verb
        TYPEMAP.put("Q108524486", EnumSet.of(Aspect.IMPERFECT));
        TYPEMAP.put("Q56649265", new HashSet<>(Arrays.asList(Tense.PRESENT, Aspect.IMPERFECT))); // present imperfect
        TYPEMAP.put("Q7240943", new HashSet<>(Arrays.asList(Tense.PRESENT, Aspect.IMPERFECT))); // present continuous/present imperfect
        TYPEMAP.put("Q56650537", new HashSet<>(Arrays.asList(Tense.PAST, Aspect.IMPERFECT))); // past continuous/present imperfect
        TYPEMAP.put("Q112785242", new HashSet<>(Arrays.asList(Aspect.IMPERFECT, VerbType.PARTICIPLE))); // imperfect participle
        TYPEMAP.put("Q113115936", new HashSet<>(Arrays.asList(Aspect.PERFECT, VerbType.PARTICIPLE))); // perfect participle
        TYPEMAP.put("Q623742", EnumSet.of(Aspect.PLUPERFECT));

        TYPEMAP.put("patronymic", EnumSet.of(DerivationType.PATRONYMIC));

        TYPEMAP.put("Q77768943", EnumSet.of(Register.INFORMAL));
        TYPEMAP.put("Q1070730", EnumSet.of(Register.INFORMAL)); // voseo in Spanish
        TYPEMAP.put("Q77768790", EnumSet.of(Register.FORMAL));
        TYPEMAP.put("Q6158182", EnumSet.of(Register.FORMAL)); // ustedeo in Spanish
        TYPEMAP.put("Q56650512", EnumSet.of(Register.FORMAL)); // Bangla polite form
        TYPEMAP.put("Q56650487", new HashSet<>(Arrays.asList(Person.SECOND, Register.FAMILIAR)));
        TYPEMAP.put("Q56650485", new HashSet<>(Arrays.asList(Person.SECOND, Register.INFORMAL)));
        TYPEMAP.put("Q66664394", EnumSet.of(Register.INTIMATE)); // endearing
        TYPEMAP.put("high", EnumSet.of(Register.HIGH));
        TYPEMAP.put("Q545779", EnumSet.of(Register.PEJORATIVE)); // pejorative
        TYPEMAP.put("Q54948374", EnumSet.of(Register.PEJORATIVE)); // depreciative form
        TYPEMAP.put("Q1521634", EnumSet.of(Register.PEJORATIVE)); // vulgarism
        TYPEMAP.put("Q75242466", EnumSet.of(Register.CONVERSATIONAL)); // chalita bhasha
        TYPEMAP.put("Q55228835", EnumSet.of(Register.CONVERSATIONAL)); // colloquial form
        TYPEMAP.put("Q20613396", EnumSet.of(Register.LITERARY)); // historical language style that was used in 19th and 20th century Bangla literary works

        TYPEMAP.put("Q124100534", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // surface representation in contrast to lexical representation
        TYPEMAP.put("Q228503", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // uninflected word
        TYPEMAP.put("Q11073520", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // kanyouku utterances with fixed forms and often non-literal meaning
        TYPEMAP.put("Q12812139", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // technical term

        TYPEMAP.put("Q1358239", EnumSet.of(Sizeness.AUGMENTATIVE));
        TYPEMAP.put("Q221446", EnumSet.of(Sizeness.AUGMENTATIVE)); // reduplication in Japanese
        TYPEMAP.put("Q6029894", EnumSet.of(Sizeness.AUGMENTATIVE)); // intensive
        TYPEMAP.put("Q108709", EnumSet.of(Sizeness.DIMINUTIVE));

        TYPEMAP.put("consonant-end", EnumSet.of(Sound.CONSONANT_END));
        TYPEMAP.put("consonant-start", EnumSet.of(Sound.CONSONANT_START));
        TYPEMAP.put("rieul-end", EnumSet.of(Sound.RIEUL_END));
        TYPEMAP.put("vowel-end", EnumSet.of(Sound.VOWEL_END));
        TYPEMAP.put("vowel-start", EnumSet.of(Sound.VOWEL_START));
        TYPEMAP.put("Q101252532", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // where consonant is unmutated
        TYPEMAP.put("Q56648699", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // soft mutation, where consonant becomes more sonorous
        TYPEMAP.put("Q117262361", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // pausal form, form of a word realised in hiatus between prosodic units
        TYPEMAP.put("Q97130345", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // hard mutation, a voiced stop becomes an unvoiced stop in the same place of articulation
        TYPEMAP.put("Q56648701", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // aspirate mutation, a consonant is transformed from an unvoiced stop into a fricative at a similar or the same point of articulation

        TYPEMAP.put("Q101583900", EnumSet.of(Alternate.SPELLING)); // alternative spelling
        TYPEMAP.put("Q115819543", EnumSet.of(Alternate.SPELLING)); // alternative spelling
        TYPEMAP.put("Q59342809", EnumSet.of(Alternate.SPELLING)); // It's like a versus an in English depending on the pronunciation properties of the word after the preposition.

        TYPEMAP.put("standard", EnumSet.of(Usage.STANDARD));
        TYPEMAP.put("Q55094451", EnumSet.of(Usage.RARE)); // rare form
        TYPEMAP.put("Q58157328", EnumSet.of(Usage.RARE)); // rare, indicates whether lexeme sense is used rarely
        TYPEMAP.put("Q8102", EnumSet.of(Usage.RARE)); // slang
        TYPEMAP.put("Q12237354", EnumSet.of(Usage.RARE)); // obsolete word
        TYPEMAP.put("Q54943392", EnumSet.of(Usage.RARE)); // obsolete form
        TYPEMAP.put("Q181970", EnumSet.of(Usage.RARE)); // archaism
        TYPEMAP.put("Q54944750", EnumSet.of(Usage.RARE)); // potential form
        TYPEMAP.put("Q55074511", EnumSet.of(Usage.RARE)); // reconstructed word

        // Phrases and other things that don't inflect
        TYPEMAP.put("Q101352", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // family name. Lots of them conflict with common nouns, like "light"
        TYPEMAP.put("Q8436", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // family. Lots of them conflict with common nouns, like "light"
        TYPEMAP.put("Q36867633", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // family name. Lots of them conflict with common nouns, like "light"
        TYPEMAP.put("Q112146129", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // family name. Plura
        TYPEMAP.put("Q3481344", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // female given name
        TYPEMAP.put("Q202444", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // given name
        TYPEMAP.put("Q22809413", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // Chinese given name
        TYPEMAP.put("Q30515", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // slogan
        TYPEMAP.put("Q184511", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // idiom
        TYPEMAP.put("Q3147869", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // idiom
        TYPEMAP.put("Q15841920", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // Redewendung (idiomatic expression)
        TYPEMAP.put("Q104692917", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // idiomatic interjection
        TYPEMAP.put("Q35102", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // proverb
        TYPEMAP.put("Q187931", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // phrase
        TYPEMAP.put("Q7188068", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // phrasal template
        TYPEMAP.put("Q1401131", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // noun phrase
        TYPEMAP.put("Q5551966", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // phraseme
        TYPEMAP.put("Q3734650", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // adverbial phrase, a phrase the head of which is an adverb
        TYPEMAP.put("Q357760", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // adjectival phrase
        TYPEMAP.put("Q56042915", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // prepositional phrase
        TYPEMAP.put("Q1778442", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // verb phrase
        TYPEMAP.put("Q384876", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // set phrase
        TYPEMAP.put("Q3062294", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // Latin phrase
        TYPEMAP.put("Q1527589", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // phrasal verb
        TYPEMAP.put("Q117606981", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // verbo-nominal syntagma
        TYPEMAP.put("Q12734432", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // attributive locution, phrase that grammatically is used as attribute
        TYPEMAP.put("Q5978305", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // conjunctive locution, a multiword expression working as a conjunct
        TYPEMAP.put("Q5978303", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // adverbial locution, a combination of words that acts as an adverb
        TYPEMAP.put("Q1167104", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // adverb that connects two independent clauses
        TYPEMAP.put("Q2034977", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // prepositional adverb
        TYPEMAP.put("Q29888377", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // nominal locution, combination of words that acts as a noun
        TYPEMAP.put("Q10319522", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // prepositional locution, a multiword expression working as a preposition
        TYPEMAP.put("Q20430476", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // pronominal locution
        TYPEMAP.put("Q10319520", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // interjectional locution
        TYPEMAP.put("Q10976085", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // verbal locution, combination of words that acts as a verb
        TYPEMAP.put("Q55964976", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // postpositive locution
        TYPEMAP.put("Q118533806", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // copulative locution
        TYPEMAP.put("Q778379", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // expression
        TYPEMAP.put("Q6935164", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // multiword expression
        TYPEMAP.put("Q130270424", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // interrogative expression
        TYPEMAP.put("Q3257782", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // locution, fixed expression
        TYPEMAP.put("Q117605926", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // perfective verbal expression
        TYPEMAP.put("Q65280376", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // collocation variant
        TYPEMAP.put("Q117605650", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // verbal expression
        TYPEMAP.put("Q41796", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // sentence
        TYPEMAP.put("Q3026787", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // saying
        TYPEMAP.put("Q63348589", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // popular saying
        TYPEMAP.put("Q45594", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // greeting
        TYPEMAP.put("Q516766", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // salutation
        TYPEMAP.put("Q1553339", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // chant
        TYPEMAP.put("Q5456361", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // fixed expression
        TYPEMAP.put("Q170239", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // onomatopoeia
        TYPEMAP.put("Q102047", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // suffix
        TYPEMAP.put("Q66614509", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // nominal suffix
        TYPEMAP.put("Q1964223", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // name suffix
        TYPEMAP.put("Q56681944", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // even suffix form
        TYPEMAP.put("Q109528443", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // inflectional suffix
        TYPEMAP.put("Q66614499", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // verbal suffix
        TYPEMAP.put("Q985836", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // bound morpheme
        TYPEMAP.put("Q43249", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // morpheme
        TYPEMAP.put("Q126728876", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // nominal modifier, suffix deriving a noun from a preceding noun
        TYPEMAP.put("Q126734687", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // verbal modifier, verbal derivational suffix
        TYPEMAP.put("Q361669", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // modal particle
        TYPEMAP.put("Q134830", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // prefix
        TYPEMAP.put("Q54792077", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // prefixoid
        TYPEMAP.put("Q125858556", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // number-person prefix
        TYPEMAP.put("Q1552433", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // preverb
        TYPEMAP.put("Q62155", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // affix
        TYPEMAP.put("Q109249055", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // pseudo-affix
        TYPEMAP.put("Q1153504", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // interfix
        TYPEMAP.put("Q201322", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // infix
        TYPEMAP.put("Q358417", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // circumposition
        TYPEMAP.put("Q57156560", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // affixed form
        TYPEMAP.put("Q3976700", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // adjectival suffix
        TYPEMAP.put("Q106610283", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // adverbial suffix
        TYPEMAP.put("Q57156426", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // standalone form of an affix
        TYPEMAP.put("Q57584278", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // case suffix, suffix that changes the case of a word
        TYPEMAP.put("Q8185162", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // lowercase letter
        TYPEMAP.put("Q98912", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // uppercase letter
        TYPEMAP.put("Q191494", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // digraph
        TYPEMAP.put("Q65048529", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // lowercase text
        TYPEMAP.put("Q111029", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // root, typically unusable and not inflected.
        TYPEMAP.put("Q18514", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // lemma, this is redundant, and it's incorrect when there is more than one gender for lemmaless inflection.
        TYPEMAP.put("Q10343770", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // indeclinable word
        TYPEMAP.put("Q66364639", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // indeclinable noun
        TYPEMAP.put("Q106644026", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // general, typically a default of something else
        TYPEMAP.put("Q210523", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // word stem, part of a word
        TYPEMAP.put("Q1122269", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // collocation, frequent occurrence of words next to each other
        TYPEMAP.put("Q18915698", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // established collocation
        TYPEMAP.put("Q1428334", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // paradigm, an inflection table instead of actual words
        TYPEMAP.put("Q102500", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // chemical symbol
        TYPEMAP.put("Q80071", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // symbol
        TYPEMAP.put("Q308229", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // currency sign
        TYPEMAP.put("Q31963", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // emoticon
        TYPEMAP.put("Q1668151", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // semantic punctuation mark
        TYPEMAP.put("Q1984758", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // misspelling, not helpful
        TYPEMAP.put("Q56161479", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // incorrect form, not helpful
        TYPEMAP.put("Q110588065", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // plural of plural, not helpful
        TYPEMAP.put("Q134489593", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // inflection_e, it's a verb being used as an adjective

        // Types that are algorithmically added instead of stored.
        TYPEMAP.put("Q69761768", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // feminine possessive
        TYPEMAP.put("Q71469738", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // masculine or feminine possessive
        TYPEMAP.put("Q69761633", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // masculine possessive
        TYPEMAP.put("Q78191294", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // singular possessive
        TYPEMAP.put("Q78191289", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // plural possessive
        TYPEMAP.put("Q71470598", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // first-person possessive
        TYPEMAP.put("Q71470837", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // second-person possessive
        TYPEMAP.put("Q71470909", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // third-person possessive
        TYPEMAP.put("Q107614077", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // The one with a fugenelement
        TYPEMAP.put("Q2105891", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // possessive
        TYPEMAP.put("Q1861696", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // English possessive
        TYPEMAP.put("Q21470140", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // possessive/proprietive case
        TYPEMAP.put("Q100952920", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // zu infinitive in German

        // Forms of limited value.
        TYPEMAP.put("Q42106", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // synonym
        TYPEMAP.put("Q124100536", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // lexical representation
        TYPEMAP.put("Q1380802", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // causal adverb
        TYPEMAP.put("Q734832", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // typographical error
        TYPEMAP.put("Q317623", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // technical standard
        TYPEMAP.put("Q4923918", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // corruption
        TYPEMAP.put("Q126729404", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // deverbal
        TYPEMAP.put("Q98772589", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // expanded contraction
        TYPEMAP.put("Q1192464", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // rendaku in Japanese
        TYPEMAP.put("Q126897884", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // denominal
        TYPEMAP.put("Q58233068", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // humorous
        TYPEMAP.put("Q43747", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // Internet slang
        TYPEMAP.put("Q89522629", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // poetic form
        TYPEMAP.put("Q543", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // censorship, like forms that are censored from profane words.
        TYPEMAP.put("Q213458", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // clitic
        TYPEMAP.put("Q6548647", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // enclitic
        TYPEMAP.put("Q340015", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // deixis, words requiring context to understand their meaning
        TYPEMAP.put("Q162940", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // diacritic
        TYPEMAP.put("Q1049294", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // emoji

        // Hard to classify noun classes.
        TYPEMAP.put("Q113335229", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335232", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335225", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113331807", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335171", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335224", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335227", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113194639", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335228", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113194715", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335233", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335234", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335237", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335235", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q122255603", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113383699", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q124513935", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113195544", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113195506", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113383692", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113383694", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113195757", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
        TYPEMAP.put("Q113335230", EnumSet.of(Ignorable.IGNORABLE_PROPERTY));

        // Undocumented properties
        TYPEMAP.put("Q123865651", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented Japanese property
        TYPEMAP.put("Q89270548", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented Japanese property
        TYPEMAP.put("Q63997520", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q94142002", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented Basque property
        TYPEMAP.put("Q25554366", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented Sanskrit property
        TYPEMAP.put("Q114902657", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q12273953", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q12975777", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q12975826", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q12988065", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q130611240", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q10638483", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q60799546", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q66664469", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented term of disparagement property
        TYPEMAP.put("Q110113203", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property
        TYPEMAP.put("Q121824824", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // undocumented property

        // TODO ignore for now
        TYPEMAP.put("Q74674702", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // unmarked verb form in Basque
        TYPEMAP.put("Q64448167", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // Dutch f/m shift, marker for the very gradual shift from feminine to masculine grammatical gender in a large group of Dutch nouns
        TYPEMAP.put("Q74674960", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // a verb that can be used as a noun
        TYPEMAP.put("Q18794", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // ezāfe
        TYPEMAP.put("Q12360803", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // Des-form. Perhaps a part of da-infinitive, inessive, present. See saatma for an example.
        TYPEMAP.put("Q15737187", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // negative form for Japanese verb
        TYPEMAP.put("Q2888577", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // conjunctive form for Japanese verb
        TYPEMAP.put("Q53608953", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // attributive form for Japanese verb
        TYPEMAP.put("Q53609593", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // hypothetical form for Japanese verb
        TYPEMAP.put("Q732699", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // grammatical modifier for Māori
        TYPEMAP.put("Q2120608", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // function word words with little/ambiguous lexical meaning that express grammatical relationships or specify the speaker’s attitude/mood of the speaker
        TYPEMAP.put("Q113389006", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // andative semantic feature of verb phrases in some languages; involves meanings related to 'take away'
        TYPEMAP.put("Q3938", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // Wiki sandbox
        TYPEMAP.put("Q94140447", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // what-who Basque verb
        TYPEMAP.put("Q4423888", EnumSet.of(Ignorable.IGNORABLE_INFLECTION)); // inflected form
        TYPEMAP.put("Q55832778", EnumSet.of(Ignorable.IGNORABLE_LEMMA)); // lujvo
        TYPEMAP.put("Q17239636", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // onbin
        TYPEMAP.put("Q126729798", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // nominaliser, derivational suffix forming a noun from a preceding verb
        TYPEMAP.put("Q503992", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // nominal, word class consisting of pronouns, nouns, adjectives and numerals
        TYPEMAP.put("Q75244800", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // future imperative
        TYPEMAP.put("Q113330960", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // basic phase
        TYPEMAP.put("Q115042433", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // subtractive of basic phase
        TYPEMAP.put("Q1909485", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // quantifier, but it might be used frequently as an indefinite noun
        TYPEMAP.put("Q694268", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // collective
        TYPEMAP.put("Q1450795", EnumSet.of(Ignorable.IGNORABLE_PROPERTY)); // singulative
    }

    static Set<? extends Enum<?>> getMappedGrammemes(String grammeme) {
        return TYPEMAP.get(grammeme);
    }
}
