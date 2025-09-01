/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_NumberLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

MlGrammarSynthesizer_NumberLookupFunction::MlGrammarSynthesizer_NumberLookupFunction()
    : super(::inflection::util::LocaleUtils::MALAYALAM(),
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_VERB()})
    , tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::MALAYALAM())))
    , dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

MlGrammarSynthesizer_NumberLookupFunction::~MlGrammarSynthesizer_NumberLookupFunction()
{
}

::std::u16string MlGrammarSynthesizer_NumberLookupFunction::determine(const ::std::u16string& word) const
{
    if (word.empty()) {
        return {};
    }

    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }

    std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(tokenizer->createTokenChain(word)));

    for (const auto& token : *tokenChain) {
        if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(&token) != nullptr) {
            if (dictionary.hasAllProperties(token.getCleanValue(), nounProperty)) {
                out = super::determine(token.getValue());
                if (!out.empty()) {
                    return out;
                }
            }
        }
    }

    // plural suffix detection

    const auto& lastToken = npc(npc(tokenChain->getEnd())->getPrevious())->getValue();
    for (const auto& suffix : {u"കൾ", u"ങ്ങൾ", u"മാർ", u"വർ", u"കളുടെ", u"ങ്ങൾക്ക്"}) {
        if (lastToken.ends_with(suffix)) {
            return GrammemeConstants::NUMBER_PLURAL();
        }
    }

    return GrammemeConstants::NUMBER_SINGULAR();
}

} // namespace inflection::grammar::synthesis
