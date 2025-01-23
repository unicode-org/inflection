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
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Converts the claims in LexemeForm from Wikidata in the JSON structure to usable Java classes.
 */
public class ClaimsJsonDeserializer extends JsonDeserializer<Map<String, List<String>>> {
    @Override
    public Map<String, List<String>> deserialize(JsonParser jsonParser, DeserializationContext deserializationContext) throws IOException {
        Map<String, List<String>> result = null;
        JsonNode node = jsonParser.getCodec().readTree(jsonParser);
        var nodeItr = node.fields();
        while (nodeItr.hasNext()) {
            var claimsItr = nodeItr.next();
            if (!ParseWikidata.PROPERTIES_WITH_PRONUNCIATION.contains(claimsItr.getKey())) {
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
                            array.add(value.textValue());
                        }
                    }
                }
                if (array != null) {
                    if (result == null) {
                        result = new TreeMap<>();
                    }
                    result.put(claimsItr.getKey(), array);
                }
            }
        }
        return result;
    }
}
