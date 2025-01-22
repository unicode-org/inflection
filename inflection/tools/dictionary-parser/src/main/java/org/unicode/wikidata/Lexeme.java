/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import java.util.List;
import java.util.Map;

/**
 * A Lexeme from Wikidata.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
public class Lexeme {
    public String type;
    public String id;
    public Map<String, LexemeRepresentation> lemmas;
    public String lexicalCategory;
    public List<LexemeForm> forms;
}
