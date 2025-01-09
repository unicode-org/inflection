/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package com.apple.lexicon;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.file.Paths;
import java.text.NumberFormat;
import java.util.Locale;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import javax.annotation.Nonnull;
import javax.annotation.Nullable;

import com.ibm.icu.util.ULocale;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import org.apache.commons.lang3.StringUtils;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
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

import static com.apple.lexicon.Grammar.TYPEMAP;
import static com.apple.lexicon.Grammar.REMAP;
import static com.apple.lexicon.Grammar.Ignorable;
import static com.apple.lexicon.Grammar.PartOfSpeech;
import static com.apple.lexicon.Grammar.LemmaCategory;
import static com.apple.lexicon.Grammar.VerbType;
import static com.apple.lexicon.Grammar.Case;
import static com.apple.lexicon.Grammar.Count;
import static com.apple.lexicon.Grammar.Gender;
import static com.apple.lexicon.Grammar.Animacy;
import static com.apple.lexicon.Grammar.Polarity;
import static com.apple.lexicon.Grammar.Miscellaneous;
import static com.apple.lexicon.Grammar.Sound;

/**
 * @see <a href="ssh://git@stash.sd.apple.com/nlp/lexicalresources.git">ssh://git@stash.sd.apple.com/nlp/lexicalresources.git</a>
 * @see <a href="https://cloud-lexi.apple.com/exports">https://cloud-lexi.apple.com/exports</a>
 */
final class EnumComparator implements Comparator<Enum<?>> {
    @Override
    public int compare(Enum<?> o1, Enum<?> o2) {
        return o1.toString().compareTo(o2.toString());
    }
}
final class ParserDefaults {
    static final TreeSet<String> wordMetaAttributes = Grammar.asTreeSet("gender", "case", "number", "animacy", "determination-type", "derivation-type");
    static final TreeSet<String> verbMetaAttributes = Grammar.asTreeSet("tense", "mood", "person", "voice", "verb-type");
    static final TreeSet<String> lemmaUsageAttributes = Grammar.asTreeSet("confirmed", "new", "rejected", "needs-review", "draft");
    static final TreeSet<String> chosenLemmaUsages = Grammar.asTreeSet("confirmed", "new");
    // FYI celebrity is the old deprecated category
    static final TreeSet<String> properNounCategories = Grammar.asTreeSet("appleproduct", "city", "country", "familyname", "femalepersonalname", "malepersonalname", "organization", "state", "waterway", "brandname", "celebrity", "publicfigure", "georegion");
    static final List<Enum<?>> LEMMA_GRAMMEMES = List.of(VerbType.INFINITIVE, Case.NOMINATIVE, Count.SINGULAR, Gender.MASCULINE, Animacy.ANIMATE);
    static final String RESOURCES_DIR = "src/main/resources/com/apple/Lexicon/";
    static final String DEFAULT_INFLECTION_FILE_NAME = "inflectional.xml";
    static final EnumComparator ENUM_COMPARATOR = new EnumComparator();
    // Put the rare inflections at the end.
    static final Comparator<Inflection> RARITY_AWARE_COMPARATOR = Comparator
            .comparing(Inflection::isRareUsage)
            .thenComparing(Inflection::compareTo);
}

final class ParserOptions {
    private static final char COLON_SEPARATOR = ':';
    static final String INFLECTIONS_FILE = "--inflections";
    static final String MAP_GRAMMEME = "--map-grammeme";
    static final String ADD_EXTRA_GRAMMEMES = "--add-extra-grammemes";
    static final String INFLECTION_TYPES = "--inflection-types";
    static final String ADD_LEMMAS_FOR_TYPES = "--add-lemma-forms-for-types";
    static final String IGNORE_GRAMMEMES_FOR_TYPES = "--ignore-grammemes-for-types";
    static final String PROPER_NOUN_CATEGORIES = "--proper-noun-categories";
    static final String IGNORE_PROPERTY_SET = "--ignore-property-set";
    static final String IGNORE_CATEGORIES = "--ignore-categories";
    static final String INCLUDE_LEMMAS_WITHOUT_WORD = "--include-lemmas-without-words";
    static final String USE_FORM_ATTRIBUTE = "--use-form-attribute";
    static final String IGNORE_AFFIXES = "--ignore-entries-with-affixes";
    static final String IGNORE_SURFACE_FORM = "--ignore-entries-with-grammemes";
    static final String IGNORE_UNANNOTATED_SURFACE_FORM = "--ignore-unannotated-entries";
    static final String LEMMAS_WITH_USAGE = "--lemmas-with-usage";
    static final String ADD_WORD_META_ATTRIBUTES = "--add-word-meta-attributes";
    static final String ADD_NORMALIZED_ENTRY = "--add-normalized-entry";
    static final String ADD_LEMMA_CATEGORY_GRAMMEMES = "--add-lemma-category-properties";
    static final String LOCALE = "--locale";
    static final String TIMESTAMP = "--timestamp";
    static final String DISABLE_NOUN_FOR_PROPER_NOUN = "--disable-adding-noun-for-proper-nouns";
    static final String ADD_DEFAULT_GRAMMEME_FOR_CATEGORY = "--add-default-grammeme-for-category";
    static final String IGNORE_UNSTRUCTURED_ENTRIES = "--ignore-unstructured-entries";
    static final String INCLUDE_WORD_WITH_SOURCES = "--include-word-with-sources";

    boolean includeLemmasWithoutWords = false;
    boolean ignoreUnannotated = false;
    boolean useFormAttribute = false;
    boolean includeLemmaForms = false;
    boolean ignoreAffixes = false;
    boolean addNormalizedEntry = false;
    boolean disableAddingPOSNounForProperNouns = false;
    boolean ignoreUnstructuredEntries = false;
    Set<String> propertiesToConsiderFromForm = null;
    final boolean additionalGrammemesToBeAdded;

    EnumSet<PartOfSpeech> posToBeInflected;
    EnumSet<PartOfSpeech> posToBeLemmatised;
    TreeSet<String> posWithoutGrammemes;
    TreeSet<String> properNounCategories;
    TreeSet<String> ignoredCategories;
    TreeSet<String> addedLemmaCategoryGrammemes;
    TreeSet<String> wordMetaAttributes;
    TreeSet<String> verbMetaAttributes;
    TreeMap<String, TreeSet<String>> additionalGrammemesDict;

    TreeMap<String, TreeMap<String, String>> defaultGrammemeForCategory;

    TreeSet<String> includedSources;

    ArrayList<String> sourceFilenames;
    String inflectionalFilename = ParserDefaults.DEFAULT_INFLECTION_FILE_NAME;
    ULocale locale = new ULocale("en_US");
    List<String> optionsUsedToInvoke = new ArrayList<>();

    private static void usage() throws Exception {
        System.err.println("Usage: ParseLexicon [OPTIONS] <file1.xml or file1.lexicon>[ <file2.xml or file2.lexicon> ...]");
        System.err.println("\nOPTIONS");
        System.err.println(INFLECTIONS_FILE + " <file.xml>\tthe file for the inflectional patterns to be generated, default: inflectional.xml");
        System.err.println(ADD_EXTRA_GRAMMEMES + " <file.lst>\tFile containing words with the extra grammemes to be added, provide path relative to tools/dictionary-parser/src/main/resources/com/apple/Lexicon/ (only to be used for a temporary grammeme addition)");
        System.err.println(INFLECTION_TYPES + " pos1[,pos2,...]\tthe pos's to be inflected, default: noun");
        System.err.println(ADD_LEMMAS_FOR_TYPES + " pos1[,pos2,...]\tthe part of speeches for which the known lemma form should be added to dict, default: (NONE)");
        System.err.println(IGNORE_GRAMMEMES_FOR_TYPES + " pos1[,pos2,...]\tthe part of speeches for which we don't want to include any grammeme info other than vowel/consonant start, default: (NONE)");
        System.err.println(PROPER_NOUN_CATEGORIES + " cat1[,cat2,...]\tthe proper-noun categories default: " + String.join(",", ParserDefaults.properNounCategories));
        System.err.println(MAP_GRAMMEME + " grammeme1,grammeme2\twhen grammeme1 is seen in the source dictionary, use grammeme2 instead of it");
        System.err.println(IGNORE_PROPERTY_SET + " grammeme1[,grammeme2,...]\tthis property set is considered to be an ignorable property.");
        System.err.println(IGNORE_CATEGORIES + " category1[,category2,...]\tignore these categories.");
        System.err.println(IGNORE_AFFIXES + "\tignore entries with affix-type=prefix. Default: do not ignore");
        System.err.println(IGNORE_SURFACE_FORM + " type1[,type2,...]\tignore entries with specified grammemes. Default: do not ignore");
        System.err.println(IGNORE_UNANNOTATED_SURFACE_FORM + " \tignore entries without any grammeme annotation. Default: do not ignore");
        System.err.println(LEMMAS_WITH_USAGE + " type1[,type2,...]\tonly choose lemmas having the specified usage attribute. Default: " + String.join(",", ParserDefaults.lemmaUsageAttributes));
        System.err.println(INCLUDE_LEMMAS_WITHOUT_WORD + "\tinclude lemma entries which do not have corresponding word-entry. Default: do not include");
        System.err.println(TIMESTAMP + "\ttimestamp of the latest lexicon used. Default: NONE");
        System.err.println(LOCALE + "\tlocale of the lexicon used. Default: en_US");
        System.err.println(ADD_WORD_META_ATTRIBUTES + "\tattr1[,attr2,....] Also add grammemes from these meta-attributes which are not included by default, attributes include by default are: " + String.join(",", ParserDefaults.wordMetaAttributes));
        System.err.println(ADD_NORMALIZED_ENTRY + "\tAdds the normalized entry of a dictionary as an additional dictionary entry, only applies for non lowercase entries. Default: false");
        System.err.println(DISABLE_NOUN_FOR_PROPER_NOUN + "\tDisables adding POS \"noun\" to proper nouns. Default: false");
        System.err.println(ADD_LEMMA_CATEGORY_GRAMMEMES + " lemmaCat1[,lemmaCat2,...]\tAdds lemma category grammeme to word within the provided list. Default: (NONE)");
        System.err.println(ADD_DEFAULT_GRAMMEME_FOR_CATEGORY + "[pos=partofSpeech1]category1=grammeme1[,category2=grammeme2.....]\t For each of the provided categories if no grammeme is present then add the default grammeme provided for that category to the word. Only applies for the provided parts of speech if pos= is supplied Default: (NONE)");
        System.err.println(IGNORE_UNSTRUCTURED_ENTRIES + " \t Ignore unstructured entries from the lexicon. Default: false");
        System.err.println(INCLUDE_WORD_WITH_SOURCES + " \t source1[,source2,....]Include surface forms(despite unstructured) from the provided sources. Default: (NONE)");
        throw new Exception();
    }

    ParserOptions(String[] args) throws Exception{
        posToBeInflected = EnumSet.of(PartOfSpeech.NOUN);
        posToBeLemmatised = EnumSet.noneOf(PartOfSpeech.class);
        posWithoutGrammemes = new TreeSet<>();
        properNounCategories = ParserDefaults.properNounCategories;
        ignoredCategories = new TreeSet<>();
        wordMetaAttributes = ParserDefaults.wordMetaAttributes;
        verbMetaAttributes = ParserDefaults.verbMetaAttributes;
        additionalGrammemesDict = new TreeMap<>();
        sourceFilenames = new ArrayList<>();
        addedLemmaCategoryGrammemes = new TreeSet<>();
        includedSources = new TreeSet<>();
        defaultGrammemeForCategory = new TreeMap<>();
        TreeSet<String> chosenLemmaUsages = ParserDefaults.chosenLemmaUsages;

        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (ParserOptions.INFLECTIONS_FILE.equals(arg)) {
                inflectionalFilename = args[++i];
            } else if (ParserOptions.ADD_EXTRA_GRAMMEMES.equals(arg)) {
                String additionalGrammemeFilename = args[++i];
                String filePath = Paths.get(ParserDefaults.RESOURCES_DIR  + additionalGrammemeFilename).toAbsolutePath().toString();
                try(BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(filePath)))) {
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
                TreeSet<String> key = Grammar.asTreeSet(propertySetToIgnore.split(","));
                TYPEMAP.put(key, EnumSet.of(Ignorable.IGNORABLE_PROPERTY));
                optionsUsedToInvoke.add(ParserOptions.IGNORE_PROPERTY_SET);
                optionsUsedToInvoke.add(propertySetToIgnore);
            } else if (ParserOptions.IGNORE_CATEGORIES.equals(arg)) {
                String categoriesToIgnore = args[++i];
                ignoredCategories.addAll(List.of(categoriesToIgnore.split(",")));
                optionsUsedToInvoke.add(ParserOptions.IGNORE_CATEGORIES);
                optionsUsedToInvoke.add(categoriesToIgnore);
            } else if (ParserOptions.IGNORE_AFFIXES.equals(arg)) {
                ignoreAffixes = true;
                TYPEMAP.put(Grammar.asTreeSet("prefix"), EnumSet.of(Ignorable.IGNORABLE_INFLECTION));
                TYPEMAP.put(Grammar.asTreeSet("suffix"), EnumSet.of(Ignorable.IGNORABLE_INFLECTION));
                TYPEMAP.put(Grammar.asTreeSet("infix"), EnumSet.of(Ignorable.IGNORABLE_INFLECTION));
                optionsUsedToInvoke.add(ParserOptions.IGNORE_AFFIXES);
            } else if (ParserOptions.USE_FORM_ATTRIBUTE.equals(arg)) {
                useFormAttribute = true;
                String formAttribute = args[++i];
                propertiesToConsiderFromForm = Grammar.asTreeSet(formAttribute.split(","));
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
            } else if (ParserOptions.PROPER_NOUN_CATEGORIES.equals(arg)) {
                String properNounCategoryValues = args[++i];

                properNounCategories.clear();
                properNounCategories.addAll(Arrays.asList(properNounCategoryValues.split(",")));

                optionsUsedToInvoke.add(ParserOptions.PROPER_NOUN_CATEGORIES);
                optionsUsedToInvoke.add(properNounCategoryValues);
            } else if (ParserOptions.INCLUDE_LEMMAS_WITHOUT_WORD.equals(arg)) {
                includeLemmasWithoutWords = true;
                optionsUsedToInvoke.add(ParserOptions.INCLUDE_LEMMAS_WITHOUT_WORD);
            } else if (ParserOptions.IGNORE_SURFACE_FORM.equals(arg)) {
                String ignoreEntriesWithGrammemesStr = args[++i];
                TreeSet<String> ignoreEntriesWithGrammemes = new TreeSet<>(Arrays.asList(ignoreEntriesWithGrammemesStr.split(",")));
                for (String grammeme : ignoreEntriesWithGrammemes) {
                    EnumSet<Ignorable> enumSet = EnumSet.of(Ignorable.IGNORABLE_INFLECTION);
                    TYPEMAP.put(Grammar.asTreeSet(grammeme), enumSet);
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
            } else if (ParserOptions.LOCALE.equals(arg)) {
                String localeStr = args[++i];
                locale = new ULocale(localeStr);
                optionsUsedToInvoke.add(ParserOptions.LOCALE);
                optionsUsedToInvoke.add(localeStr);
            } else if (ParserOptions.LEMMAS_WITH_USAGE.equals(arg)) {
                String lemmaUsagesStr = args[++i];
                chosenLemmaUsages = new TreeSet<>(Arrays.asList(lemmaUsagesStr.split(",")));
                optionsUsedToInvoke.add(ParserOptions.LEMMAS_WITH_USAGE);
                optionsUsedToInvoke.add(lemmaUsagesStr);
            } else if (ParserOptions.ADD_WORD_META_ATTRIBUTES.equals(arg)) {
                String addWordMetaAttributesStr = args[++i];
                Collections.addAll(wordMetaAttributes, addWordMetaAttributesStr.split(","));
                optionsUsedToInvoke.add(ParserOptions.ADD_WORD_META_ATTRIBUTES);
                optionsUsedToInvoke.add(addWordMetaAttributesStr);
            } else if (ParserOptions.ADD_NORMALIZED_ENTRY.equals(arg)) {
                addNormalizedEntry = true;
                optionsUsedToInvoke.add(ParserOptions.ADD_NORMALIZED_ENTRY);
            } else if (ParserOptions.DISABLE_NOUN_FOR_PROPER_NOUN.equals(arg))  {
                disableAddingPOSNounForProperNouns = true;
                optionsUsedToInvoke.add(ParserOptions.DISABLE_NOUN_FOR_PROPER_NOUN);
            } else if (ParserOptions.ADD_LEMMA_CATEGORY_GRAMMEMES.equals(arg))  {
                String addLemmaCategoryGrammemesStr = args[++i];
                addedLemmaCategoryGrammemes = new TreeSet<>(Arrays.asList(addLemmaCategoryGrammemesStr.split(",")));
                optionsUsedToInvoke.add(ParserOptions.ADD_LEMMA_CATEGORY_GRAMMEMES);
                optionsUsedToInvoke.add(addLemmaCategoryGrammemesStr);
            } else if (ParserOptions.ADD_DEFAULT_GRAMMEME_FOR_CATEGORY.equals(arg))  {
                String categoryDefaultGrammemeString = args[++i];
                String[] tokens = categoryDefaultGrammemeString.split(",");
                String posValue = "";
                for (int idx = 0; idx < tokens.length; idx += 1) {
                    String token = tokens[idx];
                    String[] tokenArgs = token.split("=");
                    if (tokenArgs.length != 2) {
                        throw new Exception("Default Grammeme for category string does not have entry in the format a=b " + token);
                    }
                    String key = tokenArgs[0].toLowerCase();
                    String value = tokenArgs[1].toLowerCase();
                    if (key.compareTo("pos") == 0) {
                        if (idx != 0) {
                            throw new Exception("pos key is not the first argument for default Grammeme for category string " + categoryDefaultGrammemeString);
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
            } else if (ParserOptions.INCLUDE_WORD_WITH_SOURCES.equals(arg))  {
                String includedSourcesString = args[++i];
                String[] tokens = includedSourcesString.split(",");
                includedSources.addAll(Arrays.asList(tokens));
                optionsUsedToInvoke.add(ParserOptions.INCLUDE_WORD_WITH_SOURCES);
                optionsUsedToInvoke.add(includedSourcesString);
            } else {
                sourceFilenames.add(arg);
            }
        }

        TreeSet<String> ignoreLemmaUsages = new TreeSet<>(ParserDefaults.lemmaUsageAttributes);
        ignoreLemmaUsages.removeAll(chosenLemmaUsages);
        for (String lemmaUsage : ignoreLemmaUsages) {
            TYPEMAP.put(Grammar.asTreeSet(lemmaUsage), EnumSet.of(Ignorable.IGNORABLE_LEMMA));
        }

        additionalGrammemesToBeAdded = !additionalGrammemesDict.isEmpty();

        if (sourceFilenames.isEmpty()) {
            usage();
        }
    }
}

final class DocumentState {
    int docCount = 0;
    int lemmaCount = 0;
    int unusableLemmaCount = 0;
    int unusableSurfaceFormCount = 0;
    int mergedCount = 0;
    int incomingSurfaceForm = 0;
    TreeMap<String, DictionaryEntry> dictionary = new TreeMap<>();
    ArrayList<InflectionPattern> inflectionPatterns = new ArrayList<>(8192);
    private static final Pattern anythingSlash = Pattern.compile(".*/");

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

    public void printDocument(ParserOptions parserOptions) throws SAXException{
        TreeMap<Enum<?>, Integer> grammemeCounts = new TreeMap<>(ParserDefaults.ENUM_COMPARATOR);
        int unclassifiedTerms = 0;
        try {
            if (isInflectional()) {
                PrintWriter inflectionalStream = new PrintWriter(new OutputStreamWriter(
                        new FileOutputStream(parserOptions.inflectionalFilename), StandardCharsets.UTF_8));
                inflectionalStream.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                        "<patterns>");

                sortInflectionPatterns(inflectionPatterns);
                for (InflectionPattern inflectionPattern : inflectionPatterns) {
                    inflectionalStream.print(inflectionPattern);
                }
                inflectionalStream.println("</patterns>");
                inflectionalStream.close();
            }
        }
        catch (FileNotFoundException e) {
            throw new SAXException(e);
        }
        for (Map.Entry<String, DictionaryEntry> entry : dictionary.entrySet()) {
            DictionaryEntry dictionaryEntry = entry.getValue();
            if (dictionaryEntry.getGrammemes().isEmpty()) {
                // We don't care about only known words. We need grammeme data
                unclassifiedTerms++;
                continue;
            }
            // Add lemmas if so requested, but only to the pos that are to be inflected (i.e. no verb-lemmas unless we ask for it)
            System.out.println(dictionaryEntry.toString(isInflectional(), parserOptions.includeLemmaForms && !Collections.disjoint(dictionaryEntry.getGrammemes(), parserOptions.posToBeLemmatised)));
            for (Enum<?> grammeme : dictionaryEntry.getGrammemes()) {
                grammemeCounts.merge(grammeme, 1, Integer::sum);
            }
        }

        NumberFormat percentFormat = NumberFormat.getPercentInstance(Locale.US);
        percentFormat.setMaximumFractionDigits(1);
        int dictionarySize = dictionary.size();
        StringBuilder source = new StringBuilder();
        for (String sourceFilename : parserOptions.sourceFilenames) {
            source.append(anythingSlash.matcher(sourceFilename).replaceAll("")).append(" ");
        }
        System.out.println("==============================================");
        System.out.printf("%30s %7s%n", "Source:", source);
        System.out.printf("%30s %7d%n", "Lemma terms:", lemmaCount);
        System.out.printf("%30s %7d%n", "Unusable lemma terms:", unusableLemmaCount);
        System.out.printf("%30s %7d%n", "Incoming surface forms:", incomingSurfaceForm);
        System.out.printf("%30s %7d%n", "Surface forms:", dictionarySize);
        System.out.printf("%30s %7d %7s%n", "Collapsed surface forms:", mergedCount, '(' + percentFormat.format((mergedCount)/(double)incomingSurfaceForm) + ')');
        System.out.printf("%30s %7d%n", "Unusable surface forms:", unusableSurfaceFormCount);
        System.out.printf("%30s %7d %7s%n", "Usable terms:", dictionarySize - unclassifiedTerms, '(' + percentFormat.format((dictionarySize - unclassifiedTerms)/(double)dictionarySize) + ')');
        System.out.printf("%30s %7d %7s%n", "Unclassified terms:", unclassifiedTerms, '(' + percentFormat.format(unclassifiedTerms/(double)dictionarySize) + ')');
        System.out.println("==============================================");
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
            System.out.printf("%s:%n", categoryName);
            var categoryValues = categoryEntry.getValue();
            categoryValues.sort(Comparator.comparing(grammemeCounts::get));
            Collections.reverse(categoryValues);
            for (var categoryValue : categoryValues) {
                System.out.printf("    %-20s %7d %7s%n", categoryValue.toString() + ':', grammemeCounts.get(categoryValue), '(' + percentFormat.format(grammemeCounts.get(categoryValue)/(double)dictionarySize) + ')');
            }
            System.out.printf("%n");
        }
    }

    DocumentState() {

    }
}

public final class ParseLexicon extends DefaultHandler {

    public enum ParseState {START, LEXICON, LEMMAS, LEMMA, CATEGORIES, CATEGORY, WORD, PROP, TERM, TRANSFORMS}
    private ParseState state = ParseState.START;
    private final StringBuilder currText = new StringBuilder(256);

    static class Lemma {
        static String value;
        static boolean isRare = false;
        static boolean isVowelStart = false;
        static final TreeSet<Enum<?>> grammemes = new TreeSet<>(ParserDefaults.ENUM_COMPARATOR);
        static final List<Inflection> inflections = new ArrayList<>(32);

        static void reset() {
            value = null;
            isRare = false;
            isVowelStart = false;
            grammemes.clear();
            inflections.clear();
        }
    }

    static class SurfaceForm {
        static String value;
        static String propertyKey;
        static final TreeMap<String, String> properties = new TreeMap<>();
        static final TreeSet<Enum<?>> grammemes = new TreeSet<>(ParserDefaults.ENUM_COMPARATOR);
        static final TreeSet<Enum<?>> formAttributeGrammemes = new TreeSet<>(ParserDefaults.ENUM_COMPARATOR);

        static boolean pathOrFormPresent = false;

        static void addProperty(String propertyValue) {
            if (propertyKey != null) {
                properties.put(propertyKey, propertyValue);
            }
        }

        static void reset() {
            value = null;
            propertyKey = null;
            properties.clear();
            grammemes.clear();
            pathOrFormPresent = false;
            formAttributeGrammemes.clear();
        }
    }

    private final ParserOptions parserOptions;
    private final DocumentState documentState;


    ParseLexicon(ParserOptions parserOptions)
    {
        this.parserOptions = parserOptions;
        this.documentState = new DocumentState();
        Lemma.reset();
        SurfaceForm.reset();
    }

    @Override
    public void startDocument() {
    }

    private void addGrammeme(TreeSet<Enum<?>> grammemes, @Nullable String grammeme) throws SAXException{
        if (grammeme != null && !grammeme.isEmpty()) {
            Set<? extends Enum<?>> values = Grammar.getMappedGrammemes(grammeme.split(","));
            if (values == null) {
                fail(grammeme + " is not a known grammeme");
            }
            else if (!values.contains(Ignorable.IGNORABLE_PROPERTY)) {
                grammemes.addAll(values);
                if (parserOptions.disableAddingPOSNounForProperNouns && grammemes.contains(PartOfSpeech.PROPER_NOUN)) {
                    grammemes.remove(PartOfSpeech.NOUN);
                }
            }
        }
    }

    private static boolean isAttributeValue(Attributes attributes, String qName, String valueToCheck) {
        String grammeme = attributes.getValue(qName);
        if (grammeme != null) {
            return grammeme.equals(valueToCheck);
        }
        return false;
    }

    private static boolean checkIgnorableLemma(Attributes attributes, String qName) {
        String grammeme = attributes.getValue(qName);
        if (grammeme != null) {
            Set<? extends Enum<?>> values = Grammar.getMappedGrammemes(grammeme);
            return values != null && values.contains(Ignorable.IGNORABLE_LEMMA);
        }
        return false;
    }

    @Override
    public void startElement(String namespaceURI, String localName, String qName, Attributes attributes) throws SAXException
    {
        if (state == ParseState.START && "lexicon".equals(localName)) {
            state = ParseState.LEXICON;
        }
        else if (state == ParseState.LEXICON && "lemmas".equals(localName)) {
            state = ParseState.LEMMAS;
        }
        else if (state == ParseState.LEMMAS && "lemma".equals(localName)) {
            documentState.lemmaCount++;

            if (checkIgnorableLemma(attributes,"usage") || checkIgnorableLemma(attributes,"status")) {
                documentState.unusableLemmaCount++;
                // Leave us in the current state until we get the next lemma.
                return;
            }

            state = ParseState.LEMMA;
            Lemma.value = attributes.getValue("name");
            Lemma.isRare = isAttributeValue(attributes,"usage", "rare");
            addGrammeme(Lemma.grammemes, attributes.getValue("pos"));
        }
        else if (state == ParseState.LEMMA && "categories".equals(localName)) {
            state = ParseState.CATEGORIES;
        }
        else if (state == ParseState.CATEGORIES && "c".equals(localName)) {
            state = ParseState.CATEGORY;
        }
        else if ((state == ParseState.LEMMA) && "word".equals(localName)) {
            state = ParseState.WORD;
            documentState.incomingSurfaceForm++;
            addGrammeme(SurfaceForm.grammemes, attributes.getValue("pos"));
            addGrammeme(SurfaceForm.grammemes, attributes.getValue("number"));
            addGrammeme(SurfaceForm.grammemes, attributes.getValue("gender"));
            if (parserOptions.useFormAttribute) {
                String formValue = attributes.getValue("form");
                if (formValue != null) {
                    for (String formEntry : formValue.toLowerCase().split(" ")) {
                        if (parserOptions.propertiesToConsiderFromForm.contains(formEntry)) {
                            addGrammeme(SurfaceForm.formAttributeGrammemes, formEntry);
                        }
                    }
                }
            }
            if (parserOptions.ignoreUnstructuredEntries) {
                if (attributes.getValue("form") != null || attributes.getValue("path") != null) {
                    SurfaceForm.pathOrFormPresent = true;
                }
            }
        }
        else if (state == ParseState.WORD && "transforms".equals(localName)) {
            state = ParseState.TRANSFORMS;
        }
        else if (state == ParseState.WORD && "t".equals(localName)) {
            state = ParseState.TERM;
        }
        else if (state == ParseState.WORD && "prop".equals(localName)) {
            state = ParseState.PROP;
            SurfaceForm.propertyKey = attributes.getValue("name");
        }
        else if (state == ParseState.WORD && "meta".equals(localName)) {
            String nameValue = attributes.getValue("name");
            String posValue = attributes.getValue("pos");
            if ("word".equals(nameValue) && !(posValue != null && parserOptions.posWithoutGrammemes.contains(posValue))) {
                for (String grammemeType : parserOptions.wordMetaAttributes) {
                    addGrammeme(SurfaceForm.grammemes, attributes.getValue(grammemeType));
                }

                if (parserOptions.posToBeInflected.contains(PartOfSpeech.VERB)) {
                    for (String grammemeType : parserOptions.verbMetaAttributes) {
                        addGrammeme(SurfaceForm.grammemes, attributes.getValue(grammemeType));
                    }
                }
            }
            if (parserOptions.ignoreAffixes) {
                String affixTypeValue = attributes.getValue("affix-type");
                String tokenValue = attributes.getValue("token");
                if (affixTypeValue != null) {
                    addGrammeme(SurfaceForm.grammemes, affixTypeValue);
                }
                if (tokenValue != null && tokenValue.endsWith("'")) {
                    Lemma.isVowelStart = true;
                }
            }
        }
    }

    @Override
    public void endElement(String namespaceURI, String localName, String qName) throws SAXException
    {
        if (state == ParseState.LEXICON && "lexicon".equals(localName)) {
            state = ParseState.START;
            if (parserOptions.additionalGrammemesToBeAdded) {
                mergeAdditionalGrammemes();
            }
        }
        else if (state == ParseState.LEMMAS && "lemmas".equals(localName)) {
            state = ParseState.LEXICON;
        }
        else if (state == ParseState.LEMMA && "lemma".equals(localName)) {
            state = ParseState.LEMMAS;
            if (!Lemma.grammemes.contains(Ignorable.IGNORABLE_LEMMA)) {
                analyzeLemma();
            }
            Lemma.reset();
        }
        else if (state == ParseState.CATEGORIES && "categories".equals(localName)) {
            state = ParseState.LEMMA;
        }
        else if (state == ParseState.CATEGORY && "c".equals(localName)) {
            state = ParseState.CATEGORIES;
            String categoryName = currText.toString();
            if (parserOptions.properNounCategories.contains(categoryName)) {
                addGrammeme(Lemma.grammemes, "proper-noun");
            }
            if (parserOptions.addedLemmaCategoryGrammemes.contains(categoryName)) {
                addGrammeme(Lemma.grammemes, categoryName);
            }
            if (parserOptions.ignoredCategories.contains(categoryName)) {
                Lemma.grammemes.add(Ignorable.IGNORABLE_LEMMA);
                state = ParseState.LEMMA;
            }
            currText.setLength(0);
        }
        else if (state == ParseState.WORD && "word".equals(localName)) {
            state = ParseState.LEMMA;

            if (SurfaceForm.properties.containsKey("abbreviation")) {
                addGrammeme(SurfaceForm.grammemes, "abbreviation");
            }
            if (SurfaceForm.properties.getOrDefault("usage", "").equals("sms")) {
                SurfaceForm.grammemes.add(Ignorable.IGNORABLE_INFLECTION);
            }
            if (parserOptions.ignoreUnannotated && SurfaceForm.grammemes.isEmpty() && !SurfaceForm.value.equals(Lemma.value)) {
                SurfaceForm.grammemes.add(Ignorable.IGNORABLE_INFLECTION);
            }

            if (!parserOptions.includedSources.contains(SurfaceForm.properties.getOrDefault("source", ""))) {
                if (parserOptions.ignoreUnstructuredEntries && !SurfaceForm.pathOrFormPresent) {
                    SurfaceForm.grammemes.add(Ignorable.IGNORABLE_INFLECTION);
                }
            }

            // We'll only add the missing grammemes and ignore the rest
            Set<String> seenGrammemeTypes = SurfaceForm.grammemes.stream().map(grammeme -> grammeme.getDeclaringClass().getSimpleName().toLowerCase())
                    .collect(Collectors.toSet());
            for (Enum<?> formGrammeme : SurfaceForm.formAttributeGrammemes) {
                if (!seenGrammemeTypes.contains(formGrammeme.getDeclaringClass().getSimpleName().toLowerCase())) {
                    SurfaceForm.grammemes.add(formGrammeme);
                }
            }

            var lemmaPOS = Lemma.grammemes.stream().filter(grammeme -> (grammeme.getDeclaringClass().getSimpleName().toLowerCase().compareTo("partofspeech") == 0)).findFirst();
            var lemmaPOSGrammemeString = lemmaPOS.map(grammeme -> grammeme.toString()).orElse("");
            var defaultGrammemeForCategoryWithPOS = parserOptions.defaultGrammemeForCategory.get(lemmaPOSGrammemeString);
            if (!SurfaceForm.grammemes.isEmpty() && defaultGrammemeForCategoryWithPOS != null) {
                for (var entry : defaultGrammemeForCategoryWithPOS.entrySet()) {
                    if (!seenGrammemeTypes.contains(entry.getKey())) {
                        addGrammeme(SurfaceForm.grammemes, entry.getValue());
                    }
                }
            }

            if (SurfaceForm.grammemes.contains(Ignorable.IGNORABLE_INFLECTION)) {
                documentState.unusableSurfaceFormCount++;
            } else {
                boolean rareUsage = SurfaceForm.properties.getOrDefault("usage", "").compareTo("rare") == 0;
                Inflection currentInflection = new Inflection(SurfaceForm.value, rareUsage);
                currentInflection.addGrammemes(SurfaceForm.grammemes);
                Lemma.inflections.add(currentInflection);
            }
            SurfaceForm.reset();
        }
        else if (state == ParseState.PROP && "prop".equals(localName)) {
            SurfaceForm.addProperty(currText.toString());
            currText.setLength(0);
            state = ParseState.WORD;
        }
        else if (state == ParseState.TERM && "t".equals(localName)) {
            SurfaceForm.value = currText.toString();
            currText.setLength(0);
            state = ParseState.WORD;
        }
        else if (state == ParseState.TRANSFORMS && "transforms".equals(localName)) {
            state = ParseState.WORD;
        }
    }

    private static boolean validateStemLength(@Nonnull List<Inflection> inflections, int stemLength) {
        for (var inflection_outer : inflections) {
            String suffix = inflection_outer.getInflection().substring(stemLength);
            boolean invalid = false;
            for (var inflection_inner : inflections) {
                var inflection_inner_str = inflection_inner.getInflection();
                if (inflection_inner_str.endsWith(suffix) && ((inflection_inner_str.length()-suffix.length()) < stemLength)) {
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
    private InflectionPattern getInflectionPattern(String lemmaSuffix,
                                                   List<Inflection> suffixes) {
        if (lemmaSuffix.indexOf('-') < 0) {
            // TODO the reading algorithm probably should handle dashes better. Tokenization splits on dashes by default.
            TreeSet<Enum<?>> newGrammemeList = new TreeSet<>(Lemma.grammemes);
            MISCELLANEOUS_LIST.forEach(newGrammemeList::remove);
            LEMMA_CATEGORY_LIST.forEach(newGrammemeList::remove);

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
        return null;
    }

    private void analyzeInflections(String lemma, List<Inflection> inputInflections) throws SAXException {
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
                inflectionGrammemes.addAll(Lemma.grammemes);
                if (Lemma.isVowelStart && inflection.inflection.charAt(0) == Lemma.value.charAt(0)) {
                    // The words aren't mixed together. So this is probably accurate.
                    inflectionGrammemes.add(Sound.VOWEL_START);
                }
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
                int stemLength = getStemLength(lemma, nonEmptyInflections);
                List<Inflection> suffixes = generateSuffixes(stemLength, nonEmptyInflections);
                inflectionPattern = getInflectionPattern(
                        lemma.substring(stemLength),
                        suffixes
                );
            }
            // else ignore this unimportant inflection pattern. This is usually trimmed for size.
        }
        boolean addLemmasForPhrase = !Collections.disjoint(Lemma.grammemes, parserOptions.posToBeLemmatised);
        for (int i = 0; i < inflections.size() ; i++) {
            var inflection = inflections.get(i);
            String phrase = inflection.getInflection();
            if (!inflection.getGrammemeSet().isEmpty() && addLemmasForPhrase && (phrase.compareTo(lemma) == 0)) {
                addGrammeme(inflection.getGrammemeSet(), "lemma_word");
            }
            String lemmaForEntry = !addLemmasForPhrase ? phrase : lemma.replace(' ', '\u2002');
            InflectionPattern inflectionPatternForDict = nonEmptyInflectionIndices.contains(i) ? inflectionPattern : null;
            documentState.addDictionaryEntry(new DictionaryEntry(phrase, lemmaForEntry, Lemma.isRare, inflection.getGrammemeSet(), inflectionPatternForDict));
            if (parserOptions.addNormalizedEntry) {
                String normalizedPhrase = phrase.toLowerCase(parserOptions.locale.toLocale()); //locale is specified in the options, by default we use en_US
                if (!normalizedPhrase.equals(phrase) && !Lemma.isRare) {
                    documentState.addDictionaryEntry(new DictionaryEntry(normalizedPhrase, lemmaForEntry, false, inflection.getGrammemeSet(), inflectionPatternForDict));
                }
            }
        }
    }

    private static String selectLemmaFromInflections(@Nonnull List<Inflection> inflections) {
        long maxMatch = -1;
        String lemma = null;
        for (Inflection inflection : inflections) {
            TreeSet<Enum<?>> grammemes = inflection.getGrammemeSet();
            long match = 0;
            for (Enum<?> lemmaGrammmeme : ParserDefaults.LEMMA_GRAMMEMES) {
                match = 2*match + (grammemes.contains(lemmaGrammmeme) ? 1 : 0);
            }
            if (match > maxMatch) {
                maxMatch = match;
                lemma = inflection.getInflection();
            }
        }
        return lemma;
    }

    //Split the current inflections into various buckets with their corresponding lemmas based on the specified grammeme types
    @Nonnull
    private Map<String, List<Inflection>> computeInflectionBuckets(@Nonnull List<Inflection> inflections, String splitType) {
        TreeMap<String, List<Inflection>> buckets = new TreeMap<>();
        for (Inflection inflection : inflections) {
            String splitKey = "";
            for (Enum<?> grammeme : inflection.getGrammemeSet()) {
                if (grammeme.getClass().getName().equals(splitType)) {
                    splitKey = grammeme.toString();
                }
            }
            if (!buckets.containsKey(splitKey)) {
                buckets.put(splitKey, new ArrayList<>());
            }
            List<Inflection> bucketList = buckets.get(splitKey);
            bucketList.add(inflection);
        }
        return buckets;
    }

    private void analyzeInflectionBucket(List<Inflection> bucket, boolean useDefaultLemma) throws SAXException{
        String lemmaForBucket = useDefaultLemma ? Lemma.value : selectLemmaFromInflections(bucket);
        analyzeInflections(lemmaForBucket, bucket);
    }

    private static final TreeSet<Class<?>> grammemeClassSet = new TreeSet<>(Inflection.ENUM_CLASS_COMPARATOR);
    private List<Inflection> enumerateInflectionsForGrammemeCombinations(Inflection inflection){
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

    private void mergeAdditionalGrammemes() throws SAXException {
        // Add any entries that are missing. The actual properties will be added elsewhere.
        TreeSet<Enum<?>> grammemes = new TreeSet<>(ParserDefaults.ENUM_COMPARATOR);
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

    private void analyzeLemma() throws SAXException {
        if (Lemma.value == null) {
            // This can happen when we have an ignorable lemma due to an uninteresting property.
            return;
        }

        if (Lemma.grammemes.contains(PartOfSpeech.VERB)) {
            Map<String, List<Inflection>> buckets = computeInflectionBuckets(Lemma.inflections, Polarity.class.getName());
            for (Map.Entry<String, List<Inflection>> bucketEntry : buckets.entrySet()) {
                analyzeInflectionBucket(bucketEntry.getValue(), bucketEntry.getKey().equals(""));
            }
        } else {
            analyzeInflections(Lemma.value, Lemma.inflections);
        }

        if (parserOptions.includeLemmasWithoutWords) {
            DictionaryEntry newDictionaryEntry = new DictionaryEntry(Lemma.value, Lemma.isRare, Lemma.grammemes, null);
            documentState.dictionary.putIfAbsent(Lemma.value, newDictionaryEntry);
        }
    }

    @Override
    public void characters(char[] ch, int start, int length) {
        if (state == ParseState.TERM || state == ParseState.CATEGORY || state == ParseState.PROP) {
            currText.append(ch, start, length);
        }
    }

    @Override
    public void endDocument() throws SAXException {
        if (++(documentState.docCount) != parserOptions.sourceFilenames.size()) {
            return;
        }
        documentState.printDocument(parserOptions);
    }
    static final List<Enum<?>> PART_OF_SPEECH_LIST = List.of(PartOfSpeech.values());
    static final List<Enum<?>> MISCELLANEOUS_LIST = List.of(Miscellaneous.values());
    static final List<Enum<?>> LEMMA_CATEGORY_LIST = List.of(LemmaCategory.values());

    //Warning and Error methods need not be overridden.
    //Override them only when using a validating parser.
    private static class MyErrorHandler extends DefaultHandler {
        private String getParseExceptionInfo(SAXParseException spe) {
            String systemId = spe.getSystemId();

            if (systemId == null) {
                systemId = "null";
            }

            return "URI=" + systemId + " Line="
                    + spe.getLineNumber() + ": " + spe.getMessage();
        }

        @Override
        public void fatalError(SAXParseException spe) throws SAXException {
            String message = "Fatal Error: " + getParseExceptionInfo(spe);
            throw new SAXException(message);
        }
    }

    private void fail(String msg) throws SAXException {
        throw new SAXException(Lemma.value+"\n"+msg);
    }

    public static void main(String[] args) throws Exception {
        ParserOptions parserOptions = new ParserOptions(args);
        long startTime = System.currentTimeMillis();
        SAXParserFactory spf = SAXParserFactory.newInstance();
        spf.setNamespaceAware(true);
        SAXParser saxParser = spf.newSAXParser();
        XMLReader xmlReader = saxParser.getXMLReader();
        xmlReader.setContentHandler(new ParseLexicon(parserOptions));
        xmlReader.setErrorHandler(new MyErrorHandler());
        // We create InputSource directly due to an occasional bugs with UTF-8 files being interpreted as malformed UTF-8.
        // We use a large buffer because we're reading a large file, and we're frequently reading file data.
        for (String sourceFilename : parserOptions.sourceFilenames) {
            InputSource inputSource = new InputSource(new BufferedReader(new InputStreamReader(
                    new FileInputStream(sourceFilename), StandardCharsets.UTF_8), 1048576));
            xmlReader.parse(inputSource);
        }
        long endTime = System.currentTimeMillis();
        long elapsedTime = (endTime-startTime);
        System.out.println("processed in " + (elapsedTime / 1000) + '.' + (elapsedTime % 1000) + " seconds");
        System.out.println("License: Apple's Lexical Resources (https://stashweb.sd.apple.com/projects/NLP/repos/lexicalresources/)");
        System.out.println("generated with options: " + String.join(" ", parserOptions.optionsUsedToInvoke));
    }
}

