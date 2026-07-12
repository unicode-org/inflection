/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/analysis/RussianExposableMorphology.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::analysis {

RussianExposableMorphology::RussianExposableMorphology()
    : ::inflection::dialog::DictionaryLookupInflector(::inflection::util::LocaleUtils::RUSSIAN(), {
        {GrammemeConstants::POS_PROPER_NOUN, GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB},
        {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_PREPOSITIONAL},
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER},
        {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
        {GrammemeConstants::ANIMACY_INANIMATE, GrammemeConstants::ANIMACY_ANIMATE},
        {GrammemeConstants::REGISTER_INFORMAL}
    }, {{GrammemeConstants::POS_PROPER_NOUN, GrammemeConstants::NUMBER_PLURAL}}, true)
{
    inflectableChars.addAll(inflection::lang::StringFilterUtil::getExemplarCharacters(::inflection::util::LocaleUtils::RUSSIAN()));
    inflectableChars.freeze();
}

RussianExposableMorphology* RussianExposableMorphology::getInstance() {
    static auto INSTANCE_ = new RussianExposableMorphology();
    return INSTANCE_;
}

RussianExposableMorphology::~RussianExposableMorphology() = default;

bool RussianExposableMorphology::isInflectable(std::u16string_view word) const {
    static constexpr const char16_t* const doNotInflect[] = {
        // This must be in sorted order.
        u"августа",
        u"апреля",
        u"декабря",
        u"июля",
        u"июня",
        u"марта",
        u"мая",
        u"мин",
        u"ноября",
        u"октября",
        u"сентября",
        u"февраля",
        u"января",
    };
    return inflectableChars.containsAllCodePoints(word)
        && !inflection::util::ArrayUtils::contains<doNotInflect>(word);
}

} // namespace inflection::analysis
