/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;

import java.util.List;
import java.util.Map;

/**
 * A Form from a Lexeme in Wikidata.
 */
public class LexemeForm {
    public String id;
    public Map<String, LexemeRepresentation> representations;
    public List<String> grammaticalFeatures;
    @JsonDeserialize(using = ClaimsJsonDeserializer.class)
    public Map<String, List<String>> claims;
}
