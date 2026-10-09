/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.EnumSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import javax.annotation.Nonnull;

/**
 * A dictionary entry represents a single surface form ({@code phrase}) in the generated dictionary,
 * along with its combined grammatical properties and references to inflection patterns.
 *
 * <p>To support multi-source inputs with fine-grained conflict resolution, a {@code DictionaryEntry}
 * partitions its grammemes and inflection patterns into per-{@link Grammar.PartOfSpeech} slices
 * ({@link #posSlices}) rather than storing a single flat set. This allows a higher-priority source
 * file (higher {@code sourceIndex}) to override a word's reading for one part of speech (e.g., {@code noun})
 * without clobbering readings for other parts of speech (e.g., {@code verb} or {@code adjective})
 * defined in earlier source files.
 *
 * <p>Rare inflections are sorted last when serialized so that they remain accessible,
 * but are not directly referenceable by default when multiple inflections share the same
 * set of grammemes.
 */
public class DictionaryEntry implements Comparable<DictionaryEntry> {

    /**
     * Holds the grammatical features and inflection patterns contributed for a single
     * {@link Grammar.PartOfSpeech} (or for a POS-less entry) from a specific {@link #sourceIndex}.
     */
    private static final class PosSlice {
        final int sourceIndex;
        final TreeSet<Enum<?>> grammemes;
        final List<InflectionPattern> inflectionalPatterns = new ArrayList<>(2);
        final List<InflectionPattern> rareInflectionalPatterns = new ArrayList<>();

        PosSlice(int sourceIndex, Comparator<? super Enum<?>> comparator) {
            this.sourceIndex = sourceIndex;
            this.grammemes = new TreeSet<>(comparator);
        }

        PosSlice copy() {
            PosSlice copy = new PosSlice(this.sourceIndex, this.grammemes.comparator());
            copy.grammemes.addAll(this.grammemes);
            copy.inflectionalPatterns.addAll(this.inflectionalPatterns);
            copy.rareInflectionalPatterns.addAll(this.rareInflectionalPatterns);
            return copy;
        }
    }

    final String phrase;
    private final Comparator<? super Enum<?>> comparator;
    // LinkedHashMap preserves PartOfSpeech insertion order so tie-breaking between
    // equal-frequency InflectionPatterns remains deterministic.
    private final Map<Grammar.PartOfSpeech, PosSlice> posSlices = new LinkedHashMap<>();
    // Holds grammemes and patterns for entries that have no PartOfSpeech tag.
    private PosSlice poslessSlice = null;
    // Holds POS-less supplemental grammemes injected via --add-extra-grammemes,
    // which are always unioned regardless of source precedence.
    private final TreeSet<Enum<?>> extraGrammemes;

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
        this.comparator = Inflection.ENUM_COMPARATOR;
        this.extraGrammemes = new TreeSet<>(this.comparator);

        // Separate PartOfSpeech values from all other grammatical categories (gender, number, case, etc.)
        // so that each PosSlice only stores its own PartOfSpeech plus the shared non-POS grammemes.
        // If a multi-POS entry (e.g. noun + proper-noun) later has only one POS overridden by a
        // higher-priority source, the remaining POS slice will not retain a stale POS tag.
        EnumSet<Grammar.PartOfSpeech> posSet = EnumSet.noneOf(Grammar.PartOfSpeech.class);
        TreeSet<Enum<?>> nonPosGrammemes = new TreeSet<>(this.comparator);
        for (Enum<?> g : grammemes) {
            if (g instanceof Grammar.PartOfSpeech pos) {
                posSet.add(pos);
            } else {
                nonPosGrammemes.add(g);
            }
        }

        if (posSet.isEmpty()) {
            this.poslessSlice = new PosSlice(sourceIndex, this.comparator);
            this.poslessSlice.grammemes.addAll(nonPosGrammemes);
            if (inflectionalPattern != null) {
                this.poslessSlice.inflectionalPatterns.add(inflectionalPattern);
                if (isLemmaRare) {
                    this.poslessSlice.rareInflectionalPatterns.add(inflectionalPattern);
                }
            }
        } else {
            for (Grammar.PartOfSpeech pos : posSet) {
                PosSlice slice = new PosSlice(sourceIndex, this.comparator);
                slice.grammemes.addAll(nonPosGrammemes);
                slice.grammemes.add(pos);
                if (inflectionalPattern != null) {
                    slice.inflectionalPatterns.add(inflectionalPattern);
                    if (isLemmaRare) {
                        slice.rareInflectionalPatterns.add(inflectionalPattern);
                    }
                }
                this.posSlices.put(pos, slice);
            }
        }
    }

    /**
     * Unions the grammemes and deduplicates inflection patterns from {@code incoming} into {@code target}.
     * Used when two entries for the same surface form and {@link Grammar.PartOfSpeech} originate from
     * the same source file (or when {@code --merge-conflicts=true} is enabled).
     */
    private static void mergeSlice(PosSlice target, PosSlice incoming) {
        target.grammemes.addAll(incoming.grammemes);
        for (InflectionPattern inflectionalPattern : incoming.inflectionalPatterns) {
            if (!target.inflectionalPatterns.contains(inflectionalPattern)) {
                target.inflectionalPatterns.add(inflectionalPattern);
            }
        }
        for (InflectionPattern inflectionalPattern : incoming.rareInflectionalPatterns) {
            if (!target.rareInflectionalPatterns.contains(inflectionalPattern)) {
                target.rareInflectionalPatterns.add(inflectionalPattern);
            }
        }
    }

    /**
     * Merges another {@code DictionaryEntry} for the same surface form into this entry.
     *
     * <p>Conflict resolution is performed independently for each {@link Grammar.PartOfSpeech} slice:
     * <ol>
     *   <li><b>New POS or higher-priority source ({@code incomingSlice.sourceIndex > existingSlice.sourceIndex})</b>:
     *       Replaces the existing slice for that POS with the incoming slice, leaving other POS slices intact.</li>
     *   <li><b>Same-priority source ({@code incomingSlice.sourceIndex == existingSlice.sourceIndex},
     *       or when {@code --merge-conflicts=true} normalizes all indices to {@code 0})</b>:
     *       Unions grammemes and inflection patterns via {@link #mergeSlice(PosSlice, PosSlice)}, preserving
     *       intra-file homograph and syncretism merging.</li>
     *   <li><b>Lower-priority source ({@code incomingSlice.sourceIndex < existingSlice.sourceIndex})</b>:
     *       Ignored for that POS.</li>
     * </ol>
     */
    public void merge(DictionaryEntry entry) {
        for (Map.Entry<Grammar.PartOfSpeech, PosSlice> incomingEntry : entry.posSlices.entrySet()) {
            Grammar.PartOfSpeech pos = incomingEntry.getKey();
            PosSlice incomingSlice = incomingEntry.getValue();
            PosSlice existingSlice = this.posSlices.get(pos);
            if (existingSlice == null || incomingSlice.sourceIndex > existingSlice.sourceIndex) {
                this.posSlices.put(pos, incomingSlice.copy());
            } else if (incomingSlice.sourceIndex == existingSlice.sourceIndex) {
                mergeSlice(existingSlice, incomingSlice);
            }
        }
        if (entry.poslessSlice != null) {
            if (this.poslessSlice == null || entry.poslessSlice.sourceIndex > this.poslessSlice.sourceIndex) {
                this.poslessSlice = entry.poslessSlice.copy();
            } else if (entry.poslessSlice.sourceIndex == this.poslessSlice.sourceIndex) {
                mergeSlice(this.poslessSlice, entry.poslessSlice);
            }
        }
        this.extraGrammemes.addAll(entry.extraGrammemes);
    }

    public void addExtraGrammemes(Set<Enum<?>> grammemes) {
        this.extraGrammemes.addAll(grammemes);
    }

    /**
     * Recombines grammemes across all active {@link PosSlice}s, {@link #poslessSlice}, and
     * {@link #extraGrammemes} into a single sorted set ordered by {@link Inflection#ENUM_COMPARATOR}.
     */
    public Set<Enum<?>> getGrammemes() {
        TreeSet<Enum<?>> combined = new TreeSet<>(this.comparator);
        for (PosSlice slice : posSlices.values()) {
            combined.addAll(slice.grammemes);
        }
        if (poslessSlice != null) {
            combined.addAll(poslessSlice.grammemes);
        }
        combined.addAll(extraGrammemes);
        return combined;
    }

    @Override
    public String toString() {
        return this.toString(true);
    }

    /**
     * Formats this entry as a single line for the output {@code dictionary_*.lst} file:
     * {@code <phrase>: <sorted-grammemes> [inflection=<pattern-id> ...]}.
     *
     * <p>When {@code isInflectional} is {@code true}, inflection patterns from all active slices
     * are deduplicated in slice insertion order, sorted by descending usage frequency
     * ({@link #INFLECTION_PATTERN_COMPARATOR}), and emitted with non-rare patterns preceding rare patterns.
     */
    public String toString(boolean isInflectional) {
        StringBuilder sb = new StringBuilder(256);
        sb.append(phrase).append(':');
        for (Enum<?> enumVal : getGrammemes()) {
            sb.append(' ').append(enumVal);
        }
        if (isInflectional) {
            List<InflectionPattern> inflectionalPatterns = new ArrayList<>();
            List<InflectionPattern> rareInflectionalPatterns = new ArrayList<>();
            for (PosSlice slice : posSlices.values()) {
                for (InflectionPattern pattern : slice.inflectionalPatterns) {
                    if (!inflectionalPatterns.contains(pattern)) {
                        inflectionalPatterns.add(pattern);
                    }
                }
                for (InflectionPattern pattern : slice.rareInflectionalPatterns) {
                    if (!rareInflectionalPatterns.contains(pattern)) {
                        rareInflectionalPatterns.add(pattern);
                    }
                }
            }
            if (poslessSlice != null) {
                for (InflectionPattern pattern : poslessSlice.inflectionalPatterns) {
                    if (!inflectionalPatterns.contains(pattern)) {
                        inflectionalPatterns.add(pattern);
                    }
                }
                for (InflectionPattern pattern : poslessSlice.rareInflectionalPatterns) {
                    if (!rareInflectionalPatterns.contains(pattern)) {
                        rareInflectionalPatterns.add(pattern);
                    }
                }
            }

            if (inflectionalPatterns.size() > 1) {
                inflectionalPatterns.sort(INFLECTION_PATTERN_COMPARATOR);
                if (rareInflectionalPatterns.size() > 1) {
                    rareInflectionalPatterns.sort(INFLECTION_PATTERN_COMPARATOR);
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