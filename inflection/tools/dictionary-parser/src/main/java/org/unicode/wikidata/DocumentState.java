/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.TreeMap;
import java.util.regex.Pattern;

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

    public void addDictionaryEntry(DictionaryEntry dictionaryEntry) {
        String phrase = dictionaryEntry.phrase;
        DictionaryEntry existingDictionaryEntry = dictionary.get(phrase);
        if (existingDictionaryEntry == null) {
            dictionary.put(phrase, dictionaryEntry);
        } else {
            mergedCount++;
            existingDictionaryEntry.merge(dictionaryEntry);
        }
    }

    void printDocument(ParserOptions parserOptions, long startTime) throws FileNotFoundException {
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
                // Print the dictionary entry to the .lst file.
                lexicalDictionaryStream.println(dictionaryEntry.toString(isInflectional()));
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
            long endTime = System.currentTimeMillis();
            long elapsedTime = (endTime - startTime);
            lexicalDictionaryStream.println("processed in " + (elapsedTime / 1000) + '.' + (elapsedTime % 1000) + " seconds");
            lexicalDictionaryStream.println("License: Creative Commons CC0 License (https://creativecommons.org/publicdomain/zero/1.0/)");
            lexicalDictionaryStream.println("generated with options: " + String.join(" ", parserOptions.optionsUsedToInvoke));
        }
    }

    DocumentState() {
    }
}
