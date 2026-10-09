/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonToken;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.exc.MismatchedInputException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Deserializes OASIS DMLex string or tag/label/pronunciation lists, accepting:
 * <ul>
 *   <li>a single string (e.g. {@code "noun"})</li>
 *   <li>an array of strings (e.g. {@code ["singular", "nominative"]})</li>
 *   <li>an object or array of objects containing {@code "tag"}, {@code "value"},
 *       {@code "transcription"}, {@code "text"}, or {@code "label"}</li>
 * </ul>
 */
public class DMLexStringListDeserializer extends JsonDeserializer<List<String>> {
    @Override
    public List<String> deserialize(JsonParser jsonParser, DeserializationContext ctxt) throws IOException {
        List<String> result = new ArrayList<>();
        JsonToken currentToken = jsonParser.currentToken();
        if (currentToken == JsonToken.VALUE_STRING) {
            String text = jsonParser.getText();
            if (text != null && !text.isBlank()) {
                result.add(text);
            }
            return result;
        }
        if (currentToken == JsonToken.START_OBJECT) {
            readValuesFromObject(jsonParser, result);
            return result;
        }
        if (currentToken != JsonToken.START_ARRAY) {
            throw MismatchedInputException.from(
                    jsonParser,
                    List.class,
                    "Expected string, object, or array for DMLex list field, got " + currentToken);
        }

        while (jsonParser.nextToken() != JsonToken.END_ARRAY) {
            JsonToken elemToken = jsonParser.currentToken();
            if (elemToken == JsonToken.VALUE_STRING) {
                String text = jsonParser.getText();
                if (text != null && !text.isBlank()) {
                    result.add(text);
                }
            } else if (elemToken == JsonToken.START_OBJECT) {
                readValuesFromObject(jsonParser, result);
            } else {
                throw MismatchedInputException.from(
                        jsonParser,
                        String.class,
                        "Expected string or object element in DMLex array, got " + elemToken);
            }
        }
        return result;
    }

    /**
     * Extracts value(s) from a single JSON object. When an object carries multiple scalar string
     * fields (for example {@code {"tag": "singular", "label": "Singular number"}} or
     * {@code {"label": "US", "transcription": "ˈæp.əl"}}), only the highest-priority non-blank
     * scalar field ({@code "transcription"} &rarr; {@code "tag"} &rarr; {@code "value"} &rarr;
     * {@code "label"} &rarr; {@code "text"}) is selected, while also supporting nested
     * {@code "transcriptions"} arrays/objects.
     */
    private static void readValuesFromObject(JsonParser jsonParser, List<String> result) throws IOException {
        String transcription = null;
        String tag = null;
        String value = null;
        String label = null;
        String text = null;
        List<String> nestedTranscriptions = null;

        while (jsonParser.nextToken() != JsonToken.END_OBJECT) {
            String fieldName = jsonParser.currentName();
            JsonToken valueToken = jsonParser.nextToken();
            if (valueToken == JsonToken.VALUE_STRING) {
                String str = jsonParser.getText();
                if (str != null && !str.isBlank()) {
                    switch (fieldName) {
                        case "transcription" -> {
                            if (transcription == null) {
                                transcription = str;
                            }
                        }
                        case "tag" -> {
                            if (tag == null) {
                                tag = str;
                            }
                        }
                        case "value" -> {
                            if (value == null) {
                                value = str;
                            }
                        }
                        case "label" -> {
                            if (label == null) {
                                label = str;
                            }
                        }
                        case "text" -> {
                            if (text == null) {
                                text = str;
                            }
                        }
                        default -> {}
                    }
                }
            } else if ("transcriptions".equals(fieldName) || "transcription".equals(fieldName)) {
                if (nestedTranscriptions == null) {
                    nestedTranscriptions = new ArrayList<>();
                }
                if (valueToken == JsonToken.START_OBJECT) {
                    readValuesFromObject(jsonParser, nestedTranscriptions);
                } else if (valueToken == JsonToken.START_ARRAY) {
                    while (jsonParser.nextToken() != JsonToken.END_ARRAY) {
                        JsonToken elemToken = jsonParser.currentToken();
                        if (elemToken == JsonToken.VALUE_STRING) {
                            String str = jsonParser.getText();
                            if (str != null && !str.isBlank()) {
                                nestedTranscriptions.add(str);
                            }
                        } else if (elemToken == JsonToken.START_OBJECT) {
                            readValuesFromObject(jsonParser, nestedTranscriptions);
                        } else if (elemToken == JsonToken.START_ARRAY) {
                            jsonParser.skipChildren();
                        }
                    }
                }
            } else if (valueToken == JsonToken.START_OBJECT || valueToken == JsonToken.START_ARRAY) {
                jsonParser.skipChildren();
            }
        }

        if (transcription != null) {
            result.add(transcription);
            if (nestedTranscriptions != null) {
                result.addAll(nestedTranscriptions);
            }
        } else if (nestedTranscriptions != null && !nestedTranscriptions.isEmpty()) {
            result.addAll(nestedTranscriptions);
        } else if (tag != null) {
            result.add(tag);
        } else if (value != null) {
            result.add(value);
        } else if (label != null) {
            result.add(label);
        } else if (text != null) {
            result.add(text);
        }
    }
}
