/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConcept.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerFactory.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/npc.hpp>

static morphuntion::dialog::SemanticConcept* createConcept(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::u16string& label, const ::std::u16string& value, bool defaultToSemantic, const ::std::vector<::std::u16string>& namedValues)
{
    auto result = new ::morphuntion::dialog::SemanticConcept(model, ::morphuntion::dialog::SemanticValue(label, value), defaultToSemantic);
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string valueStr;
        for (::morphuntion::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
            if (key.empty()) {
                key = *iterator;
            }
            else {
                valueStr = *iterator;
            }
        }
        npc(result)->putConstraint(*npc(npc(model)->getFeature(key)), valueStr);
    }
    return result;
}

static void compareInflection(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::morphuntion::dialog::SpeakableString& expected, const ::std::u16string& label, const ::std::u16string& value, bool defaultToSemantic, const ::std::vector<::std::u16string>& namedValues)
{
    auto semanticConcept = createConcept(model, label, value, defaultToSemantic, namedValues);
    auto result = npc(semanticConcept)->toSpeakableString();
    delete semanticConcept;
    if (expected.isEmpty()) {
        INFO(::std::string("actual: ") + (result ? morphuntion::util::StringUtils::to_string(npc(result)->toString()) : ::std::string("null")));
        REQUIRE(nullptr == result);
    }
    else {
        INFO(::std::string("expected: ") + morphuntion::util::StringUtils::to_string(expected.toString()));
        INFO(::std::string("actual: ") + morphuntion::util::StringUtils::to_string(npc(result)->toString()));
        REQUIRE(expected == *npc(result));
        delete result;
    }
}

static void compareInflection(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::u16string& expected, const ::std::u16string& label, const ::std::u16string& value, bool defaultToSemantic, const ::std::vector<::std::u16string>& namedValues)
{
    compareInflection(model, ::morphuntion::dialog::SpeakableString(expected), label, value, defaultToSemantic, namedValues);
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

static void compareFeature(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::morphuntion::dialog::SpeakableString& expected, const ::std::u16string& feature, const ::std::u16string& label, const ::std::u16string& value, bool defaultToSemantic, const ::std::vector<::std::u16string>& namedValues)
{
    auto semanticConcept = createConcept(model, label, value, defaultToSemantic, namedValues);
    auto result = npc(semanticConcept)->getFeatureValueByName(feature);
    delete semanticConcept;
    if (expected.isEmpty()) {
        INFO(::std::string("actual: ") + (result ? morphuntion::util::StringUtils::to_string(npc(result)->toString()) : ::std::string("null")));
        REQUIRE(nullptr == result);
    }
    else {
        INFO(::std::string("expected: ") + morphuntion::util::StringUtils::to_string(expected.toString()));
        INFO(::std::string("actual: ") + morphuntion::util::StringUtils::to_string(npc(result)->toString()));
        REQUIRE(expected == *npc(result));
        delete result;
    }
}

static void compareFeature(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::u16string& expected, const ::std::u16string& feature, const ::std::u16string& label, const ::std::u16string& value, bool defaultToSemantic, const ::std::vector<::std::u16string>& namedValues)
{
    compareFeature(model, ::morphuntion::dialog::SpeakableString(expected), feature, label, value, defaultToSemantic, namedValues);
}


TEST_CASE("SemanticConceptTest#testEnglishDefaultToSemantic")
{
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::ENGLISH());

    compareInflection(&model, u"goose", u"default", u"goose", true, {});
    compareInflection(&model, u"goose", u"default", u"goose", true, {u"number=singular"});
    compareInflection(&model, u"geese", u"default", u"goose", true, {u"number=plural"});
    compareInflection(&model, u"goose", u"default", u"geese", true, {u"number=singular"});
    compareInflection(&model, u"nouns", u"default", u"noun", true, {u"number=plural"});
    compareInflection(&model, u"trusses", u"default", u"truss", true, {u"number=plural"});
    compareInflection(&model, u"churches", u"default", u"church", true, {u"number=plural"});
    compareInflection(&model, u"siriologies", u"default", u"siriology", true, {u"number=plural"});
    compareInflection(&model, u"yen", u"default", u"yen", true, {u"number=plural"});
    compareInflection(&model, u"yen", u"default", u"yen", true, {u"number=singular"});
    compareInflection(&model, u"word phrases", u"default", u"word phrase", true, {u"number=plural"});
    compareInflection(&model, u"goose’s", u"default", u"goose", true, {u"number=singular", u"case=genitive"});
    compareInflection(&model, u"geese’s", u"default", u"goose", true, {u"number=plural", u"case=genitive"});
    compareInflection(&model, u"Ylkdajhf’s", u"default", u"Ylkdajhf", true, {u"case=genitive"});
    compareInflection(&model, u"its", u"default", u"it", true, {u"case=genitive"});
    compareInflection(&model, u"Siri's", u"default", u"Siri's", true, {u"case=genitive"});
    compareInflection(&model, u"Siri’s", u"default", u"Siri’s", true, {u"case=genitive"});
    compareInflection(&model, u"Cities’", u"default", u"Cities", true, {u"case=genitive"});
    compareInflection(&model, u"Spatula City’s", u"default", u"Spatula City", true, {u"case=genitive"});
    compareInflection(&model, u"Spatula Cities’", u"default", u"Spatula Cities", true, {u"case=genitive"});
    compareInflection(&model, u"days", u"default", u"day", true, {u"number=plural"});
    compareInflection(&model, u"boys", u"default", u"boy", true, {u"number=plural"});
}

TEST_CASE("SemanticConceptTest#testConstraining")
{
    auto originalModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::SPANISH()))->getSemanticFeatureModel();
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::SPANISH(), {
        {::morphuntion::dialog::SemanticValue(u"default", u"val1"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({})},
        {::morphuntion::dialog::SemanticValue(u"default", u"val2"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(originalModel, {u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(originalModel, {u"number=plural"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"val3"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val3.1", createConstraints(originalModel, {u"gender=masculine", u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val3.2", createConstraints(originalModel, {u"gender=masculine", u"number=plural"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"alternative", u"val3"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val3.1-alt", createConstraints(originalModel, {u"gender=feminine", u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val3.2-alt", createConstraints(originalModel, {u"gender=feminine", u"number=plural"}))
        })},
        });

    model.setDefaultDisplayFunction(nullptr);
    CHECK_THROWS(compareInflection(&model, u"val1.0", u"default", u"val1", false, {}));
    compareInflection(&model, u"val1", u"default", u"val1", true, {});
    compareInflection(&model, u"val2.1", u"default", u"val2", false, {u"number=singular"});
    compareInflection(&model, u"val2.2", u"default", u"val2", false, {u"number=plural"});
    compareInflection(&model, u"", u"default", u"val2", false, {u"gender=masculine"});
    compareInflection(&model, u"val3.1", u"default", u"val3", false, {u"number=singular"});
    compareInflection(&model, u"val3.2", u"default", u"val3", false, {u"number=plural"});
    compareInflection(&model, u"val3.1", u"default", u"val3", false, {u"gender=masculine"});
    compareInflection(&model, u"", u"default", u"val3", false, {u"gender=feminine"});
    compareInflection(&model, u"val3.1-alt", u"alternative", u"val3", false, {u"number=singular"});
    compareInflection(&model, u"val3.2-alt", u"alternative", u"val3", false, {u"number=plural"});
    compareInflection(&model, u"", u"alternative", u"val3", false, {u"gender=masculine"});
    compareInflection(&model, u"val3.1-alt", u"alternative", u"val3", false, {u"gender=feminine"});
}

TEST_CASE("SemanticConceptTest#testFeatureValue")
{
    auto originalModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::SPANISH()))->getSemanticFeatureModel();
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::SPANISH(), {
        {::morphuntion::dialog::SemanticValue(u"default", u"val1"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({})},
        {::morphuntion::dialog::SemanticValue(u"default", u"val2"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(originalModel, {u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(originalModel, {u"number=plural"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"val3"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val3.1", createConstraints(originalModel, {u"gender=masculine", u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val3.2", createConstraints(originalModel, {u"gender=masculine", u"number=plural"}))
        })},
        });

    model.putDefaultFeatureFunction(*npc(model.getFeature(u"gender")), nullptr);
    CHECK_THROWS(compareFeature(&model, u"", u"gender", u"default", u"val1", false, {}));
    compareFeature(&model, u"", u"gender", u"default", u"val1", true, {});
    compareFeature(&model, u"", u"gender", u"default", u"val2", false, {});
    compareFeature(&model, u"masculine", u"gender", u"default", u"val3", false, {});
}

TEST_CASE("SemanticConceptTest#testDutchDefaultToSemantic") {
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::DUTCH());

    compareInflection(&model, u"reis", u"default", u"reis", true, {});
    compareInflection(&model, u"reizen", u"default", u"reizen", true, {});
}

TEST_CASE("SemanticConceptTest#testGerman")
{
    auto originalModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::GERMANY()))->getSemanticFeatureModel();
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::GERMANY(), {
        {::morphuntion::dialog::SemanticValue(u"default", u"termin"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termin", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=nominative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termins", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=genitive"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termin", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=dative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termin", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=accusative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termine", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=nominative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termine", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=genitive"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Terminen", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=dative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termine", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=accusative"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"terminDefault"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Termin", {})
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"cheap"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Not a stem for some reason", createConstraints(originalModel, {u"number=singular", u"gender=masculine"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"günstig", createConstraints(originalModel, {u"stem="}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"expensive"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"Not a stem for some reason", createConstraints(originalModel, {u"number=singular", u"gender=masculine"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"teuer", createConstraints(originalModel, {u"stem=teur"}))
        })}
    });
    compareInflection(&model, u"Termine", u"default", u"termin", false, {u"case=accusative", u"number=plural", u"declension=strong"});
    compareInflection(&model, u"Termine", u"default", u"termin", false, {u"case=nominative", u"number=plural", u"declension=strong"});

    compareInflection(&model, u"Termine", u"default", u"terminDefault", false, {u"case=accusative", u"number=plural", u"declension=strong"});
    compareInflection(&model, u"Termine", u"default", u"terminDefault", false, {u"case=nominative", u"number=plural", u"declension=strong"});

    compareInflection(&model, u"günstigen", u"default", u"cheap", false, {u"case=accusative", u"number=singular", u"gender=masculine", u"declension=strong"});
    compareInflection(&model, u"günstiger", u"default", u"cheap", false, {u"case=nominative", u"number=singular", u"gender=masculine", u"declension=strong"});

    // While the display value is teuer, the stem is teur
    compareInflection(&model, u"teuren", u"default", u"expensive", false, {u"case=accusative", u"number=singular", u"gender=masculine", u"declension=strong"});
    compareInflection(&model, u"teurer", u"default", u"expensive", false, {u"case=nominative", u"number=singular", u"gender=masculine", u"declension=strong"});
}

TEST_CASE("SemanticConceptTest#testGermanHomePod")
{
    auto originalModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::GERMANY()))->getSemanticFeatureModel();
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::GERMANY(), {
        {::morphuntion::dialog::SemanticValue(u"default", u"HomePod"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePod", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=nominative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePods", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=genitive"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePod", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=dative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePod", createConstraints(originalModel, {u"gender=masculine", u"number=singular", u"case=accusative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePods", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=nominative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePods", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=genitive"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePods", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=dative"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePods", createConstraints(originalModel, {u"gender=masculine", u"number=plural", u"case=accusative"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"HomePodDefault"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
                ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"HomePod", {})
        })}
    });

    compareInflection(&model, u"HomePods", u"default", u"HomePod", false, {u"case=accusative", u"number=plural", u"declension=strong"});
    compareInflection(&model, u"HomePods", u"default", u"HomePod", false, {u"case=nominative", u"number=plural", u"declension=strong"});
}

static const int32_t MAXIMUM_WORDS_TO_TEST = 50;

TEST_CASE("SemanticConceptTest#testDefaultToSemantic")
{
    auto commonConceptFactoryProvider = ::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::morphuntion::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        auto dictionary = ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);
        auto model = npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(::morphuntion::util::LocaleUtils::SPANISH()))->getSemanticFeatureModel();

        int nounCount = 0;
        for (const auto& word : npc(dictionary)->getKnownWords()) {
            if (!npc(dictionary)->hasProperty(word, u"noun")) {
                // Uninteresting word. Most of the time we want a known noun.
                continue;
            }
            compareInflection(model, word, u"default", word, true, {});
            if (++nounCount >= MAXIMUM_WORDS_TO_TEST) {
                // That's enough to try out.
                break;
            }
        }
    }
}

TEST_CASE("SemanticConceptTest#testClone")
{
    auto originalModel = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::SPANISH()))->getSemanticFeatureModel();
    ::morphuntion::dialog::SemanticFeatureModel model(::morphuntion::util::LocaleUtils::SPANISH(), {
        {::morphuntion::dialog::SemanticValue(u"default", u"val2"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(originalModel, {u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(originalModel, {u"number=plural"}))
        })},
        });

    model.setDefaultDisplayFunction(nullptr);
    ::std::unique_ptr<::morphuntion::dialog::SemanticConcept> semanticConcept(createConcept(&model, u"default", u"val2", false, {}));
    REQUIRE(semanticConcept->toString() == u"SS[val2.1]");
    ::std::unique_ptr<::morphuntion::dialog::SemanticConcept> clonedConcept(semanticConcept->clone());
    REQUIRE(*semanticConcept == *clonedConcept);
    REQUIRE(clonedConcept->toString() == u"SS[val2.1]");
    clonedConcept->putConstraint(*npc(model.getFeature(u"number")), u"plural");
    REQUIRE(*semanticConcept != *clonedConcept);
    REQUIRE(clonedConcept->toString() == u"SS[val2.2]");
    REQUIRE(semanticConcept->toString() == u"SS[val2.1]");
    REQUIRE(clonedConcept->getSemanticValue() == u"val2");
    REQUIRE(semanticConcept->getSemanticValue() == u"val2");

    REQUIRE(clonedConcept->isExists());
    clonedConcept->putConstraint(*npc(model.getFeature(u"person")), u"first"); // overconstrain
    REQUIRE_FALSE(clonedConcept->isExists());
}
