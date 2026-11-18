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
    }

    @Test
    public void nullMappedGrammemeThrowsTest() {
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
    public void multiSourceWikidataAndDMLexMergeTest() throws Exception {
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

        // 2. "hey" merges Wikidata (interjection) and DMLex (singular noun)
        String heyEntry = entries.get("hey");
        Assertions.assertNotNull(heyEntry);
        Assertions.assertTrue(heyEntry.contains("noun"), "Expected noun in: " + heyEntry);
        Assertions.assertTrue(heyEntry.contains("interjection"), "Expected interjection in: " + heyEntry);
        Assertions.assertTrue(entries.containsKey("heys"));

        // 3. "ciel" and "cieux" merge Wikidata (masculine) and DMLex (feminine)
        String cielEntry = entries.get("ciel");
        Assertions.assertNotNull(cielEntry);
        Assertions.assertTrue(cielEntry.contains("masculine"), "Expected masculine in: " + cielEntry);
        Assertions.assertTrue(cielEntry.contains("feminine"), "Expected feminine in: " + cielEntry);
        String[] cielParts = cielEntry.split("inflection=");
        Assertions.assertEquals(3, cielParts.length, "Expected 2 inflection patterns in: " + cielEntry);
        String firstPatternId = cielParts[1].trim();
        String secondPatternId = cielParts[2].trim();

        // "ciels" only exists in Wikidata; Wikidata's pattern has count=3 vs count=1 in dmlexOverlayFile, so it comes first
        String cielsEntry = entries.get("ciels");
        Assertions.assertNotNull(cielsEntry);
        String wikidataPatternId = cielsEntry.split("inflection=")[1].trim();
        Assertions.assertEquals(wikidataPatternId, firstPatternId,
                "Expected higher-count Wikidata pattern to come first in: " + cielEntry);
        Assertions.assertNotEquals(firstPatternId, secondPatternId);

        String cieuxEntry = entries.get("cieux");
        Assertions.assertNotNull(cieuxEntry);
        Assertions.assertTrue(cieuxEntry.contains("masculine"), "Expected masculine in: " + cieuxEntry);
        Assertions.assertTrue(cieuxEntry.contains("feminine"), "Expected feminine in: " + cieuxEntry);
        assertAllXmlPatternsReferencedInDictionary(actual);
    }

    @Test
    public void multiSourceArgumentOrderDefinesEqualCountPatternOrderTest() throws Exception {
        // Verify that:
        // 1. When rarity and pattern word counts are equal ("when all other things are equal"),
        //    the CLI argument order of --wikidata and --dmlex defines the inflection pattern priority order
        //    on ambiguous surface forms (first definition comes first).
        // 2. When pattern word counts differ (e.g. "ciel" in lexiconSource has count=3 vs count=1 in dmlexOverlayFile),
        //    the higher-count pattern takes precedence regardless of CLI argument order.
        // 3. When rarity differs (a rare lemma in the earlier source vs a non-rare lemma in the later source,
        //    both with count=1), the non-rare pattern still precedes the rare pattern.
        Path tempWikidata = Files.createTempFile(Paths.get(baseDir), "wikidata-order-", ".json");
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-order-", ".json");
        Path tempRareDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-rare-order-", ".json");
        try {
            Files.writeString(tempWikidata, """
                    [
                      {
                        "type": "lexeme",
                        "id": "L100",
                        "lemmas": { "en": { "language": "en", "value": "index" } },
                        "lexicalCategory": "Q1084",
                        "claims": {},
                        "forms": [
                          {
                            "id": "L100-F1",
                            "representations": { "en": { "language": "en", "value": "index" } },
                            "grammaticalFeatures": ["Q110786"],
                            "claims": {}
                          },
                          {
                            "id": "L100-F2",
                            "representations": { "en": { "language": "en", "value": "indexes" } },
                            "grammaticalFeatures": ["Q146786"],
                            "claims": {}
                          }
                        ],
                        "senses": []
                      }
                    ]
                    """, StandardCharsets.UTF_8);
            Files.writeString(tempDmlex, """
                    {
                      "langCode": "en",
                      "entries": [
                        {
                          "id": "dmlex-index",
                          "headword": "index",
                          "partsOfSpeech": ["n"],
                          "inflectedForms": [
                            { "tag": "sg", "text": "index" },
                            { "tag": "pl", "text": "indices" }
                          ]
                        }
                      ],
                      "partOfSpeechTags": [
                        { "tag": "n", "sameAs": ["http://www.wikidata.org/entity/Q1084"] }
                      ],
                      "inflectedFormTags": [
                        { "tag": "sg", "sameAs": ["http://www.wikidata.org/entity/Q110786"] },
                        { "tag": "pl", "sameAs": ["http://www.wikidata.org/entity/Q146786"] }
                      ]
                    }
                    """, StandardCharsets.UTF_8);

            // Case 1a: Equal counts (count=1 each), --wikidata before --dmlex -> Wikidata pattern first
            String[] wikidataFirstArgs = {
                    "--inflection-types", "noun",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", tempWikidata.toString(),
                    "--dmlex", tempDmlex.toString()
            };
            Map<String, String> wdFirstEntries = parseDictionaryEntries(getParserOutput(wikidataFirstArgs));
            String wdPatternId1 = wdFirstEntries.get("indexes").split("inflection=")[1].trim();
            String dmPatternId1 = wdFirstEntries.get("indices").split("inflection=")[1].trim();
            Assertions.assertNotEquals(wdPatternId1, dmPatternId1);
            String[] wdFirstIndexParts = wdFirstEntries.get("index").split("inflection=");
            Assertions.assertEquals(3, wdFirstIndexParts.length);
            Assertions.assertEquals(wdPatternId1, wdFirstIndexParts[1].trim(),
                    "Expected --wikidata pattern first when --wikidata precedes --dmlex");
            Assertions.assertEquals(dmPatternId1, wdFirstIndexParts[2].trim());

            // Case 1b: Equal counts (count=1 each), --dmlex before --wikidata -> DMLex pattern first
            String[] dmlexFirstArgs = {
                    "--inflection-types", "noun",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", tempDmlex.toString(),
                    "--wikidata", tempWikidata.toString()
            };
            String dmFirstOutput = getParserOutput(dmlexFirstArgs);
            Map<String, String> dmFirstEntries = parseDictionaryEntries(dmFirstOutput);
            String wdPatternId2 = dmFirstEntries.get("indexes").split("inflection=")[1].trim();
            String dmPatternId2 = dmFirstEntries.get("indices").split("inflection=")[1].trim();
            Assertions.assertNotEquals(wdPatternId2, dmPatternId2);
            String[] dmFirstIndexParts = dmFirstEntries.get("index").split("inflection=");
            Assertions.assertEquals(3, dmFirstIndexParts.length);
            Assertions.assertEquals(dmPatternId2, dmFirstIndexParts[1].trim(),
                    "Expected --dmlex pattern first when --dmlex precedes --wikidata");
            Assertions.assertEquals(wdPatternId2, dmFirstIndexParts[2].trim());
            assertAllXmlPatternsReferencedInDictionary(dmFirstOutput);

            // Case 2: Unequal counts ("ciel" in lexiconSource has count=3 vs count=1 in dmlexOverlayFile) ->
            // higher-count Wikidata pattern still wins even when --dmlex precedes --wikidata
            String[] unequalCountDmlexFirstArgs = {
                    "--inflection-types", "noun,adjective,proper-noun",
                    "--add-sound", "vowel-start",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", dmlexOverlayFile,
                    "--wikidata", lexiconSource
            };
            Map<String, String> unequalEntries = parseDictionaryEntries(getParserOutput(unequalCountDmlexFirstArgs));
            String higherCountWikidataPatternId = unequalEntries.get("ciels").split("inflection=")[1].trim();
            String[] unequalCielParts = unequalEntries.get("ciel").split("inflection=");
            Assertions.assertEquals(3, unequalCielParts.length);
            Assertions.assertEquals(higherCountWikidataPatternId, unequalCielParts[1].trim(),
                    "Expected higher-count pattern to precede lower-count pattern even when --dmlex came first");

            // Case 3: Unequal rarity (rare DMLex lemma first vs non-rare Wikidata lemma second, both count=1) ->
            // non-rare Wikidata pattern precedes rare DMLex pattern despite CLI order
            Files.writeString(tempRareDmlex, """
                    {
                      "langCode": "en",
                      "entries": [
                        {
                          "id": "dmlex-index-rare",
                          "headword": "index",
                          "partsOfSpeech": ["n"],
                          "labels": ["rare"],
                          "inflectedForms": [
                            { "tag": "sg", "text": "index" },
                            { "tag": "pl", "text": "indices" }
                          ]
                        }
                      ],
                      "partOfSpeechTags": [
                        { "tag": "n", "sameAs": ["http://www.wikidata.org/entity/Q1084"] }
                      ],
                      "labelTags": [
                        { "tag": "rare", "sameAs": ["http://www.wikidata.org/entity/Q55094451"] }
                      ],
                      "inflectedFormTags": [
                        { "tag": "sg", "sameAs": ["http://www.wikidata.org/entity/Q110786"] },
                        { "tag": "pl", "sameAs": ["http://www.wikidata.org/entity/Q146786"] }
                      ]
                    }
                    """, StandardCharsets.UTF_8);
            String[] rareDmlexFirstArgs = {
                    "--inflection-types", "noun",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", tempRareDmlex.toString(),
                    "--wikidata", tempWikidata.toString()
            };
            Map<String, String> rareFirstEntries = parseDictionaryEntries(getParserOutput(rareDmlexFirstArgs));
            String nonRareWikidataPatternId = rareFirstEntries.get("indexes").split("inflection=")[1].trim();
            String rareDmlexPatternId = rareFirstEntries.get("indices").split("inflection=")[1].trim();
            String[] rareFirstIndexParts = rareFirstEntries.get("index").split("inflection=");
            Assertions.assertEquals(3, rareFirstIndexParts.length);
            Assertions.assertEquals(nonRareWikidataPatternId, rareFirstIndexParts[1].trim(),
                    "Expected non-rare pattern to precede rare pattern even when rare source came first");
            Assertions.assertEquals(rareDmlexPatternId, rareFirstIndexParts[2].trim());
        } finally {
            Files.deleteIfExists(tempWikidata);
            Files.deleteIfExists(tempDmlex);
            Files.deleteIfExists(tempRareDmlex);
        }
    }

    @Test
    public void multiSourceSameFileHomographMergeTest() throws Exception {
        // Verify that multiple entries for the same surface form ("Élie" as both proper-noun and verb)
        // in a DMLex file merge cleanly with Wikidata's entry and preserve all referenced XML patterns.
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

            String[] mergeArgs = {
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
            Set<String> mergeXmlPatternIds = assertAllXmlPatternsReferencedInDictionary(mergeOutput);
            Assertions.assertEquals(8, mergeXmlPatternIds.size());
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
        // "cat" merges dmlexLexicon.json (masculine, plural "cats") and dmlexPatch.json (feminine, plural "kitties")
        Assertions.assertTrue(entries.get("cat").contains("singular masculine feminine noun"));
        Assertions.assertEquals(2, entries.get("cat").split("inflection=").length - 1);
        Assertions.assertTrue(entries.containsKey("kitties"));
        Assertions.assertTrue(entries.containsKey("cats"));

        // "bats" merges lemma "bat" (masculine noun) and lemma "batte" (feminine noun)
        Assertions.assertTrue(entries.get("bats").contains("plural masculine feminine noun"));

        // "wow" merges dmlexLexicon.json (interjection) and dmlexPatch.json (infinitive verb)
        Assertions.assertEquals("infinitive interjection verb", entries.get("wow"));
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

        // 2. Passing Wikidata JSON array to --dmlex or DMLex JSON object to --wikidata
        String[] wikidataAsDmlexArgs = {
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--dmlex", lexiconSource
        };
        Assertions.assertThrows(MismatchedInputException.class, () -> ParseWikidata.main(wikidataAsDmlexArgs));
        String[] dmlexAsWikidataArgs = {
                "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile,
                "--wikidata", dmlexLexiconFile
        };
        Assertions.assertThrows(MismatchedInputException.class, () -> ParseWikidata.main(dmlexAsWikidataArgs));

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
    public void dmlexEntriesBeforeLangCodeImplicitSchemeAndInterleavedSourcesTest() throws Exception {
        // Tests:
        // 1. "entries" appearing before "langCode" in the root DMLex JSON object.
        // 2. Implicit transcription scheme (omitted "scheme") treated as IPA, while non-fonipa scheme is ignored.
        // 3. Interleaved sources (--dmlex <file1> --wikidata <file2> --dmlex <file3>) merging across all files.
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-interleaved-", ".json");
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

                // Both DMLex (feminine) and Wikidata (masculine) grammemes are merged on "ciel" and "cieux"
                Assertions.assertTrue(entries.get("ciel").contains("masculine"));
                Assertions.assertTrue(entries.get("ciel").contains("feminine"));
                Assertions.assertTrue(entries.containsKey("ciels"));
                Assertions.assertTrue(entries.get("cieux").contains("masculine"));
                Assertions.assertTrue(entries.get("cieux").contains("feminine"));
                // "ciel-dmlex-only" was not in Wikidata, and its non-IPA transcription ("fr-Latn": "ael")
                // must not be treated as a vowel-start IPA transcription.
                Assertions.assertTrue(entries.get("ciel-dmlex-only").startsWith("plural feminine noun inflection="));
                Assertions.assertFalse(entries.get("ciel-dmlex-only").contains("vowel-start"));

                // Non-inflected POS surface form "uh-oh" merges tempDmlex (singular) and tempDmlex2 (plural)
                Assertions.assertEquals("singular plural interjection", entries.get("uh-oh"));
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
        // 1. Primary Wikidata file with multi-variant L27261 ("fr" and "fr-x-Q56352306")
        //    and L1566399 merges both variants and both partner lexemes without throwing "Duplicate lexeme".
        // 2. Supplemental Wikidata file containing only L27261 (without L1566399)
        //    does not throw "L1566399: id not found" and merges with the primary file (with primary pattern first).
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
            Assertions.assertTrue(entries.containsKey("organisateurs-old"));
            Assertions.assertTrue(entries.containsKey("organisateurs"));
            Assertions.assertTrue(entries.containsKey("organisatrice"));
            // "organisateur" merges patterns from both Wikidata files, with primaryWikidata's pattern first
            String primaryPatternId = entries.get("organisateurs-old").split("inflection=")[1].trim();
            String supplementalPatternId = entries.get("organisateurs").split("inflection=")[1].trim();
            String[] organisateurParts = entries.get("organisateur").split("inflection=");
            Assertions.assertEquals(3, organisateurParts.length);
            Assertions.assertEquals(primaryPatternId, organisateurParts[1].trim());
            Assertions.assertEquals(supplementalPatternId, organisateurParts[2].trim());
            assertAllXmlPatternsReferencedInDictionary(actual);

            // Also verify passing the partial supplemental Wikidata file BEFORE the primary Wikidata file
            // does not fail with "L1566399: id not found" and places supplementalWikidata's pattern first.
            String[] suppFirstArgs = {
                    "--language", "fr",
                    "--inflection-types", "noun",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", supplementalWikidata.toString(), primaryWikidata.toString()
            };
            String suppFirstOutput = getParserOutput(suppFirstArgs);
            Map<String, String> suppFirstEntries = parseDictionaryEntries(suppFirstOutput);
            String suppFirstPrimaryId = suppFirstEntries.get("organisateurs-old").split("inflection=")[1].trim();
            String suppFirstSupplementalId = suppFirstEntries.get("organisateurs").split("inflection=")[1].trim();
            String[] suppFirstOrganisateurParts = suppFirstEntries.get("organisateur").split("inflection=");
            Assertions.assertEquals(3, suppFirstOrganisateurParts.length);
            Assertions.assertEquals(suppFirstSupplementalId, suppFirstOrganisateurParts[1].trim());
            Assertions.assertEquals(suppFirstPrimaryId, suppFirstOrganisateurParts[2].trim());
            assertAllXmlPatternsReferencedInDictionary(suppFirstOutput);

            // Missing deferred partner across ALL --wikidata inputs still throws IllegalArgumentException
            String[] missingPartnerArgs = {
                    "--language", "fr",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", supplementalWikidata.toString()
            };
            Assertions.assertThrows(IllegalArgumentException.class, () -> ParseWikidata.main(missingPartnerArgs));

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
