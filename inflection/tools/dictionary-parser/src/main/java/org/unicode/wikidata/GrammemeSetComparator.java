/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.Comparator;
import java.util.Set;

public class GrammemeSetComparator implements Comparator<Set<Enum<?>>> {
    @Override
    public int compare(Set<Enum<?>> list1, Set<Enum<?>> list2) {
        var size1 = list1.size();
        var size2 = list2.size();
        if (size1 != size2) {
            return size1 - size2;
        }
        var list2Itr = list2.iterator();
        for (var grammmemeEnum : list1) {
            var cmpResult = Inflection.ENUM_COMPARATOR.compare(grammmemeEnum, list2Itr.next());
            if (cmpResult != 0) {
                return cmpResult;
            }
        }
        return 0;
    }
    static final GrammemeSetComparator ENUM_COMPARATOR = new GrammemeSetComparator();
}
