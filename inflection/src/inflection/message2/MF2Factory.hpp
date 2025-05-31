/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/api.h>
// INFLECTION_CLASS_API

#include <unicode/uversion.h>

namespace U_ICU_NAMESPACE::message2 {
class FormatterFactory;
class SelectorFactory;
}  // namespace U_ICU_NAMESPACE::message2

namespace inflection::message2 {

/**
 * The MF2Factory provide factory method to create custom formatter and selector
 * factory to work with icu::message2 library.
 * The intend usage is to creat icu::message2::FormatterFactory
 * or icu::message2::SelectorFactory and use it with
 * icu::message2::MFFunctionRegistry::Builder
 * to construct a custom function registry to build MessageFormatter
 * with MessageFormatter::Builder.
 *
 * For example:
 *
 * auto customRegistry = icu::message2::MFFunctionRegistry::Builder(errorCode)
 *          .adoptFormatter(FunctionName("inflection"),
 *                          MF2Factory::CreateFormatterFactory(), errorCode)
 *          .adoptSelector(FunctionName("inflection"),
 *                          MF2Factory::CreateSelectorFactory(), errorCode)
 *          .build();
 *
 * UParseError pe;
 * auto mf1 = icu::message2::MessageFormatter::Builder(errorCode)
 *              .setFunctionRegistry(customRegistry)
 *              .setLocale(Locale::forLanguageTag("es-MX", errorCode))
 *              .setPattern("Location is {$name :inflection hello=world \
 *                           definiteness=definite number=plural \
 *                           gender=feminine}",
 *                          pe, errorCode)
 *              .build(errorCode);
 *
 * auto mf2 = icu::message2::MessageFormatter::Builder(errorCode)
 *              .setFunctionRegistry(customRegistry)
 *              .setLocale(Locale::forLanguageTag("es-MX", errorCode))
 *              .setPattern(".local $gender = {$name :inflection \
 *                                             feature=gender} \
 *                           .local $number = {$name :inflection \
 *                                             feature=number} \
 *                           .match $gender $number \
 *                           feminine singular {{Feminine Singular {$name}}}\
 *                           masculine singular {{Masculine Singular {$name}}}\
 *                           * * {{other {$name} }}\n",
 *                           pe, errorCode)
 *              .build(errorCode);
 */
class INFLECTION_CLASS_API MF2Factory {
 public:
  /**
   * Create an implementation of icu::message2::FormatterFactory*, based on the
   * infleciton library, which can be passed to
   * icu::messsage2::MFFunctionRegistry::Builder::adoptFormatter
   * to register a custom formatter factory.
   */
  static icu::message2::FormatterFactory* CreateFormatterFactory();

  /**
   * Create an implementation of icu::message2::SelectorFactory*, based on the
   * infleciton library, which can be passed to
   * icu::messsage2::MFFunctionRegistry::Builder::adoptSelector
   * to register a custom selector factory.
   */
  static icu::message2::SelectorFactory* CreateSelectorFactory();
};

}  // namespace inflection::message2
