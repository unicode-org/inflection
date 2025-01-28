/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;

import java.util.List;
import java.util.Map;

/**
 * A Lexeme from Wikidata.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
public class Lexeme {
    public String id;
    @JsonDeserialize(using = LexemesJsonDeserializer.class)
    public Map<String, LexemeRepresentation> lemmas;
    public String lexicalCategory;
    @JsonDeserialize(using = ClaimsJsonDeserializer.class)
    public Map<String, List<String>> claims;
    public List<LexemeForm> forms;
}
