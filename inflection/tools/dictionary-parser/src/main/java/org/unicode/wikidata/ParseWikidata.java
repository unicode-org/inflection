/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.file.Paths;
import java.text.NumberFormat;
import java.util.EnumMap;
import java.util.Locale;
import java.util.Properties;
import java.util.SortedSet;
import java.util.regex.Pattern;
import javax.annotation.Nonnull;
import javax.annotation.Nullable;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonToken;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.ibm.icu.util.ULocale;

import org.apache.commons.compress.compressors.CompressorStreamFactory;
import org.apache.commons.lang3.StringUtils;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.EnumSet;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

import static org.unicode.wikidata.Grammar.TYPEMAP;
import static org.unicode.wikidata.Grammar.REMAP;
import static org.unicode.wikidata.Grammar.Ignorable;
import static org.unicode.wikidata.Grammar.PartOfSpeech;
import static org.unicode.wikidata.Grammar.Sound;

/**
 * Default parser option values.
 */
final class ParserDefaults {
    static final String RESOURCES_DIR = "src/main/resources/org/unicode/wikidata/";
    static final String DEFAULT_INFLECTION_FILE_NAME = "inflectional.xml";
    static final String DEFAULT_DICTIONARY_FILE_NAME = "dictionary.lst";
    // Put the rare inflections at the end.
    static final Comparator<Inflection> RARITY_AWARE_COMPARATOR = Comparator
            .comparing(Inflection::isRareUsage)
            .thenComparing(Inflection::compareTo);
    private ParserDefaults() {}
}

/**
 * The options to extract the data from the data source.
 */
final class ParserOptions {
    private static final char COLON_SEPARATOR = ':';
    static final String INFLECTIONS_FILE = "--inflections";
    static final String DICTIONARY_FILE = "--dictionary";
    static final String MAP_GRAMMEME = "--map-grammeme";
    static final String ADD_EXTRA_GRAMMEMES = "--add-extra-grammemes";
    static final String INFLECTION_TYPES = "--inflection-types";
    static final String ADD_LEMMAS_FOR_TYPES = "--add-lemma-forms-for-types";
    static final String IGNORE_GRAMMEMES_FOR_TYPES = "--ignore-grammemes-for-types";
    static final String IGNORE_PROPERTY_SET = "--ignore-property-set";
    static final String INCLUDE_LEMMAS_WITHOUT_WORD = "--include-lemmas-without-words";
    static final String USE_FORM_ATTRIBUTE = "--use-form-attribute";
    static final String IGNORE_SURFACE_FORM = "--ignore-entries-with-grammemes";
    static final String IGNORE_UNANNOTATED_SURFACE_FORM = "--ignore-unannotated-entries";
    static final String ADD_NORMALIZED_ENTRY = "--add-normalized-entry";
    static final String LOCALE_OPT = "--locale";
    static final String TIMESTAMP = "--timestamp";
    static final String ADD_DEFAULT_GRAMMEME_FOR_CATEGORY = "--add-default-grammeme-for-category";
    static final String IGNORE_UNSTRUCTURED_ENTRIES = "--ignore-unstructured-entries";
    static final String ADD_SOUND = "--add-sound";

    boolean includeLemmasWithoutWords = false;
    boolean ignoreUnannotated = false;
    boolean useFormAttribute = false;
    boolean includeLemmaForms = false;
    boolean addNormalizedEntry = false;
    boolean ignoreUnstructuredEntries = false;
    Set<String> propertiesToConsiderFromForm = null;
    final boolean addSound;

    EnumSet<PartOfSpeech> posToBeInflected;
    EnumSet<PartOfSpeech> posToBeLemmatised;
    TreeSet<String> posWithoutGrammemes;
    TreeMap<String, TreeSet<String>> additionalGrammemesDict;
    TreeMap<String, TreeMap<String, String>> defaultGrammemeForCategory;
    TreeMap<String, EnumMap<Sound, Pattern>> claimsToSound;

    ArrayList<String> sourceFilenames;
    String inflectionalFilename = ParserDefaults.DEFAULT_INFLECTION_FILE_NAME;
    String lexicalDictionaryFilename = ParserDefaults.DEFAULT_DICTIONARY_FILE_NAME;
    String locale = null;
    List<String> optionsUsedToInvoke = new ArrayList<>();

    private static void printUsage() {
        System.err.println("Usage: ParseLexicon [OPTIONS] <file1.xml or file1.lexicon>[ <file2.xml or file2.lexicon> ...]");
        System.err.println("\nOPTIONS");
        System.err.println(INFLECTIONS_FILE + " <file.xml>\tthe file for the inflectional patterns to be generated, default: inflectional.xml");
        System.err.println(DICTIONARY_FILE + " <file.lst>\tthe file for the lexical dictionary to be generated, default: dictionary.lst");
        System.err.println(ADD_EXTRA_GRAMMEMES + " <file.lst>\tFile containing words with the extra grammemes to be added, provide path relative to tools/dictionary-parser/src/main/resources/org/unicode/wikidata/ (only to be used for a temporary grammeme addition)");
        System.err.println(INFLECTION_TYPES + " pos1[,pos2,...]\tthe pos's to be inflected, default: noun");
        System.err.println(ADD_LEMMAS_FOR_TYPES + " pos1[,pos2,...]\tthe part of speeches for which the known lemma form should be added to dict, default: (NONE)");
        System.err.println(IGNORE_GRAMMEMES_FOR_TYPES + " pos1[,pos2,...]\tthe part of speeches for which we don't want to include any grammeme info other than vowel/consonant start, default: (NONE)");
        System.err.println(MAP_GRAMMEME + " grammeme1,grammeme2\twhen grammeme1 is seen in the source dictionary, use grammeme2 instead of it");
        System.err.println(IGNORE_PROPERTY_SET + " grammeme1[,grammeme2,...]\tthis property set is considered to be an ignorable property.");
        System.err.println(IGNORE_SURFACE_FORM + " type1[,type2,...]\tignore entries with specified grammemes. Default: do not ignore");
        System.err.println(IGNORE_UNANNOTATED_SURFACE_FORM + " \tignore entries without any grammeme annotation. Default: do not ignore");
        System.err.println(INCLUDE_LEMMAS_WITHOUT_WORD + "\tinclude lemma entries which do not have corresponding word-entry. Default: do not include");
        System.err.println(TIMESTAMP + "\ttimestamp of the latest lexicon used. Default: NONE");
        System.err.println(LOCALE_OPT + "\tlocale of the lexicon used. Default: " + ULocale.ENGLISH.getName());
        System.err.println(ADD_NORMALIZED_ENTRY + "\tAdds the normalized entry of a dictionary as an additional dictionary entry, only applies for non lowercase entries. Default: false");
        System.err.println(ADD_DEFAULT_GRAMMEME_FOR_CATEGORY + "\t[pos=partofSpeech1]category1=grammeme1[,category2=grammeme2.....]\t For each of the provided categories if no grammeme is present then add the default grammeme provided for that category to the word. Only applies for the provided parts of speech if pos= is supplied Default: (NONE)");
        System.err.println(IGNORE_UNSTRUCTURED_ENTRIES + " \tIgnore unstructured entries from the lexicon. Default: false");
        System.err.println(ADD_SOUND + " grammeme1[,grammeme2,...]\tSound properties to check for.");
    }

    ParserOptions(String[] args) throws Exception{
        posToBeInflected = EnumSet.of(PartOfSpeech.NOUN);
        posToBeLemmatised = EnumSet.noneOf(PartOfSpeech.class);
        posWithoutGrammemes = new TreeSet<>();
        additionalGrammemesDict = new TreeMap<>();
        sourceFilenames = new ArrayList<>();
        defaultGrammemeForCategory = new TreeMap<>();
        claimsToSound = new TreeMap<>();

        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (ParserOptions.INFLECTIONS_FILE.equals(arg)) {
                inflectionalFilename = args[++i];
            } else if (ParserOptions.DICTIONARY_FILE.equals(arg)) {
                lexicalDictionaryFilename = args[++i];
            } else if (ParserOptions.ADD_EXTRA_GRAMMEMES.equals(arg)) {
                String additionalGrammemeFilename = args[++i];
                String filePath = Paths.get(ParserDefaults.RESOURCES_DIR  + additionalGrammemeFilename).toAbsolutePath().toString();
                try (BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(filePath), StandardCharsets.UTF_8))) {
                    String line;
                    while ((line = br.readLine()) != null) {
                        int colonIdx = line.indexOf(COLON_SEPARATOR);
                        String phrase = line.substring(0, colonIdx);
                        String grammemes = line.substring(colonIdx + 1).trim();
                        additionalGrammemesDict.put(phrase, new TreeSet<>(Arrays.asList(grammemes.split(" "))));
                    }
                    optionsUsedToInvoke.add(ParserOptions.ADD_EXTRA_GRAMMEMES);
                    optionsUsedToInvoke.add(additionalGrammemeFilename);
                }
            } else if (ParserOptions.MAP_GRAMMEME.equals(arg)) {
                String mapGrammeme = args[++i];
                String[] split = mapGrammeme.split(",", 2);
                REMAP.put(split[0], split[1]);

                optionsUsedToInvoke.add(ParserOptions.MAP_GRAMMEME);
                optionsUsedToInvoke.add(mapGrammeme);
            } else if (ParserOptions.IGNORE_PROPERTY_SET.equals(arg)) {
                String propertySetToIgnore = args[++i];
                SortedSet<String> key = Grammar.asSet(propertySetToIgnore.split(","));
                TYPEMAP.put(key, EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
                optionsUsedToInvoke.add(ParserOptions.IGNORE_PROPERTY_SET);
                optionsUsedToInvoke.add(propertySetToIgnore);
            } else if (ParserOptions.USE_FORM_ATTRIBUTE.equals(arg)) {
                useFormAttribute = true;
                String formAttribute = args[++i];
                propertiesToConsiderFromForm = Grammar.asSet(formAttribute.split(","));
                optionsUsedToInvoke.add(ParserOptions.USE_FORM_ATTRIBUTE);
                optionsUsedToInvoke.add(formAttribute);
            } else if (ParserOptions.INFLECTION_TYPES.equals(arg)) {
                String inflectionTypes = args[++i];
                posToBeInflected.clear();

                for (String pos : inflectionTypes.split(",")) {
                    posToBeInflected.add(PartOfSpeech.valueOf(pos.toUpperCase()));
                }

                optionsUsedToInvoke.add(ParserOptions.INFLECTION_TYPES);
                optionsUsedToInvoke.add(inflectionTypes);
            } else if (ParserOptions.ADD_LEMMAS_FOR_TYPES.equals(arg)) {
                includeLemmaForms = true;
                String lemmatisationTypes = args[++i];
                posToBeLemmatised.clear();

                for (String pos : lemmatisationTypes.split(",")) {
                    posToBeLemmatised.add(PartOfSpeech.valueOf(pos.toUpperCase()));
                }
                optionsUsedToInvoke.add(ParserOptions.ADD_LEMMAS_FOR_TYPES);
                optionsUsedToInvoke.add(lemmatisationTypes);
            } else if (ParserOptions.IGNORE_GRAMMEMES_FOR_TYPES.equals(arg)) {
                String ignoredGrammemeTypes = args[++i];

                posWithoutGrammemes.clear();
                posWithoutGrammemes.addAll(Arrays.asList(ignoredGrammemeTypes.split(",")));

                optionsUsedToInvoke.add(ParserOptions.IGNORE_GRAMMEMES_FOR_TYPES);
                optionsUsedToInvoke.add(ignoredGrammemeTypes);
            } else if (ParserOptions.INCLUDE_LEMMAS_WITHOUT_WORD.equals(arg)) {
                includeLemmasWithoutWords = true;
                optionsUsedToInvoke.add(ParserOptions.INCLUDE_LEMMAS_WITHOUT_WORD);
            } else if (ParserOptions.IGNORE_SURFACE_FORM.equals(arg)) {
                String ignoreEntriesWithGrammemesStr = args[++i];
                TreeSet<String> ignoreEntriesWithGrammemes = new TreeSet<>(Arrays.asList(ignoreEntriesWithGrammemesStr.split(",")));
                for (String grammeme : ignoreEntriesWithGrammemes) {
                    EnumSet<Ignorable> enumSet = EnumSet.of(Ignorable.IGNORABLE_INFLECTION);
                    TYPEMAP.put(Grammar.asSet(grammeme), enumSet);
                }
                optionsUsedToInvoke.add(ParserOptions.IGNORE_SURFACE_FORM);
                optionsUsedToInvoke.add(ignoreEntriesWithGrammemesStr);
            } else if (ParserOptions.IGNORE_UNANNOTATED_SURFACE_FORM.equals(arg)) {
                ignoreUnannotated = true;
                optionsUsedToInvoke.add(ParserOptions.IGNORE_UNANNOTATED_SURFACE_FORM);
            } else if (ParserOptions.TIMESTAMP.equals(arg)) {
                String timestamp = args[++i];
                optionsUsedToInvoke.add(ParserOptions.TIMESTAMP);
                optionsUsedToInvoke.add(timestamp);
            } else if (ParserOptions.LOCALE_OPT.equals(arg)) {
                String localeStr = args[++i];
                locale = localeStr;
                optionsUsedToInvoke.add(ParserOptions.LOCALE_OPT);
                optionsUsedToInvoke.add(localeStr);
            } else if (ParserOptions.ADD_NORMALIZED_ENTRY.equals(arg)) {
                addNormalizedEntry = true;
                optionsUsedToInvoke.add(ParserOptions.ADD_NORMALIZED_ENTRY);
            } else if (ParserOptions.ADD_DEFAULT_GRAMMEME_FOR_CATEGORY.equals(arg))  {
                String categoryDefaultGrammemeString = args[++i];
                String[] tokens = categoryDefaultGrammemeString.split(",");
                String posValue = "";
                for (int idx = 0; idx < tokens.length; idx += 1) {
                    String token = tokens[idx];
                    String[] tokenArgs = token.split("=");
                    if (tokenArgs.length != 2) {
                        throw new IllegalArgumentException("Default Grammeme for category string does not have entry in the format a=b " + token);
                    }
                    String key = tokenArgs[0].toLowerCase();
                    String value = tokenArgs[1].toLowerCase();
                    if (key.compareTo("pos") == 0) {
                        if (idx != 0) {
                            throw new IllegalArgumentException("pos key is not the first argument for default Grammeme for category string " + categoryDefaultGrammemeString);
                        }
                        posValue = value;
                        continue;
                    }
                    defaultGrammemeForCategory.putIfAbsent(posValue, new TreeMap<>());
                    defaultGrammemeForCategory.get(posValue).put(key, value);
                }

                optionsUsedToInvoke.add(ParserOptions.ADD_DEFAULT_GRAMMEME_FOR_CATEGORY);
                optionsUsedToInvoke.add(categoryDefaultGrammemeString);
            } else if (ParserOptions.IGNORE_UNSTRUCTURED_ENTRIES.equals(arg))  {
                ignoreUnstructuredEntries = true;
                optionsUsedToInvoke.add(ParserOptions.IGNORE_UNSTRUCTURED_ENTRIES);
            } else if (ParserOptions.ADD_SOUND.equals(arg))  {
                String soundGrammemeTypes = args[++i];

                List<String> additionalSoundProperties = Arrays.asList(soundGrammemeTypes.split(","));

                for (String claimID : ParseWikidata.PROPERTIES_WITH_PRONUNCIATION) {
                    Properties soundRegexes = new Properties();
                    String filePath = Paths.get(ParserDefaults.RESOURCES_DIR + claimID + ".properties").toAbsolutePath().toString();
                    try (var propertiesStream = new InputStreamReader(new FileInputStream(filePath), StandardCharsets.UTF_8)) {
                        soundRegexes.load(propertiesStream);
                        var enumMap = new EnumMap<Sound, Pattern>(Sound.class);
                        for (var entry : soundRegexes.entrySet()) {
                            var key = (String) entry.getKey();
                            if (additionalSoundProperties.contains(key)) {
                                enumMap.put(Sound.valueOf(key.toUpperCase(Locale.ROOT).replace('-', '_')), Pattern.compile((String)entry.getValue()));
                            }
                        }
                        if (enumMap.size() != additionalSoundProperties.size()) {
                            throw new IllegalArgumentException("Not all sound properties were found");
                        }
                        claimsToSound.put(claimID, enumMap);
                    }
                }

                optionsUsedToInvoke.add(ParserOptions.ADD_SOUND);
                optionsUsedToInvoke.add(soundGrammemeTypes);
            } else {
                sourceFilenames.add(arg);
            }
        }

        addSound = !claimsToSound.isEmpty();

        if (sourceFilenames.isEmpty()) {
            printUsage();
            throw new IllegalArgumentException();
        }
    }
}

/**
 * Contains statistical information on what has been analyzed.
 */
final class DocumentState {
    int lemmaCount = 0;
    int unusableLemmaCount = 0;
    int unusableSurfaceFormCount = 0;
    int mergedCount = 0;
    int incomingSurfaceForm = 0;
    TreeMap<String, DictionaryEntry> dictionary = new TreeMap<>();
    ArrayList<InflectionPattern> inflectionPatterns = new ArrayList<>(1024);

    boolean isInflectional() {
        return inflectionPatterns.size() > 1 || (inflectionPatterns.size() == 1 && inflectionPatterns.get(0).getCount() > 1);
    }

    private void sortInflectionPatterns(ArrayList<InflectionPattern> inflectionPatterns) {
        // We are sorting for the common ones first and then compare the identifier for lack of a better
        inflectionPatterns.sort(Comparator
                .comparing(InflectionPattern::getCount)
                .reversed()
                .thenComparing(InflectionPattern::getID));
        int identifierEnumeration = 1;
        for (InflectionPattern inflectionPattern : inflectionPatterns) {
            inflectionPattern.setID(identifierEnumeration++); // This is where we are reassigning identifiers to their new values.
        }
    }

    public void addDictionaryEntry(DictionaryEntry dictionaryEntry){
        String phrase = dictionaryEntry.phrase;
        DictionaryEntry existingDictionaryEntry = dictionary.get(phrase);
        if (existingDictionaryEntry == null) {
            dictionary.put(phrase, dictionaryEntry);
        }else{
            mergedCount++;
            existingDictionaryEntry.merge(dictionaryEntry);
        }
    }

    public void printDocument(ParserOptions parserOptions, long startTime) throws FileNotFoundException {
        TreeMap<Enum<?>, Integer> grammemeCounts = new TreeMap<>(EnumComparator.ENUM_COMPARATOR);
        int unclassifiedTerms = 0;
        if (isInflectional()) {
            try (PrintWriter inflectionalStream = new PrintWriter(new OutputStreamWriter(
                    new FileOutputStream(parserOptions.inflectionalFilename), StandardCharsets.UTF_8))) {
                inflectionalStream.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                        "<patterns>");

                sortInflectionPatterns(inflectionPatterns);
                for (InflectionPattern inflectionPattern : inflectionPatterns) {
                    inflectionalStream.print(inflectionPattern);
                }
                inflectionalStream.println("</patterns>");
            }
        }
        try (PrintWriter lexicalDictionaryStream = new PrintWriter(new OutputStreamWriter(
                new FileOutputStream(parserOptions.lexicalDictionaryFilename), StandardCharsets.UTF_8))) {
            for (Map.Entry<String, DictionaryEntry> entry : dictionary.entrySet()) {
                DictionaryEntry dictionaryEntry = entry.getValue();
                if (dictionaryEntry.getGrammemes().isEmpty()) {
                    // We don't care about only known words. We need grammeme data
                    unclassifiedTerms++;
                    continue;
                }
                // Add lemmas if so requested, but only to the pos that are to be inflected (i.e. no verb-lemmas unless we ask for it)
                lexicalDictionaryStream.println(dictionaryEntry.toString(isInflectional(), parserOptions.includeLemmaForms && !Collections.disjoint(dictionaryEntry.getGrammemes(), parserOptions.posToBeLemmatised)));
                for (Enum<?> grammeme : dictionaryEntry.getGrammemes()) {
                    grammemeCounts.merge(grammeme, 1, Integer::sum);
                }
            }

            NumberFormat percentFormat = NumberFormat.getPercentInstance(Locale.US);
            percentFormat.setMaximumFractionDigits(1);
            int dictionarySize = dictionary.size();
            StringBuilder source = new StringBuilder();
            Pattern anythingSlash = Pattern.compile(".*/");
            for (String sourceFilename : parserOptions.sourceFilenames) {
                source.append(anythingSlash.matcher(sourceFilename).replaceAll("")).append(" ");
            }
            lexicalDictionaryStream.println("==============================================");
            lexicalDictionaryStream.printf("%30s %7s%n", "Source:", source);
            lexicalDictionaryStream.printf("%30s %7d%n", "Lemma terms:", lemmaCount);
            lexicalDictionaryStream.printf("%30s %7d%n", "Unusable lemma terms:", unusableLemmaCount);
            lexicalDictionaryStream.printf("%30s %7d%n", "Incoming surface forms:", incomingSurfaceForm);
            lexicalDictionaryStream.printf("%30s %7d%n", "Surface forms:", dictionarySize);
            lexicalDictionaryStream.printf("%30s %7d %7s%n", "Collapsed surface forms:", mergedCount, '(' + percentFormat.format((mergedCount) / (double) incomingSurfaceForm) + ')');
            lexicalDictionaryStream.printf("%30s %7d%n", "Unusable surface forms:", unusableSurfaceFormCount);
            lexicalDictionaryStream.printf("%30s %7d %7s%n", "Usable terms:", dictionarySize - unclassifiedTerms, '(' + percentFormat.format((dictionarySize - unclassifiedTerms) / (double) dictionarySize) + ')');
            lexicalDictionaryStream.printf("%30s %7d %7s%n", "Unclassified terms:", unclassifiedTerms, '(' + percentFormat.format(unclassifiedTerms / (double) dictionarySize) + ')');
            lexicalDictionaryStream.println("==============================================");
            TreeMap<String, List<Enum<?>>> categories = new TreeMap<>();
            for (var entry : grammemeCounts.entrySet()) {
                var entryCategory = entry.getKey().getClass().getSimpleName();
                if (!categories.containsKey(entryCategory)) {
                    categories.put(entryCategory, new ArrayList<>());
                }
                var categoryValues = categories.get(entryCategory);
                categoryValues.add(entry.getKey());
            }

            for (var categoryEntry : categories.entrySet()) {
                var categoryName = categoryEntry.getKey();
                lexicalDictionaryStream.printf("%s:%n", categoryName);
                var categoryValues = categoryEntry.getValue();
                categoryValues.sort(Comparator.comparing(grammemeCounts::get));
                Collections.reverse(categoryValues);
                for (var categoryValue : categoryValues) {
                    lexicalDictionaryStream.printf("    %-20s %7d %7s%n", categoryValue.toString() + ':', grammemeCounts.get(categoryValue), '(' + percentFormat.format(grammemeCounts.get(categoryValue) / (double) dictionarySize) + ')');
                }
                lexicalDictionaryStream.printf("%n");
            }
            lexicalDictionaryStream.println("License: Creative Commons CC0 License (https://creativecommons.org/publicdomain/zero/1.0/)");
            long endTime = System.currentTimeMillis();
            long elapsedTime = (endTime-startTime);
            lexicalDictionaryStream.println("processed in " + (elapsedTime / 1000) + '.' + (elapsedTime % 1000) + " seconds");
            lexicalDictionaryStream.println("generated with options: " + String.join(" ", parserOptions.optionsUsedToInvoke));
        }
    }

    DocumentState() {
    }
}

/**
 * @see <a href="https://dumps.wikimedia.org/wikidatawiki/entities/">https://dumps.wikimedia.org/wikidatawiki/entities/</a>
 */
public final class ParseWikidata {
    static final Set<String> PROPERTIES_WITH_PRONUNCIATION = new TreeSet<>(List.of("P898"));

    static class Lemma {
        String value;
        boolean isRare = false;
        Sound soundType = null;
        final TreeSet<Enum<?>> grammemes = new TreeSet<>(EnumComparator.ENUM_COMPARATOR);
        final List<Inflection> inflections = new ArrayList<>(64);

        void reset() {
            value = null;
            isRare = false;
            soundType = null;
            grammemes.clear();
            inflections.clear();
        }
        private Lemma() {}
    }

    private final ParserOptions parserOptions;
    private final DocumentState documentState;

    ParseWikidata(ParserOptions parserOptions)
    {
        this.parserOptions = parserOptions;
        this.documentState = new DocumentState();
    }

    private void addGrammeme(TreeSet<Enum<?>> grammemes, @Nullable String grammeme) {
        if (grammeme != null && !grammeme.isEmpty()) {
            Set<? extends Enum<?>> values = Grammar.getMappedGrammemes(grammeme.split(","));
            if (values == null) {
                throw new RuntimeException(grammeme + " is not a known grammeme");
            }
            else if (!values.contains(Ignorable.IGNORABLE_PROPERTY)) {
                grammemes.addAll(values);
            }
        }
    }

    private void analyzeLexeme(Lexeme lexeme) {
        Lemma lemma = new Lemma();
        var desiredLanguage = parserOptions.locale;
        Set<? extends Enum<?>> partOfSpeechSet = null;
        for (var lemmaEntry : lexeme.lemmas.entrySet()) {
            var currentLemmaLanguage = lemmaEntry.getKey();
            if (!isContained(desiredLanguage, currentLemmaLanguage)) {
                continue;
            }
            lemma.reset();
            documentState.lemmaCount++;
            LexemeRepresentation lemmaRepresentation = lemmaEntry.getValue();
            lemma.value = lemmaRepresentation.value;
            if (partOfSpeechSet == null) {
                partOfSpeechSet = Grammar.getMappedGrammemes(lexeme.lexicalCategory);
                if (partOfSpeechSet == null) {
                    throw new IllegalArgumentException(lexeme.lexicalCategory + " is not a known part of speech grammeme for " + lexeme.id + "(" + lemma.value + ")");
                }
            }
            lemma.grammemes.addAll(partOfSpeechSet);
            if (partOfSpeechSet.contains(Ignorable.IGNORABLE_LEMMA)) {
                documentState.unusableLemmaCount++;
                return;
            }
            for (var form : lexeme.forms) {
                Inflection currentInflection = null;
                for (var representation : form.representations.entrySet()) {
                    if (!isContained(currentLemmaLanguage, lemmaEntry.getKey())) {
                        continue;
                    }
                    currentInflection = new Inflection(representation.getValue().value);
                    break;
                }
                if (currentInflection == null) {
                    // Perhaps this is an incompatible variant with the lemma. Move on.
                    break;
                }
                for (var feature : form.grammaticalFeatures) {
                    Set<? extends Enum<?>> values = Grammar.getMappedGrammemes(feature);
                    if (values == null) {
                        throw new IllegalArgumentException(feature + " is not a known grammeme for " + lexeme.id + "(" + lemma.value + ")");
                    }
                    if (!values.contains(Ignorable.IGNORABLE_PROPERTY)) {
                        currentInflection.grammemeSet.addAll(values);
                    }
                }
                currentInflection.grammemeSet.addAll(lemma.grammemes);
                if (currentInflection.grammemeSet.contains(Ignorable.IGNORABLE_LEMMA)) {
                    documentState.unusableLemmaCount++;
                    return;
                }
                if (currentInflection.grammemeSet.contains(Ignorable.IGNORABLE_INFLECTION)) {
                    documentState.unusableSurfaceFormCount++;
                    continue;
                }
                currentInflection.rareUsage = currentInflection.grammemeSet.contains(Grammar.Usage.RARE);
                if (currentInflection.rareUsage) {
                    currentInflection.grammemeSet.remove(Grammar.Usage.RARE);
                }
                lemma.inflections.add(currentInflection);
                if (parserOptions.addSound && form.claims != null && !form.claims.isEmpty() && currentInflection.inflection.charAt(0) == lemma.value.charAt(0)) {
                    // We have potential data, and the words aren't mixed together. So this is probably accurate.
                    boolean foundMatch = false;
                    for (var claimRegex : parserOptions.claimsToSound.entrySet()) {
                        var dataForClaim = form.claims.get(claimRegex.getKey());
                        if (dataForClaim != null && !dataForClaim.isEmpty()) {
                            for (var soundMatcher : claimRegex.getValue().entrySet()) {
                                for (var claimEntry : dataForClaim) {
                                    if (soundMatcher.getValue().matcher(claimEntry).find()) {
                                        currentInflection.grammemeSet.add(soundMatcher.getKey());
                                        foundMatch = true;
                                    }
                                }
                            }
                            if (!foundMatch) {
                                System.err.println("Unmatched property: " + lexeme.id + "(" + lemma.value + "): \"" + dataForClaim + "\"");
                            }
                        }
                    }
                }
            }
            documentState.incomingSurfaceForm += lemma.inflections.size();
            analyzeLemma(lemma);
        }
    }

    private static boolean validateStemLength(@Nonnull List<Inflection> inflections, int stemLength) {
        for (var inflection_outer : inflections) {
            String suffix = inflection_outer.getInflection().substring(stemLength);
            boolean invalid = false;
            for (var inflection_inner : inflections) {
                var inflectionInnerStr = inflection_inner.getInflection();
                if (inflectionInnerStr.endsWith(suffix) && ((inflectionInnerStr.length() - suffix.length()) < stemLength)) {
                    invalid = true;
                    break;
                }
            }
            if (invalid) {
                return false;
            }
        }
        return true;
    }

    //Provided lemma and all it's surface forms, return the length of the longest common prefix among them
    private static int getStemLength(String lemma, @Nonnull List<Inflection> inflections){
        String[] stringList = new String[inflections.size() + 1];
        for (int i = 0; i < inflections.size(); i++) {
            stringList[i] = inflections.get(i).getInflection();
        }
        stringList[inflections.size()] = lemma;
        int stemLength = StringUtils.indexOfDifference(stringList);
        if (stemLength == StringUtils.INDEX_NOT_FOUND) {
            stemLength = lemma.length();
        }
        while (!validateStemLength(inflections, stemLength)) {
            stemLength--;
        }
        return stemLength;
    }

    /**
     * Input: The list of surface forms, stemLength
     * Returns: The list of surface form suffixes with respect to a stem length
     */
    @Nonnull
    private static List<Inflection> generateSuffixes(int stemLength, @Nonnull List<Inflection> inflections) {
        // Create a map that doesn't check the rarity to get the uniqueness correct.
        Map<Inflection, Inflection> suffixes = new TreeMap<>();
        for (Inflection inflection : inflections) {
            Inflection suffix = new Inflection(inflection.getInflection().substring(stemLength), inflection.rareUsage);
            suffix.addGrammemes(inflection.getGrammemeSet());
            // This check prevents us from including functionally redundant data.
            if (!inflection.rareUsage || !suffixes.containsKey(suffix)) {
                // Either it's new, or we're replacing a rare inflection with a non-rare inflection.
                suffixes.put(suffix, suffix);
            }
            // else it's a rare usage, and it's already included.
            // If it's already not rare, then there is no need to add the same inflection as rare.
            // This hints at bad data not being consistent on the rarity.
        }
        // Now we resort them with the rare ones last.
        ArrayList<Inflection> result = new ArrayList<>(suffixes.values());
        if (result.size() > 1) {
            result.sort(ParserDefaults.RARITY_AWARE_COMPARATOR);
        }
        return result;
    }

    //Check whether the surface forms to be inflected or not
    private static boolean containsImportant(@Nonnull List<Inflection> inflections, EnumSet<PartOfSpeech> posToBeInflected) {
        for (Inflection inflection : inflections) {
            if (!Collections.disjoint(posToBeInflected, inflection.getGrammemeSet())) {
                return true;
            }
        }
        return false;
    }

    //Given lemma suffix and surface form suffixes, return either an existing inflection pattern or return a new one while adding to the existing inflection patterns
    private InflectionPattern getInflectionPattern(Lemma lemma, String lemmaSuffix,
                                                   List<Inflection> suffixes) {
        TreeSet<Enum<?>> newGrammemeList = new TreeSet<>(lemma.grammemes);

        InflectionPattern inflectionPattern = new InflectionPattern(
                documentState.inflectionPatterns.size() + 1,
                lemmaSuffix,
                newGrammemeList,
                suffixes
        );

        int idx = documentState.inflectionPatterns.indexOf(inflectionPattern);

        if (idx >= 0) {
            InflectionPattern existingInflectionPattern = documentState.inflectionPatterns.get(idx);
            existingInflectionPattern.merge(inflectionPattern);
            inflectionPattern = existingInflectionPattern;
        } else {
            inflectionPattern.saveInternalReferences();
            documentState.inflectionPatterns.add(inflectionPattern);
        }
        return inflectionPattern;
    }

    private void analyzeInflections(Lemma lemma, List<Inflection> inputInflections) {
        List<Inflection> inflections = new ArrayList<>();
        for (Inflection inflection : inputInflections) {
            inflections.addAll(enumerateInflectionsForGrammemeCombinations(inflection));
        }
        InflectionPattern inflectionPattern = null;
        HashSet<Integer> nonEmptyInflectionIndices = new HashSet<>();

        if (!inflections.isEmpty()) {
            ArrayList<Inflection> nonEmptyInflections = new ArrayList<>();
            // Adding lemma grammemes to all inflections
            for (int i = 0; i < inflections.size() ; i++) {
                var inflection = inflections.get(i);
                var inflectionGrammemes = inflection.getGrammemeSet();
                if (!inflectionGrammemes.isEmpty() && !InflectionPattern.isIgnorableGrammemeSet(inflectionGrammemes)) {
                    nonEmptyInflections.add(inflection);
                    nonEmptyInflectionIndices.add(i);
                }
                inflectionGrammemes.addAll(lemma.grammemes);
            }
            // If all inflections are empty then add all significant inflections to the pattern
            if (nonEmptyInflectionIndices.isEmpty()) {
                for (int i = 0; i < inflections.size() ; i++) {
                    var inflection = inflections.get(i);
                    if (!InflectionPattern.isIgnorableGrammemeSet(inflection.getGrammemeSet())) {
                        nonEmptyInflections.add(inflection);
                        nonEmptyInflectionIndices.add(i);
                    }
                }
            }

            if (containsImportant(inflections, parserOptions.posToBeInflected)) {
                int stemLength = getStemLength(lemma.value, nonEmptyInflections);
                List<Inflection> suffixes = generateSuffixes(stemLength, nonEmptyInflections);
                inflectionPattern = getInflectionPattern(lemma,
                        lemma.value.substring(stemLength),
                        suffixes
                );
            }
            // else ignore this unimportant inflection pattern. This is usually trimmed for size.
        }
        boolean addLemmasForPhrase = !Collections.disjoint(lemma.grammemes, parserOptions.posToBeLemmatised);
        Locale currLocale = parserOptions.locale == null ? Locale.ENGLISH : Locale.of(parserOptions.locale);
        for (int i = 0; i < inflections.size() ; i++) {
            var inflection = inflections.get(i);
            String phrase = inflection.getInflection();
            if (!inflection.getGrammemeSet().isEmpty() && addLemmasForPhrase && (phrase.compareTo(lemma.value) == 0)) {
                addGrammeme(inflection.getGrammemeSet(), "lemma_word");
            }
            String lemmaForEntry = !addLemmasForPhrase ? phrase : lemma.value.replace(' ', '\u2002');
            InflectionPattern inflectionPatternForDict = nonEmptyInflectionIndices.contains(i) ? inflectionPattern : null;
            documentState.addDictionaryEntry(new DictionaryEntry(phrase, lemmaForEntry, lemma.isRare, inflection.getGrammemeSet(), inflectionPatternForDict));
            if (parserOptions.addNormalizedEntry) {
                String normalizedPhrase = phrase.toLowerCase(currLocale); // locale is specified in the options, by default we use en_US
                if (!normalizedPhrase.equals(phrase) && !lemma.isRare) {
                    documentState.addDictionaryEntry(new DictionaryEntry(normalizedPhrase, lemmaForEntry, false, inflection.getGrammemeSet(), inflectionPatternForDict));
                }
            }
        }
    }

    private static final TreeSet<Class<?>> grammemeClassSet = new TreeSet<>(Inflection.ENUM_CLASS_COMPARATOR);

    private List<Inflection> enumerateInflectionsForGrammemeCombinations(Inflection inflection) {
        List<Inflection> resultInflections = new ArrayList<>();
        TreeSet<Enum<?>> grammemeSet = inflection.getGrammemeSet();
        grammemeClassSet.clear();
        List<List<Enum<?>>> results = new ArrayList<>();
        List<List<Enum<?>>> newResults = new ArrayList<>();
        results.add(new ArrayList<>(grammemeSet.size()));
        for (Enum<?> grammeme : grammemeSet) {
            Class<?> grammemeClass = grammeme.getDeclaringClass();
            if (!grammemeClassSet.contains(grammemeClass)) {
                grammemeClassSet.add(grammemeClass);
                for (List<Enum<?>> list : results) {
                    list.add(grammeme);
                }
            }else {
                newResults.clear();
                for (List<Enum<?>> list : results) {
                    ArrayList<Enum<?>> newList = new ArrayList<>(grammemeSet.size());
                    for (Enum<?> grammemeFromList : list) {
                        newList.add(grammemeFromList.getClass() == grammemeClass ? grammeme : grammemeFromList);
                    }
                    newResults.add(newList);
                }
                results.addAll(newResults);
            }
        }
        for (List<Enum<?>> list : results) {
            Inflection resultInflection = new Inflection(inflection.getInflection(), inflection.isRareUsage());
            resultInflection.addGrammemes(list);
            resultInflections.add(resultInflection);
        }
        return resultInflections;
    }

    private void mergeAdditionalGrammemes() {
        // Add any entries that are missing. The actual properties will be added elsewhere.
        TreeSet<Enum<?>> grammemes = new TreeSet<>(EnumComparator.ENUM_COMPARATOR);
        for (var entry : parserOptions.additionalGrammemesDict.entrySet()) {
            grammemes.clear();
            for (var grammeme : entry.getValue()) {
                addGrammeme(grammemes, grammeme);
            }

            DictionaryEntry newDictionaryEntry = new DictionaryEntry(entry.getKey(), false, grammemes, null);
            DictionaryEntry existingEntry = documentState.dictionary.putIfAbsent(entry.getKey(), newDictionaryEntry);
            if (existingEntry != null) {
                // Add instead of replace.
                existingEntry.getGrammemes().addAll(grammemes);
            }
        }
    }

    /**
     * Is the base desired language contained in the variant language?
     * en, en true
     * en, en-us true
     * en-us, en-us true
     * ko, kok false
     */
    public boolean isContained(String baseLanguage, String variantLanguage) {
        if (baseLanguage.indexOf('-') < 0) {
            int dash = variantLanguage.indexOf('-');
            if (dash >= 0) {
                variantLanguage = variantLanguage.substring(0, dash);
            }
        }
        return baseLanguage.equals(variantLanguage);
    }

    private void analyzeLemma(Lemma lemma) {
        analyzeInflections(lemma, lemma.inflections);

        if (parserOptions.includeLemmasWithoutWords) {
            DictionaryEntry newDictionaryEntry = new DictionaryEntry(lemma.value, lemma.isRare, lemma.grammemes, null);
            documentState.dictionary.putIfAbsent(lemma.value, newDictionaryEntry);
        }
    }

    public void writeInflectionPatterns(long startTime) throws FileNotFoundException {
        documentState.printDocument(parserOptions, startTime);
    }

    public static void main(String[] args) throws Exception {
        ParserOptions parserOptions = new ParserOptions(args);
        long startTime = System.currentTimeMillis();
        ObjectMapper objectMapper = new ObjectMapper()
                .configure(JsonParser.Feature.ALLOW_UNQUOTED_FIELD_NAMES, true)
                .configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        var lexParser = new ParseWikidata(parserOptions);

        // We create InputSource directly due to an occasional bugs with UTF-8 files being interpreted as malformed UTF-8.
        // We use a large buffer because we're reading a large file, and we're frequently reading file data.
        for (String sourceFilename : parserOptions.sourceFilenames) {
            try (InputStream fileInputStream = new FileInputStream(sourceFilename)) {
                InputStream inputStream = fileInputStream;
                if (sourceFilename.endsWith(".bz2")) {
                    inputStream = new CompressorStreamFactory().createCompressorInputStream(CompressorStreamFactory.BZIP2, inputStream);
                }
                try (JsonParser parser = objectMapper.createParser(new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8), 1048576))) {
                    JsonToken currToken;
                    while ((currToken = parser.nextToken()) != JsonToken.START_OBJECT && currToken != null) {
                        // Find the first object in the array.
                    }
                    if (currToken == JsonToken.START_OBJECT) {
                        do {
                            Lexeme lexeme = objectMapper.readValue(parser, Lexeme.class);
                            try {
                                lexParser.analyzeLexeme(lexeme);
                            } catch (IllegalArgumentException e) {
                                lexParser.documentState.unusableLemmaCount++;
                                System.err.println("Line " + parser.currentLocation().getLineNr() + ": " + e.getMessage());
                            }
                        }
                        while (parser.nextToken() != JsonToken.END_ARRAY);
                    }
                }
            }
        }

        if (!lexParser.parserOptions.additionalGrammemesDict.isEmpty()) {
            lexParser.mergeAdditionalGrammemes();
        }

        lexParser.writeInflectionPatterns(startTime);
        if (lexParser.documentState.unusableLemmaCount != 0) {
            System.exit(lexParser.documentState.unusableLemmaCount);
        }
    }
}

