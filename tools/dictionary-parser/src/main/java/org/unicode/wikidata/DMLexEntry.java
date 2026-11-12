/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

import java.util.List;

/**
 * A lexical entry in OASIS DMLex v1.0 JSON format (spec §5.2, {@code dmlex.schema.json}).
 */
@JsonIgnoreProperties({
    "homographNumber",
    "senses",
    "placeholderMarkers",
    "etymologies"
})
public class DMLexEntry {
    /**
     * Root {@code lexicographicResource} object in OASIS DMLex v1.0 JSON format.
     */
    @JsonIgnoreProperties({
        "title",
        "uri",
        "translationLanguages",
        "definitionTypeTags",
        "labelTypeTags",
        "sourceIdentityTags",
        "transcriptionSchemeTags",
        "relations",
        "relationTypes",
        "etymonLanguages",
        "etymonTypes"
    })
    public static class Lexicon {
        public String langCode;
        public List<DMLexEntry> entries;
        public List<PartOfSpeechTag> partOfSpeechTags;
        public List<InflectedFormTag> inflectedFormTags;
        public List<LabelTag> labelTags;
    }

    /**
     * A {@code partOfSpeechTag} controlled value in OASIS DMLex v1.0 JSON format.
     */
    @JsonIgnoreProperties({
        "description",
        "for"
    })
    public static class PartOfSpeechTag {
        public String tag;
        public List<String> sameAs;
    }

    /**
     * An {@code inflectedFormTag} controlled value in OASIS DMLex v1.0 JSON format.
     */
    @JsonIgnoreProperties({
        "description",
        "for"
    })
    public static class InflectedFormTag {
        public String tag;
        public List<String> sameAs;
    }

    /**
     * A {@code labelTag} controlled value in OASIS DMLex v1.0 JSON format.
     */
    @JsonIgnoreProperties({
        "typeTag",
        "description",
        "for"
    })
    public static class LabelTag {
        public String tag;
        public List<String> sameAs;
    }

    /**
     * A {@code pronunciation} object in OASIS DMLex v1.0 JSON format.
     */
    @JsonIgnoreProperties({
        "soundFile",
        "labels"
    })
    public static class Pronunciation {
        public List<Transcription> transcriptions;
    }

    /**
     * A {@code transcription} object in OASIS DMLex v1.0 JSON format.
     */
    public static class Transcription {
        public String text;
        public String scheme;
    }

    public String id;
    public String headword;
    public List<String> partsOfSpeech;
    public List<String> labels;
    public List<Pronunciation> pronunciations;
    public List<DMLexInflectedForm> inflectedForms;
}
