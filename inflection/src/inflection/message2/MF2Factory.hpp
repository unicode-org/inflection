/*
 * Copyright 2025 Google Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>
// INFLECTION_CLASS_API

#include <unicode/uversion.h>

namespace U_ICU_NAMESPACE::message2 {
class FormatterFactory;
class SelectorFactory;
}

namespace inflection::message2 {

INFLECTION_CLASS_API icu::message2::FormatterFactory* createFormatterFactory();
INFLECTION_CLASS_API icu::message2::SelectorFactory* createSelectorFactory();

}
