/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import com.ibm.icu.util.ULocale;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.regex.Pattern;

import static org.unicode.wikidata.Grammar.DEFAULTMAP;

/**
 * The options to extract the data from the data source.
 */
final class ParserOptions {
    enum SourceFormat {
        WIKIDATA,
        DMLEX
    }

    record SourceInput(String filename, SourceFormat format, int sourceIndex) {}

    private static final char COLON_SEPARATOR = ':';
    static final String WIKIDATA_FILE = "--wikidata";
    static final String DMLEX_FILE = "--dmlex";
    static final String MERGE_CONFLICTS = "--merge-conflicts";
    static final String INFLECTIONS_FILE = "--inflections";
    static final String DICTIONARY_FILE = "--dictionary";
    static final String MAP_GRAMMEME = "--map-grammeme";
    static final String ADD_EXTRA_GRAMMEMES = "--add-extra-grammemes";
    static final String EXPAND_GRAMMEMES = "--expand-grammemes";
    static final String INFLECTION_TYPES = "--inflection-types";
    static final String IGNORE_PROPERTY = "--ignore-property";
    static final String INCLUDE_LEMMAS_WITHOUT_WORD = "--include-lemmas-without-words";
    static final String IGNORE_SURFACE_FORM = "--ignore-entries-with-grammemes";
    static final String LANGUAGE_OPT = "--language";
    static final String EXCLUDE_LANGUAGE_OPT = "--exclude-language";
    static final String TIMESTAMP = "--timestamp";
    static final String ADD_SOUND = "--add-sound";

    boolean includeLemmasWithoutWords = false;
    boolean mergeConflicts = false;
    boolean debug = false;
    final boolean addSound;

    EnumSet<Grammar.PartOfSpeech> posToBeInflected;
    TreeMap<TreeSet<Enum<?>>, List<TreeSet<Enum<?>>>> expandGramemes;
    TreeMap<String, TreeSet<String>> additionalGrammemesDict;
    TreeMap<String, LinkedHashMap<Grammar.Sound, Pattern>> claimsToSound;
    // Per-run string-to-grammeme overrides configured via --map-grammeme or Q-ID arguments to
    // --ignore-property / --ignore-entries-with-grammemes. Stored per ParserOptions instance so
    // static Grammar.TYPEMAP is never mutated in place.
    private final Map<String, Set<? extends Enum<?>>> customTypeMap = new HashMap<>();
    // Per-run enum overrides configured via --ignore-property or --ignore-entries-with-grammemes
    // when specified by grammeme name (e.g. "countable", "shortForm"). Any resolved grammeme enum
    // present in this map is replaced with its configured Grammar.Ignorable token during lookup.
    private final Map<Enum<?>, Grammar.Ignorable> ignoredGrammemes = new HashMap<>();

    ArrayList<String> sourceFilenames;
    ArrayList<SourceInput> sourceInputs;
    String inflectionalFilename = ParserDefaults.DEFAULT_INFLECTION_FILE_NAME;
    String lexicalDictionaryFilename = ParserDefaults.DEFAULT_DICTIONARY_FILE_NAME;
    ArrayList<String> locales = new ArrayList<>(List.of(Locale.ENGLISH.getLanguage()));
    ArrayList<String> excludedLanguages = new ArrayList<>();
    List<String> optionsUsedToInvoke = new ArrayList<>();

    private static void printUsage() {
        System.err.println("Usage: ParseLexicon [OPTIONS] (--wikidata <file1> [<file2> ...] | --dmlex <file1> [<file2> ...]) ...");
        System.err.println("\nOPTIONS");
        System.err.println(WIKIDATA_FILE + " <file1> [<file2> ...]\tOne or more Wikidata lexeme JSON/bz2 files (space- or comma-separated)");
        System.err.println(DMLEX_FILE + " <file1> [<file2> ...]\tOne or more OASIS DMLex JSON files (space- or comma-separated)");
        System.err.println(MERGE_CONFLICTS + "[={true|false}]\tWhen true, merge conflicting cross-file entries of the same part of speech instead of overwriting, default: false");
        System.err.println(INFLECTIONS_FILE + " <file.xml>\tthe file for the inflectional patterns to be generated, default: inflectional.xml");
        System.err.println(DICTIONARY_FILE + " <file.lst>\tthe file for the lexical dictionary to be generated, default: dictionary.lst");
        System.err.println(ADD_EXTRA_GRAMMEMES + " <file.lst>\tFile containing words with the extra grammemes to be added, provide path relative to tools/dictionary-parser/src/main/resources/org/unicode/wikidata/ (only to be used for a temporary grammeme addition)");
        System.err.println(EXPAND_GRAMMEMES + " grammeme1,grammeme2...:grammeme3,grammeme4...\tWhen the first set of grammemes are matched, add the additional set of grammemes.");
        System.err.println(INFLECTION_TYPES + " pos1[,pos2,...]\tthe pos's to be inflected, default: noun");
        System.err.println(MAP_GRAMMEME + " grammeme1,grammeme2\twhen grammeme1 is seen in the source dictionary, use grammeme2 instead of it");
        System.err.println(IGNORE_PROPERTY + " grammeme1[,grammeme2,...]\teach property is considered to be an ignorable property.");
        System.err.println(IGNORE_SURFACE_FORM + " type1[,type2,...]\tignore entries with specified grammemes. Default: do not ignore");
        System.err.println(INCLUDE_LEMMAS_WITHOUT_WORD + "\tinclude lemma entries which do not have corresponding word-entry. Default: do not include");
        System.err.println(TIMESTAMP + "\ttimestamp of the latest lexicon used. Default: NONE");
        System.err.println(LANGUAGE_OPT + "\tComma separated list of languages to extract to the lexical dictionary. Default: " + ULocale.ENGLISH.getName());
        System.err.println(EXCLUDE_LANGUAGE_OPT + " variant1[,variant2,...]\tComma separated list of exact Wikidata language/variant codes to exclude (e.g. ro-md, ar-x-Q775724), even if they would otherwise match " + LANGUAGE_OPT + ". Default: none");
        System.err.println(ADD_SOUND + " grammeme1[,grammeme2,...]\tSound properties to check for.");
    }

    ParserOptions(String[] args) throws IOException {
        posToBeInflected = EnumSet.of(Grammar.PartOfSpeech.NOUN);
        additionalGrammemesDict = new TreeMap<>();
        sourceFilenames = new ArrayList<>();
        sourceInputs = new ArrayList<>();
        claimsToSound = new TreeMap<>();

        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg == null) {
                printUsage();
                throw new IllegalArgumentException("Null argument at index " + i);
            }
            if (ParserOptions.WIKIDATA_FILE.equals(arg) || arg.startsWith(ParserOptions.WIKIDATA_FILE + "=")) {
                i = consumeSourceFiles(args, i, ParserOptions.WIKIDATA_FILE, SourceFormat.WIKIDATA);
            } else if (ParserOptions.DMLEX_FILE.equals(arg) || arg.startsWith(ParserOptions.DMLEX_FILE + "=")) {
                i = consumeSourceFiles(args, i, ParserOptions.DMLEX_FILE, SourceFormat.DMLEX);
            } else if (ParserOptions.MERGE_CONFLICTS.equals(arg) || arg.startsWith(ParserOptions.MERGE_CONFLICTS + "=")) {
                String valueStr;
                if (arg.startsWith(ParserOptions.MERGE_CONFLICTS + "=")) {
                    valueStr = arg.substring((ParserOptions.MERGE_CONFLICTS + "=").length()).trim();
                } else if (i + 1 < args.length && args[i + 1] != null
                        && ("true".equalsIgnoreCase(args[i + 1]) || "false".equalsIgnoreCase(args[i + 1]))) {
                    valueStr = args[++i].trim();
                } else {
                    valueStr = "true";
                }
                if ("true".equalsIgnoreCase(valueStr)) {
                    mergeConflicts = true;
                } else if ("false".equalsIgnoreCase(valueStr)) {
                    mergeConflicts = false;
                } else {
                    printUsage();
                    throw new IllegalArgumentException("Invalid value for " + ParserOptions.MERGE_CONFLICTS + ": " + valueStr);
                }
                optionsUsedToInvoke.add(ParserOptions.MERGE_CONFLICTS + "=" + mergeConflicts);
            } else if (ParserOptions.INFLECTIONS_FILE.equals(arg)) {
                inflectionalFilename = args[++i];
            } else if (ParserOptions.DICTIONARY_FILE.equals(arg)) {
                lexicalDictionaryFilename = args[++i];
            } else if (ParserOptions.ADD_EXTRA_GRAMMEMES.equals(arg)) {
                String additionalGrammemeFilename = args[++i];
                var resourceStream = getClass().getResourceAsStream(additionalGrammemeFilename);
                if (resourceStream == null) {
                    // else oh well. It doesn't matter.
                    continue;
                }
                try (BufferedReader br = new BufferedReader(new InputStreamReader(resourceStream, StandardCharsets.UTF_8))) {
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
                TreeSet<Enum<?>> mappedSet = toEnumSet(split[1]);
                customTypeMap.put(split[0], mappedSet);
                customTypeMap.put(Grammar.normalizeGrammemeKey(split[0]), mappedSet);

                optionsUsedToInvoke.add(ParserOptions.MAP_GRAMMEME);
                optionsUsedToInvoke.add(mapGrammeme);
            } else if (ParserOptions.IGNORE_PROPERTY.equals(arg)) {
                String propertySetToIgnore = args[++i];
                setIgnoreProperty(propertySetToIgnore.split(","), Grammar.Ignorable.IGNORABLE_PROPERTY);
                optionsUsedToInvoke.add(ParserOptions.IGNORE_PROPERTY);
                optionsUsedToInvoke.add(propertySetToIgnore);
            } else if (ParserOptions.EXPAND_GRAMMEMES.equals(arg)) {
                String mapGrammemes = args[++i];
                String[] split = mapGrammemes.split(":", 2);
                var key = toEnumSet(split[0]);
                var valueArray = new ArrayList<>(List.of(toEnumSet(split[1])));
                if (expandGramemes == null) {
                    expandGramemes = new TreeMap<>(GrammemeSetComparator.ENUM_COMPARATOR);
                }
                expandGramemes.merge(key, valueArray, (oldList, newList) -> {
                    oldList.addAll(newList);
                    return oldList;
                });
                optionsUsedToInvoke.add(ParserOptions.EXPAND_GRAMMEMES);
                optionsUsedToInvoke.add(mapGrammemes);
            } else if (ParserOptions.INFLECTION_TYPES.equals(arg)) {
                String inflectionTypes = args[++i];
                posToBeInflected.clear();

                for (String pos : inflectionTypes.split(",")) {
                    posToBeInflected.add(Grammar.PartOfSpeech.valueOf(
                            Grammar.normalizeGrammemeKey(pos).toUpperCase(Locale.ROOT).replace('-', '_')));
                }

                optionsUsedToInvoke.add(ParserOptions.INFLECTION_TYPES);
                optionsUsedToInvoke.add(inflectionTypes);
            } else if (ParserOptions.INCLUDE_LEMMAS_WITHOUT_WORD.equals(arg)) {
                includeLemmasWithoutWords = true;
                optionsUsedToInvoke.add(ParserOptions.INCLUDE_LEMMAS_WITHOUT_WORD);
            } else if (ParserOptions.IGNORE_SURFACE_FORM.equals(arg)) {
                String ignoreEntriesWithGrammemesStr = args[++i];
                setIgnoreProperty(ignoreEntriesWithGrammemesStr.split(","), Grammar.Ignorable.IGNORABLE_INFLECTION);
                optionsUsedToInvoke.add(ParserOptions.IGNORE_SURFACE_FORM);
                optionsUsedToInvoke.add(ignoreEntriesWithGrammemesStr);
            } else if (ParserOptions.TIMESTAMP.equals(arg)) {
                String timestamp = args[++i];
                optionsUsedToInvoke.add(ParserOptions.TIMESTAMP);
                optionsUsedToInvoke.add(timestamp);
            } else if (ParserOptions.LANGUAGE_OPT.equals(arg)) {
                String localeStr = args[++i];
                locales.clear();
                locales.addAll(List.of(localeStr.split(",")));
                optionsUsedToInvoke.add(ParserOptions.LANGUAGE_OPT);
                optionsUsedToInvoke.add(localeStr);
            } else if (ParserOptions.EXCLUDE_LANGUAGE_OPT.equals(arg)) {
                String excludeLanguageStr = args[++i];
                excludedLanguages.addAll(List.of(excludeLanguageStr.split(",")));
                optionsUsedToInvoke.add(ParserOptions.EXCLUDE_LANGUAGE_OPT);
                optionsUsedToInvoke.add(excludeLanguageStr);
            } else if (ParserOptions.ADD_SOUND.equals(arg)) {
                String soundGrammemeTypes = args[++i];

                List<String> additionalSoundProperties = Arrays.asList(soundGrammemeTypes.split(","));

                for (String claimID : ParseWikidata.PROPERTIES_WITH_PRONUNCIATION) {
                    Properties soundRegexes = new Properties();
                    var resourceStream = getClass().getResourceAsStream(claimID + ".properties");
                    if (resourceStream == null) {
                        // else oh well. It doesn't matter.
                        continue;
                    }
                    try (var propertiesStream = new InputStreamReader(resourceStream, StandardCharsets.UTF_8)) {
                        soundRegexes.load(propertiesStream);
                        // Preserve the order requested via --add-sound: addSound() stops at the first
                        // match, so this order is the match priority, not just insertion order.
                        var orderedMap = new LinkedHashMap<Grammar.Sound, Pattern>();
                        for (var key : additionalSoundProperties) {
                            var regex = soundRegexes.getProperty(key);
                            if (regex != null) {
                                orderedMap.put(Grammar.Sound.valueOf(key.toUpperCase(Locale.ROOT).replace('-', '_')), Pattern.compile(regex));
                            }
                        }
                        if (orderedMap.size() != additionalSoundProperties.size()) {
                            throw new IllegalArgumentException("Not all sound properties were found");
                        }
                        claimsToSound.put(claimID, orderedMap);
                    }
                }

                optionsUsedToInvoke.add(ParserOptions.ADD_SOUND);
                optionsUsedToInvoke.add(soundGrammemeTypes);
            } else {
                printUsage();
                throw new IllegalArgumentException("Unknown option: " + arg);
            }
        }

        addSound = !claimsToSound.isEmpty();

        if (sourceInputs.isEmpty()) {
            printUsage();
            throw new IllegalArgumentException("At least one input file must be specified via --wikidata or --dmlex");
        }
    }

    private int consumeSourceFiles(String[] args, int i, String flagPrefix, SourceFormat format) {
        String arg = args[i];
        int startCount = sourceInputs.size();
        if (arg.startsWith(flagPrefix + "=")) {
            String inlineVal = arg.substring((flagPrefix + "=").length());
            for (String part : inlineVal.split(",")) {
                String trimmed = part.trim();
                if (!trimmed.isEmpty()) {
                    sourceInputs.add(new SourceInput(trimmed, format, sourceInputs.size()));
                    sourceFilenames.add(trimmed);
                }
            }
        }
        while (i + 1 < args.length && args[i + 1] != null && !args[i + 1].startsWith("--")) {
            String rawArg = args[++i];
            for (String part : rawArg.split(",")) {
                String trimmed = part.trim();
                if (!trimmed.isEmpty()) {
                    sourceInputs.add(new SourceInput(trimmed, format, sourceInputs.size()));
                    sourceFilenames.add(trimmed);
                }
            }
        }
        if (sourceInputs.size() == startCount) {
            printUsage();
            throw new IllegalArgumentException(flagPrefix + " requires at least one file argument");
        }
        return i;
    }

    /**
     * Records per-run ignore rules from {@code --ignore-property} or {@code --ignore-entries-with-grammemes}.
     * Known grammeme names or single-enum shorthand aliases (e.g. {@code "countable"}, {@code "shortForm"},
     * {@code "masc"}) are resolved to their non-{@link Grammar.Ignorable} enum and recorded in
     * {@link #ignoredGrammemes} so any Q-ID or DMLex label mapping to that enum is replaced with
     * {@code ignorable} at lookup time. Raw Wikidata Q-IDs (e.g. {@code "Q55194613"}) and unmapped
     * DMLex tags (e.g. {@code "archaic"}, {@code "dialectal"}) are recorded in {@link #customTypeMap}.
     */
    void setIgnoreProperty(String[] grammemes, Grammar.Ignorable ignorable) {
        var ignorableSet = EnumSet.of(ignorable);
        for (String grammeme : grammemes) {
            Enum<?> enumVal = null;
            if (!grammeme.matches("Q\\d*")) {
                enumVal = DEFAULTMAP.get(grammeme);
                if (enumVal == null) {
                    enumVal = DEFAULTMAP.get(Grammar.normalizeGrammemeKey(grammeme));
                }
                if (enumVal == null) {
                    Set<? extends Enum<?>> mapped = Grammar.getMappedGrammemes(grammeme);
                    if (mapped != null && mapped.size() == 1) {
                        enumVal = mapped.iterator().next();
                    }
                }
            }
            if (enumVal != null && !(enumVal instanceof Grammar.Ignorable)) {
                ignoredGrammemes.put(enumVal, ignorable);
            } else {
                customTypeMap.put(grammeme, ignorableSet);
                customTypeMap.put(Grammar.normalizeGrammemeKey(grammeme), ignorableSet);
            }
        }
    }

    /**
     * Resolves a Wikidata Q-ID or DMLex grammatical label to its mapped {@link Grammar} enums,
     * applying any per-run CLI overrides ({@code --map-grammeme}, {@code --ignore-property},
     * {@code --ignore-entries-with-grammemes}) without mutating static {@link Grammar#TYPEMAP}.
     */
    Set<? extends Enum<?>> getMappedGrammemes(String grammeme) {
        if (grammeme == null) {
            return null;
        }
        // 1. Check per-run string overrides (--map-grammeme or Q-ID --ignore-* rules).
        Set<? extends Enum<?>> mapped = customTypeMap.get(grammeme);
        if (mapped == null && !customTypeMap.isEmpty()) {
            mapped = customTypeMap.get(Grammar.normalizeGrammemeKey(grammeme));
        }
        // 2. Fall back to the read-only static Grammar lookup table.
        if (mapped == null) {
            mapped = Grammar.getMappedGrammemes(grammeme);
        }
        if (mapped == null || ignoredGrammemes.isEmpty()) {
            return mapped;
        }
        // 3. Replace any resolved enum present in ignoredGrammemes with its Ignorable token.
        boolean hasIgnored = false;
        for (Enum<?> e : mapped) {
            if (ignoredGrammemes.containsKey(e)) {
                hasIgnored = true;
                break;
            }
        }
        if (!hasIgnored) {
            return mapped;
        }
        if (mapped.size() == 1) {
            return EnumSet.of(ignoredGrammemes.get(mapped.iterator().next()));
        }
        Set<Enum<?>> adjusted = new HashSet<>(mapped.size());
        for (Enum<?> e : mapped) {
            Grammar.Ignorable replacement = ignoredGrammemes.get(e);
            adjusted.add(replacement != null ? replacement : e);
        }
        return adjusted;
    }

    TreeSet<Enum<?>> toEnumSet(String grammemes) {
        TreeSet<Enum<?>> grammemeSet = new TreeSet<>(Inflection.ENUM_COMPARATOR);
        for (var grammeme : grammemes.split(",")) {
            var grammemeEnum = DEFAULTMAP.get(grammeme);
            if (grammemeEnum == null) {
                grammemeEnum = DEFAULTMAP.get(Grammar.normalizeGrammemeKey(grammeme));
            }
            if (grammemeEnum == null && !grammeme.matches("Q\\d*")) {
                Set<? extends Enum<?>> mapped = Grammar.getMappedGrammemes(grammeme);
                if (mapped != null && mapped.size() == 1) {
                    grammemeEnum = mapped.iterator().next();
                }
            }
            if (grammemeEnum == null) {
                throw new NullPointerException(grammeme + " is not a valid grammeme");
            }
            grammemeSet.add(grammemeEnum);
        }
        return grammemeSet;
    }
}
