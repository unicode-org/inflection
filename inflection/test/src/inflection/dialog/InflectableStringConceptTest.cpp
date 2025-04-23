/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/Validate.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/npc.hpp>
#include <memory>

static inflection::dialog::InflectableStringConcept* createConcept(const ::inflection::dialog::SemanticFeatureModel* model, const ::inflection::dialog::SpeakableString& string, const ::std::vector<::std::u16string>& namedValues)
{
    auto result = new ::inflection::dialog::InflectableStringConcept(model, string);
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string value;
        for (::inflection::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
            if (key.empty()) {
                key = *iterator;
            }
            else {
                value = *iterator;
            }
        }
        npc(result)->putConstraint(*npc(npc(model)->getFeature(key)), value);
    }
    return result;
}

static inflection::dialog::InflectableStringConcept* createConcept(const ::inflection::dialog::SemanticFeatureModel* model, const ::std::u16string& string, const ::std::vector<::std::u16string>& namedValues)
{
    return createConcept(model, ::inflection::dialog::SpeakableString(string), namedValues);
}

static void compareInflection(const ::inflection::dialog::SemanticFeatureModel* model, const ::inflection::dialog::SpeakableString& expected, const ::inflection::dialog::SpeakableString& source, const ::std::vector<::std::u16string>& namedValues)
{
    auto inflectableConcept = createConcept(model, source, namedValues);
    auto result = npc(inflectableConcept)->toSpeakableString();
    INFO(::std::string("expected: ") + inflection::util::StringUtils::to_string(expected.toString()));
    INFO(::std::string("actual: ") + inflection::util::StringUtils::to_string(npc(result)->toString()));
    CHECK(expected == *npc(result));
    delete inflectableConcept;
    delete result;
}

static void compareInflection(const ::inflection::dialog::SemanticFeatureModel* model, const ::std::u16string& expected, const ::std::u16string& source, const ::std::vector<::std::u16string>& namedValues)
{
    compareInflection(model, ::inflection::dialog::SpeakableString(expected), ::inflection::dialog::SpeakableString(source), namedValues);
}

TEST_CASE("InflectableStringConceptTest#testDutch")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::DUTCH()))->getSemanticFeatureModel();
    ::std::u16string input(u"schip nummer");
    compareInflection(model, input, input, {u"number=singular"});
    compareInflection(model, input, input, {u"number=plural"});
    input = u"xxx yyy nummer";
    compareInflection(model, input, input, {u"number=singular"});
    compareInflection(model, input, input, {u"number=plural"});
    ::std::vector<::std::u16string> adjectives({u"oud", u"traag", u"klef", u"zat", u"saai", u"lui", u"blij", u"mooi", u"beschonken", u"roze", u"guur", u"kaal", u"boos", u"veel"});
    ::std::vector<::std::u16string> adjectivesDeclined({u"oude", u"trage", u"kleffe", u"zatte", u"saaie", u"luie", u"blije", u"mooie", u"beschonken", u"roze", u"gure", u"kale", u"boze", u"vele"});
    for (const auto& adjective : adjectives) {
        compareInflection(model, adjective, adjective, {});
        compareInflection(model, adjective, adjective, {u"pos=adjective"});
        compareInflection(model, adjective, adjective, {u"declension=undeclined"});
        compareInflection(model, adjective, adjective, {u"number=singular"});
        compareInflection(model, adjective, adjective, {u"pos=adjective", u"number=plural"});
    }
    int32_t adjectivesSize = (int32_t) adjectives.size();
    for (int32_t i = 0; i < adjectivesSize; i++) {
        auto adjective = adjectives[i];
        auto expected = adjectivesDeclined[i];
        compareInflection(model, expected, adjective, {u"declension=declined"});
        compareInflection(model, expected, adjective, {u"declension=declined", u"number=singular"});
        compareInflection(model, expected, adjective, {u"declension=declined", u"number=plural"});
    }
    ::std::vector<::std::u16string> nouns({u"koelkast", u"vergadering", u"contact", u"industrie", u"knie", u"idee", u"porie", u"bacterie", u"beer", u"contactpersoon", u"persoon", u"egel", u"auto", u"programma", u"baby"});
    ::std::vector<::std::u16string> nounsPlural({u"koelkasten", u"vergaderingen", u"contacten", u"industrieën", u"knieën", u"ideeën", u"poriën", u"bacteriën", u"beren", u"contactpersonen", u"personen", u"egels", u"auto’s", u"programma’s", u"baby’s"});
    for (const auto& noun : nouns) {
        compareInflection(model, noun, noun, {});
        compareInflection(model, noun, noun, {u"pos=adjective"});
        compareInflection(model, noun, noun, {u"number=singular"});
        compareInflection(model, noun, noun, {u"declension=declined"});
        compareInflection(model, noun, noun, {u"declension=undeclined"});
    }
    int32_t nounsSize = (int32_t) nouns.size();
    for (int32_t i = 0; i < nounsSize; i++) {
        auto noun = nouns[i];
        auto expected = nounsPlural[i];
        compareInflection(model, expected, noun, {u"number=plural"});
        compareInflection(model, expected, noun, {u"number=plural", u"declension=declined"});
        compareInflection(model, expected, noun, {u"number=plural", u"declension=undeclined"});
    }
}

static ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> createArabicConstraintMap(const ::inflection::dialog::SemanticFeatureModel* featureModel, const ::std::u16string& personStr, const ::std::u16string& numberStr, const ::std::u16string& genderStr, const ::std::u16string& caseStr, const ::std::u16string& definiteness)
{
    ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> constraintMap;
    if (!numberStr.empty()) {
        constraintMap.emplace(*npc(npc(featureModel)->getFeature(u"number")), numberStr);
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

static void appendDisplayValue(::std::vector<::inflection::dialog::DisplayValue>* displayValues, const ::std::u16string& inflection, const ::inflection::dialog::SemanticFeatureModel* model, const ::std::u16string& numberValue, const ::std::u16string& genderValue, const ::std::u16string& caseValue, const ::std::u16string& definitenessValue)
{
    auto constraintMap = createArabicConstraintMap(model, u"second", numberValue, genderValue, caseValue, definitenessValue);
    npc(displayValues)->emplace_back(inflection, constraintMap);
}

TEST_CASE("InflectableStringConceptTest#testArabic")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::ARABIC()))->getSemanticFeatureModel();
    auto displayFunction = npc(model)->getDefaultDisplayFunction();
    auto constraints = createArabicConstraintMap(model, u"", u"plural", u"feminine", u"", u"");
    ::std::vector<::inflection::dialog::DisplayValue> displayValues;
    appendDisplayValue(&displayValues, u"حانة", model, u"singular", u"feminine", u"", u"");
    appendDisplayValue(&displayValues, u"حانتان", model, u"dual", u"feminine", u"nominative", u"");
    appendDisplayValue(&displayValues, u"حانتين", model, u"dual", u"feminine", u"genitive", u"");
    appendDisplayValue(&displayValues, u"حانتا", model, u"dual", u"feminine", u"nominative", u"");
    appendDisplayValue(&displayValues, u"حانات", model, u"plural", u"feminine", u"", u"");
    appendDisplayValue(&displayValues, u"حانتي", model, u"dual", u"feminine", u"genitive", u"");
    auto displayValue = ::inflection::util::Validate::notNull(
            npc(displayFunction)->getDisplayValue(::inflection::dialog::SemanticFeatureModel_DisplayData(displayValues),
                                                  constraints, true));
    REQUIRE(u"حانات" == npc(displayValue)->getDisplayString());
    delete displayValue;
}

TEST_CASE("InflectableStringConceptTest#testExistsAPISpanish")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::SPANISH()))->getSemanticFeatureModel();
    ::std::unique_ptr<inflection::dialog::InflectableStringConcept> inflectableConcept;
    inflectableConcept.reset(createConcept(model, u"bienvenidos", {u"gender=common"}));
    CHECK_THROWS(inflectableConcept->isExists()); //It would throw since common is not a defined constraint
}

TEST_CASE("InflectableStringConceptTest#testExistsKorean")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::KOREAN()))->getSemanticFeatureModel();
    ::std::unique_ptr<inflection::dialog::InflectableStringConcept> inflectableConcept;
    inflectableConcept.reset(createConcept(model, u"", {u"case=accusative"}));
    CHECK_FALSE(inflectableConcept->isExists());
}

TEST_CASE("InflectableStringConceptTest#testExistsAPIPortuguese")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::PORTUGUESE()))->getSemanticFeatureModel();
    ::std::unique_ptr<inflection::dialog::InflectableStringConcept> inflectableConcept;
    inflectableConcept.reset(createConcept(model, u"carro", {u"gender=neuter"}));
    CHECK_THROWS(inflectableConcept->isExists()); //It would throw since neuter is not a defined constraint
}

TEST_CASE("InflectableStringConceptTest#testExistsAPIFrench")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::FRENCH()))->getSemanticFeatureModel();
    ::std::unique_ptr<inflection::dialog::InflectableStringConcept> inflectableConcept;
    inflectableConcept.reset(createConcept(model, u"bienvenu", {u"gender=neuter"}));
    CHECK_THROWS(inflectableConcept->isExists()); //It would throw since neuter is not a defined constraint

    inflectableConcept.reset(createConcept(model, u"bienvenu", {u"gender=common"}));
    CHECK_THROWS(inflectableConcept->isExists()); //It would throw since common is not a defined constraint
}

TEST_CASE("InflectableStringConceptTest#testExistsAPIItalian")
{
    auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::ITALIAN()))->getSemanticFeatureModel();
    ::std::unique_ptr<inflection::dialog::InflectableStringConcept> inflectableConcept;
    inflectableConcept.reset(createConcept(model, u"benvenuto", {u"gender=neuter"}));
    CHECK_FALSE(inflectableConcept->isExists()); //It would not throw since neuter is a defined constraint but can't inflect

    inflectableConcept.reset(createConcept(model, u"benvenuto", {u"gender=common"}));
    CHECK_THROWS(inflectableConcept->isExists()); //It would throw since common is not a defined constraint
}
