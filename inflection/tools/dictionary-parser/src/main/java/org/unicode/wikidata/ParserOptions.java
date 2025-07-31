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
import java.util.EnumMap;
import java.util.EnumSet;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.regex.Pattern;

import static org.unicode.wikidata.Grammar.DEFAULTMAP;
import static org.unicode.wikidata.Grammar.TYPEMAP;

/**
 * The options to extract the data from the data source.
 */
final class ParserOptions {
    private static final char COLON_SEPARATOR = ':';
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
    static final String TIMESTAMP = "--timestamp";
    static final String ADD_SOUND = "--add-sound";

    boolean includeLemmasWithoutWords = false;
    boolean debug = false;
    final boolean addSound;

    EnumSet<Grammar.PartOfSpeech> posToBeInflected;
    TreeMap<TreeSet<Enum<?>>, List<TreeSet<Enum<?>>>> expandGramemes;
    TreeMap<String, TreeSet<String>> additionalGrammemesDict;
    TreeMap<String, EnumMap<Grammar.Sound, Pattern>> claimsToSound;

    ArrayList<String> sourceFilenames;
    String inflectionalFilename = ParserDefaults.DEFAULT_INFLECTION_FILE_NAME;
    String lexicalDictionaryFilename = ParserDefaults.DEFAULT_DICTIONARY_FILE_NAME;
    ArrayList<String> locales = new ArrayList<>(List.of(Locale.ENGLISH.getLanguage()));
    List<String> optionsUsedToInvoke = new ArrayList<>();

    private static void printUsage() {
        System.err.println("Usage: ParseLexicon [OPTIONS] <file1.xml or file1.lexicon>[ <file2.xml or file2.lexicon> ...]");
        System.err.println("\nOPTIONS");
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
        System.err.println(ADD_SOUND + " grammeme1[,grammeme2,...]\tSound properties to check for.");
    }

    ParserOptions(String[] args) throws IOException {
        posToBeInflected = EnumSet.of(Grammar.PartOfSpeech.NOUN);
        additionalGrammemesDict = new TreeMap<>();
        sourceFilenames = new ArrayList<>();
        claimsToSound = new TreeMap<>();

        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (ParserOptions.INFLECTIONS_FILE.equals(arg)) {
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
                TYPEMAP.put(split[0], toEnumSet(split[1]));

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
                    posToBeInflected.add(Grammar.PartOfSpeech.valueOf(pos.toUpperCase().replace('-', '_')));
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
                        var enumMap = new EnumMap<Grammar.Sound, Pattern>(Grammar.Sound.class);
                        for (var entry : soundRegexes.entrySet()) {
                            var key = (String) entry.getKey();
                            if (additionalSoundProperties.contains(key)) {
                                enumMap.put(Grammar.Sound.valueOf(key.toUpperCase(Locale.ROOT).replace('-', '_')), Pattern.compile((String) entry.getValue()));
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

    void setIgnoreProperty(String[] grammemes, Grammar.Ignorable ignorable) {
        var ignorableSet = EnumSet.of(ignorable);
        for (String grammeme : grammemes) {
            if (grammeme.matches("Q\\d*")) {
                TYPEMAP.put(grammeme, ignorableSet);
            } else {
                for (Map.Entry<String, Set<? extends Enum<?>>> entry : TYPEMAP.entrySet()) {
                    for (var grammemeEnum : entry.getValue()) {
                        String name = grammemeEnum.name();
                        if (name.equalsIgnoreCase(grammeme)) {
                            if (entry.getValue().size() == 1) {
                                entry.setValue(ignorableSet);
                            } else {
                                entry.getValue().remove(grammemeEnum);
                                ArrayList<Enum<?>> clone = new ArrayList<>(entry.getValue());
                                clone.add(ignorable);
                                entry.setValue(new HashSet<>(clone));
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    TreeSet<Enum<?>> toEnumSet(String grammemes) {
        TreeSet<Enum<?>> grammemeSet = new TreeSet<>(Inflection.ENUM_COMPARATOR);
        for (var grammeme : grammemes.split(",")) {
            var grammemeEnum = DEFAULTMAP.get(grammeme);
            if (grammemeEnum == null) {
                throw new NullPointerException(grammeme + " is not a valid grammeme");
            }
            grammemeSet.add(grammemeEnum);
        }
        return grammemeSet;
    }
}
