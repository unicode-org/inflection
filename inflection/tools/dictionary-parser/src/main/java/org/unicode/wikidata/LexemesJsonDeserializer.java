/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonToken;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;

import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Converts the map of language identifiers to lexeme representations from Wikidata in the JSON structure to usable Java classes.
 */
public class LexemesJsonDeserializer extends JsonDeserializer<Map<String, LexemeRepresentation>> {
    private static String[] languages;

    public static void setLanguage(List<String> languagesArray) {
        if (languagesArray != null && !languagesArray.isEmpty()) {
            languages = languagesArray.toArray(new String[0]);
        }
        else {
            languages = null;
        }
    }

    /**
     * Is the base desired language contained in the variant language?
     * en, en true
     * en, en-us true
     * en-us, en-us true
     * ko, kok false
     */
    public static boolean isContained(String currentLanguage) {
        if (languages == null) {
            return true;
        }
        var dashIdx = currentLanguage.indexOf('-');
        if (dashIdx < 0) {
            dashIdx = currentLanguage.length();
        }
        for (var language : languages) {
            if (language.regionMatches(0, currentLanguage, 0, dashIdx)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public Map<String, LexemeRepresentation> deserialize(JsonParser jsonParser, DeserializationContext deserializationContext) throws IOException {
        Map<String, LexemeRepresentation> result = new TreeMap<>();
        var objectCodec = jsonParser.getCodec();

        while (jsonParser.nextToken() != JsonToken.END_OBJECT) {
            String languageKey = jsonParser.currentName();
            if (jsonParser.nextToken() != JsonToken.START_OBJECT) {
                throw new IOException("Start of LexemeRepresentation not found for " + languageKey);
            }
            // Only decode for known languages.
            if (isContained(languageKey)) {
                LexemeRepresentation lexemeRepresentation = objectCodec.readValue(jsonParser, LexemeRepresentation.class);
                if (result.put(languageKey, lexemeRepresentation) != null) {
                    throw new IOException("Duplicate language " + languageKey);
                }
            }
            else {
                jsonParser.skipChildren();
            }
        }

        return result;
    }
}
