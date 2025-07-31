/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.Properties;
import javax.annotation.Nonnull;
import javax.annotation.Nullable;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonToken;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;

import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;
import org.apache.commons.lang3.StringUtils;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.TreeSet;
import java.util.Arrays;
import static org.unicode.wikidata.Grammar.Gender;
import static org.unicode.wikidata.Grammar.Ignorable;
import static org.unicode.wikidata.Grammar.PartOfSpeech;

/**
 * @see <a href=
 *      "https://dumps.wikimedia.org/wikidatawiki/entities/">https://dumps.wikimedia.org/wikidatawiki/entities/</a>
 */
public final class ParseWikidata {
    static final Set<String> PROPERTIES_WITH_PRONUNCIATION = new TreeSet<>(List.of(
            "P898" // IPA transcription
    ));
    static final Set<String> PROPERTIES_WITH_GRAMMEMES = new TreeSet<>(List.of(
            "P31", // instance of. Sometimes phrase information is here.
            "P1552", // has characteristic for animacy
            "P5185", // grammatical gender
            "P11054" // grammatical number
    ));
    static final Set<String> IMPORTANT_PROPERTIES = new TreeSet<>(PROPERTIES_WITH_GRAMMEMES);

    static {
        IMPORTANT_PROPERTIES.addAll(PROPERTIES_WITH_PRONUNCIATION);
    }

    static class Lemma {
        String value;
        boolean isRare = false;
        final TreeSet<Enum<?>> grammemes = new TreeSet<>(EnumComparator.ENUM_COMPARATOR);
        final List<Inflection> inflections = new ArrayList<>(64);

        void reset() {
            value = null;
            isRare = false;
            grammemes.clear();
            inflections.clear();
        }

        private Lemma() {
        }
    }

    private final ParserOptions parserOptions;
    private final DocumentState documentState = new DocumentState();
    private final TreeSet<String> rareLemmas = new TreeSet<>();
    private final TreeSet<String> omitLemmas = new TreeSet<>();
    private final Map<String, List<String>> mergeMap = new HashMap<>();
    private final TreeSet<String> deferredLexemes = new TreeSet<>();
    private final Map<String, Lemma> deferredLemmaMap = new HashMap<>();

    ParseWikidata(ParserOptions parserOptions) {
        this.parserOptions = parserOptions;
        for (var language : parserOptions.locales) {
            Properties rareLemmasProperties = new Properties();
            var resourceStream = getClass().getResourceAsStream("filter_" + language + ".properties");
            if (resourceStream == null) {
                // else oh well. It doesn't matter.
                continue;
            }
            try (var propertiesStream = new InputStreamReader(resourceStream, StandardCharsets.UTF_8)) {
                rareLemmasProperties.load(propertiesStream);
                for (var entry : rareLemmasProperties.entrySet()) {
                    String key = entry.getKey().toString();
                    String value = entry.getValue().toString();
                    if (value.matches("^L[0-9]+")) {
                        var values = Arrays.asList(value.split(","));
                        mergeMap.computeIfAbsent(key, v -> new ArrayList<>()).addAll(values);
                        deferredLexemes.add(key);
                        deferredLexemes.addAll(values);
                    } else {
                        switch (value) {
                            case "rare": {
                                rareLemmas.add(key);
                                break;
                            }
                            case "omit": {
                                omitLemmas.add(key);
                                break;
                            }
                            default: {
                                throw new IllegalArgumentException(key + ": Unknown key value " + value);
                            }
                        }
                    }
                }
            } catch (IOException e) {
                // else oh well. It doesn't matter.
            }
        }
    }

    static final String VARIANT_SEPARATOR = "-x-";

    private void analyzeLexeme(int lineNumber, Lexeme lexeme) {
        if (omitLemmas.contains(lexeme.id)) {
            // We really don't want this junk.
            return;
        }
        Lemma lemma = new Lemma();
        Set<? extends Enum<?>> partOfSpeechSet = null;
        for (var lemmaEntry : lexeme.lemmas.entrySet()) {
            var currentLemmaLanguage = lemmaEntry.getKey();
            lemma.reset();
            documentState.lemmaCount++;
            LexemeRepresentation lemmaRepresentation = lemmaEntry.getValue();
            lemma.value = lemmaRepresentation.value;
            int qVariantIdx = currentLemmaLanguage.indexOf(VARIANT_SEPARATOR);
            if (qVariantIdx >= 0) {
                // The languages can have wierd Q entry after the desired language.
                // A spelling variant is informative. Most of the rest are irrelevant.
                var additionalCategory = currentLemmaLanguage.substring(qVariantIdx + VARIANT_SEPARATOR.length());
                var variant = Grammar.getMappedGrammemes(additionalCategory);
                if (variant == null) {
                    if (parserOptions.debug) {
                        System.err.println("Line " + lineNumber + ": " + additionalCategory
                                + " is not a known grammeme for the language variant " + lexeme.id + "(" + lemma.value
                                + ")");
                    }
                    continue;
                }
                if (variant.contains(Ignorable.IGNORABLE_INFLECTION)) {
                    // Variant that we're trying to ignore
                    continue;
                }
                lemma.grammemes.addAll(variant);
            }
            if (partOfSpeechSet == null) {
                partOfSpeechSet = Grammar.getMappedGrammemes(lexeme.lexicalCategory);
                if (partOfSpeechSet == null) {
                    throw new IllegalArgumentException(lexeme.lexicalCategory
                            + " is not a known part of speech grammeme for " + lexeme.id + "(" + lemma.value + ")");
                }
            }
            lemma.grammemes.addAll(partOfSpeechSet);
            if (rareLemmas.contains(lexeme.id)) {
                lemma.grammemes.add(Grammar.Usage.RARE);
            }
            boolean hasDuplicates = extractImportantProperties(lexeme.claims, lemma.grammemes, lexeme.id, lemma.value);
            if (lemma.grammemes.contains(Ignorable.IGNORABLE_LEMMA)
                    || lemma.grammemes.contains(Ignorable.IGNORABLE_INFLECTION)) {
                documentState.unusableLemmaCount++;
                continue;
            }
            lemma.grammemes.remove(Ignorable.IGNORABLE_PROPERTY);
            if (hasDuplicates) {
                removeConflicts(lemma.grammemes, Gender.class);
            }
            TreeSet<Enum<?>> genderlessLemmaGrammemes = null;
            if (countGrammemeType(lemma.grammemes, Gender.class) > 0) {
                genderlessLemmaGrammemes = new TreeSet<>(lemma.grammemes);
                removeGrammemeType(genderlessLemmaGrammemes, Gender.class);
            }
            for (var form : lexeme.forms) {
                Inflection currentInflection = null;
                var representation = form.representations.get(currentLemmaLanguage);
                if (representation != null) {
                    currentInflection = new Inflection(representation.value);
                } else {
                    // Couldn't find an exact match. Go to a generic match.
                    for (var rep : form.representations.entrySet()) {
                        if (isContained(currentLemmaLanguage, lemmaEntry.getKey())) {
                            currentInflection = new Inflection(rep.getValue().value);
                            break;
                        }
                    }
                    if (currentInflection == null) {
                        // Perhaps this is an incompatible variant with the lemma. Move on.
                        break;
                    }
                }
                convertGrammemes(form, currentInflection, lexeme.id, lemma.value);
                if (genderlessLemmaGrammemes == null || countGrammemeType(currentInflection.grammemeSet, Gender.class) == 0) {
                    currentInflection.grammemeSet.addAll(lemma.grammemes);
                }
                else {
                    currentInflection.grammemeSet.addAll(genderlessLemmaGrammemes);
                }
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
                currentInflection.grammemeSet.remove(Ignorable.IGNORABLE_PROPERTY);
                var grammemeExpansion = parserOptions.expandGramemes != null
                        ? parserOptions.expandGramemes.get(currentInflection.grammemeSet)
                        : null;
                if (grammemeExpansion == null) {
                    lemma.inflections.add(currentInflection);
                } else {
                    for (var grammemeSet : grammemeExpansion) {
                        var expandedInflection = new Inflection(currentInflection.inflection,
                                currentInflection.rareUsage);
                        expandedInflection.grammemeSet.addAll(currentInflection.grammemeSet);
                        expandedInflection.grammemeSet.addAll(grammemeSet);
                        lemma.inflections.add(expandedInflection);
                    }
                }
            }
            documentState.incomingSurfaceForm += lemma.inflections.size();
            lemma.isRare = lemma.grammemes.contains(Grammar.Usage.RARE);
            if (lemma.isRare) {
                lemma.grammemes.remove(Grammar.Usage.RARE);
            }
            if (lemma.inflections.isEmpty()) {
                documentState.unusableLemmaCount++;
                return;
            }
            removeGrammemeType(lemma.grammemes, Gender.class);
            if (deferredLexemes.contains(lexeme.id)) {
                if (deferredLemmaMap.put(lexeme.id, lemma) != null) {
                    throw new IllegalStateException("Duplicate lexeme " + lexeme.id);
                }
            }
            else {
                analyzeLemma(lemma);
            }
        }
    }

    // Method to process and merge lexemes
    private void processAndMergeLexemes() {
        for (Map.Entry<String, List<String>> entry : mergeMap.entrySet()) {
            Lemma lemma = deferredLemmaMap.computeIfAbsent(entry.getKey(), key -> {
                throw new IllegalArgumentException(key + ": id not found");
            });
            for (var value : entry.getValue()) {
                Lemma lemmaToMerge = deferredLemmaMap.computeIfAbsent(value, key -> {
                    throw new IllegalArgumentException(key + ": id not found");
                });
                lemma.inflections.addAll(lemmaToMerge.inflections);
                lemma.isRare |= lemmaToMerge.isRare;
                // The lemma grammemes should already have the relevant POS data.
            }
            analyzeLemma(lemma);
        }
    }

    private void removeGrammemeType(TreeSet<Enum<?>> grammemes, Class<?> grammemeType) {
        var iter = grammemes.iterator();
        while (iter.hasNext()) {
            var grammeme = iter.next();
            if (grammemeType.isInstance(grammeme)) {
                iter.remove();
            }
        }
    }

    private int countGrammemeType(TreeSet<Enum<?>> grammemes, Class<?> grammemeType) {
        int count = 0;
        var iter = grammemes.iterator();
        while (iter.hasNext()) {
            var grammeme = iter.next();
            if (grammemeType.isInstance(grammeme)) {
                count++;
            }
        }
        return count;
    }

    /**
     * When there are multiple genders at the lemma level, it's a ranking system
     * instead of applying to all forms.
     * Such data is useless. So we should ignore it.
     * When there are multiple genders at the form level, the same form is valid for
     * all specified genders.
     */
    private void removeConflicts(TreeSet<Enum<?>> grammemes, Class<?> grammemeType) {
        if (grammemes.size() > 1 && countGrammemeType(grammemes, grammemeType) > 1) {
            removeGrammemeType(grammemes, grammemeType);
        }
    }

    private void convertGrammemes(LexemeForm form, Inflection currentInflection, String id, String lemma) {
        for (var feature : form.grammaticalFeatures) {
            Set<? extends Enum<?>> values = Grammar.getMappedGrammemes(feature);
            if (values == null) {
                throw new IllegalArgumentException(feature + " is not a known grammeme for " + id + "(" + lemma + ")");
            }
            currentInflection.grammemeSet.addAll(values);
        }
        extractImportantProperties(form.claims, currentInflection.grammemeSet, id, lemma);
    }

    /**
     * Return true if there are multiple claims
     */
    private boolean extractImportantProperties(Map<String, List<String>> claims, TreeSet<Enum<?>> grammemes, String id,
            String lemma) {
        boolean conflicts = false;
        if (claims == null || claims.isEmpty()) {
            return conflicts;
        }
        for (var claimEntry : claims.entrySet()) {
            if (PROPERTIES_WITH_PRONUNCIATION.contains(claimEntry.getKey())) {
                if (parserOptions.addSound) {
                    addSound(claimEntry.getKey(), claimEntry.getValue(), grammemes, id, lemma);
                }
                continue;
            }
            var claim = claimEntry.getValue();
            conflicts = conflicts || claim.size() > 1;
            for (var grammemeStr : claim) {
                var grammemeEnum = Grammar.getMappedGrammemes(grammemeStr);
                if (grammemeEnum != null) {
                    grammemes.addAll(grammemeEnum);
                } else if (parserOptions.debug) {
                    // Most of this is irrelevant non-grammatical information, like that it's a
                    // trademark, or a study of something,
                    // but sometimes it contains grammemes that apply to all words, like grammatical
                    // gender.
                    System.err.println(grammemeStr + " is not a known grammeme for " + id + "(" + lemma + ")");
                }
            }
        }
        return conflicts;
    }

    private void addSound(String property, List<String> claims, TreeSet<Enum<?>> grammemeSet, String id, String lemma) {
        boolean foundMatch = false;
        var dataForClaim = parserOptions.claimsToSound.get(property);
        if (dataForClaim != null && !dataForClaim.isEmpty()) {
            for (var soundMatcher : dataForClaim.entrySet()) {
                for (var claim : claims) {
                    if (soundMatcher.getValue().matcher(claim).find()) {
                        grammemeSet.add(soundMatcher.getKey());
                        foundMatch = true;
                    }
                }
            }
            if (!foundMatch) {
                System.err.println("Unmatched property: " + id + "(" + lemma + "): \"" + dataForClaim + "\"");
            }
        }
    }

    private static boolean validateStemLength(@Nonnull List<Inflection> inflections, int stemLength) {
        for (var inflection_outer : inflections) {
            String suffix = inflection_outer.getInflection().substring(stemLength);
            boolean invalid = false;
            for (var inflection_inner : inflections) {
                var inflectionInnerStr = inflection_inner.getInflection();
                if (inflectionInnerStr.endsWith(suffix)
                        && ((inflectionInnerStr.length() - suffix.length()) < stemLength)) {
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

    // Provided lemma and all it's surface forms, return the length of the longest
    // common prefix among them
    private static int getStemLength(String lemma, @Nonnull List<Inflection> inflections) {
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
                // Either it's new, or we're replacing a rare inflection with a non-rare
                // inflection.
                suffixes.put(suffix, suffix);
            }
            // else it's a rare usage, and it's already included.
            // If it's already not rare, then there is no need to add the same inflection as
            // rare.
            // This hints at bad data not being consistent on the rarity.
        }
        // Now we resort them with the rare ones last.
        ArrayList<Inflection> result = new ArrayList<>(suffixes.values());
        if (result.size() > 1) {
            result.sort(ParserDefaults.RARITY_AWARE_COMPARATOR);
        }
        return result;
    }

    // Check whether the surface forms to be inflected or not
    private static boolean containsImportant(@Nonnull List<Inflection> inflections,
            EnumSet<PartOfSpeech> posToBeInflected) {
        for (Inflection inflection : inflections) {
            if (!Collections.disjoint(posToBeInflected, inflection.getGrammemeSet())) {
                return true;
            }
        }
        return false;
    }

    // Given lemma suffix and surface form suffixes, return either an existing
    // inflection pattern or return a new one while adding to the existing
    // inflection patterns
    private InflectionPattern getInflectionPattern(Lemma lemma, String lemmaSuffix,
            List<Inflection> suffixes) {
        TreeSet<Enum<?>> newGrammemeList = new TreeSet<>(lemma.grammemes);

        InflectionPattern inflectionPattern = new InflectionPattern(
                documentState.inflectionPatterns.size() + 1,
                lemmaSuffix,
                newGrammemeList,
                suffixes);

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
            for (int i = 0; i < inflections.size(); i++) {
                var inflection = inflections.get(i);
                var inflectionGrammemes = inflection.getGrammemeSet();
                if (!inflectionGrammemes.isEmpty() && !InflectionPattern.isIgnorableGrammemeSet(inflectionGrammemes)) {
                    nonEmptyInflections.add(inflection);
                    nonEmptyInflectionIndices.add(i);
                }
                inflectionGrammemes.addAll(lemma.grammemes);
            }
            // If all inflections are empty then add all significant inflections to the
            // pattern
            if (nonEmptyInflectionIndices.isEmpty()) {
                for (int i = 0; i < inflections.size(); i++) {
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
                        suffixes);
            }
            // else ignore this unimportant inflection pattern. This is usually trimmed for
            // size.
        }
        for (int i = 0; i < inflections.size(); i++) {
            var inflection = inflections.get(i);
            String phrase = inflection.getInflection();
            InflectionPattern inflectionPatternForDict = nonEmptyInflectionIndices.contains(i) ? inflectionPattern
                    : null;
            documentState.addDictionaryEntry(new DictionaryEntry(phrase, phrase, lemma.isRare,
                    inflection.getGrammemeSet(), inflectionPatternForDict));
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
            } else {
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

    private void addGrammeme(TreeSet<Enum<?>> grammemes, @Nullable String grammeme) {
        if (grammeme != null && !grammeme.isEmpty()) {
            Enum<?> value = Grammar.DEFAULTMAP.get(grammeme);
            if (value == null) {
                throw new NullPointerException(grammeme + " is not a known grammeme");
            } else if (!value.equals(Ignorable.IGNORABLE_PROPERTY)) {
                grammemes.add(value);
            }
        }
    }

    private void mergeAdditionalGrammemes() {
        // Add any entries that are missing. The actual properties will be added
        // elsewhere.
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
    public static boolean isContained(String baseLanguage, String variantLanguage) {
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
            documentState.dictionary.computeIfAbsent(lemma.value,
                    d -> new DictionaryEntry(lemma.value, lemma.isRare, lemma.grammemes, null));
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
        LexemesJsonDeserializer.setLanguage(parserOptions.locales);

        // We create InputSource directly due to an occasional bugs with UTF-8 files
        // being interpreted as malformed UTF-8.
        // We use a large buffer because we're reading a large file, and we're
        // frequently reading file data.
        for (String sourceFilename : parserOptions.sourceFilenames) {
            try (InputStream fileInputStream = new FileInputStream(sourceFilename)) {
                InputStream inputStream = fileInputStream;
                if (sourceFilename.endsWith(".bz2")) {
                    System.err.println("Warning: Consider providing the decompressed file for faster parsing.");
                    inputStream = new BZip2CompressorInputStream(new BufferedInputStream(inputStream, 32768));
                }
                try (JsonParser parser = objectMapper.createParser(inputStream)) {
                    JsonToken currToken;
                    while ((currToken = parser.nextToken()) != JsonToken.START_OBJECT && currToken != null) {
                        // Find the first object in the array.
                    }
                    if (currToken == JsonToken.START_OBJECT) {
                        do {
                            Lexeme lexeme = objectMapper.readValue(parser, Lexeme.class);
                            try {
                                lexParser.analyzeLexeme(parser.currentLocation().getLineNr(), lexeme);
                            } catch (IllegalArgumentException e) {
                                lexParser.documentState.unusableLemmaCount++;
                                System.err.println(
                                        "Line " + parser.currentLocation().getLineNr() + ": " + e.getMessage());
                            }
                        } while (parser.nextToken() != JsonToken.END_ARRAY);
                        lexParser.processAndMergeLexemes();
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
