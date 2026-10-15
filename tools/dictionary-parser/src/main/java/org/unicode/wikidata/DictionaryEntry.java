/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.IdentityHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import javax.annotation.Nonnull;

/**
 * A dictionary entry has a lemma and a set of inflections. The lemma may have additional part of speech information
 * that should be included in the inflections. Rare inflections are sorted last so that they're accessible,
 * but not directly referencable by default, which is important when there may be more than one inflection with the same
 * set of grammemes.
 */
public class DictionaryEntry implements Comparable<DictionaryEntry> {

    final String phrase;
    private final TreeSet<String> lemmas;
    private final TreeSet<Enum<?>> grammemes;
    private final List<InflectionPattern> inflectionalPatterns;
    private final List<InflectionPattern> rareInflectionalPatterns = new ArrayList<>();
    private int sourceIndex;
    private Map<InflectionPattern, Integer> patternSourceIndices = null;

    public static final Comparator<InflectionPattern> INFLECTION_PATTERN_COMPARATOR = Comparator
            .comparing(InflectionPattern::getCount)
            .reversed();

    public DictionaryEntry(String phrase, boolean isLemmaRare, TreeSet<Enum<?>> grammemes, InflectionPattern inflectionalPattern) {
        this(phrase, phrase, isLemmaRare, grammemes, inflectionalPattern, 0);
    }

    public DictionaryEntry(String phrase, String lemmas, boolean isLemmaRare, TreeSet<Enum<?>> grammemes, InflectionPattern inflectionalPattern) {
        this(phrase, lemmas, isLemmaRare, grammemes, inflectionalPattern, 0);
    }

    public DictionaryEntry(String phrase, String lemmas, boolean isLemmaRare, TreeSet<Enum<?>> grammemes, InflectionPattern inflectionalPattern, int sourceIndex) {
        this.phrase = phrase;
        this.lemmas = new TreeSet<>(Collections.singleton(lemmas));
        this.grammemes = new TreeSet<>(grammemes.comparator());
        this.grammemes.addAll(grammemes);
        this.sourceIndex = sourceIndex;
        if (inflectionalPattern != null) {
            this.inflectionalPatterns = new ArrayList<>(2);
            this.inflectionalPatterns.add(inflectionalPattern);
            if (isLemmaRare) {
                rareInflectionalPatterns.add(inflectionalPattern);
            }
        }
        else {
            this.inflectionalPatterns = new ArrayList<>();
        }
    }

    private int getPatternSourceIndex(InflectionPattern pattern) {
        if (patternSourceIndices != null) {
            return patternSourceIndices.getOrDefault(pattern, sourceIndex);
        }
        return sourceIndex;
    }

    public void merge(DictionaryEntry entry) {
        merge(entry, false);
    }

    public void merge(DictionaryEntry entry, boolean mergeConflicts) {
        if (!mergeConflicts) {
            if (entry.sourceIndex > this.sourceIndex) {
                this.sourceIndex = entry.sourceIndex;
                this.grammemes.clear();
                this.grammemes.addAll(entry.grammemes);
                this.inflectionalPatterns.clear();
                this.inflectionalPatterns.addAll(entry.inflectionalPatterns);
                this.rareInflectionalPatterns.clear();
                this.rareInflectionalPatterns.addAll(entry.rareInflectionalPatterns);
                this.lemmas.clear();
                this.lemmas.addAll(entry.lemmas);
                this.patternSourceIndices = null;
                return;
            }
            if (entry.sourceIndex < this.sourceIndex) {
                return;
            }
        } else if (entry.sourceIndex != this.sourceIndex || this.patternSourceIndices != null || entry.patternSourceIndices != null) {
            if (this.patternSourceIndices == null) {
                this.patternSourceIndices = new IdentityHashMap<>();
                for (InflectionPattern pattern : this.inflectionalPatterns) {
                    this.patternSourceIndices.put(pattern, this.sourceIndex);
                }
            }
            for (InflectionPattern pattern : entry.inflectionalPatterns) {
                this.patternSourceIndices.merge(pattern, entry.getPatternSourceIndex(pattern), Math::max);
            }
            this.sourceIndex = Math.max(this.sourceIndex, entry.sourceIndex);
        }

        this.grammemes.addAll(entry.grammemes);
        for (InflectionPattern inflectionalPattern : entry.inflectionalPatterns) {
            boolean isRareInEntry = entry.rareInflectionalPatterns.contains(inflectionalPattern);
            if (!this.inflectionalPatterns.contains(inflectionalPattern)) {
                this.inflectionalPatterns.add(inflectionalPattern);
                if (isRareInEntry) {
                    this.rareInflectionalPatterns.add(inflectionalPattern);
                }
            } else if (!isRareInEntry) {
                this.rareInflectionalPatterns.remove(inflectionalPattern);
            }
        }
        this.lemmas.addAll(entry.lemmas);
    }

    public Set<Enum<?>> getGrammemes() {
        return grammemes;
    }

    List<InflectionPattern> getInflectionalPatterns() {
        return inflectionalPatterns;
    }

    @Override
    public String toString() {
        return this.toString(true);
    }

    public String toString(boolean isInflectional) {
        StringBuilder sb = new StringBuilder(256);
        sb.append(phrase).append(':');
        for (Enum<?> enumVal : grammemes) {
            sb.append(' ').append(enumVal);
        }
        if (isInflectional) {
            Comparator<InflectionPattern> patternComparator = (patternSourceIndices == null)
                    ? INFLECTION_PATTERN_COMPARATOR
                    : Comparator.<InflectionPattern, Integer>comparing(
                            p -> patternSourceIndices.getOrDefault(p, sourceIndex), Comparator.reverseOrder())
                            .thenComparing(INFLECTION_PATTERN_COMPARATOR);
            if (inflectionalPatterns.size() > 1) {
                inflectionalPatterns.sort(patternComparator);
                if (rareInflectionalPatterns.size() > 1) {
                    rareInflectionalPatterns.sort(patternComparator);
                }
            }
            for (InflectionPattern inflectionPattern : inflectionalPatterns) {
                if (!rareInflectionalPatterns.contains(inflectionPattern)) {
                    sb.append(" inflection=").append(inflectionPattern.getID());
                }
            }
            for (InflectionPattern inflectionPattern : rareInflectionalPatterns) {
                sb.append(" inflection=").append(inflectionPattern.getID());
            }
        }
        return sb.toString();
    }

    @Override
    public int compareTo(@Nonnull DictionaryEntry o) {
        return phrase.compareTo(o.phrase);
    }
}