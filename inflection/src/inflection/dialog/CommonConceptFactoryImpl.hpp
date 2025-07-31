/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/CommonConceptFactory.hpp>
#include <inflection/dialog/Plurality.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/ULocale.hpp>
#include <map>

class inflection::dialog::CommonConceptFactoryImpl
    : public virtual CommonConceptFactory
{
public:
    typedef CommonConceptFactory super;

private: /* protected */
    ::inflection::util::ULocale language;
protected: /* protected */
    const SemanticFeatureModel semanticFeatureModel;
    const SemanticFeature* const semanticFeatureCount {  };
    const SemanticFeature* const semanticFeatureMeasure {  };
    const SemanticFeature* const semanticFeatureForNumberConcept {  };
    const ::std::map<::std::u16string, ::std::u16string> semanticFeatureToNumberConceptName {  };
private: /* protected */
    ::std::u16string startQuote {  };
    ::std::u16string endQuote {  };
    std::pair<SpeakableString,SpeakableString> itemsAnd;
    SpeakableString twoItemsAnd;
    std::pair<SpeakableString,SpeakableString> itemsOr;
    SpeakableString twoItemsOr;
    const Plurality plurality;

public:
    const SemanticFeatureModel* getSemanticFeatureModel() const override;
    const ::inflection::util::ULocale& getLanguage() const override;
    InflectableStringConcept* createConcept(const ::inflection::dialog::SpeakableString& string) const override;
    SemanticConceptList* createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createOrList(const ::std::vector<::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const final;
    SemanticConceptList* createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const final;
    NumberConcept* number(const ::inflection::util::ULocale& regionFormatLocale, int64_t number) const override;
    NumberConcept* number(const ::inflection::util::ULocale& regionFormatLocale, double number) const override;
    SpeakableString quote(const ::inflection::dialog::SpeakableString& str) const override;

protected: /* package */
    virtual ::inflection::dialog::SpeakableString quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const;
    virtual ::inflection::dialog::SpeakableString quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const;

public:
    SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;
    SpeakableString* quantifyFormatted(const NumberConcept& number, const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit CommonConceptFactoryImpl(const ::inflection::util::ULocale& language);
    CommonConceptFactoryImpl(const ::inflection::util::ULocale& language, const ::std::u16string& semanticFeatureForNumberConcept, const ::std::map<::std::u16string, ::std::u16string>& semanticFeatureToNumberConceptName);
    ~CommonConceptFactoryImpl() override;
    CommonConceptFactoryImpl(const CommonConceptFactoryImpl&) = delete;
    CommonConceptFactoryImpl& operator=(const CommonConceptFactoryImpl&) = delete;
};
