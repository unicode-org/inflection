/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.exc.MismatchedInputException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Deserializes OASIS DMLex string or tag/label/pronunciation lists, accepting a single string,
 * an array of strings, or an object/array of objects containing {@code "transcription"},
 * {@code "tag"}, {@code "value"}, {@code "label"}, or {@code "text"}.
 */
public class DMLexStringListDeserializer extends JsonDeserializer<List<String>> {
    private static final String[] OBJECT_VALUE_KEYS = {
        "transcription", "tag", "value", "label", "text"
    };

    @Override
    public List<String> deserialize(JsonParser jsonParser, DeserializationContext ctxt) throws IOException {
        JsonNode node = jsonParser.getCodec().readTree(jsonParser);
        List<String> result = new ArrayList<>();
        extractValues(node, result, jsonParser);
        return result;
    }

    private static void extractValues(JsonNode node, List<String> result, JsonParser jsonParser) throws IOException {
        if (node == null || node.isNull()) {
            return;
        }
        if (node.isTextual()) {
            String text = node.textValue();
            if (!text.isBlank()) {
                result.add(text);
            }
        } else if (node.isArray()) {
            for (JsonNode elem : node) {
                extractValues(elem, result, jsonParser);
            }
        } else if (node.isObject()) {
            JsonNode transcriptions = node.get("transcriptions");
            if (transcriptions != null) {
                extractValues(transcriptions, result, jsonParser);
            } else {
                for (String key : OBJECT_VALUE_KEYS) {
                    JsonNode field = node.get(key);
                    if (field != null && field.isTextual() && !field.textValue().isBlank()) {
                        result.add(field.textValue());
                        break;
                    }
                }
            }
        } else {
            throw MismatchedInputException.from(
                    jsonParser,
                    List.class,
                    "Expected string, object, or array for DMLex list field, got " + node.getNodeType());
        }
    }
}
