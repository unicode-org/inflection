/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/message2/fwd.hpp>

#include <unicode/uversion.h>
#include <unicode/messageformat2_function_registry.h>

/**
 * @brief The MF2Factory provides factory methods to create custom functions
 * to work with the <code>icu::message2</code> library.
 * The intended usage is to create an <code>icu::message2::Function</code>
 * and use it with <code>icu::message2::MFFunctionRegistry::Builder</code>
 * to construct a custom function registry to build a MessageFormatter
 * with <code>MessageFormatter::Builder</code>.
 *
 * <p>For example:
 * <blockquote>
 * <pre>
 * auto customRegistry = icu::message2::MFFunctionRegistry::Builder(errorCode)
 *          .adoptFunction(FunctionName("i:inflect"),
 *                         MF2Factory::createInflectFunction(), errorCode)
 *          .build();
 *
 * UParseError pe;
 * auto mf1 = icu::message2::MessageFormatter::Builder(errorCode)
 *              .setFunctionRegistry(customRegistry)
 *              .setLocale(Locale::forLanguageTag("es-MX", errorCode))
 *              .setPattern("Location is {$name :i:inflect hello=world \
 *                           definiteness=definite number=plural \
 *                           gender=feminine}",
 *                          pe, errorCode)
 *              .build(errorCode);
 *
 * auto mf2 = icu::message2::MessageFormatter::Builder(errorCode)
 *              .setFunctionRegistry(customRegistry)
 *              .setLocale(Locale::forLanguageTag("es-MX", errorCode))
 *              .setPattern(".local $gender = {$name :i:inflect \
 *                                             feature=gender} \
 *                           .local $number = {$name :i:inflect \
 *                                             feature=number} \
 *                           .match $gender $number \
 *                           feminine singular {{Feminine Singular {$name}}}\
 *                           masculine singular {{Masculine Singular {$name}}}\
 *                           * * {{other {$name} }}\n",
 *                           pe, errorCode)
 *              .build(errorCode);
 * </pre>
 * </blockquote>
 * </p>
 */
class INFLECTION_CLASS_API inflection::message2::MF2Factory {
public:
    /**
     * Controls how formatters serialize their result:
     * PLAIN emits the print form only; SSML emits Speech Synthesis
     * Markup Language (SSML) markup — a &lt;sub alias="..."&gt; element
     * when print differs from speak.
     * @see https://www.w3.org/TR/speech-synthesis/
     */
    enum class OutputMode { PLAIN, SSML };

    /**
     * Create an implementation of icu::message2::Function*, based on the
     * inflection library, which can be passed to
     * icu::message2::MFFunctionRegistry::Builder::adoptFunction
     * to register a custom function.
     */
    static icu::message2::Function* createInflectFunction(OutputMode mode = OutputMode::PLAIN);

    /**
     * Create an implementation of icu::message2::Function* for the
     * :i:numeral function (number formatting), based on the inflection library,
     * which can be passed to
     * icu::message2::MFFunctionRegistry::Builder::adoptFunction
     * to register a custom function.
     */
    static icu::message2::Function* createNumeralFunction(OutputMode mode = OutputMode::PLAIN);

    /** Create a Function for the :i:quantify function (count + unit agreement). */
    static icu::message2::Function* createQuantifyFunction(OutputMode mode = OutputMode::PLAIN);

    /** Create a Function for the :i:list function (array → plain/and/or list). */
    static icu::message2::Function* createListFunction(OutputMode mode = OutputMode::PLAIN);

    /** Create a Function for the :i:pronoun function. */
    static icu::message2::Function* createPronounFunction(OutputMode mode = OutputMode::PLAIN);
};
