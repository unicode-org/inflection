/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.annotation.JsonAlias;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;

import java.util.ArrayList;
import java.util.List;

/**
 * An inflected form of a lexical entry in OASIS DMLex v1.0 JSON format.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
public class DMLexInflectedForm {
    public String id;

    @JsonAlias({"text", "inflection"})
    public String value;

    public List<String> labels = new ArrayList<>();

    @JsonDeserialize(using = DMLexStringListDeserializer.class)
    public List<String> pronunciations;

    @JsonSetter("labels")
    @JsonAlias({"grammaticalFeatures", "features", "tag", "tags"})
    @JsonDeserialize(using = DMLexStringListDeserializer.class)
    public void addLabels(List<String> incoming) {
        if (incoming != null) {
            this.labels.addAll(incoming);
        }
    }
}
