/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <string>
#include <vector>

class inflection::dialog::DefaultArticleLookupFunction
    : public virtual DefaultFeatureFunction
{
public:
    typedef DefaultFeatureFunction super;
    typedef ::std::pair<::std::u16string, ::std::vector<::std::pair<const ::inflection::dialog::SemanticFeature* const, ::std::u16string>>> ArticleDisplayValue;
    typedef ::std::vector<ArticleDisplayValue> ArticleDisplayData;

private:
    const bool includeSemanticValue {  };
    const bool insertSpace {  };
    const SemanticFeature* speakFeature {  };

public: /* protected */
    virtual SpeakableString* createPreposition(const ::inflection::dialog::DisplayValue& displayValue, const ::std::u16string& article) const;
    virtual SpeakableString* createPreposition(const ::inflection::dialog::DisplayValue& displayValue, const SpeakableString& article) const;
    virtual SpeakableString* createPostposition(const ::inflection::dialog::DisplayValue& displayValue, const ::std::u16string& article) const;
    // Returns displayData for the articles with their constraints used in the article lookup function, needs to overriden in the derived class to be able to use it
    virtual ArticleDisplayData getPossibleArticles(const SemanticFeatureModel& model) const;
    virtual const char16_t* getDerivedSemanticName() const;
    static const ::std::u16string* getFeatureValue(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature& semanticFeature);
    static ::std::u16string getDisplayFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::inflection::dialog::SemanticFeature& semanticFeature);
    bool insertsSpace() const;

public:
    DefaultArticleLookupFunction(const SemanticFeatureModel& model, bool includeSemanticValue, bool insertSpace);
    DefaultArticleLookupFunction(const DefaultArticleLookupFunction&) = delete;
    DefaultArticleLookupFunction& operator=(const DefaultArticleLookupFunction&) = delete;
};
