/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_InflectorPatternChain.hpp>

#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_InflectorPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_InflectorPatternChain::NlGrammarSynthesizer_InflectorPatternChain(const ::std::vector<NlGrammarSynthesizer_InflectorPattern*>& inflectors, bool returnOriginalIfUnsupported) 
    : super()
    , inflectors(inflectors)
    , returnOriginalIfUnsupported(returnOriginalIfUnsupported)
{
}

NlGrammarSynthesizer_InflectorPatternChain::~NlGrammarSynthesizer_InflectorPatternChain()
{
    for (auto inflector : inflectors) {
        delete inflector;
    }
}

::std::u16string NlGrammarSynthesizer_InflectorPatternChain::inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    for (auto inflector : inflectors) {
        auto inflectedString(npc(inflector)->inflect(displayString, constraints));
        if (!inflectedString.empty()) {
            return inflectedString;
        }
    }
    return returnOriginalIfUnsupported ? displayString : ::std::u16string();
}

} // namespace morphuntion::grammar::synthesis
