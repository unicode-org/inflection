/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonNode;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Converts the claims in LexemeForm from Wikidata in the JSON structure to usable Java classes.
 */
public class ClaimsJsonDeserializer extends JsonDeserializer<Map<String, List<String>>> {

    /**
     * If there are no lemmas due to a language mismatch, there is no point in deserializing.
     */
    private static boolean isIgnorable(JsonParser jsonParser) {
        var parent = jsonParser.getParsingContext().getParent();
        while (parent != null) {
            if (parent.getCurrentValue() instanceof Lexeme lexeme) {
                return lexeme.lemmas.isEmpty();
            }
            parent = parent.getParent();
        }
        return false;
    }

    @Override
    public Map<String, List<String>> deserialize(JsonParser jsonParser, DeserializationContext deserializationContext) throws IOException {
        Map<String, List<String>> result = null;
        if (isIgnorable(jsonParser)) {
            // If there are no lemmas matching the current language, then don't bother extracting this data.
            jsonParser.skipChildren();
            return Collections.emptyMap();
        }

        JsonNode node = jsonParser.getCodec().readTree(jsonParser);
        var nodeItr = node.fields();
        while (nodeItr.hasNext()) {
            var claimsItr = nodeItr.next();
            var claimKey = claimsItr.getKey();
            if (!ParseWikidata.IMPORTANT_PROPERTIES.contains(claimKey)) {
                continue;
            }
            var claimItr = claimsItr.getValue().iterator();
            List<String> array = null;
            while (claimItr.hasNext()) {
                var claimNode = claimItr.next();
                JsonNode mainsnak = claimNode.get("mainsnak");
                if (mainsnak != null) {
                    JsonNode datavalue = mainsnak.get("datavalue");
                    if (datavalue != null) {
                        var value = datavalue.get("value");
                        if (value != null) {
                            if (array == null) {
                                array = new ArrayList<>();
                            }
                            if (value.isTextual()) {
                                array.add(value.textValue());
                            }
                            else {
                                var id = value.get("id");
                                if (id != null) {
                                    array.add(id.textValue());
                                }
                            }
                        }
                    }
                }
                if (array != null && !array.isEmpty()) {
                    if (result == null) {
                        result = new TreeMap<>();
                    }
                    result.put(claimKey, array);
                }
            }
        }
        return result;
    }
}
