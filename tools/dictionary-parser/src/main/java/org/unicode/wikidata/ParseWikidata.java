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
import com.fasterxml.jackson.databind.exc.MismatchedInputException;

import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;
import org.apache.commons.lang3.StringUtils;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.EnumMap;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
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

        private Lemma() {
        }
    }

    private record StagedLemma(Lemma lemma, int sourceIndex) {}

    private final ParserOptions parserOptions;
    private final DocumentState documentState = new DocumentState();
    private final TreeSet<String> rareLemmas = new TreeSet<>();
    private final TreeSet<String> omitLemmas = new TreeSet<>();
    private final Map<String, List<String>> mergeMap = new HashMap<>();
    private final TreeSet<String> deferredLexemes = new TreeSet<>();
    private final Map<String, Lemma> deferredLemmaMap = new HashMap<>();
    // Holds normalized lemmas in encounter order when multi-source overwrite mode is active
    // (--merge-conflicts=false with >1 source file).
    private final List<StagedLemma> stagedLemmas = new ArrayList<>();
    // Tracks the highest sourceIndex that defines a given (lemma.value, PartOfSpeech) pair
    // so earlier paradigms for the same lemma and POS can be preempted before pattern generation.
    private final Map<String, EnumMap<PartOfSpeech, Integer>> highestSourceForLemmaPos = new HashMap<>();
    // Tracks the highest sourceIndex that defines a POS-less lemma for a given headword.
    private final Map<String, Integer> highestSourceForPoslessLemma = new HashMap<>();

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

    /**
     * Returns {@code true} when multiple input files are provided and {@code --merge-conflicts} is {@code false}
     * (the default). In this mode, later files on the CLI override conflicting entries of the same
     * {@link PartOfSpeech} from earlier files via a two-tier strategy:
     * <ul>
     *   <li><b>Tier 1 (Lemma/Paradigm level)</b>: {@link #recordOrAnalyzeLemma} stages lemmas and
     *       {@link #processStagedLemmas} strips any {@code (lemma.value, PartOfSpeech)} superseded by a later
     *       source before {@link #analyzeInflections} runs. This prevents removed inflected surface forms
     *       (e.g., an obsolete plural dropped in a patch file) from lingering as orphans in {@code dictionary.lst}
     *       and prevents superseded paradigms from inflating {@link InflectionPattern} counts in {@code inflectional.xml}.</li>
     *   <li><b>Tier 2 (Surface-form level)</b>: {@link DictionaryEntry#merge} resolves remaining surface-form
     *       collisions per {@link PartOfSpeech} using {@code sourceIndex}.</li>
     * </ul>
     */
    private boolean isMultiSourceOverwriteMode() {
        return !parserOptions.mergeConflicts && parserOptions.sourceInputs.size() > 1;
    }

    /**
     * Either analyzes a normalized lemma immediately (single-source or {@code --merge-conflicts=true}, using
     * {@code effectiveSourceIndex = 0} so all sources merge identically) or stages it for Tier 1
     * cross-source lemma preemption when {@link #isMultiSourceOverwriteMode()} is active.
     */
    private void recordOrAnalyzeLemma(Lemma lemma, int sourceIndex) {
        if (!isMultiSourceOverwriteMode()) {
            analyzeLemma(lemma, 0);
            return;
        }
        stagedLemmas.add(new StagedLemma(lemma, sourceIndex));
        boolean hasPos = false;
        for (Enum<?> g : lemma.grammemes) {
            if (g instanceof PartOfSpeech pos) {
                hasPos = true;
                highestSourceForLemmaPos
                        .computeIfAbsent(lemma.value, k -> new EnumMap<>(PartOfSpeech.class))
                        .merge(pos, sourceIndex, Math::max);
            }
        }
        if (!hasPos) {
            highestSourceForPoslessLemma.merge(lemma.value, sourceIndex, Math::max);
        }
    }

    /**
     * Executes Tier 1 lemma preemption across all staged lemmas after all source files have been read.
     * For each staged lemma, any {@link PartOfSpeech} (or POS-less reading) that appears in a later
     * source file for the same headword ({@code highestSource > sourceIndex}) is stripped from the
     * lemma and its inflections; if all of the lemma's parts of speech were superseded, the earlier
     * lemma is skipped entirely.
     */
    private void processStagedLemmas() {
        for (StagedLemma staged : stagedLemmas) {
            Lemma lemma = staged.lemma();
            int sourceIndex = staged.sourceIndex();
            EnumSet<PartOfSpeech> originalPos = EnumSet.noneOf(PartOfSpeech.class);
            for (Enum<?> g : lemma.grammemes) {
                if (g instanceof PartOfSpeech pos) {
                    originalPos.add(pos);
                }
            }
            if (originalPos.isEmpty()) {
                Integer highestPoslessSource = highestSourceForPoslessLemma.get(lemma.value);
                if (highestPoslessSource != null && highestPoslessSource > sourceIndex) {
                    // This POS-less lemma was superseded by a later POS-less entry for the same headword.
                    continue;
                }
            } else {
                EnumMap<PartOfSpeech, Integer> posMap = highestSourceForLemmaPos.get(lemma.value);
                if (posMap != null) {
                    EnumSet<PartOfSpeech> supersededPos = EnumSet.noneOf(PartOfSpeech.class);
                    for (PartOfSpeech pos : originalPos) {
                        Integer highestSource = posMap.get(pos);
                        if (highestSource != null && highestSource > sourceIndex) {
                            supersededPos.add(pos);
                        }
                    }
                    if (!supersededPos.isEmpty()) {
                        if (supersededPos.size() == originalPos.size()) {
                            // Every PartOfSpeech of this lemma was superseded by a later source.
                            continue;
                        }
                        lemma.grammemes.removeAll(supersededPos);
                        for (Inflection inflection : lemma.inflections) {
                            inflection.getGrammemeSet().removeAll(supersededPos);
                        }
                    }
                }
            }
            analyzeLemma(lemma, sourceIndex);
        }
    }

    /**
     * Finalizes a single surface-form inflection with lemma grammeme propagation, ignorable filtering,
     * and rarity extraction. Shared by both Wikidata and DMLex paths. {@code --expand-grammemes}
     * expansion is deferred to {@link #expandAndCountInflections(Lemma)} at the start of {@link #analyzeLemma}.
     *
     * @return {@code false} if the entire lemma must be aborted due to {@code IGNORABLE_LEMMA}; {@code true} otherwise.
     */
    private boolean finalizeAndAddInflection(
            Lemma lemma,
            @Nullable TreeSet<Enum<?>> genderlessLemmaGrammemes,
            Inflection currentInflection) {
        // 1. Propagate lemma-level grammemes to the surface form. If the surface form specifies its
        //    own Gender, inherit genderlessLemmaGrammemes so form-level Gender overrides lemma Gender.
        if (genderlessLemmaGrammemes == null || countGrammemeType(currentInflection.grammemeSet, Gender.class) == 0) {
            currentInflection.grammemeSet.addAll(lemma.grammemes);
        } else {
            currentInflection.grammemeSet.addAll(genderlessLemmaGrammemes);
        }
        // 2. Check for grammemes configured to discard the entire lemma or this specific surface form.
        if (currentInflection.grammemeSet.contains(Ignorable.IGNORABLE_LEMMA)) {
            documentState.unusableLemmaCount++;
            return false;
        }
        if (currentInflection.grammemeSet.contains(Ignorable.IGNORABLE_INFLECTION)) {
            documentState.unusableSurfaceFormCount++;
            return true;
        }
        // 3. Extract Usage.RARE into the boolean flag on Inflection and strip IGNORABLE_PROPERTY.
        currentInflection.rareUsage = currentInflection.grammemeSet.contains(Grammar.Usage.RARE);
        if (currentInflection.rareUsage) {
            currentInflection.grammemeSet.remove(Grammar.Usage.RARE);
        }
        currentInflection.grammemeSet.remove(Ignorable.IGNORABLE_PROPERTY);
        lemma.inflections.add(currentInflection);
        return true;
    }

    /**
     * Performs post-form lemma cleanup shared by Wikidata and DMLex: extracts lemma-level
     * {@link Grammar.Usage#RARE}, rejects lemmas with no usable surface forms, and strips {@link Gender}
     * from {@code lemma.grammemes} (since forms have already inherited gender and {@code <pos>}
     * elements in {@code inflectional.xml} do not include gender).
     */
    private boolean finalizeLemmaAfterForms(Lemma lemma) {
        lemma.isRare = lemma.grammemes.contains(Grammar.Usage.RARE);
        if (lemma.isRare) {
            lemma.grammemes.remove(Grammar.Usage.RARE);
        }
        if (lemma.inflections.isEmpty()) {
            documentState.unusableLemmaCount++;
            return false;
        }
        removeGrammemeType(lemma.grammemes, Gender.class);
        return true;
    }

    private void analyzeLexeme(int lineNumber, Lexeme lexeme, int sourceIndex) {
        if (omitLemmas.contains(lexeme.id)) {
            // We really don't want this junk.
            return;
        }
        Set<? extends Enum<?>> partOfSpeechSet = null;
        boolean addedToDeferredInThisLexeme = false;
        for (var lemmaEntry : lexeme.lemmas.entrySet()) {
            var currentLemmaLanguage = lemmaEntry.getKey();
            Lemma lemma = new Lemma();
            documentState.lemmaCount++;
            LexemeRepresentation lemmaRepresentation = lemmaEntry.getValue();
            lemma.value = lemmaRepresentation.value;
            int qVariantIdx = currentLemmaLanguage.indexOf(VARIANT_SEPARATOR);
            if (qVariantIdx >= 0) {
                // The languages can have wierd Q entry after the desired language.
                // A spelling variant is informative. Most of the rest are irrelevant.
                var additionalCategory = currentLemmaLanguage.substring(qVariantIdx + VARIANT_SEPARATOR.length());
                var variant = parserOptions.getMappedGrammemes(additionalCategory);
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
                partOfSpeechSet = parserOptions.getMappedGrammemes(lexeme.lexicalCategory);
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
                        if (LexemesJsonDeserializer.isContained(rep.getKey())) {
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
                if (!finalizeAndAddInflection(lemma, genderlessLemmaGrammemes, currentInflection)) {
                    return;
                }
            }
            if (!finalizeLemmaAfterForms(lemma)) {
                continue;
            }
            if (deferredLexemes.contains(lexeme.id)) {
                if (addedToDeferredInThisLexeme) {
                    Lemma existing = deferredLemmaMap.get(lexeme.id);
                    existing.inflections.addAll(lemma.inflections);
                    existing.isRare |= lemma.isRare;
                } else if (deferredLemmaMap.put(lexeme.id, lemma) != null) {
                    throw new IllegalStateException("Duplicate lexeme " + lexeme.id);
                } else {
                    addedToDeferredInThisLexeme = true;
                }
            } else {
                recordOrAnalyzeLemma(lemma, sourceIndex);
            }
        }
    }

    /**
     * Converts a single OASIS DMLex {@link DMLexEntry} into the internal {@link Lemma} representation
     * and routes it through the shared normalization and conflict-resolution pipeline.
     */
    private void analyzeDMLexEntry(int lineNumber, DMLexEntry entry, String lexiconLangCode, int sourceIndex) {
        // 1. Filter by language and --exclude-language options (allowing entry-level langCode override).
        String entryLang = (entry.langCode != null && !entry.langCode.isBlank()) ? entry.langCode : lexiconLangCode;
        if (!LexemesJsonDeserializer.isContained(entryLang)) {
            return;
        }
        String entryId = (entry.id != null && !entry.id.isBlank()) ? entry.id : "<unknown>";
        if (omitLemmas.contains(entryId)) {
            return;
        }
        documentState.lemmaCount++;
        if (entry.headword == null || entry.headword.isBlank()) {
            throw new IllegalArgumentException("Missing headword for DMLex entry " + entryId);
        }

        Lemma lemma = new Lemma();
        lemma.value = entry.headword;

        // 2. Extract any "-x-<variant>" subtag from the language code (matching Wikidata behavior).
        int qVariantIdx = entryLang.indexOf(VARIANT_SEPARATOR);
        if (qVariantIdx >= 0) {
            var additionalCategory = entryLang.substring(qVariantIdx + VARIANT_SEPARATOR.length());
            var variant = parserOptions.getMappedGrammemes(additionalCategory);
            if (variant == null) {
                if (parserOptions.debug) {
                    System.err.println("Line " + lineNumber + ": " + additionalCategory
                            + " is not a known grammeme for the language variant " + entryId + "(" + lemma.value + ")");
                }
                return;
            }
            if (variant.contains(Ignorable.IGNORABLE_INFLECTION)) {
                return;
            }
            lemma.grammemes.addAll(variant);
        }

        // 3. Map entry-level partOfSpeech, rarity filter, grammatical labels, and IPA pronunciations.
        if (entry.partOfSpeech != null) {
            for (String posStr : entry.partOfSpeech) {
                var mappedPos = parserOptions.getMappedGrammemes(posStr);
                if (mappedPos == null) {
                    throw new IllegalArgumentException(posStr
                            + " is not a known part of speech grammeme for " + entryId + "(" + lemma.value + ")");
                }
                lemma.grammemes.addAll(mappedPos);
            }
        }
        if (rareLemmas.contains(entryId)) {
            lemma.grammemes.add(Grammar.Usage.RARE);
        }
        if (entry.labels != null) {
            for (String labelStr : entry.labels) {
                var mappedLabel = parserOptions.getMappedGrammemes(labelStr);
                if (mappedLabel == null) {
                    throw new IllegalArgumentException(labelStr
                            + " is not a known grammeme for " + entryId + "(" + lemma.value + ")");
                }
                lemma.grammemes.addAll(mappedLabel);
            }
        }
        if (parserOptions.addSound && entry.pronunciations != null && !entry.pronunciations.isEmpty()) {
            addSound("P898", entry.pronunciations, lemma.grammemes, entryId, lemma.value);
        }

        if (lemma.grammemes.contains(Ignorable.IGNORABLE_LEMMA)
                || lemma.grammemes.contains(Ignorable.IGNORABLE_INFLECTION)) {
            documentState.unusableLemmaCount++;
            return;
        }
        lemma.grammemes.remove(Ignorable.IGNORABLE_PROPERTY);

        TreeSet<Enum<?>> genderlessLemmaGrammemes = null;
        if (countGrammemeType(lemma.grammemes, Gender.class) > 0) {
            genderlessLemmaGrammemes = new TreeSet<>(lemma.grammemes);
            removeGrammemeType(genderlessLemmaGrammemes, Gender.class);
        }

        // 4. If a DMLex entry has no explicit inflectedForms (e.g. an uninflected particle or noun
        //    entry listing only its headword), synthesize a single default form from the headword.
        List<DMLexInflectedForm> forms = entry.inflectedForms;
        if (forms == null || forms.isEmpty()) {
            DMLexInflectedForm defaultForm = new DMLexInflectedForm();
            defaultForm.value = entry.headword;
            forms = List.of(defaultForm);
        }

        // 5. Convert each DMLexInflectedForm and run shared form normalization.
        for (DMLexInflectedForm form : forms) {
            if (form == null || form.value == null || form.value.isBlank()) {
                throw new IllegalArgumentException("Missing inflected form value for " + entryId + "(" + lemma.value + ")");
            }
            Inflection currentInflection = new Inflection(form.value);
            if (form.labels != null) {
                for (String labelStr : form.labels) {
                    Set<? extends Enum<?>> values = parserOptions.getMappedGrammemes(labelStr);
                    if (values == null) {
                        throw new IllegalArgumentException(labelStr
                                + " is not a known grammeme for " + entryId + "(" + lemma.value + ")");
                    }
                    currentInflection.grammemeSet.addAll(values);
                }
            }
            if (parserOptions.addSound && form.pronunciations != null && !form.pronunciations.isEmpty()) {
                addSound("P898", form.pronunciations, currentInflection.grammemeSet, entryId, lemma.value);
            }
            if (!finalizeAndAddInflection(lemma, genderlessLemmaGrammemes, currentInflection)) {
                return;
            }
        }

        if (!finalizeLemmaAfterForms(lemma)) {
            return;
        }
        recordOrAnalyzeLemma(lemma, sourceIndex);
    }

    /**
     * Merges deferred Wikidata lexemes configured in {@code filter_<lang>.properties} at the end
     * of a Wikidata file stream. For the primary Wikidata dump ({@code sourceIndex == 0}), missing
     * partner IDs throw {@link IllegalArgumentException}. For supplemental Wikidata files
     * ({@code sourceIndex > 0}), whichever partner IDs are present in that file are merged and any
     * remaining deferred lemmas are still analyzed. Clears {@link #deferredLemmaMap} afterward so
     * deferred state does not bleed into subsequent files.
     */
    private void processAndMergeLexemes(int sourceIndex) {
        if (sourceIndex == 0) {
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
                recordOrAnalyzeLemma(lemma, sourceIndex);
            }
        } else {
            for (Map.Entry<String, List<String>> entry : mergeMap.entrySet()) {
                Lemma lemma = deferredLemmaMap.remove(entry.getKey());
                for (var value : entry.getValue()) {
                    Lemma lemmaToMerge = deferredLemmaMap.remove(value);
                    if (lemmaToMerge != null) {
                        if (lemma == null) {
                            lemma = lemmaToMerge;
                        } else {
                            lemma.inflections.addAll(lemmaToMerge.inflections);
                            lemma.isRare |= lemmaToMerge.isRare;
                        }
                    }
                }
                if (lemma != null) {
                    recordOrAnalyzeLemma(lemma, sourceIndex);
                }
            }
            for (Lemma remaining : deferredLemmaMap.values()) {
                recordOrAnalyzeLemma(remaining, sourceIndex);
            }
        }
        deferredLemmaMap.clear();
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
            Set<? extends Enum<?>> values = parserOptions.getMappedGrammemes(feature);
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
                var grammemeEnum = parserOptions.getMappedGrammemes(grammemeStr);
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
        var dataForClaim = parserOptions.claimsToSound.get(property);
        if (dataForClaim != null && !dataForClaim.isEmpty()) {
            for (var soundMatcher : dataForClaim.entrySet()) {
                for (var claim : claims) {
                    if (soundMatcher.getValue().matcher(claim).find()) {
                        grammemeSet.add(soundMatcher.getKey());
                        return;
                    }
                }
            }
            System.err.println("Unmatched property: " + id + "(" + lemma + "): \"" + dataForClaim + "\"");
        }
    }

    private static boolean validateStemLength(@Nonnull List<Inflection> inflections, int stemLength) {
        for (var inflectionOuter : inflections) {
            String suffix = inflectionOuter.getInflection().substring(stemLength);
            for (var inflectionInner : inflections) {
                var inflectionInnerStr = inflectionInner.getInflection();
                if (inflectionInnerStr.endsWith(suffix)
                        && ((inflectionInnerStr.length() - suffix.length()) < stemLength)) {
                    return false;
                }
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
        if (suffixes.isEmpty()) {
            // If there are no suffixes, the lemma wasn't ignored, but all inflections were ignored.
            // It's an abandoned lemma that won't be written.
            return null;
        }
        TreeSet<Enum<?>> newGrammemeList = new TreeSet<>(lemma.grammemes);
        // Remove grammemes that are irrelevant for inflection pattern matching.
        newGrammemeList.removeIf(g -> g instanceof Grammar.Sound
                || g instanceof Grammar.Ignorable
                || g instanceof Grammar.Alternate);

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

    private void analyzeInflections(Lemma lemma, List<Inflection> inputInflections, int sourceIndex) {
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
            documentState.addDictionaryEntry(new DictionaryEntry(phrase, lemma.value, lemma.isRare,
                    inflection.getGrammemeSet(), inflectionPatternForDict, sourceIndex));
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
                existingEntry.addExtraGrammemes(grammemes);
            }
        }
    }

    /**
     * Applies any {@code --expand-grammemes} rules to {@code lemma.inflections} and records the
     * resulting surface-form count in {@link DocumentState#incomingSurfaceForm}.
     *
     * <p>Running this at the start of {@link #analyzeLemma} (after Tier 1 preemption in
     * {@link #processStagedLemmas}) ensures that:
     * <ol>
     *   <li>lemmas completely superseded by a later source file do not inflate
     *       {@code incomingSurfaceForm}, and</li>
     *   <li>{@code --expand-grammemes} keys are matched against each inflection's surviving
     *       {@link PartOfSpeech} set after any superseded POS has been stripped.</li>
     * </ol>
     */
    private void expandAndCountInflections(Lemma lemma) {
        if (parserOptions.expandGramemes != null) {
            List<Inflection> expanded = new ArrayList<>(lemma.inflections.size());
            for (Inflection currentInflection : lemma.inflections) {
                var grammemeExpansion = parserOptions.expandGramemes.get(currentInflection.grammemeSet);
                if (grammemeExpansion == null) {
                    expanded.add(currentInflection);
                } else {
                    for (var grammemeSet : grammemeExpansion) {
                        var expandedInflection = new Inflection(
                                currentInflection.inflection,
                                currentInflection.rareUsage);
                        expandedInflection.grammemeSet.addAll(currentInflection.grammemeSet);
                        expandedInflection.grammemeSet.addAll(grammemeSet);
                        expanded.add(expandedInflection);
                    }
                }
            }
            lemma.inflections.clear();
            lemma.inflections.addAll(expanded);
        }
        documentState.incomingSurfaceForm += lemma.inflections.size();
    }

    private void analyzeLemma(Lemma lemma, int sourceIndex) {
        expandAndCountInflections(lemma);
        analyzeInflections(lemma, lemma.inflections, sourceIndex);

        if (parserOptions.includeLemmasWithoutWords) {
            documentState.dictionary.computeIfAbsent(lemma.value,
                    d -> new DictionaryEntry(lemma.value, lemma.value, lemma.isRare, lemma.grammemes, null, sourceIndex));
        }
    }

    public void writeInflectionPatterns(long startTime) throws FileNotFoundException {
        documentState.printDocument(parserOptions, startTime);
    }

    /**
     * Streams a Wikidata lexeme JSON array (`[ { "id": "L...", ... }, ... ]`), analyzing each
     * {@link Lexeme} one by one and running per-file deferred lexeme merges at the end of the array.
     */
    private void parseWikidataStream(ObjectMapper objectMapper, JsonParser parser, int sourceIndex) throws IOException {
        JsonToken currToken;
        while ((currToken = parser.nextToken()) != JsonToken.START_OBJECT && currToken != null) {
            // Find the first object in the array.
        }
        if (currToken == JsonToken.START_OBJECT) {
            do {
                Lexeme lexeme = objectMapper.readValue(parser, Lexeme.class);
                try {
                    analyzeLexeme(parser.currentLocation().getLineNr(), lexeme, sourceIndex);
                } catch (IllegalArgumentException e) {
                    documentState.unusableLemmaCount++;
                    System.err.println(
                            "Line " + parser.currentLocation().getLineNr() + ": " + e.getMessage());
                }
            } while (parser.nextToken() != JsonToken.END_ARRAY);
            processAndMergeLexemes(sourceIndex);
        }
    }

    /**
     * Streams an OASIS DMLex v1.0 JSON lexicon object (`{ "langCode": "...", "entries": [ ... ] }`),
     * validating that the root is a JSON object containing both required top-level {@code "langCode"}
     * and {@code "entries"} fields.
     *
     * <p>Entries are deserialized and analyzed in streaming fashion as they are encountered. In the
     * unusual case where {@code "entries"} appears before {@code "langCode"} in the JSON key order,
     * entries are buffered in {@code entriesBeforeLangCode} until {@code "langCode"} is read.
     */
    private void parseDMLexStream(
            ObjectMapper objectMapper,
            JsonParser parser,
            ParserOptions.SourceInput sourceInput) throws IOException {
        JsonToken firstToken = parser.nextToken();
        if (firstToken != JsonToken.START_OBJECT) {
            throw MismatchedInputException.from(
                    parser,
                    DMLexEntry.class,
                    "Expected JSON object at root of DMLex file: " + sourceInput.filename());
        }

        String lexiconLangCode = null;
        boolean sawEntries = false;
        record DeferredDMLexEntry(int lineNumber, DMLexEntry entry) {}
        List<DeferredDMLexEntry> entriesBeforeLangCode = new ArrayList<>();

        while (parser.nextToken() != JsonToken.END_OBJECT) {
            if (parser.currentToken() == null) {
                throw MismatchedInputException.from(
                        parser,
                        DMLexEntry.class,
                        "Unexpected end of DMLex file: " + sourceInput.filename());
            }
            String fieldName = parser.currentName();
            JsonToken valueToken = parser.nextToken();
            if ("langCode".equals(fieldName)) {
                if (valueToken != JsonToken.VALUE_STRING) {
                    throw MismatchedInputException.from(
                            parser,
                            String.class,
                            "Expected string for DMLex 'langCode' in " + sourceInput.filename());
                }
                String langVal = parser.getText();
                if (langVal != null && !langVal.isBlank()) {
                    lexiconLangCode = langVal;
                }
            } else if ("entries".equals(fieldName)) {
                if (valueToken != JsonToken.START_ARRAY) {
                    throw MismatchedInputException.from(
                            parser,
                            List.class,
                            "Expected array for DMLex 'entries' in " + sourceInput.filename());
                }
                sawEntries = true;
                while (parser.nextToken() != JsonToken.END_ARRAY) {
                    if (parser.currentToken() != JsonToken.START_OBJECT) {
                        throw MismatchedInputException.from(
                                parser,
                                DMLexEntry.class,
                                "Expected object in DMLex 'entries' array in " + sourceInput.filename());
                    }
                    int lineNr = parser.currentLocation().getLineNr();
                    DMLexEntry entry = objectMapper.readValue(parser, DMLexEntry.class);
                    if (lexiconLangCode != null) {
                        try {
                            analyzeDMLexEntry(lineNr, entry, lexiconLangCode, sourceInput.sourceIndex());
                        } catch (IllegalArgumentException e) {
                            documentState.unusableLemmaCount++;
                            System.err.println("Line " + lineNr + ": " + e.getMessage());
                        }
                    } else {
                        // "entries" appeared before "langCode" in the JSON object; defer until "langCode" is read.
                        entriesBeforeLangCode.add(new DeferredDMLexEntry(lineNr, entry));
                    }
                }
            } else if (valueToken == JsonToken.START_OBJECT || valueToken == JsonToken.START_ARRAY) {
                // Skip any other top-level metadata arrays/objects in the DMLex lexicon.
                parser.skipChildren();
            }
        }

        if (lexiconLangCode == null || !sawEntries) {
            throw MismatchedInputException.from(
                    parser,
                    DMLexEntry.class,
                    "Invalid DMLex file (missing required 'langCode' or 'entries'): " + sourceInput.filename());
        }

        for (DeferredDMLexEntry deferred : entriesBeforeLangCode) {
            try {
                analyzeDMLexEntry(deferred.lineNumber(), deferred.entry(), lexiconLangCode, sourceInput.sourceIndex());
            } catch (IllegalArgumentException e) {
                documentState.unusableLemmaCount++;
                System.err.println("Line " + deferred.lineNumber() + ": " + e.getMessage());
            }
        }
    }

    public static void main(String[] args) throws Exception {
        ParserOptions parserOptions = new ParserOptions(args);
        long startTime = System.currentTimeMillis();
        ObjectMapper objectMapper = new ObjectMapper()
                .configure(JsonParser.Feature.ALLOW_UNQUOTED_FIELD_NAMES, true)
                .configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        var lexParser = new ParseWikidata(parserOptions);
        LexemesJsonDeserializer.setLanguage(parserOptions.locales);
        LexemesJsonDeserializer.setExcludedLanguages(parserOptions.excludedLanguages);

        // We create InputSource directly due to an occasional bugs with UTF-8 files
        // being interpreted as malformed UTF-8.
        // We use a large buffer because we're reading a large file, and we're
        // frequently reading file data.
        // Iterate through all configured source files (--wikidata and --dmlex) in CLI order.
        for (ParserOptions.SourceInput sourceInput : parserOptions.sourceInputs) {
            String sourceFilename = sourceInput.filename();
            try (InputStream fileInputStream = new FileInputStream(sourceFilename)) {
                InputStream inputStream = fileInputStream;
                if (sourceFilename.endsWith(".bz2")) {
                    System.err.println("Warning: Consider providing the decompressed file for faster parsing.");
                    inputStream = new BZip2CompressorInputStream(new BufferedInputStream(inputStream, 32768));
                }
                try (JsonParser parser = objectMapper.createParser(inputStream)) {
                    if (sourceInput.format() == ParserOptions.SourceFormat.DMLEX) {
                        lexParser.parseDMLexStream(objectMapper, parser, sourceInput);
                    } else {
                        lexParser.parseWikidataStream(objectMapper, parser, sourceInput.sourceIndex());
                    }
                }
            }
        }

        // In multi-source overwrite mode (--merge-conflicts=false with >1 source file),
        // preempt superseded (lemma, PartOfSpeech) paradigms and analyze the surviving lemmas now.
        if (lexParser.isMultiSourceOverwriteMode()) {
            lexParser.processStagedLemmas();
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
