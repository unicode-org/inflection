/*
 * Copyright 2014-2024 Apple Inc. All rights reserved.
 */
package com.apple.wiktionary;

import java.util.Locale;
import java.util.ResourceBundle;

/**
 * @author George Rhoten
 * @since 2014-02-10
 */
public class ModernizeLanguageCodeResourceBundleControl extends ResourceBundle.Control {
    @Override
    public String toBundleName(String baseName, Locale locale) {
        if (locale == Locale.ROOT) {
            return baseName;
        }

        String language = locale.getLanguage();
        //String script = locale.getScript();
        String country = locale.getCountry();
        String variant = locale.getVariant();

        if (language.isEmpty() && country.isEmpty() && variant.isEmpty()) {
            return baseName;
        }

        if ("iw".equals(language)) {
            language = "he";
        }
        else if ("in".equals(language)) {
            language = "id";
        }
        else if ("ji".equals(language)) {
            language = "yi";
        }

        StringBuilder sb = new StringBuilder(baseName);
        sb.append('_');
//        if (script != "") {
//            if (variant != "") {
//                sb.append(language).append('_').append(script).append('_').append(country).append('_').append(variant);
//            } else if (country != "") {
//                sb.append(language).append('_').append(script).append('_').append(country);
//            } else {
//                sb.append(language).append('_').append(script);
//            }
//        } else {
        if (!variant.isEmpty()) {
            sb.append(language).append('_').append(country).append('_').append(variant);
        } else if (!country.isEmpty()) {
            sb.append(language).append('_').append(country);
        } else {
            sb.append(language);
        }
//        }
        return sb.toString();

    }}
