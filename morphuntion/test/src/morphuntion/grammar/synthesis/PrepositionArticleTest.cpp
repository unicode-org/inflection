/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DefaultFeatureFunction.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

static const morphuntion::dialog::DefaultFeatureFunction* getFeatureFunction(const ::morphuntion::dialog::SemanticFeatureModel* featureModel, const ::std::u16string& featureName)
{
    return npc(featureModel)->getDefaultFeatureFunction(*npc(npc(featureModel)->getFeature(featureName)));
}

static ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> createConstraints(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::vector<::std::u16string>& namedValues)
{
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> constraintMap;
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string value;
        for (::morphuntion::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
            if (key.empty()) {
                key = *iterator;
            }
            else {
                value = *iterator;
            }
        }
        constraintMap.emplace(*npc(npc(model)->getFeature(key)), value);
    }
    return constraintMap;
}

static morphuntion::dialog::SemanticFeatureModel_DisplayValue createConstrainedDisplayValue(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::u16string& value, const ::std::vector<::std::u16string>& namedValues)
{
    auto constraintMap = createConstraints(model, namedValues);
    return ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(value, constraintMap);
}

static void compare(const ::morphuntion::dialog::SpeakableString& expected, ::morphuntion::dialog::SpeakableString* result) {
    INFO(::std::string("expected: ") + morphuntion::util::StringUtils::to_string(expected.toString()));
    INFO(::std::string("actual: ") + morphuntion::util::StringUtils::to_string(npc(result)->toString()));
    CHECK(expected == *npc(result));
    delete result;
}

static void compare(const ::std::u16string& expected, ::morphuntion::dialog::SpeakableString* result) {
    compare(::morphuntion::dialog::SpeakableString(expected), result);
}

static void compare(const ::std::u16string& expected, ::morphuntion::dialog::SemanticFeatureModel_DisplayValue* result) {
    INFO(::std::string("expected: ") + morphuntion::util::StringUtils::to_string(expected));
    auto resultStr(npc(result)->getDisplayString());
    INFO(::std::string("actual: ") + morphuntion::util::StringUtils::to_string(resultStr));
    CHECK(expected == resultStr);
    delete result;
}


TEST_CASE("PrepositionArticleTest#testArticleDanish")
{
    auto featureModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::DANISH()))->getSemanticFeatureModel();
    auto featureWithDefiniteArticleFunction = getFeatureFunction(featureModel, u"withDefArticle");
    compare(::morphuntion::dialog::SpeakableString(u"den value", u"dén value"), npc(featureWithDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"value", {}), {}));
    compare(u"det value", npc(featureWithDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"value", {u"number=singular", u"gender=neuter"}), {}));
    compare(u"de value", npc(featureWithDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"value", {u"number=plural"}), {}));
    auto featureDefiniteArticleFunction = getFeatureFunction(featureModel, u"defArticle");
    compare(::morphuntion::dialog::SpeakableString(u"den", u"dén"), npc(featureDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"value", {}), {}));
    compare(u"det", npc(featureDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"value", {u"number=singular", u"gender=neuter"}), {}));
    compare(u"de", npc(featureDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"value", {u"number=plural"}), {}));
    compare(::morphuntion::dialog::SpeakableString(u"den", u"dén"), npc(featureDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"hund", {}), {}));
    compare(u"det", npc(featureDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"fonem", {}), {}));
    compare(u"de", npc(featureDefiniteArticleFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"hunde", {}), {}));
}

static ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> createArabicConstraintMap(const ::morphuntion::dialog::SemanticFeatureModel* featureModel, const ::std::u16string& personStr, const ::std::u16string& countStr, const ::std::u16string& genderStr, const ::std::u16string& caseStr, const ::std::u16string& definiteness)
{
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> constraintMap;
    if (!countStr.empty()) {
        constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"number")), countStr);
    }
    if (!genderStr.empty()) {
        constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"gender")), genderStr);
    }
    if (!caseStr.empty()) {
        constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"case")), caseStr);
    }
    constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"numberPronoun")), u"plural");
    if (!personStr.empty()) {
        constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"person")), personStr);
    }
    constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"genderPronoun")), u"masculine");
    if (!definiteness.empty()) {
        constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"definiteness")), definiteness);
    }
    return constraintMap;
}

static morphuntion::dialog::SemanticFeatureModel_DisplayData createArabicDisplayData(const ::morphuntion::dialog::SemanticFeatureModel* featureModel, const ::std::u16string& value, const ::std::u16string& definiteness)
{
    auto constraintMap = createArabicConstraintMap(featureModel, u"", u"", u"", u"", definiteness);
    return ::morphuntion::dialog::SemanticFeatureModel_DisplayData({::morphuntion::dialog::SemanticFeatureModel_DisplayValue(value, constraintMap)});
}

TEST_CASE("PrepositionArticleTest#testArticleAr")
{
    auto const featureModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::ARABIC()))->getSemanticFeatureModel();
    auto displayFunction = npc(featureModel)->getDefaultDisplayFunction();
    auto constraintMap = createArabicConstraintMap(featureModel, u"second", u"singular", u"masculine", u"accusative", u"definite");
    compare(u"", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"", u"definite"),constraintMap, true));
    compare(u"الهاتف", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"هاتف", u"definite"), constraintMap, true));
    compare(u"الـ iPhone", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"iPhone", u"definite"), constraintMap, true));
    compare(u"الهاتف", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u" هاتف", u"definite"), constraintMap, true));
    compare(u"الـ iPhone", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u" iPhone", u"definite"), constraintMap, true));
    compare(u"الهاتف", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"الهاتف", u"definite"), constraintMap, true));
    constraintMap = createArabicConstraintMap(featureModel, u"second", u"singular", u"masculine", u"accusative", u"indefinite");
    compare(u"iPhone", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"iPhone", u"definite"), constraintMap, true));
    constraintMap = createArabicConstraintMap(featureModel, u"second", u"singular", u"masculine", u"nominative", u"indefinite");
    compare(u"أخو", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"أخ", u"definite"), constraintMap, true));
    constraintMap = createArabicConstraintMap(featureModel, u"second", u"singular", u"masculine", u"accusative", u"indefinite");
    compare(u"أخا", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"أخ", u"definite"), constraintMap, true));
    constraintMap = createArabicConstraintMap(featureModel, u"second", u"singular", u"masculine", u"genitive", u"indefinite");
    compare(u"أخي", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"أخ", u"definite"), constraintMap, true));
    constraintMap = createArabicConstraintMap(featureModel, u"second", u"singular", u"", u"accusative", u"definite");
    compare(u"لللل اا غففف ي قق ففف", npc(displayFunction)->getDisplayValue(createArabicDisplayData(featureModel, u"لللل اا غففف ي قق ففف", u"definite"), constraintMap, true));
    auto featureFunction = getFeatureFunction(featureModel, u"withPossessivePron");
    compare(u"", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"", {u"number=singular", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"كتابكم", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"كتاب", {u"number=plural", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"أخيكم", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"أخي", {u"number=plural", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"baby", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"baby", {u"number=plural", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"حالة فيس بوك", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"حالة فيس بوك", {u"number=plural", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"الغالي", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"الغالي", {u"number=plural", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"حبيبي", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"حبيبي", {u"number=plural", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"قطتكم", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"قطة", {u"number=plural", u"gender=masculine", u"case=genitive"}), createConstraints(featureModel, {u"person=second", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"رسالتهم", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"رسالة", {u"number=singular", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=third", u"numberPronoun=plural", u"genderPronoun=masculine"})));
    compare(u"بريدهن", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"بريد", {u"number=singular", u"gender=masculine", u"case=accusative"}), createConstraints(featureModel, {u"person=third", u"numberPronoun=plural", u"genderPronoun=feminine"})));
    compare(u"رسالتنا", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"رسالة", {u"case=nominative"}), createConstraints(featureModel, {u"person=first", u"numberPronoun=plural", u"genderPronoun=feminine"})));
    compare(u"نتاكيست", npc(featureFunction)->getFeatureValue(createConstrainedDisplayValue(featureModel, u"نتاكيست", {u"number=plural", u"case=nominative"}), createConstraints(featureModel, {u"person=first", u"numberPronoun=plural"})));
}
