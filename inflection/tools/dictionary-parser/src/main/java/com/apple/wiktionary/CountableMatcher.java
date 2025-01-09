/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
package com.apple.wiktionary;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

final class CountableMatcher implements Comparable<CountableMatcher> {
    private final String name;
    private final Pattern pattern;
    private final String targetMatch;
    private final String nameMarker;
    private final boolean relevant;
    //ArrayList inflectors;
    private final int matchingGroup;
    private int count = 0;

    CountableMatcher(String name, String patternString, int matchingGroup, String targetMatch, String nameMarker, boolean relevant) {
        this.name = name;
        this.pattern = Pattern.compile(patternString);
        this.matchingGroup = matchingGroup;
        this.targetMatch = targetMatch;
        this.nameMarker = nameMarker;
        this.relevant = relevant;
    }

    CountableMatcher(String name, String patternString, int matchingGroup, String targetMatch, String nameMarker, boolean relevant, int flags) {
        this.name = name;
        this.pattern = Pattern.compile(patternString, flags);
        this.matchingGroup = matchingGroup;
        this.targetMatch = targetMatch;
        this.nameMarker = nameMarker;
        this.relevant = relevant;
    }

    public Matcher matcher(String str) {
        return pattern.matcher(str);
    }

    public int getMatchingGroup() {
        return matchingGroup;
    }

    public void incCount() {
        count++;
    }

    public int getCount() {
        return count;
    }

    public boolean isRelevant() {
        return relevant;
    }

    public String getTargetMatch() {
        return targetMatch;
    }

    public String getNameMarker() {
        return nameMarker;
    }

    public String getName() {
        return name;
    }

/*    public void setInflectors(ArrayList inflectors) {
        this.inflectors = (ArrayList)inflectors.clone();
    }

    public ArrayList getInflectors() {
        return inflectors;
    }*/

    @Override
    public int compareTo(CountableMatcher o) {
        return name.compareTo(o.name);
    }
}
