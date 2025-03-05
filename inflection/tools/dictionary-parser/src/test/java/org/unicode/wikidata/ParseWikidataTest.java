/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.lang.invoke.MethodHandles;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Objects;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.xml.sax.SAXException;

class ParseWikidataTest {
    private static final String lexiconSource = Objects
            .requireNonNull(MethodHandles.lookup().lookupClass().getClassLoader().getResource("sourceLexicon.json")).getFile();
    private static final String baseDir = lexiconSource.substring(0, lexiconSource.lastIndexOf("/"));
    private static final String inflectionalFile = baseDir+"/LexiconInflectOut.xml";
    private static final String dictionaryFile = baseDir+"/LexiconInflectOut.lst";
    private static final String expectedOutputFile = baseDir+ "/lexiconCorrectOut.txt";
    private static final String fatalErrorFile = baseDir+ "/lexiconFatalError.json";
    private static final String missingGrammemeErrorFile = baseDir+ "/lexiconMissingGrammemeError.json";
    private static final String caseInsensitiveGrammemeFile = baseDir+ "/lexiconcaseInsenstiveGrammeme.json";
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

    @Test
    public void missingInputFile() {
        Assertions.assertThrows(Exception.class,() -> ParseWikidata.main(new String[1]));
    }

    @Test
    public void enumTest() {
        String fieldValue = Grammar.Alternate.SPELLING.toString();
        Assertions.assertEquals("spelling", fieldValue);
    }

    @Test
    public void fatalErrorHandlerTest() {
        String[] args = {"--inflections", inflectionalFile, "--dictionary", dictionaryFile, fatalErrorFile};
        Assertions.assertThrows(SAXException.class,() -> ParseWikidata.main(args));
    }

    @Test
    public void missingGrammemeTest() {
        String[] args = {"--inflections", inflectionalFile, "--dictionary", dictionaryFile, missingGrammemeErrorFile};
        Assertions.assertThrows(SAXException.class,() -> ParseWikidata.main(args));
    }

    @Test
    public void caseInsensitiveGrammemeTest() {
        String[] args = {"--inflections", inflectionalFile, "--dictionary", dictionaryFile, caseInsensitiveGrammemeFile};
        Assertions.assertDoesNotThrow(() -> ParseWikidata.main(args));
    }

    @Test
    public void lexiconParserTest() throws Exception {
        String[] args = {"--inflection-types", "noun,adjective,proper-noun", "--inflections", inflectionalFile,
                "--dictionary", dictionaryFile, lexiconSource};
        String actual = getParserOutput(args);
        String expected = Files.readString(Paths.get(expectedOutputFile), StandardCharsets.UTF_8);
        compareOutputs(actual, expected);
    }
}
