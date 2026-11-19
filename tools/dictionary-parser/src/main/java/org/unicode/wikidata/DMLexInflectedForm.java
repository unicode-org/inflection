/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.List;

/**
 * An {@code inflectedForm} of a lexical entry in OASIS DMLex v1.0 JSON format (spec §5.2, {@code dmlex.schema.json}).
 */
public class DMLexInflectedForm {
    public String tag;
    public String text;
    public List<String> labels;
    public List<DMLexEntry.Pronunciation> pronunciations;
}
