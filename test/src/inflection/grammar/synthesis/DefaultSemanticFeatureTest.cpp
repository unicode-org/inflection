/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerFactory.hpp>
#include <inflection/npc.hpp>

#include "util/TestUtils.hpp"

inflection::dialog::InflectableStringConcept* createConcept(const ::inflection::dialog::SemanticFeatureModel* model, const ::std::u16string& string, const ::std::map<::std::u16string, ::std::u16string>& namedValues)
{
    auto result = new ::inflection::dialog::InflectableStringConcept(model, ::inflection::dialog::SpeakableString(string));
    for (const auto& namedValue : namedValues) {
        npc(result)->putConstraintByName(namedValue.first, namedValue.second);
    }
    return result;
}

const ::std::set<::std::u16string>& IGNORED_GRAMMEMES()
{
    static auto IGNORED_GRAMMEMES_ = new ::std::set<::std::u16string>({u"gender", u"pos", u"sound", u"type", u"mode", u"speak", u"nickname", u"nicknameSpeak", u"uniqueId"});
    return *npc(IGNORED_GRAMMEMES_);
}

void testCustomFeatures(const ::inflection::dialog::SemanticFeatureModel* featureModel, const ::inflection::util::ULocale& locale, const ::std::u16string& word)
{
    INFO(::inflection::util::StringUtils::to_string(u"===" + locale.toString() + u"==="));
    auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale.getLanguage());
    auto inflectableConcept = createConcept(featureModel, word, {});
    auto speakableString = npc(inflectableConcept)->toSpeakableString();
    INFO(locale.getName());
    REQUIRE(word == ::inflection::util::Validate::notNull(speakableString)->getPrint());
    delete inflectableConcept;
    delete speakableString;

    if (npc(featureModel)->getDefaultDisplayFunction() != nullptr) {
        for (const auto& entry : features.getCategories()) {
            auto category = entry.second;
            if (IGNORED_GRAMMEMES().find(category.getName()) != IGNORED_GRAMMEMES().end()) {
                continue;
            }
            for (const auto& categoryValue : category.getValues()) {
                inflectableConcept = createConcept(featureModel, word, {{category.getName(), categoryValue}});
                speakableString = npc(inflectableConcept)->toSpeakableString();
                auto inflectedResult = npc(::inflection::util::Validate::notNull(speakableString))->getPrint();
                INFO(::inflection::util::StringUtils::to_string(category.getName() + u'('
                    + categoryValue
                    + u")->"
                    + inflectedResult));
                REQUIRE(word.empty() == inflectedResult.empty());
                delete inflectableConcept;
                delete speakableString;
            }
        }
    }
    inflectableConcept = createConcept(featureModel, word, {});
    speakableString = npc(inflectableConcept)->toSpeakableString();
    auto defaultResult = npc(::inflection::util::Validate::notNull(speakableString))->getPrint();
    INFO(::inflection::util::StringUtils::to_string(defaultResult));
    INFO(locale.getName());
    REQUIRE(word == defaultResult);
    for (const auto& feature : features.getFeatures()) {
        auto semanticFeature = npc(featureModel)->getFeature(feature.getName());
        if (npc(featureModel)->getDefaultFeatureFunction(*npc(semanticFeature)) == nullptr || IGNORED_GRAMMEMES().find(feature.getName()) != IGNORED_GRAMMEMES().end()) {
            continue;
        }
        auto featureResult = npc(inflectableConcept)->getFeatureValue(*npc(semanticFeature));
        if (featureResult != nullptr) {
            INFO(::inflection::util::StringUtils::to_string(u"\"" + word
                + u"\"."
                + feature.getName()
                + u"->"
                + npc(featureResult)->getPrint()));
            delete featureResult;
        }
        else {
            WARN(::inflection::util::StringUtils::to_string(::std::u16string(u"Warning: ")
                                                         + locale.toString()
                                                         + u" \""
                                                         + word
                                                         + u"\"."
                                                         + feature.getName()
                                                         + u" formats to null"));
        }
    }
    delete inflectableConcept;
    delete speakableString;
}

TEST_CASE("DefaultSemanticFeatureTest#testEmptyString")
{
    auto commonConceptFactoryProvider = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::inflection::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        testCustomFeatures(npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel(), locale, u"");
    }
}

TEST_CASE("DefaultSemanticFeatureTest#testSpace")
{
    auto commonConceptFactoryProvider = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::inflection::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        testCustomFeatures(npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel(), locale, u" ");
    }
}

TEST_CASE("DefaultSemanticFeatureTest#testEmoji")
{
    auto commonConceptFactoryProvider = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::inflection::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        testCustomFeatures(npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel(), locale, u"🤔");
    }
}

static constexpr int32_t MAXIMUM_WORDS_TO_TEST { int32_t(10) };

TEST_CASE("DefaultSemanticFeatureTest#testNouns")
{
    auto commonConceptFactoryProvider = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    auto testLocales = TestUtils::getTestLocaleMap();
    for (const auto& locale : ::inflection::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        if (testLocales.find(locale.getLanguage()) != testLocales.end())
        {
            auto dictionary = ::inflection::dictionary::DictionaryMetaData::createDictionary(locale);
            int32_t nounCount = 0;
            const ::inflection::dialog::SemanticFeatureModel* model = npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel();
            for (const auto& word : npc(dictionary)->getKnownWords()) {
                if (!npc(dictionary)->hasProperty(word, u"noun")) {
                    continue;
                }
                testCustomFeatures(model, locale, word);
                if (++nounCount >= MAXIMUM_WORDS_TO_TEST) {
                    break;
                }
            }
        }
    }
}
