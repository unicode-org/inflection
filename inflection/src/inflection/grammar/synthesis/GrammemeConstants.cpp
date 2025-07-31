/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

const ::std::u16string& GrammemeConstants::POS_ADJECTIVE()
{
    static auto POS_ADJECTIVE_ = new ::std::u16string(u"adjective");
    return *npc(POS_ADJECTIVE_);
}

const ::std::u16string& GrammemeConstants::POS_PARTICLE()
{
    static auto POS_PARTICLE_ = new ::std::u16string(u"particle");
    return *npc(POS_PARTICLE_);
}

const ::std::u16string& GrammemeConstants::POS_NOUN()
{
    static auto POS_NOUN_ = new ::std::u16string(u"noun");
    return *npc(POS_NOUN_);
}

const ::std::u16string& GrammemeConstants::POS_ADPOSITION()
{
    static auto POS_ADPOSITION_ = new ::std::u16string(u"adposition");
    return *npc(POS_ADPOSITION_);
}

const ::std::u16string& GrammemeConstants::POS_VERB() {
    static auto POS_VERB_ = new ::std::u16string(u"verb");
    return *npc(POS_VERB_);
}

const ::std::u16string& GrammemeConstants::POS_ARTICLE() {
    static auto POS_DETERMINER_ = new ::std::u16string(u"article");
    return *npc(POS_DETERMINER_);
}

const ::std::u16string& GrammemeConstants::POS_PROPER_NOUN() {
    static auto POS_PROPER_NOUN_ = new ::std::u16string(u"proper-noun");
    return *npc(POS_PROPER_NOUN_);
}

const ::std::u16string& GrammemeConstants::POS_PRONOUN() {
    static auto POS_PRONOUN_ = new ::std::u16string(u"pronoun");
    return *npc(POS_PRONOUN_);
}

const ::std::u16string& GrammemeConstants::CASE_NOMINATIVE()
{
    static auto CASE_NOMINATIVE_ = new ::std::u16string(u"nominative");
    return *npc(CASE_NOMINATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_ABLATIVE()
{
    static auto CASE_ABLATIVE_ = new ::std::u16string(u"ablative");
    return *npc(CASE_ABLATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_ACCUSATIVE()
{
    static auto CASE_ACCUSATIVE_ = new ::std::u16string(u"accusative");
    return *npc(CASE_ACCUSATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_ADESSIVE()
{
    static auto CASE_ADESSIVE_ = new ::std::u16string(u"adessive");
    return *npc(CASE_ADESSIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_ALLATIVE()
{
    static auto CASE_ALLATIVE_ = new ::std::u16string(u"allative");
    return *npc(CASE_ALLATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_COMITATIVE()
{
    static auto CASE_COMITATIVE_ = new ::std::u16string(u"comitative");
    return *npc(CASE_COMITATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_DATIVE()
{
    static auto CASE_DATIVE_ = new ::std::u16string(u"dative");
    return *npc(CASE_DATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_DIRECT()
{
    static auto CASE_DIRECT_ = new ::std::u16string(u"direct");
    return *npc(CASE_DIRECT_);
}

const ::std::u16string& GrammemeConstants::CASE_ELATIVE()
{
    static auto CASE_ELATIVE_ = new ::std::u16string(u"elative");
    return *npc(CASE_ELATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_ESSIVE()
{
    static auto CASE_ESSIVE_ = new ::std::u16string(u"essive");
    return *npc(CASE_ESSIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_GENITIVE()
{
    static auto CASE_GENITIVE_ = new ::std::u16string(u"genitive");
    return *npc(CASE_GENITIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_ILLATIVE()
{
    static auto CASE_ILLATIVE_ = new ::std::u16string(u"illative");
    return *npc(CASE_ILLATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_INESSIVE()
{
    static auto CASE_INESSIVE_ = new ::std::u16string(u"inessive");
    return *npc(CASE_INESSIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_INSTRUMENTAL()
{
    static auto CASE_INSTRUMENTAL_ = new ::std::u16string(u"instrumental");
    return *npc(CASE_INSTRUMENTAL_);
}

const ::std::u16string& GrammemeConstants::CASE_LOCATIVE()
{
    static auto CASE_LOCATIVE_ = new ::std::u16string(u"locative");
    return *npc(CASE_LOCATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_OBLIQUE()
{
    static auto CASE_OBLIQUE_ = new ::std::u16string(u"oblique");
    return *npc(CASE_OBLIQUE_);
}

const ::std::u16string& GrammemeConstants::CASE_PARTITIVE()
{
    static auto CASE_PARTITIVE_ = new ::std::u16string(u"partitive");
    return *npc(CASE_PARTITIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_PREPOSITIONAL()
{
    static auto CASE_PREPOSITIONAL_ = new ::std::u16string(u"prepositional");
    return *npc(CASE_PREPOSITIONAL_);
}

const ::std::u16string& GrammemeConstants::CASE_SOCIATIVE()
{
    static auto CASE_SOCIATIVE_ = new ::std::u16string(u"sociative");
    return *npc(CASE_SOCIATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_TRANSLATIVE()
{
    static auto CASE_TRANSLATIVE_ = new ::std::u16string(u"translative");
    return *npc(CASE_TRANSLATIVE_);
}

const ::std::u16string& GrammemeConstants::CASE_VOCATIVE()
{
    static auto CASE_VOCATIVE_ = new ::std::u16string(u"vocative");
    return *npc(CASE_VOCATIVE_);
}

const ::std::u16string& GrammemeConstants::NUMBER_SINGULAR()
{
    static auto NUMBER_SINGULAR_ = new ::std::u16string(u"singular");
    return *npc(NUMBER_SINGULAR_);
}

const ::std::u16string& GrammemeConstants::NUMBER_DUAL()
{
    static auto NUMBER_DUAL_ = new ::std::u16string(u"dual");
    return *npc(NUMBER_DUAL_);
}

const ::std::u16string& GrammemeConstants::NUMBER_PLURAL()
{
    static auto NUMBER_PLURAL_ = new ::std::u16string(u"plural");
    return *npc(NUMBER_PLURAL_);
}

const ::std::u16string& GrammemeConstants::GENDER_MASCULINE()
{
    static auto GENDER_MASCULINE_ = new ::std::u16string(u"masculine");
    return *npc(GENDER_MASCULINE_);
}

const ::std::u16string& GrammemeConstants::GENDER_FEMININE()
{
    static auto GENDER_FEMININE_ = new ::std::u16string(u"feminine");
    return *npc(GENDER_FEMININE_);
}

const ::std::u16string& GrammemeConstants::GENDER_NEUTER()
{
    static auto GENDER_NEUTER_ = new ::std::u16string(u"neuter");
    return *npc(GENDER_NEUTER_);
}

const ::std::u16string& GrammemeConstants::GENDER_COMMON()
{
    static auto GENDER_COMMON_ = new ::std::u16string(u"common");
    return *npc(GENDER_COMMON_);
}

const ::std::u16string& GrammemeConstants::DEFINITENESS_INDEFINITE()
{
    static auto DEFINITENESS_INDEFINITE_ = new ::std::u16string(u"indefinite");
    return *npc(DEFINITENESS_INDEFINITE_);
}

const ::std::u16string& GrammemeConstants::DEFINITENESS_DEFINITE()
{
    static auto DEFINITENESS_DEFINITE_ = new ::std::u16string(u"definite");
    return *npc(DEFINITENESS_DEFINITE_);
}

const ::std::u16string& GrammemeConstants::DEFINITENESS_CONSTRUCT()
{
    static auto DEFINITENESS_CONSTRUCT_ = new ::std::u16string(u"construct");
    return *npc(DEFINITENESS_CONSTRUCT_);
}

const ::std::u16string& GrammemeConstants::PERSON_FIRST()
{
    static auto PERSON_FIRST_ = new ::std::u16string(u"first");
    return *npc(PERSON_FIRST_);
}

const ::std::u16string& GrammemeConstants::PERSON_SECOND()
{
    static auto PERSON_SECOND_ = new ::std::u16string(u"second");
    return *npc(PERSON_SECOND_);
}

const ::std::u16string& GrammemeConstants::PERSON_THIRD()
{
    static auto PERSON_THIRD_ = new ::std::u16string(u"third");
    return *npc(PERSON_THIRD_);
}

const ::std::u16string& GrammemeConstants::ANIMACY_ANIMATE()
{
    static auto ANIMACY_ANIMATE_ = new ::std::u16string(u"animate");
    return *npc(ANIMACY_ANIMATE_);
}

const ::std::u16string& GrammemeConstants::ANIMACY_INANIMATE()
{
    static auto ANIMACY_INANIMATE_ = new ::std::u16string(u"inanimate");
    return *npc(ANIMACY_INANIMATE_);
}

const ::std::u16string& GrammemeConstants::ANIMACY_HUMAN()
{
    static auto ANIMACY_HUMAN_ = new ::std::u16string(u"human");
    return *npc(ANIMACY_HUMAN_);
}

const ::std::u16string& GrammemeConstants::MOOD_IMPERATIVE()
{
    static auto MOOD_IMPERATIVE_ = new ::std::u16string(u"imperative");
    return *npc(MOOD_IMPERATIVE_);
}

const ::std::u16string& GrammemeConstants::MOOD_INDICATIVE()
{
    static auto MOOD_INDICATIVE_ = new ::std::u16string(u"indicative");
    return *npc(MOOD_INDICATIVE_);
}

const ::std::u16string& GrammemeConstants::TENSE_PAST()
{
    static auto TENSE_PAST_ = new ::std::u16string(u"past");
    return *npc(TENSE_PAST_);
}

const ::std::u16string& GrammemeConstants::TENSE_PRESENT()
{
    static auto TENSE_PRESENT_ = new ::std::u16string(u"present");
    return *npc(TENSE_PRESENT_);
}

const ::std::u16string& GrammemeConstants::TENSE_FUTURE()
{
    static auto TENSE_FUTURE_ = new ::std::u16string(u"future");
    return *npc(TENSE_FUTURE_);
}

const ::std::u16string& GrammemeConstants::CLUSIVITY_INCLUSIVE()
{
    static auto CLUSIVITY_INCLUSIVE_ = new ::std::u16string(u"inclusive");
    return *npc(CLUSIVITY_INCLUSIVE_);
}

const ::std::u16string& GrammemeConstants::CLUSIVITY_EXCLUSIVE()
{
    static auto CLUSIVITY_EXCLUSIVE_ = new ::std::u16string(u"exclusive");
    return *npc(CLUSIVITY_EXCLUSIVE_);
}

const ::std::u16string& GrammemeConstants::COMPARISON_PLAIN()
{
    static auto COMPARISON_PLAIN_ = new ::std::u16string(u"plain");
    return *npc(COMPARISON_PLAIN_);
}

const ::std::u16string& GrammemeConstants::COMPARISON_COMPARATIVE()
{
    static auto COMPARISON_COMPARATIVE_ = new ::std::u16string(u"comparative");
    return *npc(COMPARISON_COMPARATIVE_);
}

const ::std::u16string& GrammemeConstants::COMPARISON_SUPERLATIVE()
{
    static auto COMPARISON_SUPERLATIVE_ = new ::std::u16string(u"superlative");
    return *npc(COMPARISON_SUPERLATIVE_);
}

const ::std::u16string& GrammemeConstants::SIZENESS_UNSIZED()
{
    static auto SIZENESS_UNSIZED_ = new ::std::u16string(u"unsized");
    return *npc(SIZENESS_UNSIZED_);
}

const ::std::u16string& GrammemeConstants::SIZENESS_DIMINUTIVE()
{
    static auto SIZENESS_DIMINUTIVE_ = new ::std::u16string(u"diminutive");
    return *npc(SIZENESS_DIMINUTIVE_);
}

const ::std::u16string& GrammemeConstants::SIZENESS_DOUBLE_DIMINUTIVE()
{
    static auto SIZENESS_DOUBLE_DIMINUTIVE_ = new ::std::u16string(u"double-diminutive");
    return *npc(SIZENESS_DOUBLE_DIMINUTIVE_);
}

const ::std::u16string &GrammemeConstants::VERBTYPE_INFINITIVE() {
    static auto VERBTYPE_INFINITIVE_ = new ::std::u16string(u"infinitive");
    return *npc(VERBTYPE_INFINITIVE_);
}

const ::std::u16string &GrammemeConstants::REGISTER_INFORMAL() {
    static auto REGISTER_INFORMAL_ = new ::std::u16string(u"informal");
    return *npc(REGISTER_INFORMAL_);
}

const ::std::u16string &GrammemeConstants::DERIVATIONTYPE_PATRONYMIC(){
    static auto DERIVATIONTYPE_PATRONYMIC_ = new ::std::u16string(u"patronymic");
    return *npc(DERIVATIONTYPE_PATRONYMIC_);
}

const ::std::u16string& GrammemeConstants::VERBTYPE_PARTICIPLE() {
    static auto VERBTYPE_PARTICIPLE_ = new ::std::u16string(u"participle");
    return *npc(VERBTYPE_PARTICIPLE_);
}

const ::std::u16string &GrammemeConstants::ASPECT_PERFECTIVE(){
    static auto ASPECT_PERFECTIVE_ = new ::std::u16string(u"perfective");
    return *npc(ASPECT_PERFECTIVE_);
}

} // namespace inflection::grammar::synthesis
