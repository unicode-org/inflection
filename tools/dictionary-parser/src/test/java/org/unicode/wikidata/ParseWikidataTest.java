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
import java.util.Map;
import java.util.Objects;
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
    private static final String dmlexInvalidSchemaFile = baseDir + "/dmlexInvalidSchema.json";
    private static final String LINE_END = "==============================================";

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

        // London uses camelCase properNoun normalized to proper-noun
        Assertions.assertEquals("singular proper-noun inflection=3", entries.get("London"));

        // wow has no inflectedForms array, synthesized default form
        Assertions.assertEquals("interjection", entries.get("wow"));

        // French entry "bonjour" must be filtered out because default --language is en
        Assertions.assertFalse(entries.containsKey("bonjour"));

        // Verify inflectional XML was generated
        String xml = Files.readString(Paths.get(inflectionalFile), StandardCharsets.UTF_8);
        Assertions.assertTrue(xml.contains("<patterns>"));
        Assertions.assertTrue(xml.contains("<pattern name=\"1\" words=\"2\">"));
    }

    @Test
    public void multiSourceWikidataAndDMLexOverwriteTest() throws Exception {
        // Default --merge-conflicts=false: later source (dmlexOverlayFile) overrides same-POS conflicts
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

        // 2. "hey" was interjection in Wikidata and noun in DMLex -> both POS are preserved
        String heyEntry = entries.get("hey");
        Assertions.assertNotNull(heyEntry);
        Assertions.assertTrue(heyEntry.contains("noun"), "Expected noun in: " + heyEntry);
        Assertions.assertTrue(heyEntry.contains("interjection"), "Expected interjection in: " + heyEntry);
        Assertions.assertTrue(entries.containsKey("heys"));

        // 3. "ciel" was masculine noun (ciel/ciels/cieux) in Wikidata, overridden by feminine noun (ciel/cieux) in DMLex
        String cielEntry = entries.get("ciel");
        Assertions.assertNotNull(cielEntry);
        Assertions.assertTrue(cielEntry.contains("feminine"), "Expected feminine in: " + cielEntry);
        Assertions.assertFalse(cielEntry.contains("masculine"), "Did not expect masculine in: " + cielEntry);

        String cieuxEntry = entries.get("cieux");
        Assertions.assertNotNull(cieuxEntry);
        Assertions.assertTrue(cieuxEntry.contains("feminine"), "Expected feminine in: " + cieuxEntry);
        Assertions.assertFalse(cieuxEntry.contains("masculine"), "Did not expect masculine in: " + cieuxEntry);

        // Orphaned Wikidata form "ciels" must not be present when ciel(noun) was superseded
        Assertions.assertFalse(entries.containsKey("ciels"), "Expected orphaned form 'ciels' to be removed");

        // 4. Verify inflectional XML: preempting "ciel" (L7, L8) at Tier 1 leaves only "idéal" (L9, L10)
        // in the -l/-ls/-ux pattern, so its word count is words="2" (instead of words="3" or words="4")
        // and "idéal" references a single merged pattern instead of two split patterns.
        String idealEntry = entries.get("idéal");
        Assertions.assertNotNull(idealEntry);
        Assertions.assertEquals(1, idealEntry.split("inflection=").length - 1, "Expected 1 merged inflection pattern in: " + idealEntry);

        String xml = Files.readString(Paths.get(inflectionalFile), StandardCharsets.UTF_8);
        Assertions.assertTrue(xml.contains("<pattern name=\"1\" words=\"2\">"));
        Assertions.assertFalse(xml.contains("words=\"3\""));
        Assertions.assertFalse(xml.contains("words=\"4\""));
    }

    @Test
    public void multiSourceWikidataAndDMLexMergeConflictsTrueTest() throws Exception {
        // --merge-conflicts=true: conflicting entries of the same POS across files are merged
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

        // In merge mode, "ciel" merges Wikidata (masculine) and DMLex (feminine), keeping both patterns and "ciels"
        String cielEntry = entries.get("ciel");
        Assertions.assertNotNull(cielEntry);
        Assertions.assertTrue(cielEntry.contains("masculine"), "Expected masculine in: " + cielEntry);
        Assertions.assertTrue(cielEntry.contains("feminine"), "Expected feminine in: " + cielEntry);
        // Should have two inflection=... references (one from Wikidata pattern, one from DMLex pattern)
        Assertions.assertEquals(2, cielEntry.split("inflection=").length - 1, "Expected 2 inflection patterns in: " + cielEntry);

        // "ciels" from Wikidata is preserved in merge mode
        Assertions.assertTrue(entries.containsKey("ciels"));
        Assertions.assertTrue(entries.get("ciels").contains("plural masculine noun"));
    }

    @Test
    public void multiSourcePartialMultiPosOverrideTest() throws Exception {
        // In sourceLexicon.json, "Élie" (L2) has Q147276 which maps to BOTH noun and proper-noun (plural masculine).
        // When a later DMLex file overrides ONLY proper-noun on "Élie" (singular feminine proper-noun),
        // the noun slice from Wikidata (plural masculine noun) must be preserved while proper-noun is replaced.
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-partial-pos-", ".json");
        try {
            String json = """
                    {
                      "langCode": "en",
                      "entries": [
                        {
                          "id": "elie-override",
                          "headword": "Élie",
                          "partOfSpeech": ["proper-noun"],
                          "tag": "feminine",
                          "labels": ["rare"],
                          "pronunciations": [
                            {
                              "transcriptions": [
                                { "text": "e.li", "scheme": "IPA" }
                              ]
                            }
                          ],
                          "inflectedForms": [
                            {
                              "value": "Élie",
                              "tag": "singular",
                              "labels": ["rare"]
                            }
                          ]
                        }
                      ]
                    }
                    """;
            Files.writeString(tempDmlex, json, StandardCharsets.UTF_8);

            String[] args = {
                    "--inflection-types", "noun,adjective,proper-noun",
                    "--add-sound", "vowel-start",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--wikidata", lexiconSource,
                    "--dmlex=" + tempDmlex
            };
            String actual = getParserOutput(args);
            Map<String, String> entries = parseDictionaryEntries(actual);

            String elieEntry = entries.get("Élie");
            Assertions.assertNotNull(elieEntry);
            // Both noun (plural masculine from Wikidata) and proper-noun (singular feminine vowel-start from DMLex) are present,
            // with 2 distinct inflection patterns (non-rare noun pattern first, rare proper-noun pattern second).
            Assertions.assertEquals(
                    "singular plural masculine feminine vowel-start noun proper-noun inflection=3 inflection=8",
                    elieEntry);
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
        // "cat" (noun) was overridden by dmlexPatch.json: feminine, plural "kitties", and "cats" removed
        Assertions.assertTrue(entries.get("cat").contains("singular feminine noun"));
        Assertions.assertFalse(entries.get("cat").contains("masculine"));
        Assertions.assertTrue(entries.containsKey("kitties"));
        Assertions.assertFalse(entries.containsKey("cats"));

        // "bats" surface form from lemma "bat" (masculine noun) was overridden at Tier 2 by lemma "batte" (feminine noun)
        Assertions.assertTrue(entries.get("bats").contains("plural feminine noun"));
        Assertions.assertFalse(entries.get("bats").contains("masculine"));

        // "wow" keeps interjection from dmlexLexicon.json and adds verb from dmlexPatch.json
        Assertions.assertEquals("interjection verb", entries.get("wow"));
        Assertions.assertEquals("verb", entries.get("wowed"));
    }

    @Test
    public void invalidDMLexSchemaAndUnknownOptionTest() {
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
    }

    @Test
    public void dmlexCliOptionsCompatibilityTest() throws Exception {
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-opts-", ".json");
        try {
            String json = """
                    {
                      "langCode": "ro",
                      "entries": [
                        {
                          "id": "ro-1",
                          "headword": "casă",
                          "partOfSpeech": ["noun"],
                          "labels": ["feminine", "countable", "dialectal"],
                          "inflectedForms": [
                            { "value": "casă", "labels": ["singular", "customNomAcc"] },
                            { "value": "case", "labels": ["plural", "customNomAcc"] },
                            { "value": "casă-short", "labels": ["singular", "short-form"] },
                            { "value": "casă-archaic", "labels": ["singular", "archaic"] }
                          ]
                        },
                        {
                          "id": "ro-md-1",
                          "headword": "moldovanWord",
                          "langCode": "ro-md",
                          "partOfSpeech": ["noun"],
                          "inflectedForms": [
                            { "value": "moldovanWord", "labels": ["singular"] }
                          ]
                        }
                      ]
                    }
                    """;
            Files.writeString(tempDmlex, json, StandardCharsets.UTF_8);

            String[] args = {
                    "--language", "ro",
                    "--exclude-language", "ro-md",
                    "--inflection-types", "noun,properNoun",
                    "--map-grammeme", "customNomAcc,nominative,accusative",
                    "--expand-grammemes", "singular,nominative,accusative,feminine,noun:indefinite",
                    "--ignore-property", "countable,dialectal",
                    "--ignore-entries-with-grammemes", "shortForm,archaic",
                    "--inflections", inflectionalFile,
                    "--dictionary", dictionaryFile,
                    "--dmlex", tempDmlex.toString()
            };
            String actual = getParserOutput(args);
            Map<String, String> entries = parseDictionaryEntries(actual);

            // ro-md entry excluded via --exclude-language
            Assertions.assertFalse(entries.containsKey("moldovanWord"));
            // short-form and unmapped "archaic" surface forms ignored via --ignore-entries-with-grammemes
            Assertions.assertFalse(entries.containsKey("casă-short"));
            Assertions.assertFalse(entries.containsKey("casă-archaic"));
            // countable and unmapped "dialectal" ignored via --ignore-property;
            // customNomAcc mapped to nominative,accusative; singular expanded with indefinite via --expand-grammemes
            Assertions.assertEquals("singular accusative nominative indefinite feminine noun", entries.get("casă"));
            Assertions.assertEquals("plural accusative nominative feminine noun", entries.get("case"));
        } finally {
            Files.deleteIfExists(tempDmlex);
        }
    }

    @Test
    public void dmlexEntriesBeforeLangCodeMultiFieldObjectsAndReverseOrderTest() throws Exception {
        // Tests:
        // 1. "entries" appearing before "langCode" in the root DMLex JSON object.
        // 2. Single-object multi-field labels ({"tag": "singular", "label": "Singular number"}) and
        //    pronunciations ({"label": "US", "transcription": "ˈæp.əl"}).
        // 3. Reverse source order (--dmlex <file> --wikidata <file>), where Wikidata overrides DMLex.
        Path tempDmlex = Files.createTempFile(Paths.get(baseDir), "dmlex-reverse-", ".json");
        try {
            String json = """
                    {
                      "entries": [
                        {
                          "id": "dmlex-apple",
                          "headword": "apple",
                          "partOfSpeech": [
                            { "label": "Common noun", "tag": "noun" }
                          ],
                          "labels": [
                            { "tag": "nonHuman", "label": "Non-human animacy" }
                          ],
                          "pronunciations": [
                            { "label": "US", "transcription": "ˈæp.əl" }
                          ],
                          "inflectedForms": [
                            {
                              "value": "apple",
                              "labels": [
                                { "tag": "singular", "label": "Singular number" }
                              ]
                            },
                            {
                              "value": "apples",
                              "labels": [
                                { "label": "Plural number", "tag": "plural" }
                              ]
                            }
                          ]
                        },
                        {
                          "id": "dmlex-ciel-early",
                          "headword": "ciel",
                          "partOfSpeech": ["noun"],
                          "labels": ["feminine"],
                          "inflectedForms": [
                            { "value": "ciel", "labels": ["singular"] },
                            { "value": "ciel-obsolete", "labels": ["plural"] }
                          ]
                        },
                        {
                          "id": "dmlex-posless-early",
                          "headword": "uh-oh",
                          "inflectedForms": [
                            { "value": "uh-oh", "labels": ["singular"] },
                            { "value": "uh-oh-obsolete", "labels": ["plural"] }
                          ]
                        }
                      ],
                      "langCode": "en"
                    }
                    """;
            Files.writeString(tempDmlex, json, StandardCharsets.UTF_8);

            Path tempDmlex2 = Files.createTempFile(Paths.get(baseDir), "dmlex-posless-late-", ".json");
            try {
                String json2 = """
                        {
                          "langCode": "en",
                          "entries": [
                            {
                              "id": "dmlex-posless-late",
                              "headword": "uh-oh",
                              "inflectedForms": [
                                { "value": "uh-oh", "labels": ["plural"] }
                              ]
                            }
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

                // "apple" from DMLex is parsed with multi-field objects and deferred "langCode"
                Assertions.assertTrue(entries.get("apple").startsWith("singular nonhuman vowel-start noun inflection="));
                Assertions.assertTrue(entries.get("apples").startsWith("plural nonhuman vowel-start noun inflection="));

                // Because --wikidata came AFTER --dmlex, Wikidata's masculine "ciel" (ciel/ciels/cieux) overrides DMLex's feminine "ciel",
                // and DMLex's orphaned form "ciel-obsolete" is preempted at Tier 1.
                Assertions.assertTrue(entries.get("ciel").contains("masculine"));
                Assertions.assertFalse(entries.get("ciel").contains("feminine"));
                Assertions.assertTrue(entries.containsKey("ciels"));
                Assertions.assertTrue(entries.containsKey("cieux"));
                Assertions.assertFalse(entries.containsKey("ciel-obsolete"));

                // POS-less lemma "uh-oh" from tempDmlex was preempted at Tier 1 by tempDmlex2, removing "uh-oh-obsolete".
                Assertions.assertEquals("plural", entries.get("uh-oh"));
                Assertions.assertFalse(entries.containsKey("uh-oh-obsolete"));
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
        //    does not throw "L1566399: id not found" and overrides the earlier paradigm.
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
            Assertions.assertFalse(entries.containsKey("organisateurs-old"));

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
