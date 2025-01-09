/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package com.apple.lexicon;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.lang.invoke.MethodHandles;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Objects;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.xml.sax.SAXException;

class ParseLexiconTest {
    private final static String lexiconSource = Objects
            .requireNonNull(MethodHandles.lookup().lookupClass().getClassLoader().getResource("sourceLexicon.lexicon")).getFile();
    private final static String baseDir = lexiconSource.substring(0, lexiconSource.lastIndexOf("/"));
    private final static String inflectionalFile = baseDir+"/LexiconInflectOut.xml";
    private final static String expectedOutputFile = baseDir+ "/lexiconCorrectOut.txt";
    private final static String fatalErrorFile = baseDir+ "/lexiconFatalError.lexicon";
    private final static String missingGrammemeErrorFile = baseDir+ "/lexiconMissingGrammemeError.lexicon";
    private final static String caseInsensitiveGrammemeFile = baseDir+ "/lexiconcaseInsenstiveGrammeme.lexicon";
    private final static String LINE_END = "==============================================";

    private void compareOutputs(String actual, String expected) {
        String[] actual_lines = actual.split("\n");
        String[] expected_lines = expected.split("\n");
        for(int i=0 ; (i<actual_lines.length && i<expected_lines.length) ; i++) {
            String actual_line = actual_lines[i];
            String expected_line = expected_lines[i];
            if(actual_line.equals(LINE_END)){
                break;
            }
            Assertions.assertEquals(expected_line, actual_line);
        }
    }

    private String getParserOutput(String[] args) throws Exception {
        ByteArrayOutputStream pipeOut = new ByteArrayOutputStream();
        PrintStream old_out = System.out;
        System.setOut(new PrintStream(pipeOut, false, StandardCharsets.UTF_8));
        ParseLexicon.main(args);
        System.setOut(old_out);
        return pipeOut.toString(StandardCharsets.UTF_8);
    }

    @Test
    public void missingInputFile() {
        Assertions.assertThrows(Exception.class,() -> ParseLexicon.main(new String[1]));
    }

    @Test
    public void enumTest() {
        String fieldValue = Grammar.Alternate.SPELLING.toString();
        Assertions.assertEquals(fieldValue, "spelling");
    }

    @Test
    public void fatalErrorHandlerTest() {
        String[] args = {"--inflections", inflectionalFile, fatalErrorFile};
        Assertions.assertThrows(SAXException.class,() -> ParseLexicon.main(args));
    }

    @Test
    public void missingGrammemeTest() {
        String[] args = {"--inflections", inflectionalFile, missingGrammemeErrorFile};
        Assertions.assertThrows(SAXException.class,() -> ParseLexicon.main(args));
    }

    @Test
    public void caseInsensitiveGrammemeTest() {
        String[] args = {"--inflections", inflectionalFile, caseInsensitiveGrammemeFile};
        Assertions.assertDoesNotThrow(() -> ParseLexicon.main(args));
    }

    @Test
    public void lexiconParserTest() throws Exception {
        String[] args = {"--ignore-entries-with-affixes", "--inflection-types", "noun,adjective", "--ignore-grammemes-for-types", "verb", "--inflections",
                inflectionalFile, lexiconSource};
        String actual = getParserOutput(args);
        String expected = Files.readString(Paths.get(expectedOutputFile), StandardCharsets.UTF_8);
        compareOutputs(actual, expected);
    }
}
