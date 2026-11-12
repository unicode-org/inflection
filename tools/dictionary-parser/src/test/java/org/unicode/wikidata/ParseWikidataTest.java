/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.lang.invoke.MethodHandles;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.exc.MismatchedInputException;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class ParseWikidataTest {
    private static final String lexiconSource = Objects
            .requireNonNull(MethodHandles.lookup().lookupClass().getClassLoader().getResource("sourceLexicon.json")).getFile();
    private static final String baseDir = lexiconSource.substring(0, lexiconSource.lastIndexOf("/"));
    private static final String inflectionalFile = baseDir + "/LexiconInflectOut.xml";
    private static final String dictionaryFile = baseDir + "/LexiconInflectOut.lst";
    private static final String expectedOutputFile = baseDir + "/lexiconCorrectOut.txt";
    private static final String fatalErrorFile = baseDir + "/lexiconFatalError.json";
    private static final String missingGrammemeErrorFile = baseDir + "/lexiconMissingGrammemeError.json";
    private static final String caseInsensitiveGrammemeFile = baseDir + "/lexiconcaseInsenstiveGrammeme.json";
    private static final String dmlexLexiconFile = baseDir + "/dmlexLexicon.json";
    private static final String dmlexOverlayFile = baseDir + "/dmlexOverlay.json";
    private static final String dmlexPatchFile = baseDir + "/dmlexPatch.json";
    private static final String dmlexExampleA28File = baseDir + "/dmlexExampleA28.json";
    private static final String dmlexInvalidSchemaFile = baseDir + "/dmlexInvalidSchema.json";
    private static final String LINE_END = "==============================================";
    private static final Pattern XML_PATTERN_NAME = Pattern.compile("<pattern name=\"([^\"]+)\"");
    private static final Pattern DICT_INFLECTION_ID = Pattern.compile("\\binflection=([0-9a-f]+)\\b");

    private void compareOutputs(String actual, String expected) {
        String[] actualLines = actual.split("\n");
        String[] expectedLines = expected.split("\n");
        for (int i = 0; i < actualLines.length && i < expectedLines.length; i++) {
            String actualLine = actualLines[i];
            String expectedLine = expectedLines[i];
            if (actualLine.equals(LINE_END)) {
                break;
            }
            Assertions.assertEquals(expectedLine, actualLine);
        }
    }

    private String getParserOutput(String[] args) throws Exception {
        ParseWikidata.main(args);
        return Files.readString(Paths.get(dictionaryFile), StandardCharsets.UTF_8);
    }

    private Map<String, String> parseDictionaryEntries(String output) {
        return Arrays.stream(output.split("\n"))
                .takeWhile(line -> !line.equals(LINE_END))
                .filter(line -> line.contains(":"))
                .collect(Collectors.toMap(
                        line -> line.substring(0, line.indexOf(':')),
                        line -> line.substring(line.indexOf(':') + 1).trim()));
    }

    private Set<String> assertAllXmlPatternsReferencedInDictionary(String dictionaryOutput) throws Exception {
        String xml = Files.readString(Paths.get(inflectionalFile), StandardCharsets.UTF_8);
        Set<String> xmlPatternIds = new TreeSet<>();
        Matcher xmlMatcher = XML_PATTERN_NAME.matcher(xml);
        while (xmlMatcher.find()) {
            Assertions.assertTrue(xmlPatternIds.add(xmlMatcher.group(1)),
                    "Duplicate pattern name in inflectional XML: " + xmlMatcher.group(1));
        }
        Set<String> dictPatternIds = new TreeSet<>();
        for (String line : dictionaryOutput.split("\n")) {
            if (line.equals(LINE_END)) {
                break;
            }
            Matcher dictMatcher = DICT_INFLECTION_ID.matcher(line);
            while (dictMatcher.find()) {
                dictPatternIds.add(dictMatcher.group(1));
            }
        }
        Assertions.assertEquals(dictPatternIds, xmlPatternIds,
                "Every <pattern name=\"X\"> in inflectional.xml must be referenced as inflection=X in dictionary.lst");
        return xmlPatternIds;
    }

    @Test
    public void missingInputFile() {
        Assertions.assertThrows(Exception.class, () -> ParseWikidata.main(new String[1]));
    }

    @Test
    public void enumTest() {
        String fieldValue = Grammar.Alternate.SPELLING.toString();
        Assertions.assertEquals("spelling", fieldValue);
        Assertions.assertThrows(IllegalArgumentException.class, () -> Grammar.getMappedGrammemes(null));
    }

    @Test
    public void fatalErrorHandlerTest() {
        // Malformed JSON syntax cannot be parsed by the Wikidata reader.
        String[] args = {"--inflections", inflectionalFile, "--dictionary", dictionaryFile, "--wikidata", fatalErrorFile};
        Assertions.assertThrows(JsonParseException.class, () -> ParseWikidata.main(args));
    }

    @Test
    public void missingGrammemeTest() {
        // The grammatical features are not in the expected array form, so the reader rejects the input.
        String[] args = {"--inflections", inflectionalFile, "--dictionary", dictionaryFile, "--wikidata", missingGrammemeErrorFile};
        Assertions.assertThrows(MismatchedInputException.class, () -> ParseWikidata.main(args));
    }

    @Test
    public void caseInsensitiveGrammemeTest() {
        String[] args = {"--inflections", inflectionalFile, "--dictionary", dictionaryFile, "--wikidata", caseInsensitiveGrammemeFile};
        Assertions.assertDoesNotThrow(() -> ParseWikidata.main(args));
    }

    @Test
    public void lexiconParserTest() throws Exception {
        String[] args = {"--inflection-types", "noun,adjective,proper-noun", "--add-sound", "vowel-start",
                "--inflections", inflectionalFile, "--dictionary", dictionaryFile, "--wikidata", lexiconSource};
        String actual = getParserOutput(args);
        String expected = Files.readString(Paths.get(expectedOutputFile), StandardCharsets.UTF_8);
        compareOutputs(actual, expected);
    }

    @Test
    public void standaloneDMLexParserTest() throws Exception {
        String[] args = {
                "--inflection-types", "noun,proper-noun",
                "--add-sound", "vowel-start",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", dmlexLexiconFile
        };
        String actual = getParserOutput(args);
        Map<String, String> entries = parseDictionaryEntries(actual);

        // cat and bat share pattern 1 (count=2, most frequent)
        Assertions.assertEquals("singular masculine noun inflection=1", entries.get("cat"));
        Assertions.assertEquals("plural masculine noun inflection=1", entries.get("cats"));
        Assertions.assertEquals("singular masculine noun inflection=1", entries.get("bat"));
        Assertions.assertEquals("plural masculine noun inflection=1", entries.get("bats"));

        // apple has IPA pronunciation matching vowel-start
        Assertions.assertEquals("singular vowel-start noun inflection=2", entries.get("apple"));
        Assertions.assertEquals("plural vowel-start noun inflection=2", entries.get("apples"));

        // London uses proper-noun
        Assertions.assertEquals("singular proper-noun inflection=3", entries.get("London"));

        // wow has no inflectedForms array, synthesized default form
        Assertions.assertEquals("interjection", entries.get("wow"));

        // Verify inflectional XML was generated
        String xml = Files.readString(Paths.get(inflectionalFile), StandardCharsets.UTF_8);
        Assertions.assertTrue(xml.contains("<patterns>"));
        Assertions.assertTrue(xml.contains("<pattern name=\"1\" words=\"2\">"));
    }

    @Test
    public void dmlexSpecExampleA28Test() throws Exception {
        // With default --language en, the Irish ("ga") lexicon is skipped
        String[] enArgs = {
                "--inflection-types", "noun",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", dmlexExampleA28File
        };
        Assertions.assertTrue(parseDictionaryEntries(getParserOutput(enArgs)).isEmpty());

        String[] args = {
                "--language", "ga",
                "--inflection-types", "noun",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", dmlexExampleA28File
        };
        String actual = getParserOutput(args);
        Map<String, String> entries = parseDictionaryEntries(actual);

        Assertions.assertEquals("singular masculine noun", entries.get("folúsghlantóir"));
        Assertions.assertEquals("singular genitive masculine noun", entries.get("folúsghlantóra"));
        Assertions.assertEquals("plural masculine noun", entries.get("folúsghlantóirí"));
    }

    @Test
    public void multiSourceWikidataAndDMLexOverwriteTest() throws Exception {
        // Default --merge-conflicts=false (Option 4 + Option 3): normal processing happens for earlier
        // files, and when a surface form is encountered in a later file, the new pattern and grammemes
        // override all previous data for that surface form.
        String[] args = {
                "--inflection-types", "noun,adjective,proper-noun",
                "--add-sound", "vowel-start",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--wikidata", lexiconSource,
                "--dmlex", dmlexOverlayFile
        };
        String actual = getParserOutput(args);
        Map<String, String> entries = parseDictionaryEntries(actual);

        // 1. New lemma "book" from DMLex is added
        Assertions.assertTrue(entries.containsKey("book"));
        Assertions.assertTrue(entries.get("book").startsWith("singular noun inflection="));
        Assertions.assertTrue(entries.containsKey("book's"));
        Assertions.assertTrue(entries.containsKey("books"));
        Assertions.assertTrue(entries.containsKey("books'"));

        // 2. "hey" was interjection in Wikidata and singular noun in DMLex -> DMLex overrides "hey"
        String heyEntry = entries.get("hey");
        Assertions.assertNotNull(heyEntry);
        Assertions.assertTrue(heyEntry.startsWith("singular noun inflection="), "Expected singular noun override in: " + heyEntry);
        Assertions.assertFalse(heyEntry.contains("interjection"), "Did not expect interjection after override in: " + heyEntry);
        Assertions.assertTrue(entries.containsKey("heys"));

        // 3. "ciel" and "cieux" were masculine noun in Wikidata, overridden by feminine noun in DMLex
        String cielEntry = entries.get("ciel");
        Assertions.assertNotNull(cielEntry);
        Assertions.assertTrue(cielEntry.contains("feminine"), "Expected feminine in: " + cielEntry);
        Assertions.assertFalse(cielEntry.contains("masculine"), "Did not expect masculine in: " + cielEntry);
        Assertions.assertEquals(1, cielEntry.split("inflection=").length - 1, "Expected 1 override pattern in: " + cielEntry);

        String cieuxEntry = entries.get("cieux");
        Assertions.assertNotNull(cieuxEntry);
        Assertions.assertTrue(cieuxEntry.contains("feminine"), "Expected feminine in: " + cieuxEntry);
        Assertions.assertFalse(cieuxEntry.contains("masculine"), "Did not expect masculine in: " + cieuxEntry);
        Assertions.assertEquals(1, cieuxEntry.split("inflection=").length - 1, "Expected 1 override pattern in: " + cieuxEntry);

        // "ciels" was processed in Wikidata and not overridden in DMLex (Option 4)
        Assertions.assertTrue(entries.containsKey("ciels"));
        Assertions.assertTrue(entries.get("ciels").startsWith("plural masculine noun inflection="));
        assertAllXmlPatternsReferencedInDictionary(actual);
    }

    @Test
    public void multiSourceWikidataAndDMLexMergeConflictsTrueTest() throws Exception {
        // --merge-conflicts=true (Option 4 + Option 2): conflicting entries across files mix in
        // their grammemes and patterns, and the later source's conflicting pattern comes first.
        String[] args = {
                "--merge-conflicts=true",
                "--inflection-types", "noun,adjective,proper-noun",
                "--add-sound", "vowel-start",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--wikidata", lexiconSource,
                "--dmlex", dmlexOverlayFile
        };
        String actual = getParserOutput(args);
        Map<String, String> entries = parseDictionaryEntries(actual);

        // "hey" merges Wikidata (interjection) and DMLex (singular noun)
        String heyEntry = entries.get("hey");
        Assertions.assertNotNull(heyEntry);
        Assertions.assertTrue(heyEntry.contains("noun"), "Expected noun in: " + heyEntry);
        Assertions.assertTrue(heyEntry.contains("interjection"), "Expected interjection in: " + heyEntry);

        // "ciel" merges Wikidata (masculine) and DMLex (feminine), keeping both patterns,
        // with the DMLex conflict pattern placed FIRST before the higher-count Wikidata pattern!
        String cielEntry = entries.get("ciel");
        Assertions.assertNotNull(cielEntry);
        Assertions.assertTrue(cielEntry.contains("masculine"), "Expected masculine in: " + cielEntry);
        Assertions.assertTrue(cielEntry.contains("feminine"), "Expected feminine in: " + cielEntry);
        String[] cielParts = cielEntry.split("inflection=");
        Assertions.assertEquals(3, cielParts.length, "Expected 2 inflection patterns in: " + cielEntry);
        String firstPatternId = cielParts[1].trim();
        String secondPatternId = cielParts[2].trim();

        // "ciels" only exists in Wikidata, so its pattern ID is the Wikidata pattern ID.
        // Verify that on "ciel", the Wikidata pattern ID is second and the DMLex override pattern ID is first!
        String cielsEntry = entries.get("ciels");
        Assertions.assertNotNull(cielsEntry);
        String wikidataPatternId = cielsEntry.split("inflection=")[1].trim();
        Assertions.assertEquals(wikidataPatternId, secondPatternId,
                "Expected Wikidata pattern to be second after DMLex override pattern in: " + cielEntry);
        Assertions.assertNotEquals(firstPatternId, secondPatternId);
        assertAllXmlPatternsReferencedInDictionary(actual);
    }

    @Test
    public void multiSourceSameFileHomographAndMergePriorityTest() throws Exception {
        // Tests:
        // 1. When a later DMLex file defines multiple entries for the same surface form ("Élie" as both
        //    proper-noun and verb in the same file), under --merge-conflicts=false (Option 4 + Option 3)
        //    the first entry overrides Wikidata's "Élie" and the second entry from the same DMLex file
        //    merges with it.
        // 2. Under --merge-conflicts=true (Option 4 + Option 2), both Wikidata and DMLex grammemes/patterns
        //    are mixed in.
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-homograph-", ".json");
        try {
            String json = """
                    {
                      "langCode": "en",
                      "entries": [
                        {
                          "id": "elie-proper-noun",
                          "headword": "Élie",
                          "partsOfSpeech": ["propn-fem"],
                          "pronunciations": [
                            {
                              "transcriptions": [
                                { "text": "e.li", "scheme": "en-fonipa" }
                              ]
                            }
                          ],
                          "inflectedForms": [
                            {
                              "tag": "sg",
                              "text": "Élie"
                            }
                          ]
                        },
                        {
                          "id": "elie-verb",
                          "headword": "Élie",
                          "partsOfSpeech": ["v"]
                        }
                      ],
                      "partOfSpeechTags": [
                        {
                          "tag": "propn-fem",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q147276",
                            "http://www.wikidata.org/entity/Q1775415"
                          ]
                        },
                        {
                          "tag": "v",
                          "sameAs": ["http://www.wikidata.org/entity/Q24905"]
                        }
                      ],
                      "inflectedFormTags": [
                        {
                          "tag": "sg",
                          "sameAs": ["http://www.wikidata.org/entity/Q110786"]
                        }
                      ]
                    }
                    """;
            Files.writeString(tempDmlex, json, StandardCharsets.UTF_8);

            String[] overwriteArgs = {
                    "--inflection-types", "noun,adjective,proper-noun",
                    "--add-sound", "vowel-start",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", lexiconSource,
                    "--dmlex=" + tempDmlex
            };
            String overwriteOutput = getParserOutput(overwriteArgs);
            Map<String, String> overwriteEntries = parseDictionaryEntries(overwriteOutput);
            String elieOverwrite = overwriteEntries.get("Élie");
            Assertions.assertNotNull(elieOverwrite);
            // Wikidata's "plural masculine noun proper-noun" was overridden by tempDmlex's first entry
            // ("singular feminine vowel-start proper-noun"), and tempDmlex's second entry ("verb") merged with it.
            Assertions.assertTrue(elieOverwrite.startsWith("singular feminine vowel-start proper-noun verb inflection="),
                    "Unexpected overwrite entry: " + elieOverwrite);
            Assertions.assertEquals(1, elieOverwrite.split("inflection=").length - 1);
            // Because Wikidata's "Élie" was the only surface form referencing its inflection pattern,
            // overriding "Élie" must prune that superseded pattern from inflectional.xml so all XML patterns
            // are referenced in dictionary.lst (7 patterns total instead of 8).
            Set<String> overwriteXmlPatternIds = assertAllXmlPatternsReferencedInDictionary(overwriteOutput);
            Assertions.assertEquals(7, overwriteXmlPatternIds.size());

            String[] mergeArgs = {
                    "--merge-conflicts=true",
                    "--inflection-types", "noun,adjective,proper-noun",
                    "--add-sound", "vowel-start",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", lexiconSource,
                    "--dmlex=" + tempDmlex
            };
            String mergeOutput = getParserOutput(mergeArgs);
            Map<String, String> mergeEntries = parseDictionaryEntries(mergeOutput);
            String elieMerge = mergeEntries.get("Élie");
            Assertions.assertNotNull(elieMerge);
            Assertions.assertTrue(
                    elieMerge.startsWith("singular plural masculine feminine vowel-start noun proper-noun verb inflection="),
                    "Unexpected merge entry: " + elieMerge);
            Assertions.assertEquals(2, elieMerge.split("inflection=").length - 1);
            // Under --merge-conflicts=true, "Élie" retains both the DMLex pattern and the Wikidata pattern,
            // so all 8 patterns remain referenced in dictionary.lst and present in inflectional.xml.
            Set<String> mergeXmlPatternIds = assertAllXmlPatternsReferencedInDictionary(mergeOutput);
            Assertions.assertEquals(8, mergeXmlPatternIds.size());

            // Verify that if a pattern was rare in an earlier source (sourceIndex=0) alongside a non-rare
            // pattern, and a later source (sourceIndex=1) uses that same pattern non-rarely, it is promoted
            // to non-rare and sorted first under --merge-conflicts=true.
            TreeSet<Enum<?>> grammemes = new TreeSet<>(EnumComparator.ENUM_COMPARATOR);
            grammemes.add(Grammar.PartOfSpeech.NOUN);
            Inflection suffixS = new Inflection("s", false);
            suffixS.addGrammemes(List.of(Grammar.PartOfSpeech.NOUN, Grammar.Number.PLURAL));
            Inflection suffixEs = new Inflection("es", false);
            suffixEs.addGrammemes(List.of(Grammar.PartOfSpeech.NOUN, Grammar.Number.PLURAL));
            InflectionPattern pattern1 = new InflectionPattern(1, "", grammemes, List.of(suffixS));
            InflectionPattern pattern2 = new InflectionPattern(2, "", grammemes, List.of(suffixEs));
            // Give pattern2 a higher count than pattern1
            pattern2.merge(pattern2);

            DictionaryEntry baseRareEntry = new DictionaryEntry("word", "word", true, grammemes, pattern1, 0);
            DictionaryEntry baseNonRareEntry = new DictionaryEntry("word", "word", false, grammemes, pattern2, 0);
            baseRareEntry.merge(baseNonRareEntry, true);
            // Before override, pattern1 is rare so pattern2 (non-rare) comes first:
            Assertions.assertEquals("word: noun inflection=2 inflection=1", baseRareEntry.toString(true));

            // Now merge a later source (sourceIndex=1) that uses pattern1 as non-rare:
            DictionaryEntry overrideNonRareEntry = new DictionaryEntry("word", "word", false, grammemes, pattern1, 1);
            baseRareEntry.merge(overrideNonRareEntry, true);
            Assertions.assertEquals("word: noun inflection=1 inflection=2", baseRareEntry.toString(true));
        } finally {
            Files.deleteIfExists(tempDmlex);
        }
    }

    @Test
    public void multiFileDMLexSpaceAndCommaSeparatedTest() throws Exception {
        String[] spaceArgs = {
                "--inflection-types", "noun,proper-noun",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", dmlexLexiconFile, dmlexPatchFile
        };
        String spaceOutput = getParserOutput(spaceArgs);

        String[] commaArgs = {
                "--inflection-types", "noun,proper-noun",
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", dmlexLexiconFile + "," + dmlexPatchFile
        };
        String commaOutput = getParserOutput(commaArgs);

        compareOutputs(spaceOutput, commaOutput);

        Map<String, String> entries = parseDictionaryEntries(spaceOutput);
        // "cat" surface form was overridden by dmlexPatch.json: feminine, plural "kitties" added,
        // while "cats" from dmlexLexicon.json remains from normal processing (Option 4 + Option 3).
        Assertions.assertTrue(entries.get("cat").contains("singular feminine noun"));
        Assertions.assertFalse(entries.get("cat").contains("masculine"));
        Assertions.assertTrue(entries.containsKey("kitties"));
        Assertions.assertTrue(entries.containsKey("cats"));

        // "bats" surface form from lemma "bat" (masculine noun) was overridden by lemma "batte" (feminine noun)
        Assertions.assertTrue(entries.get("bats").contains("plural feminine noun"));
        Assertions.assertFalse(entries.get("bats").contains("masculine"));

        // "wow" surface form was overridden by dmlexPatch.json (verb)
        Assertions.assertEquals("infinitive verb", entries.get("wow"));
        Assertions.assertEquals("past verb", entries.get("wowed"));
        assertAllXmlPatternsReferencedInDictionary(spaceOutput);
    }

    @Test
    public void invalidDMLexSchemaAndUnknownOptionTest() throws Exception {
        // 1. JSON object missing required "langCode" / "entries"
        String[] invalidObjArgs = {
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", dmlexInvalidSchemaFile
        };
        Assertions.assertThrows(MismatchedInputException.class, () -> ParseWikidata.main(invalidObjArgs));

        // 2. Passing Wikidata JSON array to --dmlex
        String[] wikidataAsDmlexArgs = {
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", lexiconSource
        };
        Assertions.assertThrows(MismatchedInputException.class, () -> ParseWikidata.main(wikidataAsDmlexArgs));

        // 3. Bare positional file without --wikidata or --dmlex is rejected
        String[] bareFileArgs = {
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                lexiconSource
        };
        Assertions.assertThrows(IllegalArgumentException.class, () -> ParseWikidata.main(bareFileArgs));

        // 4. --dmlex with no file argument is rejected
        String[] emptyDmlexArgs = {
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex"
        };
        Assertions.assertThrows(IllegalArgumentException.class, () -> ParseWikidata.main(emptyDmlexArgs));

        // 5. Strict validation: unknown property, missing/multiple POS, undefined tag, unresolvable sameAs,
        //    and missing headword in inflectedForms must fail loudly with file name and entry id.
        Path tempInvalid = Files.createTempFile(Paths.get(baseDir), "dmlex-strict-invalid-", ".json");
        try {
            // 5a. Unknown JSON property on entry ("partOfSpeech" instead of "partsOfSpeech")
            Files.writeString(tempInvalid, """
                    {
                      "langCode": "en",
                      "entries": [
                        { "id": "bad-prop", "headword": "cat", "partOfSpeech": ["n"] }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            String[] badArgs = {
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", tempInvalid.toString()
            };
            MismatchedInputException badPropEx = Assertions.assertThrows(
                    MismatchedInputException.class, () -> ParseWikidata.main(badArgs));
            Assertions.assertTrue(badPropEx.getMessage().contains(tempInvalid.toString()));

            // 5b. Entry with no part of speech
            Files.writeString(tempInvalid, """
                    {
                      "langCode": "en",
                      "entries": [
                        { "id": "no-pos", "headword": "cat" }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            IllegalArgumentException noPosEx = Assertions.assertThrows(
                    IllegalArgumentException.class, () -> ParseWikidata.main(badArgs));
            Assertions.assertTrue(noPosEx.getMessage().contains(tempInvalid.toString()));
            Assertions.assertTrue(noPosEx.getMessage().contains("no-pos"));

            // 5c. Entry with multiple parts of speech
            Files.writeString(tempInvalid, """
                    {
                      "langCode": "en",
                      "entries": [
                        { "id": "multi-pos", "headword": "Czech", "partsOfSpeech": ["n", "adj"] }
                      ],
                      "partOfSpeechTags": [
                        { "tag": "n", "sameAs": ["http://www.wikidata.org/entity/Q1084"] },
                        { "tag": "adj", "sameAs": ["http://www.wikidata.org/entity/Q34698"] }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            IllegalArgumentException multiPosEx = Assertions.assertThrows(
                    IllegalArgumentException.class, () -> ParseWikidata.main(badArgs));
            Assertions.assertTrue(multiPosEx.getMessage().contains(tempInvalid.toString()));
            Assertions.assertTrue(multiPosEx.getMessage().contains("multi-pos"));

            // 5d. Undefined tag in partsOfSpeech
            Files.writeString(tempInvalid, """
                    {
                      "langCode": "en",
                      "entries": [
                        { "id": "undef-tag", "headword": "cat", "partsOfSpeech": ["n-missing"] }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            IllegalArgumentException undefTagEx = Assertions.assertThrows(
                    IllegalArgumentException.class, () -> ParseWikidata.main(badArgs));
            Assertions.assertTrue(undefTagEx.getMessage().contains(tempInvalid.toString()));
            Assertions.assertTrue(undefTagEx.getMessage().contains("undef-tag"));

            // 5e. Unresolvable sameAs URI
            Files.writeString(tempInvalid, """
                    {
                      "langCode": "en",
                      "entries": [
                        { "id": "bad-uri", "headword": "cat", "partsOfSpeech": ["n"] }
                      ],
                      "partOfSpeechTags": [
                        { "tag": "n", "sameAs": ["http://www.wikidata.org/entity/Q999999999"] }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            IllegalArgumentException badUriEx = Assertions.assertThrows(
                    IllegalArgumentException.class, () -> ParseWikidata.main(badArgs));
            Assertions.assertTrue(badUriEx.getMessage().contains(tempInvalid.toString()));
            Assertions.assertTrue(badUriEx.getMessage().contains("bad-uri"));

            // 5f. Non-empty inflectedForms missing the headword form (unmodified A.28 without headword form)
            Files.writeString(tempInvalid, """
                    {
                      "langCode": "ga",
                      "entries": [
                        {
                          "id": "folúsghlantóir-n",
                          "headword": "folúsghlantóir",
                          "partsOfSpeech": ["n-masc"],
                          "inflectedForms": [
                            { "tag": "sg-gen", "text": "folúsghlantóra" },
                            { "tag": "pl", "text": "folúsghlantóirí" }
                          ]
                        }
                      ],
                      "partOfSpeechTags": [
                        {
                          "tag": "n-masc",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q1084",
                            "http://www.wikidata.org/entity/Q499327"
                          ]
                        }
                      ],
                      "inflectedFormTags": [
                        {
                          "tag": "sg-gen",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q110786",
                            "http://www.wikidata.org/entity/Q146233"
                          ]
                        },
                        {
                          "tag": "pl",
                          "sameAs": ["http://www.wikidata.org/entity/Q146786"]
                        }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            String[] gaBadArgs = {
                    "--language", "ga",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", tempInvalid.toString()
            };
            IllegalArgumentException missingHwEx = Assertions.assertThrows(
                    IllegalArgumentException.class, () -> ParseWikidata.main(gaBadArgs));
            Assertions.assertTrue(missingHwEx.getMessage().contains(tempInvalid.toString()));
            Assertions.assertTrue(missingHwEx.getMessage().contains("folúsghlantóir-n"));
        } finally {
            Files.deleteIfExists(tempInvalid);
        }
    }

    @Test
    public void dmlexCliOptionsCompatibilityTest() throws Exception {
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-opts-", ".json");
        Path tempExcludedDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-opts-md-", ".json");
        Map<String, Set<? extends Enum<?>>> savedTypeMap = new HashMap<>(Grammar.TYPEMAP.size());
        for (var entry : Grammar.TYPEMAP.entrySet()) {
            savedTypeMap.put(entry.getKey(), new HashSet<>(entry.getValue()));
        }
        try {
            String json = """
                    {
                      "langCode": "ro",
                      "entries": [
                        {
                          "id": "ro-1",
                          "headword": "casă",
                          "partsOfSpeech": ["n-fem"],
                          "labels": ["cnt", "dial"],
                          "inflectedForms": [
                            { "tag": "sg-nom-acc", "text": "casă" },
                            { "tag": "pl-nom-acc", "text": "case" },
                            { "tag": "sg-short", "text": "casă-short" },
                            { "tag": "sg", "text": "casă-archaic", "labels": ["arch"] }
                          ]
                        }
                      ],
                      "partOfSpeechTags": [
                        {
                          "tag": "n-fem",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q1084",
                            "http://www.wikidata.org/entity/Q1775415"
                          ]
                        }
                      ],
                      "labelTags": [
                        { "tag": "cnt", "sameAs": ["http://www.wikidata.org/entity/Q1520033"] },
                        { "tag": "dial", "sameAs": ["http://www.wikidata.org/entity/Q99990001"] },
                        { "tag": "arch", "sameAs": ["http://www.wikidata.org/entity/Q99990002"] }
                      ],
                      "inflectedFormTags": [
                        {
                          "tag": "sg",
                          "sameAs": ["http://www.wikidata.org/entity/Q110786"]
                        },
                        {
                          "tag": "sg-nom-acc",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q110786",
                            "http://www.wikidata.org/entity/Q99990003"
                          ]
                        },
                        {
                          "tag": "pl-nom-acc",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q146786",
                            "http://www.wikidata.org/entity/Q99990003"
                          ]
                        },
                        {
                          "tag": "sg-short",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q110786",
                            "http://www.wikidata.org/entity/Q96406487"
                          ]
                        }
                      ]
                    }
                    """;
            String excludedJson = """
                    {
                      "langCode": "ro-md",
                      "entries": [
                        {
                          "id": "ro-md-1",
                          "headword": "moldovanWord",
                          "partsOfSpeech": ["n"],
                          "inflectedForms": [
                            { "tag": "sg", "text": "moldovanWord" }
                          ]
                        }
                      ],
                      "partOfSpeechTags": [
                        { "tag": "n", "sameAs": ["http://www.wikidata.org/entity/Q1084"] }
                      ],
                      "inflectedFormTags": [
                        { "tag": "sg", "sameAs": ["http://www.wikidata.org/entity/Q110786"] }
                      ]
                    }
                    """;
            Files.writeString(tempDmlex, json, StandardCharsets.UTF_8);
            Files.writeString(tempExcludedDmlex, excludedJson, StandardCharsets.UTF_8);

            String[] args = {
                    "--language", "ro",
                    "--exclude-language", "ro-md",
                    "--inflection-types", "noun,proper-noun",
                    "--map-grammeme", "Q99990003,nominative,accusative",
                    "--expand-grammemes", "singular,nominative,accusative,feminine,noun:indefinite",
                    "--ignore-property", "countable,Q99990001",
                    "--ignore-entries-with-grammemes", "short-form,Q99990002",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", tempDmlex.toString(), tempExcludedDmlex.toString()
            };
            String actual = getParserOutput(args);
            Map<String, String> entries = parseDictionaryEntries(actual);

            // ro-md lexicon excluded via --exclude-language
            Assertions.assertFalse(entries.containsKey("moldovanWord"));
            // short-form and Q99990002 ("arch") surface forms ignored via --ignore-entries-with-grammemes
            Assertions.assertFalse(entries.containsKey("casă-short"));
            Assertions.assertFalse(entries.containsKey("casă-archaic"));
            // countable and Q99990001 ("dial") ignored via --ignore-property;
            // Q99990003 mapped to nominative,accusative; singular expanded with indefinite via --expand-grammemes
            Assertions.assertEquals("singular accusative nominative indefinite feminine noun", entries.get("casă"));
            Assertions.assertEquals("plural accusative nominative feminine noun", entries.get("case"));
        } finally {
            Grammar.TYPEMAP.clear();
            Grammar.TYPEMAP.putAll(savedTypeMap);
            Files.deleteIfExists(tempDmlex);
            Files.deleteIfExists(tempExcludedDmlex);
        }
    }

    @Test
    public void dmlexEntriesBeforeLangCodeImplicitSchemeAndReverseOrderTest() throws Exception {
        // Tests:
        // 1. "entries" appearing before "langCode" in the root DMLex JSON object.
        // 2. Implicit transcription scheme (omitted "scheme") treated as IPA, while non-fonipa scheme is ignored.
        // 3. Reverse source order (--dmlex <file> --wikidata <file>), where Wikidata overrides DMLex.
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-reverse-", ".json");
        try {
            String json = """
                    {
                      "entries": [
                        {
                          "id": "dmlex-apple",
                          "headword": "apple",
                          "partsOfSpeech": ["n"],
                          "labels": ["nonhuman"],
                          "pronunciations": [
                            {
                              "labels": ["US"],
                              "transcriptions": [
                                { "text": "ˈæp.əl" }
                              ]
                            }
                          ],
                          "inflectedForms": [
                            {
                              "tag": "sg",
                              "text": "apple"
                            },
                            {
                              "tag": "pl",
                              "text": "apples"
                            }
                          ]
                        },
                        {
                          "id": "dmlex-ciel-early",
                          "headword": "ciel",
                          "partsOfSpeech": ["n-fem"],
                          "pronunciations": [
                            {
                              "labels": ["en-US"],
                              "transcriptions": [
                                { "text": "sjɛl", "scheme": "fr-fonipa" },
                                { "text": "ael", "scheme": "fr-Latn" }
                              ]
                            }
                          ],
                          "inflectedForms": [
                            { "tag": "sg", "text": "ciel" },
                            { "tag": "pl", "text": "cieux" },
                            { "tag": "pl", "text": "ciel-dmlex-only" }
                          ]
                        },
                        {
                          "id": "dmlex-intj-early",
                          "headword": "uh-oh",
                          "partsOfSpeech": ["intj"],
                          "inflectedForms": [
                            { "tag": "sg", "text": "uh-oh" }
                          ]
                        }
                      ],
                      "partOfSpeechTags": [
                        { "tag": "n", "sameAs": ["http://www.wikidata.org/entity/Q1084"] },
                        {
                          "tag": "n-fem",
                          "sameAs": [
                            "http://www.wikidata.org/entity/Q1084",
                            "http://www.wikidata.org/entity/Q1775415"
                          ]
                        },
                        { "tag": "intj", "sameAs": ["http://www.wikidata.org/entity/Q83034"] }
                      ],
                      "labelTags": [
                        { "tag": "nonhuman", "sameAs": ["http://www.wikidata.org/entity/Q67372837"] }
                      ],
                      "inflectedFormTags": [
                        { "tag": "sg", "sameAs": ["http://www.wikidata.org/entity/Q110786"] },
                        { "tag": "pl", "sameAs": ["http://www.wikidata.org/entity/Q146786"] }
                      ],
                      "langCode": "en"
                    }
                    """;
            Files.writeString(tempDmlex, json, StandardCharsets.UTF_8);

            Path tempDmlex2 = Files.createTempFile(Paths.get(baseDir), "dmlex-intj-late-", ".json");
            try {
                String json2 = """
                        {
                          "langCode": "en",
                          "entries": [
                            {
                              "id": "dmlex-intj-late",
                              "headword": "uh-oh",
                              "partsOfSpeech": ["intj"],
                              "inflectedForms": [
                                { "tag": "pl", "text": "uh-oh" }
                              ]
                            }
                          ],
                          "partOfSpeechTags": [
                            { "tag": "intj", "sameAs": ["http://www.wikidata.org/entity/Q83034"] }
                          ],
                          "inflectedFormTags": [
                            { "tag": "pl", "sameAs": ["http://www.wikidata.org/entity/Q146786"] }
                          ]
                        }
                        """;
                Files.writeString(tempDmlex2, json2, StandardCharsets.UTF_8);

                String[] args = {
                        "--inflection-types", "noun,adjective,proper-noun",
                        "--add-sound", "vowel-start",
                        "--inflections", inflectionalFile,
                        "--dictionary", dictionaryFile,
                        "--dmlex", tempDmlex.toString(),
                        "--wikidata", lexiconSource,
                        "--dmlex", tempDmlex2.toString()
                };
                String actual = getParserOutput(args);
                Map<String, String> entries = parseDictionaryEntries(actual);

                // "apple" from DMLex is parsed with implicit IPA scheme and "entries" before "langCode"
                Assertions.assertTrue(entries.get("apple").startsWith("singular nonhuman vowel-start noun inflection="));
                Assertions.assertTrue(entries.get("apples").startsWith("plural nonhuman vowel-start noun inflection="));

                // Because --wikidata came AFTER --dmlex, Wikidata's masculine "ciel" overrides DMLex's feminine "ciel"
                Assertions.assertTrue(entries.get("ciel").contains("masculine"));
                Assertions.assertFalse(entries.get("ciel").contains("feminine"));
                Assertions.assertTrue(entries.containsKey("ciels"));
                Assertions.assertTrue(entries.get("cieux").contains("masculine"));
                Assertions.assertFalse(entries.get("cieux").contains("feminine"));
                // "ciel-dmlex-only" was not in Wikidata, and its non-IPA transcription ("fr-Latn": "ael")
                // must not be treated as a vowel-start IPA transcription.
                Assertions.assertTrue(entries.get("ciel-dmlex-only").startsWith("plural feminine noun inflection="));
                Assertions.assertFalse(entries.get("ciel-dmlex-only").contains("vowel-start"));

                // Non-inflected POS surface form "uh-oh" from tempDmlex was overridden by tempDmlex2
                Assertions.assertEquals("plural interjection", entries.get("uh-oh"));
                assertAllXmlPatternsReferencedInDictionary(actual);
            } finally {
                Files.deleteIfExists(tempDmlex2);
            }
        } finally {
            Files.deleteIfExists(tempDmlex);
        }
    }

    @Test
    public void deferredLexemeMultiVariantAndSupplementalMergeTest() throws Exception {
        // filter_fr.properties defines L27261=L1566399 (organisateur = organisatrice).
        // Tests:
        // 1. Primary Wikidata file (sourceIndex == 0) with multi-variant L27261 ("fr" and "fr-x-Q56352306")
        //    and L1566399 merges both variants and both partner lexemes without throwing "Duplicate lexeme".
        // 2. Supplemental Wikidata file (sourceIndex == 1) containing only L27261 (without L1566399)
        //    does not throw "L1566399: id not found" and overrides the "organisateur" surface form.
        // 3. Duplicate top-level Lexeme objects with the same deferred ID in one file still throw IllegalStateException.
        Path primaryWikidata = Files.createTempFile(Paths.get(baseDir), "wikidata-fr-primary-", ".json");
        Path supplementalWikidata = Files.createTempFile(Paths.get(baseDir), "wikidata-fr-supp-", ".json");
        Path duplicateWikidata = Files.createTempFile(Paths.get(baseDir), "wikidata-fr-dup-", ".json");
        try {
            String primaryJson = """
                    [
                      {
                        "type": "lexeme",
                        "id": "L27261",
                        "lemmas": {
                          "fr": { "language": "fr", "value": "organisateur" },
                          "fr-x-Q56352306": { "language": "fr-x-Q56352306", "value": "organisateur" }
                        },
                        "lexicalCategory": "Q1084",
                        "claims": {},
                        "forms": [
                          {
                            "id": "L27261-F1",
                            "representations": {
                              "fr": { "language": "fr", "value": "organisateur" }
                            },
                            "grammaticalFeatures": ["Q110786", "Q499327"],
                            "claims": {}
                          },
                          {
                            "id": "L27261-F2",
                            "representations": {
                              "fr": { "language": "fr", "value": "organisateurs-old" }
                            },
                            "grammaticalFeatures": ["Q146786", "Q499327"],
                            "claims": {}
                          }
                        ],
                        "senses": []
                      },
                      {
                        "type": "lexeme",
                        "id": "L1566399",
                        "lemmas": {
                          "fr": { "language": "fr", "value": "organisatrice" }
                        },
                        "lexicalCategory": "Q1084",
                        "claims": {},
                        "forms": [
                          {
                            "id": "L1566399-F1",
                            "representations": {
                              "fr": { "language": "fr", "value": "organisatrice" }
                            },
                            "grammaticalFeatures": ["Q110786", "Q1775415"],
                            "claims": {}
                          }
                        ],
                        "senses": []
                      }
                    ]
                    """;
            String supplementalJson = """
                    [
                      {
                        "type": "lexeme",
                        "id": "L27261",
                        "lemmas": {
                          "fr": { "language": "fr", "value": "organisateur" }
                        },
                        "lexicalCategory": "Q1084",
                        "claims": {},
                        "forms": [
                          {
                            "id": "L27261-F1",
                            "representations": {
                              "fr": { "language": "fr", "value": "organisateur" }
                            },
                            "grammaticalFeatures": ["Q110786", "Q499327"],
                            "claims": {}
                          },
                          {
                            "id": "L27261-F2",
                            "representations": {
                              "fr": { "language": "fr", "value": "organisateurs" }
                            },
                            "grammaticalFeatures": ["Q146786", "Q499327"],
                            "claims": {}
                          }
                        ],
                        "senses": []
                      }
                    ]
                    """;
            String duplicateJson = """
                    [
                      {
                        "type": "lexeme",
                        "id": "L27261",
                        "lemmas": { "fr": { "language": "fr", "value": "organisateur" } },
                        "lexicalCategory": "Q1084",
                        "claims": {},
                        "forms": [
                          {
                            "id": "L27261-F1",
                            "representations": { "fr": { "language": "fr", "value": "organisateur" } },
                            "grammaticalFeatures": ["Q110786"],
                            "claims": {}
                          }
                        ],
                        "senses": []
                      },
                      {
                        "type": "lexeme",
                        "id": "L27261",
                        "lemmas": { "fr": { "language": "fr", "value": "organisateur" } },
                        "lexicalCategory": "Q1084",
                        "claims": {},
                        "forms": [
                          {
                            "id": "L27261-F1",
                            "representations": { "fr": { "language": "fr", "value": "organisateur" } },
                            "grammaticalFeatures": ["Q110786"],
                            "claims": {}
                          }
                        ],
                        "senses": []
                      }
                    ]
                    """;
            Files.writeString(primaryWikidata, primaryJson, StandardCharsets.UTF_8);
            Files.writeString(supplementalWikidata, supplementalJson, StandardCharsets.UTF_8);
            Files.writeString(duplicateWikidata, duplicateJson, StandardCharsets.UTF_8);

            String[] args = {
                    "--language", "fr",
                    "--inflection-types", "noun",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", primaryWikidata.toString(), supplementalWikidata.toString()
            };
            String actual = getParserOutput(args);
            Map<String, String> entries = parseDictionaryEntries(actual);

            Assertions.assertTrue(entries.containsKey("organisateur"));
            Assertions.assertTrue(entries.containsKey("organisateurs"));
            Assertions.assertTrue(entries.containsKey("organisatrice"));
            // "organisateur" was overridden by supplementalWikidata, so it shares the same pattern as "organisateurs"
            Assertions.assertEquals(
                    entries.get("organisateurs").split("inflection=")[1],
                    entries.get("organisateur").split("inflection=")[1]);
            assertAllXmlPatternsReferencedInDictionary(actual);

            String[] dupArgs = {
                    "--language", "fr",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", duplicateWikidata.toString()
            };
            Assertions.assertThrows(IllegalStateException.class, () -> ParseWikidata.main(dupArgs));
        } finally {
            Files.deleteIfExists(primaryWikidata);
            Files.deleteIfExists(supplementalWikidata);
            Files.deleteIfExists(duplicateWikidata);
        }
    }
}
