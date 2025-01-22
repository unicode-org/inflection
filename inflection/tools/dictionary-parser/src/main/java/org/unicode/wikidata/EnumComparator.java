/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.Comparator;

public final class EnumComparator implements Comparator<Enum<?>> {
    public static final EnumComparator ENUM_COMPARATOR = new EnumComparator();
    @Override
    public int compare(Enum<?> o1, Enum<?> o2) {
        return o1.toString().compareTo(o2.toString());
    }
}
