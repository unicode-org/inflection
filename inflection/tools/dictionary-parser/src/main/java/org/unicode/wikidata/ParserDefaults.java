/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.Comparator;

/**
 * Default parser option values.
 */
final class ParserDefaults {
    static final String DEFAULT_INFLECTION_FILE_NAME = "inflectional.xml";
    static final String DEFAULT_DICTIONARY_FILE_NAME = "dictionary.lst";
    // Put the rare inflections at the end.
    static final Comparator<Inflection> RARITY_AWARE_COMPARATOR = Comparator
            .comparing(Inflection::isRareUsage)
            .thenComparing(Inflection::compareTo);

    private ParserDefaults() {
    }
}
