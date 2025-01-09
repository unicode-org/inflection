/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package com.apple.wiktionary;


import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.nio.charset.StandardCharsets;
import java.util.LinkedList;
import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.ibm.icu.lang.UScript;
import com.ibm.icu.text.NumberFormat;
import com.ibm.icu.text.UnicodeSet;
import com.ibm.icu.util.ULocale;
import org.xml.sax.Attributes;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

/**
 * @see <a href="http://en.wiktionary.org/wiki/Wiktionary:Entry_layout_explained">Wiktionary:Entry_layout_explained</a>
 */
public class ParseWiktionary extends DefaultHandler {
    private static final String REDIRECT_STRING = "#redirect";
    private static final String PROPERTY_KEY_SEPARATOR = ".";
    private static final String CASE_SENSITIVE_SUBPROPERTY = "caseSensitive";
    private static final String MATCHING_GROUP_SUBPROPERTY = "group";
    private static final String MATCHING_TARGET_SUBPROPERTY = "target";
    private static final String MATCHING_MARKER_SUBPROPERTY = "nameMarker";
    private static final String RELEVANT_SUBPROPERTY = "relevant";
    private static final String LANGUAGE_HEADER_KEY = "config.language.header";
    private static final String LANGUAGE_HEADER_KEY_GROUP = "config.language.header.group";
    private static final String LANGUAGE_HEADER_KEY_NAME = "config.language.header.name";
    private static final String LANGUAGE_HEADER_KEY_EXISTS = "config.language.header.exists";
    private static final String LANGUAGE_REDIRECT_KEY = "config.language.redirect";
    private static final String LANGUAGE_BANNED_KEY = "config.language.banned";
    private static final String ENTRY_BANNED_KEY = "config.language.entry.banned";

    private static final String NAME_MARKER_REPLACEMENT = "_";

    private ParseState state = ParseState.FIND_LOCALE;
    private final String filename;
    private final StringBuilder currText = new StringBuilder(131070);
    private String title = null;
    private static final Pattern COMMENT_STRIPPER = Pattern.compile("<!--[^>]*-->");
    private static final Pattern MARKUP_STRIPPER = Pattern.compile("([\\[\\]…¡!\"]+|''+)");

    private int articles = 0;
    private int terms = 0;
    private int unusableTerms = 0;
    private int redirects = 0;
    private int specialPages = 0;
    private int wrongScript = 0;

    private final TreeSet<CountableMatcher> partsOfSpeech = new TreeSet<>();
    private final LinkedList<String> unknownTerms = new LinkedList<>();
    private final LinkedList<String> untaggedTerms;

    private UnicodeSet languageSet;
    private ULocale locale;
    // When specified, we're extracting data from a language that is not native to the defined Wiktionary data.
    private final String requestedLocale;
    private Pattern localeName;
    private Pattern localeNameHeader;
    private Pattern headerSearchPattern;
    private Pattern redirectPattern;
    private Pattern bannedPattern;
    // Ignore templates, declension tables, phrases, suffixes and prefixes.
    private static final Pattern bannedPatternDefault = Pattern.compile("[:()/]|^-|-$");
    private Pattern bannedEntryPattern;
    private int headerSearchGroup;

    public enum ParseState {FIND_LOCALE, START, TITLE, FIND_TEXT, TEXT}

    ParseWiktionary(String filename, String requestedLocale, boolean listUntaggedTerms) {
        this.filename = filename;
        this.requestedLocale = requestedLocale;
        if (listUntaggedTerms) {
            untaggedTerms = new LinkedList<>();
        }
        else {
            untaggedTerms = null;
        }
    }

    private void initializeParsing(String localeString) throws SAXException {
        locale = ULocale.forLanguageTag(requestedLocale == null ? localeString : requestedLocale);

        partsOfSpeech.clear();

        // These work best when irrelevant languages are stripped from the articles.
        ResourceBundle rb = ResourceBundle.getBundle("com.apple.wiktionary.partsOfSpeech" + (requestedLocale == null ? "" : "-" + localeString),
                locale.toLocale(), new ModernizeLanguageCodeResourceBundleControl());
        if (!rb.getLocale().equals(locale.toLocale())) {
            throw new SAXException("No partsOfSpeech configuration for locale " + localeString);
        }
        for (String key : rb.keySet()) {
            if (!key.contains(PROPERTY_KEY_SEPARATOR)) {
                String regex = rb.getString(key);
                boolean caseSensitive = false;
                try {
                    caseSensitive = Boolean.parseBoolean(rb.getString(key + PROPERTY_KEY_SEPARATOR + CASE_SENSITIVE_SUBPROPERTY));
                }
                catch (MissingResourceException e) {
                    // Case sensitive by default.
                }
                boolean relevant = true;
                try {
                    relevant = Boolean.parseBoolean(rb.getString(key + PROPERTY_KEY_SEPARATOR + RELEVANT_SUBPROPERTY));
                }
                catch (MissingResourceException e) {
                    // Relevant by default.
                }
                int matchingGroup = -1;
                try {
                    matchingGroup = Integer.parseInt(rb.getString(key + PROPERTY_KEY_SEPARATOR + MATCHING_GROUP_SUBPROPERTY));
                }
                catch (MissingResourceException e) {
                    // Relevant by default.
                }
                String targetRegex = null;
                try {
                    targetRegex = rb.getString(key + PROPERTY_KEY_SEPARATOR + MATCHING_TARGET_SUBPROPERTY);
                }
                catch (MissingResourceException e) {
                    // Relevant by default.
                }
                String nameMarker = null;
                try {
                    nameMarker = rb.getString(key + PROPERTY_KEY_SEPARATOR + MATCHING_MARKER_SUBPROPERTY);
                }
                catch (MissingResourceException e) {
                    // Relevant by default.
                }
                if (caseSensitive) {
                    partsOfSpeech.add(new CountableMatcher(key, regex, matchingGroup, targetRegex, nameMarker, relevant));
                }
                else {
                    partsOfSpeech.add(new CountableMatcher(key, regex, matchingGroup, targetRegex, nameMarker, relevant, Pattern.CASE_INSENSITIVE));
                }
            }
        }

        headerSearchPattern = Pattern.compile(rb.getString(LANGUAGE_HEADER_KEY));
        headerSearchGroup = Integer.parseInt(rb.getString(LANGUAGE_HEADER_KEY_GROUP));
        localeName = Pattern.compile(rb.getString(LANGUAGE_HEADER_KEY_NAME));
        localeNameHeader = Pattern.compile(rb.getString(LANGUAGE_HEADER_KEY_EXISTS));
        try {
            bannedPattern = Pattern.compile(rb.getString(LANGUAGE_BANNED_KEY));
        }
        catch (MissingResourceException e) {
            // use the default.
            bannedPattern = bannedPatternDefault;
        }
        try {
            bannedEntryPattern = Pattern.compile(rb.getString(ENTRY_BANNED_KEY));
        }
        catch (MissingResourceException e) {
            // use the default.
        }
        try {
            redirectPattern = Pattern.compile(rb.getString(LANGUAGE_REDIRECT_KEY), Pattern.CASE_INSENSITIVE);
        }
        catch (MissingResourceException e) {
            // Use the default behavior
        }

        languageSet = new UnicodeSet();
        for (int script : UScript.getCode(locale)) {
            languageSet.addAll(new UnicodeSet("[:script=" + UScript.getName(script) + ":]"));
        }
        languageSet = languageSet.freeze();
    }

    @Override
    public void startDocument() {
    }

    @Override
    public void startElement(String namespaceURI, String localName, String qName, Attributes attribs)
        throws SAXException
    {
        if (state == ParseState.START && "title".equals(localName)) {
            state = ParseState.TITLE;
        }
        else if (state == ParseState.FIND_TEXT && "text".equals(localName)) {
            state = ParseState.TEXT;
        }
        else if (state == ParseState.FIND_LOCALE && "mediawiki".equals(localName)) {
            initializeParsing(attribs.getValue("xml:lang"));
            state = ParseState.START;
        }
    }

    @Override
    public void endElement(String namespaceURI, String localName, String qName) {
        if (state == ParseState.TITLE) {
            title = currText.toString();
            currText.setLength(0);
            articles++;
            if (bannedPattern.matcher(title).find() || !languageSet.containsSome(title)) {
                // A template, a help page or an easy to find foreign word
                if (title.indexOf(':') >= 0) {
                    specialPages++;
                }
                else {
                    wrongScript++;
                }
                unusableTerms++;
                resetState();
                return;
            }
            state = ParseState.FIND_TEXT;
        }
        else if (state == ParseState.TEXT) {
            String text = currText.toString();
            if (!text.isEmpty() && (redirectPattern != null
                    ? redirectPattern.matcher(text).find()
                    : (text.charAt(0) == REDIRECT_STRING.charAt(0) && text.toLowerCase(Locale.ENGLISH).startsWith(REDIRECT_STRING))))
            {
                // This might be interesting to investigate further in the future, but skip it for now.
                redirects++;
                resetState();
                return;
            }
            text = stripIrrelevantContent(text);
            if (text == null || (bannedEntryPattern != null && bannedEntryPattern.matcher(text).find())) {
                // This probably isn't an English article. Don't waste time on it.
                unusableTerms++;
                resetState();
                return;
            }
            currText.setLength(0);
            currText.append(title);
            currText.append(':');
            boolean understood = false;
            boolean redundant = false;
//            boolean relevant = false;
            for (CountableMatcher entry : partsOfSpeech) {
                Matcher entryMatcher = entry.matcher(text);
                int matchingGroup = entry.getMatchingGroup();
                if (entryMatcher.find() && (matchingGroup <= 0 || !entryMatcher.group(matchingGroup).isEmpty())) {
                    entry.incCount();
                    understood = true;
                    String lastMatch = null;
                    do {
                        if (entry.getTargetMatch() != null) {
                            // Replacement
                            String rawMatch = entry.matcher(entryMatcher.group()).replaceAll(entry.getTargetMatch());
                            String newMatch = rawMatch.trim().replace(' ', '\u2002');
                            if (lastMatch == null || !lastMatch.equals(newMatch)) {
                                lastMatch = newMatch;
                                currText.append(' ');
                                currText.append(entry.getName());
                                currText.append('=');
                                currText.append(newMatch);
                            }
                        }
                        else if (matchingGroup < 0) {
                            // binary property
                            currText.append(' ');
                            currText.append(entry.getName());
                        }
                        else {
                            int group = matchingGroup;
                            int groupCount = matchingGroup;
                            if (matchingGroup == 0) {
                                // We don't allow matching the entire match. Iterate and get all the submatches.
                                group = 1;
                                groupCount = entryMatcher.groupCount();
                            }
                            for (; group <= groupCount; group++) {
                                String rawMatch = entryMatcher.group(group);
                                if (rawMatch != null) {
                                    String newMatch = MARKUP_STRIPPER.matcher(rawMatch.trim().replace(' ', '\u2002')).replaceAll("");
                                    if (lastMatch == null || !lastMatch.equals(newMatch)) {
                                        lastMatch = newMatch;
                                        currText.append(' ');
                                        currText.append(entry.getName());
                                        currText.append('=');
                                        if (entry.getNameMarker() != null) {
                                            currText.append(newMatch.replaceFirst(entry.getNameMarker(), NAME_MARKER_REPLACEMENT));
                                        } else {
                                            currText.append(newMatch);
                                        }
                                    }
                                    // else a redundant entry.
                                }
                            }
                        }
                    }
                    while (matchingGroup >= 0 && entryMatcher.find());
                }
            }
            if (!understood) {
                if (localeNameHeader.matcher(text).find()) {
                    // We have a language header? We're probably not parsing the part of speech correctly.
                    unknownTerms.add(title);
                }
                else {
                    // No language header and no grammar notation? This probably isn't a native word we care about.
                    if (untaggedTerms != null) {
                        untaggedTerms.add(title);
                    }
                    unusableTerms++;
                }
            }
            else if (!redundant || currText.charAt(currText.length() - 1) != ':') {
                terms++;
                System.out.println(currText);
            }
            resetState();
        }
    }

    private void resetState() {
        title = null;
        state = ParseState.START;
        currText.setLength(0);
    }

    private String stripIrrelevantContent(String article) {
        article = COMMENT_STRIPPER.matcher(article).replaceAll("");
        Matcher headerMatcher = headerSearchPattern.matcher(article);
        boolean headerFound = false;
        while (headerMatcher.find()) {
            headerFound = true;
            String headertitle = headerMatcher.group(headerSearchGroup);
            if (localeName.matcher(headertitle.trim()).find()) {
                int start = headerMatcher.start();
                while (headerMatcher.find()) {
                    headertitle = headerMatcher.group(headerSearchGroup);
                    if (!localeName.matcher(headertitle.trim()).find()) {
                        int end = headerMatcher.start();
                        return article.substring(start, end + 1);
                    }
                    // 2 groups by the same name? We better include the second definition too.
                }
                return article.substring(start);
            }
        }

        // If we found a header, but none of them are native to the language,
        // it's probably not an English article.
        return headerFound || requestedLocale != null ? null : article;
    }

    @Override
    public void characters(char[] ch, int start, int length) {
        if (state == ParseState.TITLE || state == ParseState.TEXT) {
            currText.append(ch, start, length);
        }
    }

    @Override
    public void endDocument() {
        if (untaggedTerms != null) {
            System.out.println("=========================================");
            System.out.println("Untagged terms");
            System.out.println("=========================================");
            for (String term : untaggedTerms) {
                System.out.println(term);
            }
        }
        System.out.println("=========================================");
        System.out.println("Unclassified terms");
        System.out.println("=========================================");
        for (String term : unknownTerms) {
            System.out.println(term);
        }
        System.out.println("=========================================");
        NumberFormat percentFormat = NumberFormat.getPercentInstance();
        percentFormat.setMaximumFractionDigits(1);

        System.out.println(String.format("%30s %7s", "Source:", new File(filename).getName()));
        System.out.println(String.format("%30s %7d", "Articles:", articles));
        System.out.println(String.format("%30s %7d %7s", "Non-term articles:", specialPages, '(' + percentFormat.format(specialPages/(double)articles) + ')'));
        System.out.println(String.format("%30s %7d %7s", locale.getDisplayName() + " terms:", terms, '(' + percentFormat.format(terms/(double)articles) + ')'));
        System.out.println(String.format("%30s %7d %7s", "Aliases:", redirects, '(' + percentFormat.format(redirects/(double)articles) + ')'));
        // Tagged with a language, but nothing was parsable beyond that.
        System.out.println(String.format("%30s %7d %7s", "Unclassified terms:", unknownTerms.size(), '(' + percentFormat.format(unknownTerms.size()/(double)articles) + ')'));
        if (untaggedTerms != null) {
            // No language header, and no parts of speech
            System.out.println(String.format("%30s %7d %7s", "Untagged terms:", untaggedTerms.size(), '(' + percentFormat.format(untaggedTerms.size()/(double)articles) + ')'));
        }
        // Wrong script words, templates, user pages help pages
        System.out.println(String.format("%30s %7d %7s", "Unusable terms:", unusableTerms, '(' + percentFormat.format(unusableTerms/(double)articles) + ')'));
        System.out.println(String.format("%30s %7d %7s", "Wrong script terms:", wrongScript, '(' + percentFormat.format(wrongScript/(double)articles) + ')'));
        System.out.println();
        System.out.println(String.format("%30s %7s %7s", "Term Type:", "", locale.getDisplayName()));
        System.out.println("=========================================");
        for (CountableMatcher entry : partsOfSpeech) {
            int count = entry.getCount();
            System.out.println(String.format("%30s %7d %7s", capitalizeFirst(entry.getName()) + " terms:", count, '(' + percentFormat.format(count/(double)terms) + ')'));
        }
    }

    private static String capitalizeFirst(String string) {
        int firstCodePoint = string.codePointAt(0);
        if(!Character.isUpperCase(firstCodePoint)) {
            return new String(Character.toChars(firstCodePoint)).toUpperCase(Locale.ENGLISH) + string.substring(1);
        }
        return string;
    }


    private static class MyErrorHandler implements ErrorHandler {
        private final PrintStream out;

        MyErrorHandler(PrintStream out) {
            this.out = out;
        }

        private String getParseExceptionInfo(SAXParseException spe) {
            String systemId = spe.getSystemId();

            if (systemId == null) {
                systemId = "null";
            }

            return "URI=" + systemId + " Line="
                + spe.getLineNumber() + ": " + spe.getMessage();
        }

        @Override
        public void warning(SAXParseException spe) {
            out.println("Warning: " + getParseExceptionInfo(spe));
        }

        @Override
        public void error(SAXParseException spe) throws SAXException {
            String message = "Error: " + getParseExceptionInfo(spe);
            throw new SAXException(message);
        }

        @Override
        public void fatalError(SAXParseException spe) throws SAXException {
            String message = "Fatal Error: " + getParseExceptionInfo(spe);
            throw new SAXException(message);
        }
    }

    private static void usage() {
        System.err.println("Usage: ParseWiktionary [--force <language>] <file.xml>");
        System.exit(1);
    }

    static public void main(String[] args) throws Exception {
        String filename = null;
        String requestedLocale = null;
        for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if ("--force".equals(arg)) {
                // We are extracting foreign data because the native data is insufficient.
                requestedLocale = args[++i];
            }
            else {
                filename = arg;
                if (i != args.length - 1) {
                    usage();
                }
            }
        }

        if (filename == null) {
            usage();
            return;
        }

        long startTime = System.currentTimeMillis();
        SAXParserFactory spf = SAXParserFactory.newInstance();
        spf.setNamespaceAware(true);
        SAXParser saxParser = spf.newSAXParser();
        XMLReader xmlReader = saxParser.getXMLReader();
        xmlReader.setContentHandler(new ParseWiktionary(filename, requestedLocale, false));
        xmlReader.setErrorHandler(new MyErrorHandler(System.err));
        // We create InputSource directly due to a occasional bugs with UTF-8 files being interpreted as malformed UTF-8.
        // We use a large buffer because we're reading a large file, and we're frequently reading file data.
        InputSource inputSource = new InputSource(new BufferedReader(new InputStreamReader(
                new FileInputStream(filename), StandardCharsets.UTF_8), 1048576));
        xmlReader.parse(inputSource);
        long endTime = System.currentTimeMillis();
        long elapsedTime = (endTime-startTime);
        System.out.println("processed in " + (elapsedTime / 1000) + '.' + (elapsedTime % 1000) + " seconds");
        System.out.println("Wiktionary ( https://web.opensource.apple.com/OSS/browseRequest.php?requestKey=4515&requestType=Internal)");
    }
}
