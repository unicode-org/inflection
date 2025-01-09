/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>

namespace inflection::grammar::synthesis {

SvGrammarSynthesizer_GenderLookupFunction::SvGrammarSynthesizer_GenderLookupFunction()
: super(::inflection::util::LocaleUtils::SWEDISH(), {GrammemeConstants::GENDER_NEUTER(), GrammemeConstants::GENDER_COMMON()})
, dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryCommon, {GrammemeConstants::GENDER_COMMON()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {GrammemeConstants::GENDER_NEUTER()}));
}

SvGrammarSynthesizer_GenderLookupFunction::~SvGrammarSynthesizer_GenderLookupFunction()
{
}

::std::u16string SvGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (out.empty()) {
        int64_t binaryType = 0;
        if(dictionary.getCombinedBinaryType(&binaryType, word) != nullptr){
            if ((binaryType & dictionaryCommon) != 0) {
                out = GrammemeConstants::GENDER_COMMON();
            }
            if ((binaryType & dictionaryNeuter) != 0) {
                out = GrammemeConstants::GENDER_NEUTER();
            }
        }
    }
    return out;
}

} // namespace inflection::grammar::synthesis
