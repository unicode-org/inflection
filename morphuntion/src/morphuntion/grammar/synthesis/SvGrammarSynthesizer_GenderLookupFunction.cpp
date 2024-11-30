/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Validate.hpp>

namespace morphuntion::grammar::synthesis {

SvGrammarSynthesizer_GenderLookupFunction::SvGrammarSynthesizer_GenderLookupFunction()
: super(::morphuntion::util::LocaleUtils::SWEDISH(), {GrammemeConstants::GENDER_NEUTER(), GrammemeConstants::GENDER_COMMON()})
, dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryCommon, {GrammemeConstants::GENDER_COMMON()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {GrammemeConstants::GENDER_NEUTER()}));
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

} // namespace morphuntion::grammar::synthesis
