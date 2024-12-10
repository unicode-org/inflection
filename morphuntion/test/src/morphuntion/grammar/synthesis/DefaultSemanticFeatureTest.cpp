/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DefaultFeatureFunction.hpp>
#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerFactory.hpp>
#include <morphuntion/npc.hpp>

#include "util/TestUtils.hpp"

morphuntion::dialog::InflectableStringConcept* createConcept(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::u16string& string, const ::std::map<::std::u16string, ::std::u16string>& namedValues)
{
    auto result = new ::morphuntion::dialog::InflectableStringConcept(model, ::morphuntion::dialog::SpeakableString(string));
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

void testCustomFeatures(const ::morphuntion::dialog::SemanticFeatureModel* featureModel, const ::morphuntion::util::ULocale& locale, const ::std::u16string& word)
{
    INFO(::morphuntion::util::StringUtils::to_string(u"===" + locale.toString() + u"==="));
    auto features = ::morphuntion::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale.getLanguage());
    auto inflectableConcept = createConcept(featureModel, word, {});
    auto speakableString = npc(inflectableConcept)->toSpeakableString();
    INFO(locale.getName());
    REQUIRE(word == ::morphuntion::util::Validate::notNull(speakableString)->getPrint());
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
                auto inflectedResult = npc(::morphuntion::util::Validate::notNull(speakableString))->getPrint();
                INFO(::morphuntion::util::StringUtils::to_string(category.getName() + u'('
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
    auto defaultResult = npc(::morphuntion::util::Validate::notNull(speakableString))->getPrint();
    INFO(::morphuntion::util::StringUtils::to_string(defaultResult));
    INFO(locale.getName());
    REQUIRE(word == defaultResult);
    for (const auto& feature : features.getFeatures()) {
        auto semanticFeature = npc(featureModel)->getFeature(feature.getName());
        if (npc(featureModel)->getDefaultFeatureFunction(*npc(semanticFeature)) == nullptr || IGNORED_GRAMMEMES().find(feature.getName()) != IGNORED_GRAMMEMES().end()) {
            continue;
        }
        auto featureResult = npc(inflectableConcept)->getFeatureValue(*npc(semanticFeature));
        if (featureResult != nullptr) {
            INFO(::morphuntion::util::StringUtils::to_string(u"\"" + word
                + u"\"."
                + feature.getName()
                + u"->"
                + npc(featureResult)->getPrint()));
            delete featureResult;
        }
        else {
            WARN(::morphuntion::util::StringUtils::to_string(::std::u16string(u"Warning: ")
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
    auto commonConceptFactoryProvider = ::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::morphuntion::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        testCustomFeatures(npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel(), locale, u"");
    }
}

TEST_CASE("DefaultSemanticFeatureTest#testSpace")
{
    auto commonConceptFactoryProvider = ::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::morphuntion::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        testCustomFeatures(npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel(), locale, u" ");
    }
}

TEST_CASE("DefaultSemanticFeatureTest#testEmoji")
{
    auto commonConceptFactoryProvider = ::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& locale : ::morphuntion::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        testCustomFeatures(npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel(), locale, u"🤔");
    }
}

static constexpr int32_t MAXIMUM_WORDS_TO_TEST { int32_t(10) };

TEST_CASE("DefaultSemanticFeatureTest#testNouns")
{
    auto commonConceptFactoryProvider = ::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    auto testLocales = TestUtils::getTestLocaleMap();
    for (const auto& locale : ::morphuntion::grammar::synthesis::GrammarSynthesizerFactory::getKnownLocales()) {
        if (testLocales.find(locale.getLanguage()) != testLocales.end())
        {
            auto dictionary = ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);
            int32_t nounCount = 0;
            const ::morphuntion::dialog::SemanticFeatureModel* model = npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel();
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
