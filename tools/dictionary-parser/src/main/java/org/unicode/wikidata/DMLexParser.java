/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.exc.MismatchedInputException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.annotation.Nullable;

import static org.unicode.wikidata.Grammar.Gender;
import static org.unicode.wikidata.Grammar.Ignorable;
import static org.unicode.wikidata.Grammar.PartOfSpeech;

/**
 * Parses OASIS DMLex v1.0 JSON lexicographic resources (spec §5.2, {@code dmlex.schema.json}),
 * resolves tags via the Controlled Values module ({@code sameAs} Wikidata entity URIs), and feeds
 * normalized {@link ParseWikidata.Lemma} entries into {@link ParseWikidata}.
 */
final class DMLexParser {
    /**
     * DMLex Controlled Values map tags to external inventories using full URIs in {@code sameAs}
     * (e.g. {@code "http://www.wikidata.org/entity/Q1084"}), whereas {@link Grammar#TYPEMAP} is
     * keyed by bare Wikidata Q-IDs (e.g. {@code "Q1084"}). This pattern extracts the trailing
     * {@code Q<digits>} entity ID from a Wikidata entity URI.
     */
    private static final Pattern WIKIDATA_ENTITY_PATTERN = Pattern.compile("entity/(Q\\d+)$");

    private final ParseWikidata parseWikidata;

    DMLexParser(ParseWikidata parseWikidata) {
        this.parseWikidata = parseWikidata;
    }

    void parseDMLexStream(
            ObjectMapper objectMapper,
            JsonParser parser,
            ParserOptions.SourceInput sourceInput) throws IOException {
        String sourceFilename = sourceInput.filename();
        DMLexEntry.Lexicon lexicon;
        try {
            lexicon = objectMapper
                    .readerFor(DMLexEntry.Lexicon.class)
                    .with(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES)
                    .readValue(parser);
        } catch (MismatchedInputException e) {
            throw MismatchedInputException.from(
                    parser,
                    e.getTargetType(),
                    sourceFilename + ": " + e.getOriginalMessage());
        }
        if (lexicon == null || lexicon.langCode == null || lexicon.langCode.isBlank() || lexicon.entries == null) {
            throw MismatchedInputException.from(
                    parser,
                    DMLexEntry.Lexicon.class,
                    "Invalid DMLex file (missing required 'langCode' or 'entries'): " + sourceFilename);
        }
        Map<String, List<String>> posTagSameAsMap = new HashMap<>();
        if (lexicon.partOfSpeechTags != null) {
            for (DMLexEntry.PartOfSpeechTag posTag : lexicon.partOfSpeechTags) {
                if (posTag == null || posTag.tag == null || posTag.tag.isBlank()) {
                    throw new IllegalArgumentException(sourceFilename + ": Missing tag in partOfSpeechTags");
                }
                if (posTagSameAsMap.containsKey(posTag.tag)) {
                    throw new IllegalArgumentException(
                            sourceFilename + ": Duplicate partOfSpeechTag '" + posTag.tag + "'");
                }
                posTagSameAsMap.put(posTag.tag, posTag.sameAs);
            }
        }
        Map<String, List<String>> formTagSameAsMap = new HashMap<>();
        if (lexicon.inflectedFormTags != null) {
            for (DMLexEntry.InflectedFormTag formTag : lexicon.inflectedFormTags) {
                if (formTag == null || formTag.tag == null || formTag.tag.isBlank()) {
                    throw new IllegalArgumentException(sourceFilename + ": Missing tag in inflectedFormTags");
                }
                if (formTagSameAsMap.containsKey(formTag.tag)) {
                    throw new IllegalArgumentException(
                            sourceFilename + ": Duplicate inflectedFormTag '" + formTag.tag + "'");
                }
                formTagSameAsMap.put(formTag.tag, formTag.sameAs);
            }
        }
        Map<String, List<String>> labelTagSameAsMap = new HashMap<>();
        if (lexicon.labelTags != null) {
            for (DMLexEntry.LabelTag labelTag : lexicon.labelTags) {
                if (labelTag == null || labelTag.tag == null || labelTag.tag.isBlank()) {
                    throw new IllegalArgumentException(sourceFilename + ": Missing tag in labelTags");
                }
                if (labelTagSameAsMap.containsKey(labelTag.tag)) {
                    throw new IllegalArgumentException(
                            sourceFilename + ": Duplicate labelTag '" + labelTag.tag + "'");
                }
                labelTagSameAsMap.put(labelTag.tag, labelTag.sameAs);
            }
        }
        Map<String, Set<Enum<?>>> resolvedPosTagCache = new HashMap<>();
        Map<String, Set<Enum<?>>> resolvedFormTagCache = new HashMap<>();
        Map<String, Set<Enum<?>>> resolvedLabelTagCache = new HashMap<>();
        ParseWikidata.Lemma lemma = new ParseWikidata.Lemma();
        for (DMLexEntry entry : lexicon.entries) {
            analyzeDMLexEntry(
                    lemma,
                    entry,
                    lexicon.langCode,
                    sourceFilename,
                    sourceInput.sourceIndex(),
                    posTagSameAsMap,
                    formTagSameAsMap,
                    labelTagSameAsMap,
                    resolvedPosTagCache,
                    resolvedFormTagCache,
                    resolvedLabelTagCache);
        }
        for (var posEntry : posTagSameAsMap.entrySet()) {
            resolvedPosTagCache.computeIfAbsent(
                    posEntry.getKey(),
                    k -> resolveSameAsGrammemes(
                            posEntry.getValue(), sourceFilename, "<partOfSpeechTags>", k,
                            "partOfSpeechTag", k));
        }
        for (var formEntry : formTagSameAsMap.entrySet()) {
            resolvedFormTagCache.computeIfAbsent(
                    formEntry.getKey(),
                    k -> resolveSameAsGrammemes(
                            formEntry.getValue(), sourceFilename, "<inflectedFormTags>", k,
                            "inflectedFormTag", k));
        }
        for (var labelEntry : labelTagSameAsMap.entrySet()) {
            resolvedLabelTagCache.computeIfAbsent(
                    labelEntry.getKey(),
                    k -> resolveSameAsGrammemes(
                            labelEntry.getValue(), sourceFilename, "<labelTags>", k,
                            "labelTag", k));
        }
    }

    private void analyzeDMLexEntry(
            ParseWikidata.Lemma lemma,
            DMLexEntry entry,
            String lexiconLangCode,
            String sourceFilename,
            int sourceIndex,
            Map<String, List<String>> posTagSameAsMap,
            Map<String, List<String>> formTagSameAsMap,
            Map<String, List<String>> labelTagSameAsMap,
            Map<String, Set<Enum<?>>> resolvedPosTagCache,
            Map<String, Set<Enum<?>>> resolvedFormTagCache,
            Map<String, Set<Enum<?>>> resolvedLabelTagCache) {
        if (entry == null) {
            throw new IllegalArgumentException(sourceFilename + ": Null DMLex entry");
        }
        String entryId = (entry.id != null && !entry.id.isBlank()) ? entry.id : "<unknown>";
        if (entry.headword == null || entry.headword.isBlank()) {
            throw new IllegalArgumentException(
                    sourceFilename + " (" + entryId + "): Missing headword for DMLex entry");
        }
        if (!LexemesJsonDeserializer.isContained(lexiconLangCode)) {
            return;
        }
        if (parseWikidata.omitLemmas.contains(entryId)) {
            return;
        }
        lemma.reset();
        parseWikidata.documentState.lemmaCount++;
        lemma.value = entry.headword;

        int qVariantIdx = lexiconLangCode.indexOf(ParseWikidata.VARIANT_SEPARATOR);
        if (qVariantIdx >= 0) {
            var additionalCategory = lexiconLangCode.substring(qVariantIdx + ParseWikidata.VARIANT_SEPARATOR.length());
            var variant = Grammar.getMappedGrammemes(additionalCategory);
            if (variant == null) {
                throw new IllegalArgumentException(
                        sourceFilename + " (" + entryId + " / " + lemma.value + "): " + additionalCategory
                                + " is not a known grammeme for the language variant");
            }
            if (variant.contains(Ignorable.IGNORABLE_INFLECTION)) {
                return;
            }
            lemma.grammemes.addAll(variant);
        }

        int resolvedPosCount = 0;
        if (entry.partsOfSpeech != null) {
            for (String posTag : entry.partsOfSpeech) {
                if (posTag == null || !posTagSameAsMap.containsKey(posTag)) {
                    throw new IllegalArgumentException(
                            sourceFilename + " (" + entryId + " / " + lemma.value + "): Undefined partOfSpeech tag '"
                                    + posTag + "'");
                }
                Set<Enum<?>> mappedPos = resolvedPosTagCache.computeIfAbsent(
                        posTag,
                        k -> resolveSameAsGrammemes(
                                posTagSameAsMap.get(k), sourceFilename, entryId, lemma.value, "partOfSpeechTag", k));
                for (Enum<?> g : mappedPos) {
                    if (g instanceof PartOfSpeech) {
                        resolvedPosCount++;
                    }
                }
                lemma.grammemes.addAll(mappedPos);
            }
        }
        if (parseWikidata.rareLemmas.contains(entryId)) {
            lemma.grammemes.add(Grammar.Usage.RARE);
        }
        if (entry.labels != null) {
            for (String labelTag : entry.labels) {
                if (labelTag == null || !labelTagSameAsMap.containsKey(labelTag)) {
                    throw new IllegalArgumentException(
                            sourceFilename + " (" + entryId + " / " + lemma.value + "): Undefined label tag '"
                                    + labelTag + "'");
                }
                Set<Enum<?>> mappedLabel = resolvedLabelTagCache.computeIfAbsent(
                        labelTag,
                        k -> resolveSameAsGrammemes(
                                labelTagSameAsMap.get(k), sourceFilename, entryId, lemma.value, "labelTag", k));
                for (Enum<?> g : mappedLabel) {
                    if (g instanceof PartOfSpeech) {
                        resolvedPosCount++;
                    }
                }
                lemma.grammemes.addAll(mappedLabel);
            }
        }
        if (entry.pronunciations != null && !entry.pronunciations.isEmpty()) {
            List<String> ipaTexts = extractIpaTranscriptions(
                    entry.pronunciations, sourceFilename, entryId, lemma.value);
            if (parseWikidata.parserOptions.addSound && !ipaTexts.isEmpty()) {
                parseWikidata.addSound("P898", ipaTexts, lemma.grammemes, entryId, lemma.value);
            }
        }

        if (lemma.grammemes.contains(Ignorable.IGNORABLE_LEMMA)
                || lemma.grammemes.contains(Ignorable.IGNORABLE_INFLECTION)) {
            parseWikidata.documentState.unusableLemmaCount++;
            return;
        }
        lemma.grammemes.remove(Ignorable.IGNORABLE_PROPERTY);

        int posCount = Math.max(resolvedPosCount, parseWikidata.countGrammemeType(lemma.grammemes, PartOfSpeech.class));
        if (parseWikidata.countGrammemeType(lemma.grammemes, PartOfSpeech.class) != 1 || resolvedPosCount != 1) {
            throw new IllegalArgumentException(
                    sourceFilename + " (" + entryId + " / " + lemma.value
                            + "): Expected exactly 1 part of speech, found " + posCount);
        }

        TreeSet<Enum<?>> genderlessLemmaGrammemes = null;
        if (parseWikidata.countGrammemeType(lemma.grammemes, Gender.class) > 0) {
            genderlessLemmaGrammemes = new TreeSet<>(lemma.grammemes);
            parseWikidata.removeGrammemeType(genderlessLemmaGrammemes, Gender.class);
        }

        List<DMLexInflectedForm> forms = entry.inflectedForms;
        boolean synthesizedDefaultForm = false;
        if (forms == null || forms.isEmpty()) {
            DMLexInflectedForm defaultForm = new DMLexInflectedForm();
            defaultForm.text = entry.headword;
            forms = List.of(defaultForm);
            synthesizedDefaultForm = true;
        } else {
            boolean hasHeadwordForm = false;
            for (DMLexInflectedForm form : forms) {
                if (form != null && entry.headword.equals(form.text)) {
                    hasHeadwordForm = true;
                    break;
                }
            }
            if (!hasHeadwordForm) {
                throw new IllegalArgumentException(
                        sourceFilename + " (" + entryId + " / " + lemma.value
                                + "): inflectedForms must include the headword '" + entry.headword + "'");
            }
        }

        for (DMLexInflectedForm form : forms) {
            if (form == null || form.text == null || form.text.isBlank()) {
                throw new IllegalArgumentException(
                        sourceFilename + " (" + entryId + " / " + lemma.value + "): Missing inflectedForm text");
            }
            Inflection currentInflection = new Inflection(form.text);
            if (!synthesizedDefaultForm || form.tag != null) {
                if (form.tag == null || form.tag.isBlank()) {
                    throw new IllegalArgumentException(
                            sourceFilename + " (" + entryId + " / " + lemma.value
                                    + "): Missing inflectedForm tag for form '" + form.text + "'");
                }
                if (!formTagSameAsMap.containsKey(form.tag)) {
                    throw new IllegalArgumentException(
                            sourceFilename + " (" + entryId + " / " + lemma.value
                                    + "): Undefined inflectedForm tag '" + form.tag + "'");
                }
                Set<Enum<?>> mappedFormTag = resolvedFormTagCache.computeIfAbsent(
                        form.tag,
                        k -> resolveSameAsGrammemes(
                                formTagSameAsMap.get(k), sourceFilename, entryId, lemma.value, "inflectedFormTag", k));
                currentInflection.grammemeSet.addAll(mappedFormTag);
            }
            if (form.labels != null) {
                for (String labelTag : form.labels) {
                    if (labelTag == null || !labelTagSameAsMap.containsKey(labelTag)) {
                        throw new IllegalArgumentException(
                                sourceFilename + " (" + entryId + " / " + lemma.value
                                        + "): Undefined label tag '" + labelTag + "'");
                    }
                    Set<Enum<?>> mappedLabel = resolvedLabelTagCache.computeIfAbsent(
                            labelTag,
                            k -> resolveSameAsGrammemes(
                                    labelTagSameAsMap.get(k), sourceFilename, entryId, lemma.value, "labelTag", k));
                    currentInflection.grammemeSet.addAll(mappedLabel);
                }
            }
            if (form.pronunciations != null && !form.pronunciations.isEmpty()) {
                List<String> ipaTexts = extractIpaTranscriptions(
                        form.pronunciations, sourceFilename, entryId, lemma.value);
                if (parseWikidata.parserOptions.addSound && !ipaTexts.isEmpty()) {
                    parseWikidata.addSound("P898", ipaTexts, currentInflection.grammemeSet, entryId, lemma.value);
                }
            }
            if (!parseWikidata.finalizeAndAddInflection(lemma, genderlessLemmaGrammemes, currentInflection)) {
                return;
            }
        }

        if (!parseWikidata.finalizeLemmaAfterForms(lemma)) {
            return;
        }
        parseWikidata.analyzeLemma(lemma, sourceIndex);
    }

    private static Set<Enum<?>> resolveSameAsGrammemes(
            List<String> sameAsUris,
            String sourceFilename,
            String entryId,
            String headword,
            String tagType,
            String tag) {
        if (sameAsUris == null || sameAsUris.isEmpty()) {
            throw new IllegalArgumentException(
                    sourceFilename + " (" + entryId + " / " + headword + "): Missing or empty sameAs mapping for "
                            + tagType + " '" + tag + "'");
        }
        Set<Enum<?>> resolved = new HashSet<>();
        for (String uri : sameAsUris) {
            if (uri == null || uri.isBlank()) {
                throw new IllegalArgumentException(
                        sourceFilename + " (" + entryId + " / " + headword + "): Empty sameAs URI in " + tagType + " '" + tag + "'");
            }
            Matcher matcher = WIKIDATA_ENTITY_PATTERN.matcher(uri.trim());
            Set<? extends Enum<?>> mapped = matcher.find() ? Grammar.getMappedGrammemes(matcher.group(1)) : null;
            if (mapped == null) {
                throw new IllegalArgumentException(
                        sourceFilename + " (" + entryId + " / " + headword + "): Unrecognized sameAs URI '" + uri
                                + "' for " + tagType + " '" + tag + "'");
            }
            resolved.addAll(mapped);
        }
        return resolved;
    }

    private static boolean isIpaScheme(@Nullable String scheme) {
        if (scheme == null || scheme.isBlank()) {
            return true;
        }
        for (String subtag : scheme.trim().split("[-_]")) {
            if ("fonipa".equalsIgnoreCase(subtag)) {
                return true;
            }
        }
        return false;
    }

    private static List<String> extractIpaTranscriptions(
            List<DMLexEntry.Pronunciation> pronunciations,
            String sourceFilename,
            String entryId,
            String headword) {
        List<String> ipaTexts = new ArrayList<>();
        for (DMLexEntry.Pronunciation pron : pronunciations) {
            if (pron == null) {
                throw new IllegalArgumentException(
                        sourceFilename + " (" + entryId + " / " + headword + "): Null pronunciation object");
            }
            if (pron.transcriptions == null) {
                continue;
            }
            for (DMLexEntry.Transcription transcription : pron.transcriptions) {
                if (transcription == null || transcription.text == null || transcription.text.isBlank()) {
                    throw new IllegalArgumentException(
                            sourceFilename + " (" + entryId + " / " + headword + "): Missing transcription text");
                }
                if (isIpaScheme(transcription.scheme)) {
                    ipaTexts.add(transcription.text);
                }
            }
        }
        return ipaTexts;
    }
}
