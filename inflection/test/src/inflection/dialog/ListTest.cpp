/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>

static const inflection::dialog::CommonConceptFactory* getCommonConceptFactory(const inflection::util::ULocale& locale) {
    return npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale));
}

static ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> toConcepts(const ::inflection::dialog::SemanticFeatureModel* model, const ::std::vector<::std::u16string>& strings)
{
    ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> concepts;
    concepts.reserve(strings.size());
    for (const auto& string : strings) {
        concepts.emplace_back(new ::inflection::dialog::InflectableStringConcept(model, inflection::dialog::SpeakableString(string)));
    }
    return concepts;
}

static ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> toConcepts(const ::inflection::dialog::SemanticFeatureModel* model, const ::std::vector<::inflection::dialog::SpeakableString>& strings)
{
    ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> concepts;
    concepts.reserve(strings.size());
    for (const auto& string : strings) {
        concepts.emplace_back(new ::inflection::dialog::InflectableStringConcept(model, string));
    }
    return concepts;
}

static ::std::u16string andList(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::std::vector<::std::u16string>& args)
{
    ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> concepts(toConcepts(npc(conceptFactory)->getSemanticFeatureModel(), args));
    inflection::dialog::SemanticConceptList* andConceptList = npc(conceptFactory)->createAndList(concepts);
    auto speakableResult = npc(andConceptList)->toSpeakableString();
    ::std::u16string result;
    if (speakableResult != nullptr) {
        result = npc(speakableResult)->getPrint();
    }
    for (auto inflectableConcept : concepts) {
        delete inflectableConcept;
    }
    delete andConceptList;
    delete speakableResult;

    return result;
}

static ::std::u16string andHonorificList(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::std::u16string& honorific, const ::std::vector<::std::u16string>& args)
{
    ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> concepts(toConcepts(npc(conceptFactory)->getSemanticFeatureModel(), args));
    auto andConceptList = npc(conceptFactory)->createAndList(concepts);
    npc(andConceptList)->setItemSuffix(inflection::dialog::SpeakableString(honorific));
    auto speakableResult = npc(andConceptList)->toSpeakableString();
    ::std::u16string result;
    if (speakableResult != nullptr) {
        result = npc(speakableResult)->getPrint();
    }
    for (auto inflectableConcept : concepts) {
        delete inflectableConcept;
    }
    delete andConceptList;
    delete speakableResult;

    return result;
}

static ::std::u16string orList(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::std::vector<::std::u16string>& args)
{
    ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> concepts(toConcepts(npc(conceptFactory)->getSemanticFeatureModel(), args));
    inflection::dialog::SemanticConceptList* andConceptList = npc(conceptFactory)->createOrList(concepts);
    auto speakableResult = npc(andConceptList)->toSpeakableString();
    ::std::u16string result = npc(speakableResult)->getPrint();
    for (auto inflectableConcept : concepts) {
        delete inflectableConcept;
    }
    delete andConceptList;
    delete speakableResult;

    return result;
}

TEST_CASE("ListTest#testSemanticFeatures")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::SPANISH());
    auto inputElements = toConcepts(npc(factory)->getSemanticFeatureModel(), {u"gato", u"gata", u"gatos", u"gatas"});
    auto andListSpeakable = npc(factory)->createAndList(inputElements);
    auto speakableValue = npc(andListSpeakable)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"gato, gata, gatos y gatas"));

    delete speakableValue;
    speakableValue = npc(andListSpeakable)->getFeatureValueByName(u"gender");
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"masculine"));

    delete speakableValue;
    speakableValue = npc(andListSpeakable)->getFeatureValueByName(u"withDePrepArticle");
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"del gato, de la gata, de los gatos y de las gatas"));

    delete andListSpeakable;
    delete speakableValue;
    andListSpeakable = npc(factory)->createAndList(::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>({inputElements[0], inputElements[1]}));
    npc(andListSpeakable)->putConstraintByName(u"number", u"plural");
    speakableValue = npc(andListSpeakable)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"gatos y gatas"));
    npc(andListSpeakable)->reset();
    delete speakableValue;
    speakableValue = npc(andListSpeakable)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"gato y gata"));
    npc(andListSpeakable)->putConstraintByName(u"number", u"plural");
    delete speakableValue;
    speakableValue = npc(andListSpeakable)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"gatos y gatas"));
    npc(andListSpeakable)->clearConstraintByName(u"number");
    delete speakableValue;
    speakableValue = npc(andListSpeakable)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"gato y gata"));

    delete andListSpeakable;
    delete speakableValue;
    for (auto inflectableConcept : inputElements) {
        delete inflectableConcept;
    }
}

TEST_CASE("ListTest#testSetters")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::ENGLISH());
    auto inputElements = toConcepts(npc(factory)->getSemanticFeatureModel(), {u"a", u"b", u"c", u"d"});
    auto andList = npc(factory)->createAndList(inputElements);
    auto speakableValue = npc(andList)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"a, b, c, and d"));

    npc(andList)->setBeforeFirst(inflection::dialog::SpeakableString(u"1"));
    npc(andList)->setItemPrefix(inflection::dialog::SpeakableString(u"2"));
    npc(andList)->setItemSuffix(inflection::dialog::SpeakableString(u"3"));
    npc(andList)->setAfterFirst(inflection::dialog::SpeakableString(u"4"));
    npc(andList)->setItemDelimiter(inflection::dialog::SpeakableString(u"5"));
    npc(andList)->setBeforeLast(inflection::dialog::SpeakableString(u"6"));
    npc(andList)->setAfterLast(inflection::dialog::SpeakableString(u"7"));

    delete speakableValue;
    speakableValue = npc(andList)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"12a342b352c362d37"));
    REQUIRE(npc(andList)->toString() == u"[a, b, c, d]");

    npc(andList)->setAfterFirst(inflection::dialog::SpeakableString(u""));
    npc(andList)->setBeforeLast(inflection::dialog::SpeakableString(u""));

    delete speakableValue;
    speakableValue = npc(andList)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"12a352b352c352d37"));
    REQUIRE(npc(andList)->isExists());

    delete andList;
    andList = npc(factory)->createAndList(::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>());
    npc(andList)->setBeforeFirst(inflection::dialog::SpeakableString(u"1"));
    npc(andList)->setItemPrefix(inflection::dialog::SpeakableString(u"2"));
    npc(andList)->setItemSuffix(inflection::dialog::SpeakableString(u"3"));
    npc(andList)->setAfterFirst(inflection::dialog::SpeakableString(u"4"));
    npc(andList)->setItemDelimiter(inflection::dialog::SpeakableString(u"5"));
    npc(andList)->setBeforeLast(inflection::dialog::SpeakableString(u"6"));
    npc(andList)->setAfterLast(inflection::dialog::SpeakableString(u"7"));
    REQUIRE(npc(andList)->toSpeakableString() == nullptr);
    REQUIRE_FALSE(npc(andList)->isExists());
    REQUIRE(npc(andList)->getFeatureValue(*npc(npc(npc(factory)->getSemanticFeatureModel())->getFeature(u"number"))) == nullptr);

    delete andList;
    delete speakableValue;
    andList = npc(factory)->createAndList(::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>({inputElements[0]}));
    npc(andList)->setBeforeFirst(inflection::dialog::SpeakableString(u"1"));
    npc(andList)->setItemPrefix(inflection::dialog::SpeakableString(u"2"));
    npc(andList)->setItemSuffix(inflection::dialog::SpeakableString(u"3"));
    npc(andList)->setAfterFirst(inflection::dialog::SpeakableString(u"4"));
    npc(andList)->setItemDelimiter(inflection::dialog::SpeakableString(u"5"));
    npc(andList)->setBeforeLast(inflection::dialog::SpeakableString(u"6"));
    npc(andList)->setAfterLast(inflection::dialog::SpeakableString(u"7"));
    speakableValue = npc(andList)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"12a37"));

    delete andList;
    delete speakableValue;
    andList = npc(factory)->createAndList(::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>({inputElements[0], inputElements[1]}));
    npc(andList)->setBeforeFirst(inflection::dialog::SpeakableString(u"1"));
    npc(andList)->setItemPrefix(inflection::dialog::SpeakableString(u"2"));
    npc(andList)->setItemSuffix(inflection::dialog::SpeakableString(u"3"));
    npc(andList)->setAfterFirst(inflection::dialog::SpeakableString(u"4"));
    npc(andList)->setItemDelimiter(inflection::dialog::SpeakableString(u"5"));
    npc(andList)->setBeforeLast(inflection::dialog::SpeakableString(u"6"));
    npc(andList)->setAfterLast(inflection::dialog::SpeakableString(u"7"));
    speakableValue = npc(andList)->toSpeakableString();
    REQUIRE(*npc(speakableValue) == inflection::dialog::SpeakableString(u"12a3462b37"));

    delete andList;
    delete speakableValue;
    for (auto inflectableConcept : inputElements) {
        delete inflectableConcept;
    }
}

TEST_CASE("ListTest#testListFromConcepts")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::US());
    auto one = inflection::dialog::NumberConcept((int64_t)1, inflection::util::LocaleUtils::US(), inflection::util::LocaleUtils::US()).toSpeakableString();
    auto two = inflection::dialog::NumberConcept((int64_t)2, inflection::util::LocaleUtils::US(), inflection::util::LocaleUtils::US()).toSpeakableString();
    auto three = inflection::dialog::NumberConcept((int64_t)3, inflection::util::LocaleUtils::US(), inflection::util::LocaleUtils::US()).toSpeakableString();
    auto four = inflection::dialog::NumberConcept((int64_t)4, inflection::util::LocaleUtils::US(), inflection::util::LocaleUtils::US()).toSpeakableString();

    auto concepts = toConcepts(npc(factory)->getSemanticFeatureModel(), {*npc(one), *npc(two), *npc(three), *npc(four)});
    auto andListFromConcepts = npc(factory)->createAndList(concepts);
    auto actual = npc(andListFromConcepts)->toSpeakableString();
    REQUIRE(actual != nullptr);
    REQUIRE(u"1, 2, 3, and 4" == npc(actual)->getPrint());
    auto actualSecondItem = npc(npc(andListFromConcepts)->get(1))->toSpeakableString();
    auto expectedSecondItem = npc(concepts[1])->toSpeakableString();
    REQUIRE(*npc(actualSecondItem) == *npc(expectedSecondItem));
    delete actualSecondItem;
    delete expectedSecondItem;
    delete actual;
    for (auto inflectableConcept : concepts) {
        delete inflectableConcept;
    }
    delete one;
    delete two;
    delete three;
    delete four;
    delete andListFromConcepts;
}

TEST_CASE("ListTest#testListEn")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::US());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 and 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2, and 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3, and 4");
    REQUIRE(andList(factory, {u"1 and 2", u"3 and 4", u"5 and 6"}) == u"1 and 2, 3 and 4, and 5 and 6");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane, and Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 or 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2, or 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3, or 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane, or Joan");
}

TEST_CASE("ListTest#testSpeakAndValueEn")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::US());
    auto inputElements = toConcepts(npc(factory)->getSemanticFeatureModel(), {
            inflection::dialog::SpeakableString(u"1", u"one"),
            inflection::dialog::SpeakableString(u"2", u"two"),
            inflection::dialog::SpeakableString(u"3", u"three")});
    ::std::u16string expectedPrint(u"1, 2, and 3");
    ::std::u16string expectedSpeak(u"one, two, and three");
    auto andListSpeakable = npc(factory)->createAndList(inputElements);
    auto speakableValue = npc(andListSpeakable)->toSpeakableString();
    REQUIRE(speakableValue != nullptr);
    auto printValue = npc(speakableValue)->getPrint();
    auto speakValue = npc(speakableValue)->getSpeak();
    inflection::dialog::SpeakableString expectedSpeakable(expectedPrint, expectedSpeak);
    REQUIRE(printValue == expectedPrint);
    REQUIRE(speakValue == expectedSpeak);
    REQUIRE(*npc(speakableValue) == expectedSpeakable);
    delete andListSpeakable;
    delete speakableValue;
    for (auto inflectableConcept : inputElements) {
        delete inflectableConcept;
    }
}

TEST_CASE("ListTest#testSpeakOrValueEn")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::US());
    auto inputElements = toConcepts(npc(factory)->getSemanticFeatureModel(), {
            inflection::dialog::SpeakableString(u"1", u"one"),
            inflection::dialog::SpeakableString(u"2", u"two"),
            inflection::dialog::SpeakableString(u"3", u"three")});
    ::std::u16string expectedPrint(u"1, 2, or 3");
    ::std::u16string expectedSpeak(u"one, two, or three");
    auto orListSpeakable = npc(factory)->createOrList(inputElements);
    auto speakableValue = npc(orListSpeakable)->toSpeakableString();
    REQUIRE(speakableValue != nullptr);
    auto printValue = npc(speakableValue)->getPrint();
    auto speakValue = npc(speakableValue)->getSpeak();
    inflection::dialog::SpeakableString expectedSpeakable(expectedPrint, expectedSpeak);
    REQUIRE(printValue == expectedPrint);
    REQUIRE(speakValue == expectedSpeak);
    REQUIRE(*npc(speakableValue) == expectedSpeakable);
    delete orListSpeakable;
    delete speakableValue;
    for (auto inflectableConcept : inputElements) {
        delete inflectableConcept;
    }
}

TEST_CASE("ListTest#testListDe")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::GERMANY());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 und 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 und 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 und 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane und Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 oder 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 oder 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 oder 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane oder Joan");
}

TEST_CASE("ListTest#testListFr")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::FRANCE());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 et 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 et 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 et 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane et Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 ou 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 ou 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 ou 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane ou Joan");
}

TEST_CASE("ListTest#testListTh")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::THAILAND());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 และ 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 และ 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 และ 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane และ Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 หรือ 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 หรือ 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 หรือ 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane หรือ Joan");
}

TEST_CASE("ListTest#testListAr")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::SAUDI_ARABIA());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1، و2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1، و2، و3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1، و2، و3، و4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe، وJohn Junior، وJane، وJoan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1، أو 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1، أو 2، أو 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1، أو 2، أو 3، أو 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe، أو John Junior، أو Jane، أو Joan");
}

TEST_CASE("ListTest#testListHe")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::ISRAEL());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 ו-2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 ו-3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 ו-4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane ו-Joan");
    REQUIRE(andList(factory, {u"אחת", u"שתיים", u"שלוש", u"ארבע"}) == u"אחת, שתיים, שלוש וארבע");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"ארבע"}) == u"1, 2, 3 וארבע");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 או 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 או 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 או 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane או Joan");
    REQUIRE(orList(factory, {u"אחת", u"שתיים", u"שלוש", u"ארבע"}) == u"אחת, שתיים, שלוש או ארבע");
}

TEST_CASE("ListTest#testListJa")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::JAPAN());

    REQUIRE(andHonorificList(factory, u"さん", {}).empty());
    REQUIRE(andHonorificList(factory, u"さん", {u"1"}) == u"1さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"1", u"2"}) == u"1さんと2さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"1", u"2", u"3"}) == u"1さんと2さん、3さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"1", u"2", u"3", u"4"}) == u"1さんと2さん、3さん、4さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"JoeさんとJohn Juniorさん、Janeさん、Joanさん");

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1と2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1と2、3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1と2、3、4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"JoeとJohn Junior、Jane、Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1または2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1または2、3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1または2、3、4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"JoeまたはJohn Junior、Jane、Joan");

    REQUIRE(andHonorificList(factory, u"さん", {u"お母さん"}) == u"お母さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"太郎"}) == u"太郎さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"太郎さん"}) == u"太郎さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"お母さん", u"お父さん"}) == u"お母さんとお父さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"お母さん", u"お父さん", u"おじいさん"}) == u"お母さんとお父さん、おじいさん");
    REQUIRE(andHonorificList(factory, u"さん", {u"お母さん", u"太郎", u"愛子"}) == u"お母さんと太郎さん、愛子さん");
    REQUIRE(andHonorificList(factory, u"さん", {u"斎藤さん", u"工藤", u"近藤", u"後藤さん"}) == u"斎藤さんと工藤さん、近藤さん、後藤さん");
}

TEST_CASE("ListTest#testListEs")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::SPAIN_SPANISH());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 y 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 y 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 y 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane y Joan");
    // "y" changes to "e" when followed by the "i" sound
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Ivan"}) == u"Joe, John Junior, Jane e Ivan");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Hivan"}) == u"Joe, John Junior, Jane e Hivan");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Ívan"}) == u"Joe, John Junior, Jane e Ívan");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Hívan"}) == u"Joe, John Junior, Jane e Hívan");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Yvan"}) == u"Joe, John Junior, Jane e Yvan");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Hyvan"}) == u"Joe, John Junior, Jane e Hyvan");
    // This rule does not apply when the words begin with a diphthong (2 vowels in a row)
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Hierro"}) == u"Joe, John Junior, Jane y Hierro");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Yeso"}) == u"Joe, John Junior, Jane y Yeso");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Hyeso"}) == u"Joe, John Junior, Jane y Hyeso");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 o 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 o 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 o 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane o Joan");
    // "o" changes to "u" when followed by the "o" sound
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Omar"}) == u"Joe, John Junior, Jane u Omar");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Homar"}) == u"Joe, John Junior, Jane u Homar");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Ómar"}) == u"Joe, John Junior, Jane u Ómar");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Hómar"}) == u"Joe, John Junior, Jane u Hómar");
    // The diphthong exception doesn't apply to or lists
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Oita"}) == u"Joe, John Junior, Jane u Oita");
}

TEST_CASE("ListTest#testListDirectFromCommonConceptFactoryProvider")
{
    // This is slow code because it's not cached, but we want to allow people to create subclasses.
    inflection::dialog::LocalizedCommonConceptFactoryProvider provider;
    auto factory = provider.getCommonConceptFactory(inflection::util::LocaleUtils::MEXICO_SPANISH());

    REQUIRE(andList(factory, {u"Jane", u"Joan"}) == u"Jane y Joan");
    REQUIRE(andList(factory, {u"Jane", u"Ivan"}) == u"Jane e Ivan");
}

TEST_CASE("ListTest#testListIt")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::ITALY());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 e 2");
    REQUIRE(andList(factory, {u"Jane", u"Joan"}) == u"Jane e Joan");
    REQUIRE(andList(factory, {u"Jane", u"Angela"}) == u"Jane e Angela");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 e 3");
    REQUIRE(andList(factory, {u"Joe", u"Jane", u"Joan"}) == u"Joe, Jane e Joan");
    REQUIRE(andList(factory, {u"Joe", u"Jane", u"Angela"}) == u"Joe, Jane e Angela");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 e 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane e Joan");
    REQUIRE(andList(factory, {u"aereo", u"elicottero"}) == u"aereo ed elicottero");
    REQUIRE(andList(factory, {u"elicottero", u"aereo"}) == u"elicottero e aereo");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 o 2");
    REQUIRE(orList(factory, {u"Jane", u"Joan"}) == u"Jane o Joan");
    REQUIRE(orList(factory, {u"Jane", u"Angela"}) == u"Jane o Angela");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 o 3");
    REQUIRE(orList(factory, {u"Joe", u"Jane", u"Joan"}) == u"Joe, Jane o Joan");
    REQUIRE(orList(factory, {u"Joe", u"Jane", u"Angela"}) == u"Joe, Jane o Angela");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 o 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane o Joan");
    REQUIRE(orList(factory, {u"casa", u"ostello"}) == u"casa o ostello");
    REQUIRE(orList(factory, {u"ostello", u"casa"}) == u"ostello o casa");
    REQUIRE(orList(factory, {u"casa", u"hotel"}) == u"casa o hotel");
}

TEST_CASE("ListTest#testListKo")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::KOREA());

    REQUIRE(andHonorificList(factory, u"님", {}).empty());
    REQUIRE(andHonorificList(factory, u"님", {u"1"}) == u"1님");
    REQUIRE(andHonorificList(factory, u"님", {u"1", u"2"}) == u"1님과 2님");

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1과 2");
    REQUIRE(andList(factory, {u"Angela", u"Joan"}) == u"Angela와 Joan");
    REQUIRE(andList(factory, {u"John", u"Joan"}) == u"John과 Joan");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2과 3");
    REQUIRE(andList(factory, {u"Joe", u"Angela", u"Joan"}) == u"Joe, Angela와 Joan");
    REQUIRE(andList(factory, {u"Joe", u"John", u"Joan"}) == u"Joe, John과 Joan");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3과 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Angela", u"Joan"}) == u"Joe, John Junior, Angela와 Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1또는 2");
    REQUIRE(orList(factory, {u"Jane", u"Joan"}) == u"Jane또는 Joan");
    REQUIRE(orList(factory, {u"Jane", u"Angela"}) == u"Jane또는 Angela");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2또는 3");
    REQUIRE(orList(factory, {u"Joe", u"Jane", u"Joan"}) == u"Joe, Jane또는 Joan");
    REQUIRE(orList(factory, {u"Joe", u"Jane", u"Angela"}) == u"Joe, Jane또는 Angela");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3또는 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Angela", u"Joan"}) == u"Joe, John Junior, Angela또는 Joan");
}

TEST_CASE("ListTest#testListZh")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::CHINA());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1和2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1、2和3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1、2、3和4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe、John Junior、Jane和Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1或2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1、2或3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1、2、3或4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe、John Junior、Jane或Joan");
}

TEST_CASE("ListTest#testListYue")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::HONGKONG_CHINESE());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1同2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1、2同3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1、2、3同4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe、John Junior、Jane同Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1或2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1、2或3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1、2、3或4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe、John Junior、Jane或Joan");
}

TEST_CASE("ListTest#testListSv")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::SWEDISH());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 och 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 och 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 och 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane och Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 eller 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 eller 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 eller 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane eller Joan");
}

TEST_CASE("ListTest#testListNb")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::NORWEGIAN());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 og 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 og 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 og 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane og Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 eller 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 eller 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 eller 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane eller Joan");
}

TEST_CASE("ListTest#testListDa")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::DANISH());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 og 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 og 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 og 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane og Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 eller 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 eller 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 eller 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane eller Joan");
}

TEST_CASE("ListTest#testListPt")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::PORTUGUESE());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 e 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 e 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 e 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane e Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 ou 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 ou 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 ou 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane ou Joan");
}

TEST_CASE("ListTest#testListNl")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::DUTCH());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 en 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 en 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 en 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane en Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 of 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 of 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 of 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane of Joan");
}

TEST_CASE("ListTest#testListMs")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::MALAY());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 dan 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 dan 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 dan 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane dan Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 atau 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2, atau 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3, atau 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane, atau Joan");
}

TEST_CASE("ListTest#testListId")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::INDONESIAN());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 dan 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2, dan 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3, dan 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane, dan Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 atau 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2, atau 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3, atau 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane, atau Joan");
}

TEST_CASE("ListTest#testListTr")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::TURKISH());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 ve 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 ve 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 ve 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane ve Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 ya da 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 ya da 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 ya da 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane ya da Joan");
}

TEST_CASE("ListTest#testListRu")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::RUSSIAN());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 и 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2 и 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 и 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane и Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 или 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 или 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 или 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane или Joan");
}

TEST_CASE("ListTest#testListHi")
{
    auto factory = getCommonConceptFactory(inflection::util::LocaleUtils::HINDI());

    REQUIRE(andList(factory, {u"1"}) == u"1");
    REQUIRE(andList(factory, {u"1", u"2"}) == u"1 और 2");
    REQUIRE(andList(factory, {u"1", u"2", u"3"}) == u"1, 2, और 3");
    REQUIRE(andList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3, और 4");
    REQUIRE(andList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane, और Joan");

    REQUIRE(orList(factory, {u"1"}) == u"1");
    REQUIRE(orList(factory, {u"1", u"2"}) == u"1 या 2");
    REQUIRE(orList(factory, {u"1", u"2", u"3"}) == u"1, 2 या 3");
    REQUIRE(orList(factory, {u"1", u"2", u"3", u"4"}) == u"1, 2, 3 या 4");
    REQUIRE(orList(factory, {u"Joe", u"John Junior", u"Jane", u"Joan"}) == u"Joe, John Junior, Jane या Joan");
}

TEST_CASE("ListTest#testClone")
{
    static constexpr int32_t MAXIMUM_WORDS_TO_TEST { 3 };
    auto commonConceptFactoryProvider = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();
    for (const auto& localeEntry : ::inflection::util::LocaleUtils::getSupportedLocaleMap()) {
        inflection::util::ULocale locale(localeEntry.first);
        const inflection::dialog::CommonConceptFactory* conceptFactory = nullptr;
        conceptFactory = npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale));

        auto dictionary = ::inflection::dictionary::DictionaryMetaData::createDictionary(locale);
        int32_t nounCount = 0;
        ::std::vector<::std::u16string> list;
        int64_t pluralProperty = 0;
        if (npc(dictionary)->getCombinedBinaryType(&pluralProperty, u"plural") != nullptr) {
            for (const auto& word : npc(dictionary)->getKnownWords()) {
                if (!npc(dictionary)->hasProperty(word, u"singular")) {
                    continue;
                }
                list.emplace_back(word);
                if (++nounCount >= MAXIMUM_WORDS_TO_TEST) {
                    break;
                }
            }
        }

        bool canBePlural = !list.empty();
        if (!canBePlural) {
            list.emplace_back(u"A");
            list.emplace_back(u"B");
            list.emplace_back(u"C");
        }

        const ::inflection::dialog::SemanticFeatureModel* model = npc(conceptFactory)->getSemanticFeatureModel();

        ::std::vector<inflection::dialog::SemanticFeatureConceptBase*> concepts(toConcepts(model, list));
        inflection::dialog::SemanticConceptList* andConceptList = npc(conceptFactory)->createAndList(concepts);
        if (canBePlural) {
            npc(andConceptList)->putConstraintByName(u"number", u"plural");
        }
        npc(andConceptList)->setItemPrefix(inflection::dialog::SpeakableString(u"prefix "));
        auto speakableResult = npc(andConceptList)->toSpeakableString();

        inflection::dialog::SemanticConceptList* andConceptListClone = npc(andConceptList)->clone();
        auto speakableResultFromClone = npc(andConceptList)->toSpeakableString();

        REQUIRE(*npc(speakableResult) == *npc(speakableResultFromClone));

        delete andConceptList;
        delete andConceptListClone;
        delete speakableResult;
        delete speakableResultFromClone;

        inflection::dialog::SemanticConceptList* orConceptList = npc(conceptFactory)->createOrList(concepts);
        if (canBePlural) {
            npc(orConceptList)->putConstraintByName(u"number", u"plural");
        }
        npc(orConceptList)->setItemPrefix(inflection::dialog::SpeakableString(u"prefix "));
        speakableResult = npc(orConceptList)->toSpeakableString();

        inflection::dialog::SemanticConceptList* orConceptListClone = npc(orConceptList)->clone();
        speakableResultFromClone = npc(orConceptListClone)->toSpeakableString();

        REQUIRE(*npc(speakableResult) == *npc(speakableResultFromClone));

        for (auto inflectableConcept : concepts) {
            delete inflectableConcept;
        }
        delete orConceptList;
        delete orConceptListClone;
        delete speakableResult;
        delete speakableResultFromClone;
    }
}
