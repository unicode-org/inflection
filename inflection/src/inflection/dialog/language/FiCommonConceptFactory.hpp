/*
 * Copyright 2019-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>
#include <inflection/dictionary/fwd.hpp>

class inflection::dialog::language::FiCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    int64_t singularProperty {  };
    int64_t pluralProperty {  };

public: /* package */
    ::inflection::dialog::SpeakableString quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const override;

private:
    bool isPluraleTantumPattern(const ::inflection::dictionary::Inflector_InflectionPattern& pattern) const;
    bool isPluraleTantum(std::u16string_view word) const;
    bool shouldUsePlural(const ::inflection::dialog::SemanticFeatureConceptBase& semanticConceptClone) const;

public:
    ::inflection::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit FiCommonConceptFactory(const ::inflection::util::ULocale& language);
};
