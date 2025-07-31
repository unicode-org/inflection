/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2014-2024 Apple Inc. All rights reserved.
 */
package org.unicode.wikidata;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeSet;

/**
 * An inflection pattern is a non-unique pattern to represent an ordered set of inflections.
 * For example, take the singular form of "city" and the plural form of "cities".
 * The lemma suffix is y (singular), and making the plural form of cities changes that y to an ies suffix.
 * @author George Rhoten 2014/11/18
 */
public class InflectionPattern {
    private String id;
    private int count = 1; // We have at least ourselves.
    List<String> lemmaSuffixes = new ArrayList<>(1);
    List<TreeSet<Enum<?>>> grammemeList = new ArrayList<>(1);
    List<Inflection> inflectionSuffixes;
    /** Are there multiple non-unique surface forms for the set of grammatical states? */
    boolean inflectional;

    public InflectionPattern(int id, String lemmaSuffix, TreeSet<Enum<?>> grammemeSet, List<Inflection> inflectionSuffixes) {
        this(Integer.toHexString(id), lemmaSuffix, grammemeSet, inflectionSuffixes);
    }

    InflectionPattern(String id, String lemmaSuffix, TreeSet<Enum<?>> grammemeSet, List<Inflection> inflectionSuffixes) {
        this.id = id;
        this.grammemeList.add(grammemeSet);
        this.lemmaSuffixes.add(lemmaSuffix);
        this.inflectionSuffixes = inflectionSuffixes;
        inflectional = isInflectional();
    }

    public void merge(InflectionPattern inflectionPattern) {
        for (String suffix : inflectionPattern.lemmaSuffixes) {
            if (!lemmaSuffixes.contains(suffix)) {
                lemmaSuffixes.add(suffix);
            }
        }
        mergeGrammemes(inflectionPattern.grammemeList);
        inflectional = isInflectional();
        if (inflectional) {
            List<Inflection> newSuffixes = generateMergedInflections(inflectionPattern.inflectionSuffixes);
            if (newSuffixes == null) {
                throw new IllegalStateException("Disjoint inflection suffixes were attempted to be merged");
            }
            inflectionSuffixes = newSuffixes;
        }
        ++count;
    }

    public int getCount() {
        return count;
    }

    public String getID() {
        return id;
    }

    public void setID(int id) {
        this.id = Integer.toHexString(id);
    }

    private static boolean isUsefulGrammeme(Enum<?> grammeme) {
        return !(grammeme instanceof Grammar.Alternate);
    }

    private void mergeGrammemes(List<TreeSet<Enum<?>>> newGrammemeList) {
        // Do a deep copy
        for (TreeSet<Enum<?>> grammemeSet : newGrammemeList) {
            if (!grammemeList.contains(grammemeSet)) {
                TreeSet<Enum<?>> newSet = new TreeSet<>(grammemeSet.comparator());
                for (Enum<?> enumVal : grammemeSet) {
                    if (isUsefulGrammeme(enumVal)) {
                        newSet.add(enumVal);
                    }
                }
                grammemeList.add(newSet);
            }
        }
    }

    /**
     * We're doing this so that we can use this object as a key until we know we want to keep it around.
     */
    public void saveInternalReferences() {
        lemmaSuffixes = new ArrayList<>(lemmaSuffixes);
        List<TreeSet<Enum<?>>> oldGrammemeList = grammemeList;
        grammemeList = new ArrayList<>(this.grammemeList.size() + 1);
        mergeGrammemes(oldGrammemeList);
        inflectionSuffixes = new ArrayList<>(inflectionSuffixes);
    }


    private static Map<Class<?>, String> createEnumToAttribute()
    {
        Map<Class<?>, String> result = new HashMap<>();
        result.put(Grammar.Gender.class, "gender");
        result.put(Grammar.Number.class, "number");
        result.put(Grammar.Person.class, "person");
        result.put(Grammar.PartOfSpeech.class, ""); // pos
        result.put(Grammar.Tense.class, "tense");
        result.put(Grammar.Mood.class, "mood");
        result.put(Grammar.Voice.class, "voice");
        result.put(Grammar.VerbType.class, "verb-type");
        result.put(Grammar.Aspect.class, "aspect");
        result.put(Grammar.Case.class, "case");
        result.put(Grammar.Animacy.class, "animacy");
        result.put(Grammar.Definiteness.class, "definiteness");
        result.put(Grammar.Count.class, "count");
        result.put(Grammar.Sound.class, ""); // sound, like vowel-start or consonant-start
        result.put(Grammar.DerivationType.class, "derivation-type");
        result.put(Grammar.AdjectiveType.class, "adjective-type");
        result.put(Grammar.PronounType.class, "pronoun-type");
        result.put(Grammar.DeclensionClass.class, "declension");
        result.put(Grammar.Register.class, "register");
        result.put(Grammar.FormType.class, "form-type");
        result.put(Grammar.Polarity.class, "polarity");
        result.put(Grammar.ComparisonDegree.class, "comparison-degree");
        result.put(Grammar.Transitivity.class, "transitivity");
        result.put(Grammar.Sizeness.class, "sizeness");
        result.put(Grammar.Usage.class, "");
        result.put(Grammar.WordOrder.class, "word-order");
        return result;
    }

    private static final Map<Class<?>, String> ENUM_TO_ATTRIBUTE = createEnumToAttribute();

    private static void toAttribute(StringBuilder sb, Enum<?> grammeme) {
        var attribute = ENUM_TO_ATTRIBUTE.get(grammeme.getClass());
        if (attribute == null) {
            fail(grammeme + " (" + grammeme.getClass().getName() + ") is unknown");
        }
        else if (!attribute.isEmpty()) {
            sb.append(" ").append(attribute).append("=\"").append(grammeme).append('"');
        }
        // else recognize, but omit the attribute
    }

    private boolean isInflectional() {

        boolean hasNonIgnorableInflections = false;

        // The German dict is producing patterns where all suffixes have the same grammemes, because the lr-dict has word forms with non-unique metadata,
        // but if we do not know why the forms differ, we really cannot use the pattern...
        boolean hasDistinctGrammemes = false;

        // For patterns having single inflection, if they have a non-empty printable grammeme set then it is inflectional
        if (inflectionSuffixes.size() == 1) {
            int numPrintableGrammemes = 0;
            var grammemeSet = inflectionSuffixes.get(0).getGrammemeSet();
            for (var grammeme : grammemeSet) {
                Class<?> declaringClass = grammeme.getDeclaringClass();
                if (!Inflection.excludedGrammemeType(declaringClass)) {
                    numPrintableGrammemes += 1;
                }
            }
            if (numPrintableGrammemes > 0) {
                hasDistinctGrammemes = true;
            }
        }

        Inflection firstInflectionSuffix = inflectionSuffixes.isEmpty() ? null : inflectionSuffixes.get(0);
        for (Inflection inflectionSuffix : inflectionSuffixes) {
            hasNonIgnorableInflections = hasNonIgnorableInflections || !isIgnorableGrammemeSet(inflectionSuffix);
            hasDistinctGrammemes = hasDistinctGrammemes || !inflectionSuffix.getGrammemeSet().equals(firstInflectionSuffix.getGrammemeSet());

            if (hasNonIgnorableInflections && hasDistinctGrammemes) {
                return true;
            }
        }

        return false;
    }

    /**
     * This allows us to split up inflection lines. This is only needed when the grammatical property has more than one valid value for a suffix.
     * TODO Revisit this choice in case space needs to be saved. It could become comma separated, but then the reading code would have to add both grammemes.
     */
    private void toAttributes(List<StringBuilder> attributes, Map<Class<?>, List<Enum<?>>> grammemeMap) {
        for (var entry : grammemeMap.entrySet()) {
            var list = entry.getValue();

            // duplicate the current attributes multiplied by the number of conflicting attributes.
            int currentAttributeSize = attributes.size();
            for (int duplicateCount = list.size() - 1; duplicateCount > 0; duplicateCount--) {
                for (int idx = 0; idx < currentAttributeSize; idx++) {
                    attributes.add(new StringBuilder(attributes.get(idx)));
                }
            }
            // fill up the string builder objects
            int attributeOffset = 0;
            for (var grammemeEnum : list) {
                for (int sbOffset = 0; sbOffset < currentAttributeSize; sbOffset++) {
                    toAttribute(attributes.get((attributeOffset * currentAttributeSize) + sbOffset), grammemeEnum);
                }
                attributeOffset++;
            }
        }
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(2000);
        sb.append("    <pattern name=\"").append(id).append("\" words=\"").append(count).append("\">\n");

        // We build up the POS data this way because sometimes certain combinations create redundancies.
        StringBuilder posBuilder = new StringBuilder(1000);
        TreeSet<String> posSet = new TreeSet<>();
        for (TreeSet<Enum<?>> grammemeSet : grammemeList) {
            ArrayList<Enum<?>> posOnly = new ArrayList<>(grammemeSet.size());
            ArrayList<Enum<?>> grammemeOnly = new ArrayList<>(grammemeSet.size());
            for (Enum<?> grammeme : grammemeSet) {
                if (grammeme instanceof Grammar.PartOfSpeech) {
                    posOnly.add(grammeme);
                } else {
                    grammemeOnly.add(grammeme);
                }
            }
            // The grammemes apply to all pos
            for (Enum<?> pos : posOnly) {
                posBuilder.append("        <pos>");
                if (!grammemeOnly.isEmpty()) {
                    posBuilder.append("<!--");
                    for (Enum<?> grammeme : grammemeOnly) {
                        if (!Inflection.excludedGrammemeType(grammeme.getClass()) && isUsefulGrammeme(grammeme)) {
                            toAttribute(posBuilder, grammeme);
                        }
                    }
                    posBuilder.append(" -->");
                }
                posBuilder.append(pos).append("</pos>\n");
                posSet.add(posBuilder.toString());
                posBuilder.setLength(0);
            }
        }
        for (String pos : posSet) {
            sb.append(pos);
        }

        if (inflectional) {
            for (String suffix : lemmaSuffixes) {
                if (suffix.isEmpty()) {
                    sb.append("        <suffix/>\n");
                }
                else {
                    sb.append("        <suffix>").append(suffix).append("</suffix>\n");
                }
            }
            sb.append("        <inflections>\n");
            for (Inflection inflection : inflectionSuffixes) {
                if (isIgnorableGrammemeSet(inflection)) {
                    // We don't care about this inflection.
                    continue;
                }

                Map<Class<?>, List<Enum<?>>> grammemeMap = new HashMap<>();
                for (Enum<?> grammeme : inflection.getGrammemeSet()) {
                    Class<?> declaringClass = grammeme.getDeclaringClass();
                    if (Inflection.excludedGrammemeType(declaringClass) || !isUsefulGrammeme(grammeme)) {
                        continue;
                    }
                    grammemeMap.computeIfAbsent(declaringClass, k -> new ArrayList<>()).add(grammeme);
                }
                List<StringBuilder> attributeList = new ArrayList<>();
                attributeList.add(new StringBuilder());
                toAttributes(attributeList, grammemeMap);

                for (var attributesStr : attributeList) {
                    sb.append("            <inflection").append(attributesStr).append("><t><stem/>").append(inflection.getInflection().replace("& ", "&amp; ")).append("</t></inflection>\n");
                }
            }
            sb.append("        </inflections>\n");
        }
        sb.append("    </pattern>\n");
        return sb.toString();
    }

    private boolean isIgnorableGrammemeSet(Inflection inflection) {
        return isIgnorableGrammemeSet(inflection.getGrammemeSet());
    }

    public static boolean isIgnorableGrammemeSet(TreeSet<Enum<?>> grammemeSet) {
        return grammemeSet.contains(Grammar.PartOfSpeech.ABBREVIATION);
    }

    @Nullable
    private static Inflection findGrammemeEquivalentInflection(@Nonnull List<Inflection> inflections, @Nonnull Inflection similarInflection) {
        TreeSet<Enum<?>> grammemeSet = similarInflection.getGrammemeSet();
        for (Inflection inflection : inflections) {
            if (grammemeSet.equals(inflection.getGrammemeSet()) && !similarInflection.equals(inflection)) {
                return inflection;
            }
        }
        return null;
    }

    @Nullable
    private List<Inflection> generateMergedInflections(@Nonnull List<Inflection> otherInflections) {
        if (inflectionSuffixes.size() != otherInflections.size() || !inflectionSuffixes.containsAll(otherInflections)) {
            return null;
        }
        if (inflectionSuffixes.equals(otherInflections)) {
            return inflectionSuffixes;
        }
        // They are the same size, and we have containment of the inflections, but they aren't equal.
        // Perhaps they're in a different order.
        // Ensure that given all the possible grammemes of the provided inflections that there is at least 1 that is not rare.
        ArrayList<Inflection> result = new ArrayList<>(inflectionSuffixes.size());
        if (!inflectionSuffixes.isEmpty()) {
            ArrayList<Inflection> rare = new ArrayList<>(inflectionSuffixes.size());
            ArrayList<Inflection> nonRare = new ArrayList<>(inflectionSuffixes.size());
            for (Inflection inflection : inflectionSuffixes) {
                if (inflection.isRareUsage()) {
                    rare.add(inflection);
                }
                else {
                    nonRare.add(inflection);
                }
            }
            for (Inflection otherInflection : otherInflections) {
                if (otherInflection.isRareUsage() != rare.contains(otherInflection)) {
                    // Uh oh. We have a disjoint situation with regards to the inflection.
                    if (findGrammemeEquivalentInflection(nonRare, otherInflection) == null) {
                        // Oops! Nothing is better! These are disjoint sets.
                        return null;
                    }
                    if (otherInflection.isRareUsage()) {
                        // We're making this one rare when there is another one with the same constraints that isn't rare.
                        // This is the sometimes rare scenario. We prefer the always non-rare scenario over this one.
                        nonRare.remove(otherInflection);
                        rare.add(0, otherInflection);
                    }
                    // Else this one is already rare, and the current one is fine.
                }
                // Else nothing needs to change
            }
            result.addAll(nonRare);
            result.addAll(rare);
        }
        return result;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof InflectionPattern)) {
            return false;
        }
        InflectionPattern other = (InflectionPattern)o;
        // non-inflectional ones are just a bag of inflections without annotation. So we collapse them to the same pattern.
        // If they're both inflectional, then verify that the suffixes can be safely merged and that the lemmas are the same.
        return this == o || (!inflectional && !other.inflectional && grammemeList.equals(other.grammemeList))
            || (inflectional == other.inflectional
                && generateMergedInflections(other.inflectionSuffixes) != null
                && lemmaSuffixes.size() == other.lemmaSuffixes.size()
                && lemmaSuffixes.equals(other.lemmaSuffixes)
                && grammemeList.equals(other.grammemeList));
    }

    @Override
    public int hashCode() {
        return inflectionSuffixes.hashCode();
    }

    private static void fail(String msg) {
        System.out.println(msg);
        System.exit(1);
    }
}
