/*
 * Copyright 2014-2024 Apple Inc. All rights reserved.
 */
package com.apple.lexicon;

import javax.annotation.Nonnull;
import java.util.Comparator;
import java.util.Iterator;
import java.util.Collection;
import java.util.TreeSet;
import static com.apple.lexicon.Grammar.PartOfSpeech;
import static com.apple.lexicon.Grammar.LemmaCategory;
import static com.apple.lexicon.Grammar.Miscellaneous;


/**
 * @author George Rhoten 2014/11/20
 */
public final class Inflection implements Comparable<Inflection> {
    static boolean excludedGrammemeType(Class<?> o1) {
        return o1.equals(PartOfSpeech.class) || o1.equals(Miscellaneous.class) || o1.equals(LemmaCategory.class);
    }
    public static class EnumClassComparator implements Comparator<Class<?>> {
        static int compareClasses(Class<?> o1, Class<?> o2) {
            int cmpResult = Boolean.compare(excludedGrammemeType(o1), excludedGrammemeType(o2));
            if (cmpResult != 0) {
                return cmpResult;
            }
            String o1Class = o1.toString();
            String o2Class = o2.toString();
            cmpResult = Boolean.compare(!o1Class.endsWith("Count"), !o2Class.endsWith("Count"));
            if (cmpResult != 0) {
                return cmpResult;
            }
            return o1Class.compareTo(o2Class);
        }

        @Override
        public int compare(Class<?> o1, Class<?> o2) {
            return compareClasses(o1, o2);
        }
    }
    public static final EnumClassComparator ENUM_CLASS_COMPARATOR = new EnumClassComparator();

    public static class EnumComparator implements Comparator<Enum<?>> {
        @Override
        public int compare(Enum<?> o1, Enum<?> o2) {
            int cmpResult = EnumClassComparator.compareClasses(o1.getClass(), o2.getClass());
            if (cmpResult != 0) {
                return cmpResult;
            }
            return Integer.compare(o1.ordinal(), o2.ordinal());
        }
    }
    static final EnumComparator ENUM_COMPARATOR = new EnumComparator();

    final String inflection;
    final TreeSet<Enum<?>> grammemeSet = new TreeSet<>(Inflection.ENUM_COMPARATOR);
    public final boolean rareUsage;

    public Inflection(String inflection, boolean rareUsage) {
        this.inflection = inflection;
        this.rareUsage = rareUsage;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(inflection);
        for (Enum<?> grammeme : grammemeSet) {
            sb.append(' ').append(grammeme.toString());
        }
        return sb.toString();
    }

    public String getInflection() {
        return inflection;
    }

    public TreeSet<Enum<?>> getGrammemeSet() {
        return grammemeSet;
    }

    public boolean isRareUsage() {
        return rareUsage;
    }

    public void addGrammemes(Collection<Enum<?>> grammemes) {
        grammemeSet.addAll(grammemes);
    }

    public int compareToGrammemeSet(@Nonnull TreeSet<Enum<?>> otherGrammemeSet) {
        int cmpResult = otherGrammemeSet.size() - grammemeSet.size();
        if (cmpResult != 0) {
            return cmpResult;
        }
        // They are the same size
        Iterator<Enum<?>> thisGrammemeSetItr = grammemeSet.iterator();
        Iterator<Enum<?>> otherGrammemeSetItr = otherGrammemeSet.iterator();
        while (thisGrammemeSetItr.hasNext()) {
            Enum<?> grammeme = thisGrammemeSetItr.next();
            Enum<?> otherGrammeme = otherGrammemeSetItr.next();
            cmpResult = Inflection.ENUM_COMPARATOR.compare(grammeme, otherGrammeme);
            if (cmpResult != 0) {
                return cmpResult;
            }
        }
        return 0;
    }

    @Override
    public int compareTo(@Nonnull Inflection other) {
        int cmpResult = compareToGrammemeSet(other.grammemeSet);
        if (cmpResult != 0) {
            return cmpResult;
        }
        return inflection.compareTo(other.inflection);
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof Inflection)) {
            return false;
        }
        Inflection other = (Inflection)o;
        return this == o || (grammemeSet.equals(other.grammemeSet) && inflection.equals(other.inflection));
    }

    @Override
    public int hashCode() {
        int result = inflection != null ? inflection.hashCode() : 0;
        result = 31 * result + grammemeSet.hashCode();
        return result;
    }
}
