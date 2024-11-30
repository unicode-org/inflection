/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactory.hpp>
#include <morphuntion/dialog/Plurality.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <map>

class morphuntion::dialog::CommonConceptFactoryImpl
    : public virtual CommonConceptFactory
{
public:
    typedef CommonConceptFactory super;

private: /* protected */
    ::morphuntion::util::ULocale language;
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
    const ::morphuntion::util::ULocale& getLanguage() const override;
    InflectableStringConcept* createConcept(const ::morphuntion::dialog::SpeakableString& string) const override;
    SemanticConceptList* createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createOrList(const ::std::vector<::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const final;
    SemanticConceptList* createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const final;
    NumberConcept* number(const ::morphuntion::util::ULocale& regionFormatLocale, int64_t number) const override;
    NumberConcept* number(const ::morphuntion::util::ULocale& regionFormatLocale, double number) const override;
    SpeakableString quote(const ::morphuntion::dialog::SpeakableString& str) const override;

protected: /* package */
    virtual ::morphuntion::dialog::SpeakableString quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const;
    virtual ::morphuntion::dialog::SpeakableString quantifyType(const ::morphuntion::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const;

public:
    SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;
    SpeakableString* quantify(const NumberConcept& number, const ::morphuntion::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit CommonConceptFactoryImpl(const ::morphuntion::util::ULocale& language);
    CommonConceptFactoryImpl(const ::morphuntion::util::ULocale& language, const ::std::u16string& semanticFeatureForNumberConcept, const ::std::map<::std::u16string, ::std::u16string>& semanticFeatureToNumberConceptName);
    ~CommonConceptFactoryImpl() override;
};
